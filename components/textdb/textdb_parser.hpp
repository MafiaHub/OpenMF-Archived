#ifndef FORMAT_PARSERS_TEXTDB_DEF_H
#define FORMAT_PARSERS_TEXTDB_DEF_H

#include <base_parser.hpp>
#include <map>

namespace MFFormat
{

class DataFormatTextdbDEF: public DataFormat
{
public:
    #pragma pack(push,1)
    typedef struct
    {
        uint32_t mNumStrings;
        uint32_t mUnk;
    } Header;

    typedef struct
    {
        uint32_t mTextId;
        uint32_t mTextOffset;
    } TextBlock;
    #pragma pack(pop)

    virtual bool load(std::ifstream &srcFile) override;
    inline std::map<uint32_t, std::string> getTextEntries()     { return mTextEntries;        }
    inline size_t getNumTextEntries()                           { return mTextEntries.size(); }

protected:
    std::vector<TextBlock> mTextBlocks;
    std::map<uint32_t, std::string> mTextEntries;
};

}

#endif
