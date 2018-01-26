#ifndef FORMAT_PARSERS_CACHE_BIN_H
#define FORMAT_PARSERS_CACHE_BIN_H

#include <base_parser.hpp>

namespace MFFormat
{

class DataFormatCacheBIN: public DataFormat
{
public:
    #pragma pack(push, 1)
    typedef struct
    {
        uint16_t mType;
        uint32_t mSize;
    } Header;
    #pragma pack(pop)

    typedef struct
    {
        Header mHeader;
        std::string mModelName;
        MFMath::Vec3 mPos;
        MFMath::Quat mRot;
        MFMath::Vec3 mScale;
        uint32_t mUnk0;

        // NOTE(zaklaus): This field is rarely used for some instances
        // around the docks or right next to Central Hoboken station.
        MFMath::Vec3 mScale2;
    } Instance;

    typedef struct
    {
        Header mHeader;
        std::string mObjectName;
        int8_t mBounds[0x4C];
        std::vector<Instance> mInstances;
    } Object;

    typedef struct
    {
        uint32_t mVersion; // NOTE(zaklaus): Should always be 1.
    } Chunk;

    virtual bool load(std::ifstream &srcFile) override;

    inline std::vector<Object> getObjects()     { return mObjects; }
    inline size_t getNumObjects()               { return mObjects.size(); }
    inline Object* getObject(size_t index)      { return &mObjects.at(index); }
private:
    std::vector<Object> mObjects;
};

}

#endif
