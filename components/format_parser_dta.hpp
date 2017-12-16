#ifndef FORMAT_PARSERS_DTA_H
#define FORMAT_PARSERS_DTA_H

#include <format_parsers.hpp>

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
    decrypt((char *) &mFileHeader,sizeof(mFileHeader),mKey1,mKey2);

    // read the content headers:
        
    srcFile.seekg(mFileHeader.mContentOffset);
    mContentHeaders.clear();

    unsigned int headerArraySize = mFileHeader.mFileCount * sizeof(ContentHeader);
    ContentHeader *headerArray = (ContentHeader *) malloc(headerArraySize);
    srcFile.read((char *) headerArray,headerArraySize);
    decrypt((char *) headerArray,headerArraySize,mKey1,mKey2);

    for (int i = 0; i < mFileHeader.mFileCount; ++i)
        mContentHeaders.push_back(headerArray[i]);

    free(headerArray);

    // read the data headers:

    mDataHeaders.clear();

    for (int i = 0; i < mContentHeaders.size(); ++i)
    {
        DataHeader h;
        srcFile.seekg(mContentHeaders[i].mDataOffset);
        srcFile.read(reinterpret_cast<char *>(&h),sizeof(DataHeader));
        decrypt(reinterpret_cast<char *>(&h),sizeof(DataHeader),mKey1,mKey2);
        h.mName[h.mNameLength] = 0;    // terminate the string
        mDataHeaders.push_back(h);
    }

    return true;
}

void DataFormatDTA::getFile(std::ifstream &srcFile, unsigned int index, char **dstBuffer, unsigned int &length)
{
    length = getFileSize(index);
    *dstBuffer = (char *) malloc(length);

    unsigned int fileOffset = 0;   // TODO: where is the file?

    srcFile.clear();
    srcFile.seekg(fileOffset);
    srcFile.read(*dstBuffer,length);
    decrypt(*dstBuffer,length,mKey1,mKey2);
}

unsigned int DataFormatDTA::getFileSize(unsigned int index)
{
    return mDataHeaders[index].mSize;
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
    return std::string(reinterpret_cast<char *>(mDataHeaders[index].mName));
}
    
void DataFormatDTA::decrypt(char *buffer, unsigned int bufferLen, uint32_t key1, uint32_t key2)
{
    key1 ^= 0x39475694;    // has to be done for some reason
    key2 ^= 0x34985762;

    uint32_t keys[2] = {key1, key2};
    char *keyBytes = reinterpret_cast<char *>(keys);

    for (unsigned int i = 0; i < bufferLen; ++i)
    {
        char dataByte = buffer[i];
        char keyByte = keyBytes[i % sizeof(keys)];

        buffer[i] = (char) ( ~((~dataByte) ^ keyByte) );
    }
}

}

#endif
