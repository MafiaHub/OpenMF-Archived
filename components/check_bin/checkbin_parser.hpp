#ifndef FORMAT_PARSERS_CHECK_BIN_H
#define FORMAT_PARSERS_CHECK_BIN_H

#include <base_parser.hpp>

namespace MFFormat
{

class DataFormatCheckBIN: public DataFormat
{
public:
    typedef enum {
        POINTTYPE_PEDESTRIAN = 0x1,
        POINTTYPE_AI = 0x2,
        POINTTYPE_VEHICLE = 0x4,
        POINTTYPE_TRAM_STATION = 0x8,
        POINTTYPE_SPECIAL = 0x10,
    } PointType;

    typedef enum
    {
        LINKTYPE_PEDESTRIAN = 1,
        LINKTYPE_AI = 2,
        LINKTYPE_TRAINSANDSALINAS_FORWARD = 4,
        LINKTYPE_TRAINSANDSALINAS_REVERSE = 0x8400,
        LINKTYPE_OTHER = 0x1000
    } LinkType;

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
    #pragma pack(pop)

    virtual bool load(std::ifstream &srcFile) override;
    inline std::vector<Point> getPoints()     { return mPoints; }
    inline size_t getNumPoints()              { return mPoints.size(); }
    inline std::vector<Link> getLinks()       { return mLinks; }
    inline size_t getNumLinks()               { return mLinks.size(); }

private:
    std::vector<Point> mPoints;
    std::vector<Link> mLinks;
};

}

#endif
