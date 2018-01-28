#ifndef FORMAT_PARSERS_LOAD_DEF_H
#define FORMAT_PARSERS_LOAD_DEF_H

#include <formats/base_parser.hpp>

namespace MFFormat
{

class DataFormatLoadDEF: public DataFormat
{
public:
    #pragma pack(push,1)
    typedef struct
    {
        char mMissionName[32];
        char mFileName[32];
        uint32_t mTextId;
    } LoadingScreen;
    #pragma pack(pop)

    virtual bool load(std::ifstream &srcFile) override;
    inline std::vector<LoadingScreen> getLoadingScreens()   { return mLoadingScreens; }
    inline size_t getNumLoadingScreens()                    { return mLoadingScreens.size(); }

private:
    std::vector<LoadingScreen> mLoadingScreens;
};

}

#endif
