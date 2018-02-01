#include <dta/key_extractor.hpp>

namespace MFFormat
{

std::vector<DataFormatDTAKeyExtrator::DTAFile> DataFormatDTAKeyExtrator::fetchExecutableKeys(std::ifstream &srcFile, std::vector<DataFormatDTAKeyExtrator::DTAFile> & filesToFetch, uint32_t startOffset)
{
    uint32_t currentFileOffset = startOffset;
    std::vector<DTAFile> toReturn;

    for (auto & dtaFile : filesToFetch)
    {
        uint32_t val;

        //read 4bytes pushed value
        srcFile.seekg(currentFileOffset);
        srcFile.read(reinterpret_cast<char*>(&val), sizeof(uint32_t));
        dtaFile.mFileKey2 = val;

        //skip push instruction
        currentFileOffset += 0x5;

        srcFile.seekg(currentFileOffset);
        srcFile.read(reinterpret_cast<char*>(&val), sizeof(uint32_t));
        dtaFile.mFileKey1 = val;

        //skip instructions to next push
        currentFileOffset += 0x14;

        toReturn.push_back(dtaFile);
    }

    return toReturn;
}

bool DataFormatDTAKeyExtrator::load(std::ifstream &srcFile)
{
    if (srcFile.good())
    {
        char AB_DTA[7] = {};
        srcFile.seekg(versionCheckOffset);
        srcFile.read(AB_DTA, sizeof(char) * 7);

        //Check for 1.0 or 1.2
        //TODO(DavoSK): add check for 1.1 version !
        if (strcmp(AB_DTA, "ab.dta") == 0)
            mFetchedFiles = fetchExecutableKeys(srcFile, mFilesToFetchVer1, firstKeyOffsetVer1);
        else 
            mFetchedFiles = fetchExecutableKeys(srcFile, mFilesToFetchVer2, firstKeyOffsetVer2);

    } else return false;

    return true;
}

}
