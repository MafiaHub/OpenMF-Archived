#ifndef FORMAT_PARSERS_EFFECTS_BIN_HPP
#define FORMAT_PARSERS_EFFECTS_BIN_HPP

#include <base_parser.hpp>

namespace MFFormat
{

class DataFormatEffectsBIN : public DataFormat
{
public:
    #pragma pack(push,1)
    typedef struct
    {
        uint16_t mMagic;
        uint32_t mFileSize;
    } Header;

    typedef struct
    {
        uint16_t mSign;
        uint32_t mSize;
        uint8_t mUnk0[48];
        MFMath::Vec3 mPos;
        float mUnk1;
        uint32_t mEffectId; // from effects.tbl
    } Effect;
    #pragma pack(pop)

    virtual bool load(std::ifstream &srcFile) override;
    inline size_t getNumEffects()             { return mEffects.size(); }
    inline std::vector<Effect> getEffects()   { return mEffects; }

private:
    std::vector<Effect> mEffects;
};

}

#endif // FORMAT_PARSERS_EFFECTS_BIN_HPP
