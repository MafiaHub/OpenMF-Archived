#ifndef FORMAT_PARSERS_SCENE2_BIN_H
#define FORMAT_PARSERS_SCENE2_BIN_H

#include <cstring>
#include <math.hpp>
#include <base_parser.hpp>

#include <stdio.h>
#include <iostream>

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
       HEADER_ENTITIES = 0xAE20,
       HEADER_INIT = 0xAE50,
        // WORLD subHeader
       HEADER_OBJECT = 0x4010
    } HeaderType;

    typedef enum {
        OBJECT_TYPE = 0x4011,
        OBJECT_POSITION = 0x0020,
        OBJECT_ROTATION = 0x0022,
        OBJECT_POSITION_2 = 0x002C,
        OBJECT_SCALE = 0x002D,
        OBJECT_PARENT = 0x4020,
        OBJECT_NAME = 0x0010, 
        OBJECT_MODEL = 0x2012,
        OBJECT_LIGHT_MAIN = 0x4040,
        OBJECT_LIGHT_TYPE = 0x4041,
        OBJECT_LIGHT_COLOUR = 0x0026,
        OBJECT_LIGHT_POWER = 0x4042,
        OBJECT_LIGHT_UNK_1 = 0x4043,
        OBJECT_LIGHT_RANGE = 0x4044,
        OBJECT_LIGHT_FLAGS = 0x4045,
        OBJECT_LIGHT_SECTOR= 0x4046,
        OBJECT_LIGHT_MAP= 0x40A0
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
    } Object;

    typedef enum {
        LIGHTMAP_VERTEX = 0x5,
        LIGHTMAP_MAP = 0x6
    } LightmapType;

    #pragma pack(push, 1)
    typedef struct {
        uint8_t mLightmapVersion;   
        uint8_t mTypeOfLightmap;              
        uint32_t mNuberOfParts;     
        float mUnkA;             
        float mUnkB;             
        uint8_t mLevelId;        
    } LightmapGeneralData;
    #pragma pack(pop)

    virtual bool load(std::ifstream &srcFile);
    
    inline size_t getNumObjects()                               { return mObjects.size(); }
    inline Object* getObject(std::string name)                  { return &mObjects.at(name); }
    inline std::unordered_map<std::string, Object> getObjects() { return mObjects; }
    inline float getFov()                                       { return mFov; }
    inline void setFov(float value)                             { mFov = value; }
    inline float getViewDistance()                              { return mViewDistance; }
    inline void setViewDistance(float value)                    { mViewDistance = value; }
    inline MFMath::Vec2  getClippingPlanes()                            { return mClippingPlanes; }
    inline void  setClippingPlanes(MFMath::Vec2 value)                  { mClippingPlanes = value; }
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

std::string DataFormatScene2BIN::lightTypeToStr(LightType t)
{
    switch (t)
    {
        case MFFormat::DataFormatScene2BIN::LIGHT_TYPE_POINT: return "point"; break;
        case MFFormat::DataFormatScene2BIN::LIGHT_TYPE_DIRECTIONAL: return "directional"; break;
        case MFFormat::DataFormatScene2BIN::LIGHT_TYPE_AMBIENT: return "ambient"; break;
        case MFFormat::DataFormatScene2BIN::LIGHT_TYPE_FOG: return "fog"; break;
        case MFFormat::DataFormatScene2BIN::LIGHT_TYPE_POINT_AMBIENT: return "point ambient"; break;
        case MFFormat::DataFormatScene2BIN::LIGHT_TYPE_LAYERED_FOG: return "layered fog"; break;
        default: break;
    }

    return "unknown";
}

bool DataFormatScene2BIN::load(std::ifstream &srcFile)
{
    Header newHeader = {};
    read(srcFile, &newHeader);
    uint32_t position = 6;

    while(position + 6 < newHeader.mSize)
    {
        srcFile.seekg(position, srcFile.beg);
        Header nextHeader = {};
        read(srcFile, &nextHeader);
        readHeader(srcFile, &nextHeader, position + 6);
        position += nextHeader.mSize;
    }

    return true;
}

void DataFormatScene2BIN::readHeader(std::ifstream &srcFile, Header* header, uint32_t offset)
{
    switch(header->mType)
    {
        case HEADER_WORLD:
        {
            uint32_t position = offset;
            while(position + 6 < offset + header->mSize)
            {
                Header nextHeader = {};
                srcFile.seekg(position, srcFile.beg);
                read(srcFile, &nextHeader);
                readHeader(srcFile, &nextHeader, position + 6);
                position += nextHeader.mSize;
            }
        }
        break;

        case HEADER_VIEW_DISTANCE:
        {
            read(srcFile, &mViewDistance);
        } 
        break;

        case HEADER_CLIPPING_PLANES:
        {
            read(srcFile, &mClippingPlanes);
        }
        break;

        case HEADER_FOV:
        {
            read(srcFile, &mFov);
        } 
        break;

        case HEADER_OBJECT:
        {
            uint32_t position = offset;
            Object newObject = {};
            while(position + 6 < offset + header->mSize)
            {
                Header nextHeader = {};
                srcFile.seekg(position, srcFile.beg);
                read(srcFile, &nextHeader);
                readObject(srcFile, &nextHeader, &newObject, position + 6);
                position += nextHeader.mSize;
            }

            mObjects.insert(make_pair(newObject.mName, newObject));
        } 
        break;
    }
}

