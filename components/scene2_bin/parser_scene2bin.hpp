#ifndef FORMAT_PARSERS_SCENE2_BIN_H
#define FORMAT_PARSERS_SCENE2_BIN_H

#include <cstring>
#include <utils/math.hpp>
#include <base_parser.hpp>

namespace MFFormat
{

class DataFormatScene2BIN: public DataFormat
{
public:
    typedef enum {
        // top Headers
       HEADER_MISSION = 0x4c53,
       HEADER_META = 0x0001,
       HEADER_UNK_FILE = 0xAFFF,
       HEADER_UNK_FILE2 = 0x3200,
       HEADER_FOV = 0x3010,
       HEADER_VIEW_DISTANCE = 0x3011,
       HEADER_CLIPPING_PLANES = 0x3211,
       HEADER_WORLD = 0x4000,
       HEADER_SPECIAL_WORLD = 0xAE20,
       HEADER_ENTITIES = 0xAE20,
       HEADER_INIT = 0xAE50,
        // WORLD subHeader
       HEADER_OBJECT = 0x4010,
       HEADER_SPECIAL_OBJECT = 0xAE21,
    } HeaderType;

    typedef enum {
        OBJECT_TYPE_SPECIAL = 0xAE22,
        OBJECT_TYPE_NORMAL = 0x4011,
        OBJECT_POSITION = 0x0020,
        OBJECT_ROTATION = 0x0022,
        OBJECT_POSITION_2 = 0x002C,
        OBJECT_SCALE = 0x002D,
        OBJECT_PARENT = 0x4020,
        OBJECT_NAME = 0x0010, 
        OBJECT_NAME_SPECIAL = 0xAE23,
        OBJECT_MODEL = 0x2012,
        OBJECT_LIGHT_MAIN = 0x4040,
        OBJECT_LIGHT_TYPE = 0x4041,
        OBJECT_LIGHT_COLOUR = 0x0026,
        OBJECT_LIGHT_POWER = 0x4042,
        OBJECT_LIGHT_UNK_1 = 0x4043,
        OBJECT_LIGHT_RANGE = 0x4044,
        OBJECT_LIGHT_FLAGS = 0x4045,
        OBJECT_LIGHT_SECTOR= 0x4046,
        OBJECT_SPECIAL_DATA= 0xAE24,
    } ObjectProperty;

    typedef enum {
        OBJECT_TYPE_LIGHT = 0x02,
        OBJECT_TYPE_CAMERA = 0x03,
        OBJECT_TYPE_SOUND = 0x04,
        OBJECT_TYPE_MODEL = 0x09,
        OBJECT_TYPE_OCCLUDER = 0x0C,
        OBJECT_TYPE_SECTOR = 0x99,
        OBJECT_TYPE_SCRIPT = 0x9B
    } ObjectType;

    typedef enum {
        SPECIAL_OBJECT_TYPE_NONE = 0,
        SPECIAL_OBJECT_TYPE_PHYSICAL = 0x23,
        SPECIAL_OBJECT_TYPE_PLAYER = 0x02,
        SPECIAL_OBJECT_TYPE_CHARACTER = 0x1B,
    } SpecialObjectType;

    typedef enum {
        LIGHT_TYPE_POINT = 0x01,
        LIGHT_TYPE_DIRECTIONAL = 0x03,
        LIGHT_TYPE_AMBIENT = 0x04,
        LIGHT_TYPE_FOG = 0x05,
        LIGHT_TYPE_POINT_AMBIENT = 0x06,
        LIGHT_TYPE_LAYERED_FOG = 0x08,
    } LightType;

    #pragma pack(push, 1)
    typedef struct
    {
        uint16_t mType;
        uint32_t mSize;
    } Header;
    #pragma pack(pop)

    typedef struct _Object
    {
        uint32_t mType;
        uint32_t mSpecialType;
        MFMath::Vec3 mPos;
        MFMath::Quat mRot;
        MFMath::Vec3 mPos2; // precomputed final world transform position
        MFMath::Vec3 mScale;
        std::string mName;
        std::string mModelName;
        std::string mParentName;

        // Light properties
        LightType mLightType;
        MFMath::Vec3 mLightColour;
        int32_t mLightFlags;
        float mLightPower;           // 1.0 = 100% (can be even over 1.0)
        float mLightUnk0;
        float mLightUnk1;
        float mLightNear;
        float mLightFar;
        char mLightSectors[5000];

        struct {
            // Physical object properties
            float mMovVal1;
            float mMovVal2;
            float mFriction;
            float mMovVal4;

            int32_t mMovVal5;
            float mWeight;
            int32_t mSound;
        } mSpecialProps;
    } Object;

    virtual bool load(std::ifstream &srcFile);
    
    inline size_t getNumObjects()                               { return mObjects.size(); }
    inline Object* getObject(std::string name)                  { return &mObjects.at(name); }
    inline std::unordered_map<std::string, Object> getObjects() { return mObjects; }
    inline float getFov()                                       { return mFov; }
    inline void setFov(float value)                             { mFov = value; }
    inline float getViewDistance()                              { return mViewDistance; }
    inline void setViewDistance(float value)                    { mViewDistance = value; }
    inline MFMath::Vec2  getClippingPlanes()                    { return mClippingPlanes; }
    inline void  setClippingPlanes(MFMath::Vec2 value)          { mClippingPlanes = value; }
    static std::string lightTypeToStr(LightType t);

private:
    void readHeader(std::ifstream &srcFile, Header* header, uint32_t offset);
    void readObject(std::ifstream &srcFile, Header* header, Object* object, uint32_t offset);
    void readLight (std::ifstream &srcFile, Header* header, Object* object);
    
    std::unordered_map<std::string, Object> mObjects;
    float mFov;
    float mViewDistance;
    MFMath::Vec2  mClippingPlanes;
};

}

#endif
