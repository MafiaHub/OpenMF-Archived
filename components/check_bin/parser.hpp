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
        uint8_t mExitLinks; // equals mEnterLinks
    } Point;

    typedef struct
    {
        uint16_t mTargetPoint;
        uint16_t mLinkType;
        float mUnk;
    } Link;
    #pragma pop()

    inline std::vector<Point> getPoints()     { return mPoints; }
    inline size_t getNumPoints()              { return mPoints.size(); }
    inline std::vector<Link> getLinks()       { return mLinks; }
    inline size_t getNumLinks()               { return mLinks.size(); }

private:
    std::vector<Point> mPoints;
    std::vector<Link> mLinks;
};

bool DataFormatCheckBIN::load(std::ifstream &srcFile)
{
    Header header = {};
    read(srcFile, &header);

    if (header.mMagic != 0x1ABCEDF)
    {
        return false;
    }

    int numLinks = 0;

    for (int i = 0; i < header.mNumPoints; i++)
    {
        Point point = {};
        read(srcFile, &point);

        numLinks += point.mEnterLinks;

        mPoints.push_back(point);
    }

    // Each point references 0 or more links.
    // For example, if point 0 has mEnterLinks = 2, it means that the first 2 links belong to it.
    // Consequent links in a row belong to point 1, 2 and so on.
    for (int i = 0; i < numLinks; i++)
    {
        Link link = {};
        read(srcFile, &link);

        mLinks.push_back(link);
    }
}

}

#endif
