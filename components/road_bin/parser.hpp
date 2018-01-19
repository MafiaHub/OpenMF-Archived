#ifndef FORMAT_PARSERS_ROAD_BIN_H
#define FORMAT_PARSERS_ROAD_BIN_H

#include <base_parser.hpp>

namespace MFFormat
{

class DataFormatRoadBIN: public DataFormat
{
public:
    #pragma pack(push,1)    
    typedef struct
    {
        uint8_t mDirectionLink;
        uint8_t mUnk1;
    } WaypointLink;

    typedef struct
    {
        uint16_t mUnk1;
        uint8_t mType;
        uint8_t mUnk2; // 0xCC
        float mDistance;
    } Lane;

    typedef struct
    {
        uint16_t mFarActiveCrossPoint;
        uint16_t mUnk1;
        float mFarCrosspointDistance;
        float mAngle; // radians
        uint8_t mUnk2[2];
        uint32_t mPriority; // right of way
        uint8_t mUnk3[2];
        Lane mLanes[4];
    } DirectionLink;

    typedef struct
    {
        MFMath::Vec3 mPos;
        uint8_t mSemaphore;
        uint8_t mUnk1[3]; // 0xCCCCCC
        float mSpeed; // multiply by 3 to get km/h
        WaypointLink mWaypointLinks[4];
        DirectionLink mDirectionLink[4];
    } Crossroad;

    typedef struct
    {
        MFMath::Vec3 mPos;
        float mSpeed; // multiply by 3 to get km/h
        uint8_t mPrevPoint;
        uint8_t mPrevPointType; // 0x00 - Cross, 0x80 - Way
        uint8_t mNextPoint;
        uint8_t mNextPointType; // 0x00 - Cross, 0x80 - Way
        uint8_t mFarPrevCrosspoint;
        uint8_t mUnk1;
        uint8_t mFarNextCrosspoint;
        uint8_t mUnk2;
    } Waypoint;
    #pragma pack(pop)

    virtual bool load(std::ifstream &srcFile) override;
    inline std::vector<Crossroad> getCrossroads()     { return mCrossroads; }
    inline size_t getNumCrossroads()              { return mCrossroads.size(); }
    inline std::vector<Waypoint> getWaypoints()       { return mWaypoints; }
    inline size_t getNumWaypoints()               { return mWaypoints.size(); }

private:
    std::vector<Crossroad> mCrossroads;
    std::vector<Waypoint> mWaypoints;
};

bool DataFormatRoadBIN::load(std::ifstream &srcFile)
{
    uint32_t header = 0;
    read(srcFile, &header);

    if (header != 0x2)
    {
        return false;
    }

    uint32_t numCrossroads = 0;
    read(srcFile, &numCrossroads);

    for (uint32_t  i = 0; i < numCrossroads; i++)
    {
        Crossroad crossroad = {};
        read(srcFile, &crossroad);

        mCrossroads.push_back(crossroad);
    }

    uint32_t numWaypoints = 0;
    read(srcFile, &numWaypoints);

    for (uint32_t  i = 0; i < numWaypoints; i++)
    {
        Waypoint waypoint = {};
        read(srcFile, &waypoint);

        mWaypoints.push_back(waypoint);
    }

    return true;
}

}

#endif
