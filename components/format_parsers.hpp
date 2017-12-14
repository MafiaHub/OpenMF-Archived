#include <fstream>
#include <iostream>
#include <cstdint>

namespace formats
{

/// Abstract class representing a game data format.

class DataFormat
{
public:
    virtual bool load(std::ifstream &srcFile)=0;
    virtual bool save(std::ofstream &dstFile)=0;
};

class DataFormat4DS: public DataFormat
{
};

class DataFormat5DS: public DataFormat
{
};

class DataFormatDTA: public DataFormat
{
public:
    virtual bool load(std::ifstream &srcFile) override;
    virtual bool save(std::ofstream &dstFile) override;

    void setDecryptKeys(uint32_t key1, uint32_t key2);

    typedef struct
    {
        uint32_t mFileCount;
        uint32_t mContentOffset;
        uint32_t mContentSize;
        uint32_t mUnknown;
    } FileHeader;

    typedef struct
    {
        uint32_t mUnknown;
        uint32_t mDataOffset;
        uint32_t mUnknown2;
        char name[16];
    } ContentHeader;

protected:
    void decrypt(char *buffer, unsigned int bufferLen, uint32_t key1, uint32_t key2);

    FileHeader mHeader;
    uint32_t mKey1;
    uint32_t mKey2;
};

void DataFormatDTA::setDecryptKeys(uint32_t key1, uint32_t key2)
{
    mKey1 = key1;
    mKey2 = key2;
}
    
void DataFormatDTA::decrypt(char *buffer, unsigned int bufferLen, uint32_t key1, uint32_t key2)
{
    key1 ^= 0x39475694;    // has to be done for some reason
    key2 ^= 0x34985762;

    uint32_t keys[2] = {key1, key2};
    unsigned char *keyBytes = (unsigned char *) keys;

    for (unsigned int i = 0; i < bufferLen; ++i)
    {
        unsigned char dataByte = (unsigned char) buffer[i];
        unsigned char keyByte = keyBytes[i % sizeof(keys)];

        buffer[i] = (char) ( ~((~dataByte) ^ keyByte) );
    }
}

bool DataFormatDTA::load(std::ifstream &srcFile)
{
    int32_t magicNo;
    srcFile.read((char *) &magicNo,4);

    if (magicNo != 0x30445349)     // "ISD0"
      return false;

    srcFile.read((char *) &mHeader,sizeof(FileHeader));

    decrypt((char *) &mHeader,sizeof(mHeader),0xa94b8d3c,0x771f3888);

    std::cout << mHeader.mFileCount << std::endl;
    std::cout << mHeader.mContentOffset << std::endl;
    std::cout << mHeader.mContentSize << std::endl;
    std::cout << mHeader.mUnknown << std::endl;

    return true;
}

bool DataFormatDTA::save(std::ofstream &srcFile)
{
    return true;
}

}
