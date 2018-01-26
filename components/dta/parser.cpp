#include <dta/parser.hpp>

namespace MFFormat
{

bool DataFormatDTA::load(std::ifstream &srcFile)
{
    int32_t magicNo;

    srcFile.read((char *) &magicNo,4);

    if (magicNo != 0x30445349)     // magic number == "ISD0"?
        return false;

    // read the file header:

    srcFile.read((char *) &mFileHeader,sizeof(FileHeader));
    decrypt((char *) &mFileHeader,sizeof(mFileHeader));

    // read the content headers:

    srcFile.seekg(mFileHeader.mFileTableOffset);

    if (!srcFile.good())
        return false;

    mFileTableRecords.clear();

    unsigned int headerArraySize = mFileHeader.mFileCount * sizeof(FileTableRecord);
    ScopedBuffer headerArray(headerArraySize);
    srcFile.read((char *) headerArray,headerArraySize);

    if (!srcFile.good())
        return false;

    decrypt((char *) headerArray,headerArraySize);

    for (size_t i = 0; i < mFileHeader.mFileCount; ++i)
        mFileTableRecords.push_back(headerArray.as<FileTableRecord>()[i]);

    // read the data headers:

    mDataFileHeaders.clear();

    for (int i = 0; i < (int) mFileTableRecords.size(); ++i)
    {
        DataFileHeader h;
        srcFile.seekg(mFileTableRecords[i].mHeaderOffset);
        srcFile.read(reinterpret_cast<char *>(&h),sizeof(DataFileHeader));

        if (!srcFile.good())
            return false;

        decrypt(reinterpret_cast<char *>(&h),sizeof(DataFileHeader));
        h.mName[h.mNameLength] = 0;    // terminate the string
        mDataFileHeaders.push_back(h);
    }

    return true;
}

int DataFormatDTA::getFileIndex(std::string fileName)
{
    for (int i = 0; i < (int) mDataFileHeaders.size(); i++)
        if (fileName.compare((char *) mDataFileHeaders[i].mName) == 0)
            return i;

    return -1;
}

ScopedBuffer DataFormatDTA::getFile(std::ifstream &srcFile, unsigned int index)
{
    unsigned length = getFileSize(index);
    ScopedBuffer dstBuffer(length);

    unsigned int fileOffset = mFileTableRecords[index].mDataOffset;

    srcFile.clear();
    srcFile.seekg(fileOffset);

    unsigned int bufferPos = 0;

    bool blockEncrypted = mDataFileHeaders[index].mFlags[0] & 0x80;

    mWavHeaderRead = false;

    for (uint32_t i = 0; i < mDataFileHeaders[index].mCompressedBlockCount; ++i)
    {
        // FIXME: make this function nicer
        uint32_t blockSize;     // compressed size

        srcFile.read((char *) &blockSize,sizeof(blockSize));
        blockSize = blockSize & 0xffff;

        ScopedBuffer block(blockSize);

        srcFile.read(block,blockSize);

        if (blockEncrypted)
            decrypt(block,blockSize);

        unsigned char blockType = block[0];

        dstBuffer.copy_from(bufferPos, block + 1, blockSize - 1);      // 1 - don't include the type byte

        std::vector<unsigned char> decompressed;

        int dpcmType = -1;

        switch (blockType)
        {
            case BLOCK_UNCOMPRESSED:
                for (uint32_t j = 0; j < blockSize - 1; ++j)           // just copy the data
                    decompressed.push_back(block[1 + j]);
                break;

            case BLOCK_LZSS_RLE:
                decompressed = decompressLZSS((unsigned char *) (*dstBuffer + bufferPos),blockSize - 1);
                break;

            case BLOCK_DPCM0: dpcmType = 0; break;
            case BLOCK_DPCM1: dpcmType = 1; break;
            case BLOCK_DPCM2: dpcmType = 2; break;
            case BLOCK_DPCM3: dpcmType = 3; break;
            case BLOCK_DPCM4: dpcmType = 4; break;
            case BLOCK_DPCM5: dpcmType = 5; break;
            case BLOCK_DPCM6: dpcmType = 6; break;
            default: break;
        }

        if (dpcmType >= 0)
            decompressed = decompressDPCM(&WAV_DELTAS[128 * dpcmType],(unsigned char *) (*dstBuffer + bufferPos),blockSize - 1);

        dstBuffer.copy_from(bufferPos,decompressed.data(),decompressed.size());
        bufferPos += (uint32_t) decompressed.size();
    }

    return dstBuffer;
}

unsigned int DataFormatDTA::getFileSize(unsigned int index)
{
    return mDataFileHeaders[index].mSize;
}

void DataFormatDTA::setDecryptKeys(uint32_t key1, uint32_t key2)
{
    mKey1 = key1;
    mKey2 = key2;
}

void DataFormatDTA::setDecryptKeys(uint32_t keys[2])
{
    mKey1 = keys[0];
    mKey2 = keys[1];
}

unsigned int DataFormatDTA::getNumFiles()
{
    return mFileHeader.mFileCount;
}

std::string DataFormatDTA::getFileName(unsigned int index)
{
    return std::string(reinterpret_cast<char *>(mDataFileHeaders[index].mName));
}

void DataFormatDTA::decrypt(char *buffer, unsigned int bufferLen, unsigned int relativeShift)
{
    uint32_t key1 = mKey1;
    uint32_t key2 = mKey2;

    key1 ^= 0x39475694;    // has to be done for some reason
    key2 ^= 0x34985762;

    uint32_t keys[2] = {key1, key2};
    char *keyBytes = reinterpret_cast<char *>(keys);

    for (unsigned int i = 0; i < bufferLen; ++i)
    {
        char dataByte = buffer[i];
        char keyByte = keyBytes[(i + relativeShift) % sizeof(keys)];

        buffer[i] = (char) ( ~((~dataByte) ^ keyByte) );
    }
}

std::vector<unsigned char> DataFormatDTA::decompressDPCM(uint16_t *delta, unsigned char *buffer, unsigned int bufferLen)
{
    unsigned int position = 0;
    std::vector<unsigned char> decompressed;

    if (!mWavHeaderRead)
    {
        memcpy((char *) &mWavHeader,buffer,sizeof(mWavHeader));
        position += sizeof(mWavHeader);
        decrypt((char *) &mWavHeader,sizeof(mWavHeader));
        decompressed.insert(decompressed.end(),(char *) &mWavHeader, ((char *) &mWavHeader) + sizeof(mWavHeader));
        mWavHeaderRead = true;
    }

    if (mWavHeader.mChannels == 1)
    {
        decompressed.push_back(buffer[position]);
        decompressed.push_back(buffer[position + 1]);

        uint16_t value = *((uint16_t *) &(buffer[position]));

        position += 2;

        while (position < bufferLen)
        {
            int sign = (buffer[position] & 0x80) == 0 ? 1.0 : -1.0;
            value += sign * delta[buffer[position] & 0x7f];
            decompressed.push_back(value & 0xff);
            decompressed.push_back(value >> 8);
            position += 1;
        }
    }
    else
    {
        // TODO: find a stereo WAV and implement this or prove there are no stereo WAVs
    }

    return decompressed;
}

std::vector<unsigned char> DataFormatDTA::decompressLZSS(unsigned char *buffer, unsigned int bufferLen)
{
    // rewritten version of hdmaster's source

    unsigned int position = 0;
    std::vector<unsigned char> decompressed;

    while (position < bufferLen)
    {
        uint16_t value = (buffer[position] << 8) | (buffer[position + 1]);  // get first two bytes => 16bit LZSS group
        position += 2;

        if (value == 0)  // 0 means just copy the next (at most) 16 bytes
        {
            unsigned int n = std::min(bufferLen - position,(unsigned int) 16);

            for (unsigned int j = 0; j < n; ++j)
                decompressed.push_back(buffer[position + j]);

            position += n;
        }
        else
        {
            // go bit by bit in the group from the left
            for (unsigned int i = 0; i < 16 && position < bufferLen; ++i, value <<= 1)
            {
                if (value & 0x8000)    // bit set => copy a sequence from already decompressed sequence
                {
                    uint32_t offset = (buffer[position] << 4) | (buffer[position + 1] >> 4);
                    uint32_t n = buffer[position + 1] & 0x0f;

                    // offset points back to already decompressed bytes, n defines the sequence length

                    if (offset == 0)
                    {
                        n = ((n << 8) | (buffer[position + 2])) + 16;
                        decompressed.insert(decompressed.end(),n,buffer[position + 3]);
                        position += 4;
                    }
                    else
                    {
                        n += 3;
                        if (n > offset)
                        {
                            for (unsigned int j = 0; j < n; ++j)
                                decompressed.emplace_back(*(decompressed.end() - offset));
                        }
                        else
                        {
                            decompressed.insert(decompressed.end(),decompressed.end() - offset,decompressed.end() - offset + n);
                        }

                        position += 2;
                    }
                }
                else    // bit not set => just copy to output
                {
                    decompressed.push_back(buffer[position]);
                    position++;
                }
            }
        }
    }

    return decompressed;
}

}
