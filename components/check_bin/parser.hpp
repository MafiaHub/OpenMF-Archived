#ifndef FORMAT_PARSERS_CHECK_BIN_H
#define FORMAT_PARSERS_CHECK_BIN_H

#include <base_parser.hpp>

namespace MFFormat
{

class DataFormatCheckBIN: public DataFormat
{
public:
    typedef enum
    {
        POINTTYPE_PEDESTRIAN = 1,
        POINTTYPE_AI = 2,
        POINTTYPE_TRAINSANDSALINAS = 4,
        POINTTYPE_SALINASTOP = 8,
        POINTTYPE_SPECIALAI = 10
    } PointType;
    
    typedef enum
    {
        LINKTYPE_PEDESTRIAN = 1,
        LINKTYPE_AI = 2,
        LINKTYPE_TRAINSANDSALINAS_FORWARD = 4,
        LINKTYPE_TRAINSANDSALINAS_REVERSE = 0x8400,
        LINKTYPE_OTHER = 0x1000
    } LinkType;

    #pragma pack(push, 1)
    typedef struct 
    {
        Vec3 mPos;              //position of point 
        uint32_t mType;         //type of point 
        uint16_t mRadius;       //radius of transition between points
        float mUnk1;            //unk stuff to be reversed
        uint16_t mUnk2; 
        uint16_t mUnk3;
        uint16_t mUnk4;
        uint8_t mUnk5;
        uint8_t mUnk6;
    } Point;

    typedef struct 
    {
        uint16_t mPointIndex;  //? meybe reference to a point 
        uint16_t mLinkType;    //type of link/joint
        float mDisplacement;   //says in wich way 
    } Link;

    typedef struct
    {
        uint32_t mHeader;
        uint32_t mPointsCount;
    } Header;
    #pragma pack(pop)

    virtual bool load(std::ifstream &srcFile) override;
    inline std::vector<Point> getPoints()   { return mPoints; }
    inline size_t getPointsCount()          { return mPoints.size(); } 
    inline std::vector<Link> getLinks()     { return mLinks; }
    inline size_t getLinksCount()           { return mLinks.size(); }
private:
    std::vector<Point> mPoints;
    std::vector<Link>  mLinks;
};

bool DataFormatCheckBIN::load(std::ifstream &srcFile) 
{
    Header newHeader = {};
    read(srcFile, &newHeader);

    //NOTE(DavoSK): read sector for points 
    //sectors are aligned straight after each other
    for(uint32_t i = 0; i < newHeader.mPointsCount; i++)
    {
        Point newPoint = {};
        read(srcFile, &newPoint);
        mPoints.push_back(newPoint);
    }

    //NOTE(DavoSK): read links
    for(uint32_t i = 0; i < newHeader.mPointsCount; i++)
    {
        Link newLink = {};
        read(srcFile, &newLink);
        mLinks.push_back(newLink);
    }

    return true;
}

}

#endif
