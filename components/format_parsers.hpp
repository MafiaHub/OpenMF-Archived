#include <iostream>
#include <cstdint>
#include <vector>
#include <string>

namespace MFformat
{

/// Abstract class representing a game data format.

class DataFormat
{
public:
    virtual bool load(std::ifstream &srcFile)=0;
    virtual bool save(std::ofstream &dstFile) { return false; /* optional */ };

protected:
    std::streamsize fileLength(std::ifstream &f);
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
    virtual bool load(std::ifstream &srcFile) override; ///< Loads the file table from the DTA file.
    void setDecryptKeys(uint32_t key1, uint32_t key2);  ///< Decrypting keys have to be set before load(...) is called.
    unsigned int getNumFiles();                         ///< Get the number of files inside the DTA.
    std::string getFileName(unsigned int index);

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
        uint32_t mDataEnd;
        char mName[16];
    } ContentHeader;

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
    } DataHeader;

protected:
    void decrypt(char *buffer, unsigned int bufferLen, uint32_t key1, uint32_t key2);

    FileHeader mFileHeader;
    std::vector<ContentHeader> mContentHeaders;
    std::vector<DataHeader> mDataHeaders;
    uint32_t mKey1;
    uint32_t mKey2;
};

std::streamsize DataFormat::fileLength(std::ifstream &f)
{
    std::streampos fsize = f.tellg();
    f.seekg( 0, std::ios::end );
    fsize = f.tellg() - fsize;
    f.seekg( 0, std::ios_base::beg );
    return fsize;
}

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
        srcFile.read((char *) &h,sizeof(DataHeader));
        decrypt((char *) &h,sizeof(DataHeader),mKey1,mKey2);
        h.mName[h.mNameLength] = 0;    // terminate the string
        mDataHeaders.push_back(h);
    }

    return true;
}

void DataFormatDTA::setDecryptKeys(uint32_t key1, uint32_t key2)
{
    mKey1 = key1;
    mKey2 = key2;
}

unsigned int DataFormatDTA::getNumFiles()
{
    return mFileHeader.mFileCount;
}

std::string DataFormatDTA::getFileName(unsigned int index)
{
    return std::string((char *) mDataHeaders[index].mName);
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

}
