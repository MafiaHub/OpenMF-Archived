#ifndef FORMAT_PARSERS_DTA_H
#define FORMAT_PARSERS_DTA_H

#include <base_parser.hpp>
#include <utils/scoped_buffer.hpp>
#include <string.h>
#include <vector>

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
    std::string getFileName(unsigned int index);
    ScopedBuffer getFile(std::ifstream &srcFile, unsigned int index);   ///< Get the concrete file from within the DST file into a buffer.
    int getFileIndex(std::string fileName);

    void decrypt(char *buffer, unsigned int bufferLen, unsigned int relativeShift=0);
    std::vector<unsigned char> decompressLZSS(unsigned char *buffer, unsigned int bufferLen);
    std::vector<unsigned char> decompressDPCM(uint16_t *delta, unsigned char *buffer, unsigned int bufferLen);

    typedef struct
    {
        uint32_t mFileCount;
        uint32_t mFileTableOffset;
        uint32_t mFileTableSize;
        uint32_t mUnknown;
    } FileHeader;

    typedef struct
    {
        uint16_t mFileNameChecksum;        // checksum of upper file name
        uint16_t mFileNameLength;
        uint32_t mHeaderOffset;            // where DataFileHeader starts
        uint32_t mDataOffset;              // where the data start
        char mNameHint[16];                // file name, often incomplete
    } FileTableRecord;

    typedef struct
    {
        uint32_t mUnknown;
        uint32_t mUnknown2;
        uint64_t mTimeStamp;
        uint32_t mSize;
        uint32_t mCompressedBlockCount;
        unsigned char mNameLength;
        unsigned char mFlags[7];
        unsigned char mName[256];
    } DataFileHeader;

    typedef enum
    {
        BLOCK_UNCOMPRESSED = 0,
        BLOCK_LZSS_RLE     = 1,
        BLOCK_DPCM0        = 8,
        BLOCK_DPCM1        = 12,
        BLOCK_DPCM2        = 16,
        BLOCK_DPCM3        = 20,
        BLOCK_DPCM4        = 24,
        BLOCK_DPCM5        = 28,
        BLOCK_DPCM6        = 32
    } BlockType;

    typedef struct
    {
        uint32_t mChunkId;
        uint32_t mChunkSize;
        uint32_t mType;

        uint32_t mChunkId2;
        uint32_t mChunkSize2;
        uint16_t mTags;
        uint16_t mChannels;
        uint32_t mSamplesPerSecond;
        uint32_t mBytesPerSecond;
        uint16_t mBlockAlign;
        uint16_t mSampleBits;

        uint32_t mChunkId3;
        uint32_t mChunkSize3;
    } WavHeader;

    inline std::vector<FileTableRecord> getFileTableRecords() { return mFileTableRecords; };
    inline std::vector<DataFileHeader>  getDataFileHeaders()  { return mDataFileHeaders;  };

protected:
    FileHeader mFileHeader;
    std::vector<FileTableRecord> mFileTableRecords;
    std::vector<DataFileHeader> mDataFileHeaders;
    uint32_t mKey1;
    uint32_t mKey2;
    bool mWavHeaderRead;
    WavHeader mWavHeader;
};

}

#endif