void DataFormatScene2BIN::readObject(std::ifstream &srcFile, Header* header, Object* object, uint32_t offset)
{
    switch(header->mType)
    {
        case OBJECT_TYPE:
        {
            read(srcFile, &object->mType);
        }
        break;

        case OBJECT_NAME:
        {
            char *name = reinterpret_cast<char*>(malloc(header->mSize + 1));
            read(srcFile, name, header->mSize - 1);
            name[header->mSize] = '\0';

            object->mName = std::string(name);
            free(name);
        }
        break;

        case OBJECT_MODEL:
        {
            char *modelName = reinterpret_cast<char*>(malloc(header->mSize + 1));
            read(srcFile, modelName, header->mSize);
            modelName[strlen(modelName) - 4] = '\0';
            sprintf(modelName, "%s.4ds", modelName);
            modelName[header->mSize] = '\0';

            object->mModelName = std::string(modelName);
            free(modelName);
        }
        break;

        case OBJECT_POSITION:
        {
            MFMath::Vec3 newPosition = {};
            read(srcFile, &newPosition);
            object->mPos = newPosition;
        } 
        break;

        case OBJECT_ROTATION:
        {
            MFMath::Quat newRotation = {};
            read(srcFile, &newRotation);
            newRotation.fromMafia();
            object->mRot = newRotation;
        } 
        break;

        case OBJECT_POSITION_2:
        {
            MFMath::Vec3 newPosition = {};
            read(srcFile, &newPosition);
            object->mPos2 = newPosition;
        } 
        break;

        case OBJECT_SCALE:
        {
            MFMath::Vec3 newScale = {};
            read(srcFile, &newScale);
            object->mScale = newScale;
        } 
        break;

        case OBJECT_LIGHT_MAIN:
        {
            uint32_t position = offset;
            while (position + 6 < offset + header->mSize)
            {
                Header lightHeader = {};
                read(srcFile, &lightHeader);
                readLight(srcFile, &lightHeader, object);
                position += lightHeader.mSize;
            }
        }
        break;

        case OBJECT_PARENT:
        {
            Header parentHeader = {};
            read(srcFile, &parentHeader);
            Object parentObject = {};
            readObject(srcFile, &parentHeader, &parentObject, offset + 6);

            object->mParentName = parentObject.mName;
        }
        break;
        case OBJECT_LIGHT_MAP:
        {
            uint8_t bitmapOfLevelOfDetails;
            read(srcFile, &bitmapOfLevelOfDetails);

            // if no lightmap is present for this object
            if(bitmapOfLevelOfDetails == 0)
                break;

            unsigned char countOfLightmaps = 0;
            // count the number of lightmap levels
            uint8_t temporaryBitmap = bitmapOfLevelOfDetails;
            std::cout << "Bitmap: ";
            while(temporaryBitmap!= 0)
            {
                std::cout << (int) (temporaryBitmap & 1);
                if((temporaryBitmap & 1) == 1)
                    countOfLightmaps++;
                temporaryBitmap >>=1;
            }
            std::cout << "\n";

            std::cout << "Count of ligtmaps: " << (int) countOfLightmaps << "\n";
            for(unsigned char i = 0; i < countOfLightmaps; i++)
            {
                // read general data which are common for both BMP / vertex lightmaps
                LightmapGeneralData generalHeader;
                read(srcFile, &generalHeader);
                
                std::cout << "Obj. lightmap starting offset: " <<  srcFile.tellg() << "\n";
                std::cout << "\tGeneral Section:" << "\n";
                std::cout << "\tLightmap type:\t" << (int) generalHeader.mTypeOfLightmap<< "\n";
                std::cout << "\tLightmap no. parts:\t" << (int) generalHeader.mNuberOfParts<< "\n";
                std::cout << "\tLightmap level ID:\t" << (int) generalHeader.mLevelId << "\n";
                std::cout << "\tFloat A:\t" << generalHeader.mUnkA<< "\n";
                std::cout << "\tFloat B:\t" << generalHeader.mUnkB<< "\n";



                for(unsigned part = 0; part < generalHeader.mNuberOfParts; part++)
                {
                    uint16_t unkShit;
                    read(srcFile, &unkShit);

                    switch(generalHeader.mTypeOfLightmap)
                    {


                        case LIGHTMAP_VERTEX:
                        {
                                std::cout << "Obj. lightmap starting offset: " <<  srcFile.tellg() << "\n";
                                uint32_t numberOfVertices;
                                read(srcFile, &numberOfVertices);
                                std::cout << "\tVertices: \t" << numberOfVertices << "\n";
                                

                                
                                uint32_t* arrayofRGBA = new uint32_t[numberOfVertices];
                                read(srcFile, arrayofRGBA, sizeof(uint32_t)*numberOfVertices);
                                //srcFile.read((char*)arrayofRGBA, (sizeof(uint32_t)*numberOfVertices));
                                
                        }
                        break;
                        case LIGHTMAP_MAP:
                        {
                            uint16_t numberOfVertices;
                            read(srcFile, &numberOfVertices);
                            uint16_t numberOfFacets;
                            read(srcFile, &numberOfFacets);

                            std::cout << "\tVerties: \t" << numberOfVertices << "\n";
                            std::cout << "\tFacets: \t" << numberOfFacets<< "\n";

                            uint8_t flagIsDwordPresent;
                            read(srcFile, &flagIsDwordPresent);

                            std::cout << "\tFlag: \t" << (int) flagIsDwordPresent << "\n";

                            uint32_t unkDword;
                            if(flagIsDwordPresent)
                                read(srcFile, &unkDword);


                            uint32_t countOfSomething;
                            read(srcFile, &countOfSomething);

                            std::cout << "\tCount: \t" << (int) countOfSomething<< "\n";


                            for(unsigned s = 0; s < countOfSomething; s++)
                            {
                                std::cout << "Float pos>>: " <<  srcFile.tellg() << "\n";
                                uint32_t sizeA;
                                read(srcFile, &sizeA);
                                uint32_t sizeB;
                                read(srcFile, &sizeB);
                                std::cout << "\tSize: [" << (int) sizeA << "," << sizeB << "]\n";

                                uint32_t sizeOfArray = sizeA*sizeB*3;
                                uint8_t* arrayofRGB = new uint8_t[sizeOfArray];
                                read(srcFile, arrayofRGB, sizeOfArray);

                            }
    

                            
                        }
                        break;
                    }
                }
                return;

            }
            return;
        
            uint16_t numberOfVertices;
            read(srcFile, &numberOfVertices);

            uint16_t numberOfLBMP;
            read(srcFile, &numberOfLBMP);

            uint16_t typeOfFollowing;
            read(srcFile, &typeOfFollowing);


            std::cout << "\t" << "Section2:\n";
            std::cout << "\t\t" << "noVert:\t"<<numberOfVertices<<"\n";
            std::cout << "\t\t" << "noLBMP:\t"<<numberOfLBMP<<"\n";
            std::cout << "\t\t" << "typeFollows:\t"<<typeOfFollowing<<"\n";

            uint8_t hasDword;
            read(srcFile, &hasDword);
			
			
            uint32_t countOfSomething;
            read(srcFile, &countOfSomething);

            uint32_t sizeA;
            read(srcFile, &sizeA);

            uint32_t sizeB;
            read(srcFile, &sizeB);

		
            std::cout << "\t\t" << "Flag:\t"<<(int) hasDword<<"\n";
            std::cout << "\t\t" << "SizeA:\t"<<sizeA<<"\n";
            std::cout << "\t\t" << "SizeB:\t"<<sizeB<<"\n";
            
        }
        break;

    }
}

void DataFormatScene2BIN::readLight(std::ifstream &srcFile, Header* header, Object* object)
{
    switch(header->mType)
    {
        case OBJECT_LIGHT_TYPE:
        {
            read(srcFile, &object->mLightType);
        }
        break;
        
        case OBJECT_LIGHT_COLOUR:
        {
            read(srcFile, &object->mLightColour);
        }
        break;
        
        case OBJECT_LIGHT_POWER:
        {
            read(srcFile, &object->mLightPower);
        }
        break;
        
        case OBJECT_LIGHT_RANGE:
        {
            read(srcFile, &object->mLightNear);
            read(srcFile, &object->mLightFar);
        }
        break;
        
        case OBJECT_LIGHT_SECTOR:
        {
            //read(srcFile, object->mLightSectors, header->mSize);
        }
        break;
        
        case OBJECT_LIGHT_FLAGS:
        {
            read(srcFile, &object->mLightFlags);
        }
        break;

        case OBJECT_LIGHT_UNK_1:
        {
            read(srcFile, &object->mLightUnk0);
            read(srcFile, &object->mLightUnk1);
        }
        break;
    }
}

}

#endif
