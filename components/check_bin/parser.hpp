#ifndef FORMAT_PARSERS_CHECK_BIN_H
#define FORMAT_PARSERS_CHECK_BIN_H

#include <base_parser.hpp>

namespace MFFormat
{

class DataFormatCheckBIN: public DataFormat
{
public:
    typedef enum {
        PEDESTRIANTS_PATH_POINT = 0x1,
        NAVIGATION_POINT = 0x2,
        VEHICLE_POINT = 0x4,
        TRAM_STATION = 0x8,
        SPECIAL_POINT = 0x10,
    } PointType;

    #pragma pack(push,1)
    typedef struct
    {
        // should be 0x1ABCEDF
        uint32_t mMagic;
        uint32_t mNumPoints;
    } Header;

    typedef struct
    {
        MFMath::Vec3 mPos;
        uint16_t mType;
        uint16_t mID;
        uint16_t mAreaSize;
        uint8_t mUnk[10];
        uint8_t mEnterLinks;
        uint8_t mExitLinks;
    } Point;
    #pragma pop()

    inline std::vector<Point> getPoints()     { return mPoints; }
    inline size_t getNumPoints()              { return mPoints.size(); }

private:
    std::vector<Point> mPoints;
};

bool DataFormatCheckBIN::load(std::ifstream &srcFile)
{
    Header header = {};
    uint32_t magic = read(srcFile, &header);

    if (header.mMagic != 0x1ABCEDF)
    {
        return false;
    }

    for (int i = 0; i < header.mNumPoints; i++)
    {
        Point point = {};
        read(srcFile, &point);

        mPoints.push_back(point);
    }
}

}

#endif
