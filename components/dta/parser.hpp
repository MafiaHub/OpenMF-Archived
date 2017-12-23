#ifndef FORMAT_PARSERS_DTA_H
#define FORMAT_PARSERS_DTA_H

#include <base_parser.hpp>

namespace MFFormat
{

class DataFormatDTA: public DataFormat
{
public:
    virtual bool load(std::ifstream &srcFile) override; ///< Loads the file table from the DTA file.
    void setDecryptKeys(uint32_t key1, uint32_t key2);  ///< Decrypting keys have to be set before load(...) is called.
    void setDecryptKeys(uint32_t keys[2]);
    unsigned int getNumFiles();                         ///< Get the number of files inside the DTA.
    unsigned int getFileSize(unsigned int index);
    void decrypt(char *buffer, unsigned int bufferLen, unsigned int relativeShift=0);
    std::string getFileName(unsigned int index);
    void getFile(std::ifstream &srcFile, unsigned int index, char **dstBuffer, unsigned int &length);   ///< Get the concrete file from within the DST file into a buffer.
    int getFileIndex(std::string fileName);

    static uint32_t A0_KEYS[2];   // decrypting keys
    static uint32_t A1_KEYS[2];
    static uint32_t A2_KEYS[2];
    static uint32_t A3_KEYS[2];
    static uint32_t A4_KEYS[2];
    static uint32_t A5_KEYS[2];
    static uint32_t A6_KEYS[2];
    static uint32_t A7_KEYS[2];
    static uint32_t A8_KEYS[2];
    static uint32_t A9_KEYS[2];
    static uint32_t AA_KEYS[2];
    static uint32_t AB_KEYS[2];
    static uint32_t AC_KEYS[2];

    typedef struct
    {
        uint32_t mFileCount;
        uint32_t mFileTableOffset;
        uint32_t mFileTableSize;
        uint32_t mUnknown;
    } FileHeader;

    typedef struct
    {
        uint16_t mFileNameChecksum;     
        uint16_t mFileNameLength; 
        uint32_t mDataOffset;
        uint32_t mDataEnd;
        char mNameHint[16];
    } FileTableRecord;

    typedef struct
    {
        uint32_t mUnknown;
        uint32_t mUnknown2;
        uint32_t mUnknown3;
        uint32_t mUnknown4;
        uint32_t mSize;
        uint32_t mUnknown6;
        unsigned char mNameLength;
        unsigned char mUnknown7[7];
        unsigned char mName[256];
    } DataFileHeader;

    inline std::vector<FileTableRecord> getFileTableRecords() { return mFileTableRecords; };
    inline std::vector<DataFileHeader> getDataFileHeaders() { return mDataFileHeaders; };

protected:
    FileHeader mFileHeader;
    std::vector<FileTableRecord> mFileTableRecords;
    std::vector<DataFileHeader> mDataFileHeaders;
    uint32_t mKey1;
    uint32_t mKey2;
}; 

uint32_t DataFormatDTA::A0_KEYS[2] = {0x7f3d9b74, 0xec48fe17};
uint32_t DataFormatDTA::A1_KEYS[2] = {0xe7375f59, 0x900210e};
uint32_t DataFormatDTA::A2_KEYS[2] = {0x1417d340, 0xb6399e19};
uint32_t DataFormatDTA::A3_KEYS[2] = {0xa94b8d3c, 0x771f3888};
uint32_t DataFormatDTA::A4_KEYS[2] = {0xa94b8d3c, 0x771f3888};
uint32_t DataFormatDTA::A5_KEYS[2] = {0x4f4bb0c6, 0xea340420};
uint32_t DataFormatDTA::A6_KEYS[2] = {0x728e2db9, 0x5055da68};
uint32_t DataFormatDTA::A7_KEYS[2] = {0xf4f03a72, 0xe266fe62};
uint32_t DataFormatDTA::A8_KEYS[2] = {0xd8dd8fac, 0x5324ace5};  // not working?
uint32_t DataFormatDTA::A9_KEYS[2] = {0x959d1117, 0x5b763446};
uint32_t DataFormatDTA::AA_KEYS[2] = {0xd4ad90c6, 0x67da216e};
uint32_t DataFormatDTA::AB_KEYS[2] = {0x7f3d9b74, 0xec48fe17};
uint32_t DataFormatDTA::AC_KEYS[2] = {0xa94b8d3c, 0x771f3888};

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
    FileTableRecord *headerArray = (FileTableRecord *) malloc(headerArraySize);
    srcFile.read((char *) headerArray,headerArraySize);

    if (!srcFile.good())
        return false;

    decrypt((char *) headerArray,headerArraySize);

    for (size_t i = 0; i < mFileHeader.mFileCount; ++i)
        mFileTableRecords.push_back(headerArray[i]);

    free(headerArray);

    // read the data headers:

    mDataFileHeaders.clear();

    for (int i = 0; i < mFileTableRecords.size(); ++i)
    {
        DataFileHeader h;
        srcFile.seekg(mFileTableRecords[i].mDataOffset);
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
    for (int i = 0; i < mDataFileHeaders.size(); i++)
        if (fileName.compare((char *) mDataFileHeaders[i].mName) == 0)
            return i;

    return -1;
}

void DataFormatDTA::getFile(std::ifstream &srcFile, unsigned int index, char **dstBuffer, unsigned int &length)
{
    length = getFileSize(index);
    *dstBuffer = (char *) malloc(length);

    unsigned int fileOffset = mFileTableRecords[index].mDataEnd + 5;   // why + 5?
    // TODO: some files are probably compressed

    srcFile.clear();
    srcFile.seekg(fileOffset);
    srcFile.read(*dstBuffer,length);
    decrypt(*dstBuffer,length,1);
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

}

#endif
