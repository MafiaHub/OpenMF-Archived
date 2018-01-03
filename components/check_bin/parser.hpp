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

    #pragma pack(push, 1)
    typedef struct 
    {
        Vec3 mPos;
        uint32_t mType;
        uint16_t mAngleAroundY;
        float mUnk1;
        uint16_t mUnk2;
        uint16_t mUnk3;
        uint16_t mUnk4;
        uint8_t mUnk5;
        uint8_t mUnk6;
    } Point;

    typedef struct
    {
        uint32_t mHeader;
        uint32_t mPointsCount;
    } Header;
    #pragma pack(pop)

    virtual bool load(std::ifstream &srcFile) override;
    inline std::vector<Point> getPoints()   { return mPoints; }
    inline size_t getPointsCount()          { return mPoints.size(); } 
private:
    std::vector<Point> mPoints;
};

bool DataFormatCheckBIN::load(std::ifstream &srcFile) 
{
    Header newHeader = {};
    read(srcFile, &newHeader);

    for(uint32_t i = 0; i < newHeader.mPointsCount; i++)
    {
        Point newPoint = {};
        read(srcFile, &newPoint);

        mPoints.push_back(newPoint);
    }
    return true;
}

}

#endif
