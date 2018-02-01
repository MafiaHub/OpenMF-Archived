#ifndef KEY_EXTRACTOR_DTA_H
#define KEY_EXTRACTOR_DTA_H

#include <base_parser.hpp>
#include <string.h>
#include <vector>

namespace MFFormat
{

class DataFormatDTAKeyExtrator: public DataFormat
{
public:
    virtual bool load(std::ifstream &srcFile) override; ///< Loads the file table from the DTA file.
    typedef struct
    {
        std::string mFileName;  //name of dta file
        uint32_t mFileKey1;     //key1 wich contains offset from game.exe before replacement
        uint32_t mFileKey2; 
    } DTAFile;

    unsigned int getFileCount()     { return mFetchedFiles.size(); }
    std::vector<DTAFile> getFiles() { return mFetchedFiles; }

private:
    uint32_t firstKeyOffsetVer1 = 0x1FA453;
    uint32_t firstKeyOffsetVer2 = 0x1BF62D;
    uint32_t versionCheckOffset = 0x251430;
    
    std::vector<DTAFile> mFilesToFetchVer1 =
    {
        { "a2.dta", 0x0, 0x0 },
        { "a6.dta", 0x0, 0x0 },
        { "a0.dta", 0x0, 0x0 },
        { "a1.dta", 0x0, 0x0 },
        { "a3.dta", 0x0, 0x0 },
        { "ac.dta", 0x0, 0x0 },
        { "a4.dta", 0x0, 0x0 },
        { "aa.dta", 0x0, 0x0 },
        { "a5.dta", 0x0, 0x0 },
        { "a7.dta", 0x0, 0x0 },
        { "a9.dta", 0x0, 0x0 },
        { "ab.dta", 0x0, 0x0 },
    };

    std::vector<DTAFile> mFilesToFetchVer2 =
    {
        { "a8.dta", 0x0, 0x0 },
        { "b8.dta", 0x0, 0x0 },
        { "a2.dta", 0x0, 0x0 },
        { "a6.dta", 0x0, 0x0 },
        { "a0.dta", 0x0, 0x0 },
        { "a1.dta", 0x0, 0x0 },
        { "a3.dta", 0x0, 0x0 },
        { "ac.dta", 0x0, 0x0 },
        { "a4.dta", 0x0, 0x0 },
        { "aa.dta", 0x0, 0x0 },
        { "a5.dta", 0x0, 0x0 },
        { "a7.dta", 0x0, 0x0 },
        { "a9.dta", 0x0, 0x0 },
        { "ab.dta", 0x0, 0x0 }
    };
    
    std::vector<DTAFile> mFetchedFiles;
    std::vector<DTAFile> fetchExecutableKeys(std::ifstream &srcFile, std::vector<DTAFile> & filesToFetch, uint32_t startOffset);
};

}

#endif
