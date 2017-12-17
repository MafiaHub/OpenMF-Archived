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
        Vec3 mPos;
        Quat mRot;
        Vec3 mScale;
        uint32_t mUnk0;

        // NOTE(zaklaus): This field is rarely used for some instances
        // around the docks or right next to Central Hoboken station.
        Vec3 mScale2;
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
    std::vector<Object> getObjects();
    size_t              getNumObjects();
    Object*             getObject(size_t index);
private:
    std::vector<Object> mObjects;
};

bool DataFormatCacheBIN::load(std::ifstream &srcFile) 
{
    Header new_header = {};
    read(srcFile, &new_header);

    Chunk new_chunk = {};
    read(srcFile, &new_chunk.mVersion);

    while(srcFile.tellg() < new_header.mSize - sizeof(uint32_t)) 
    {
        Object new_object = {};
        uint32_t object_name_length, model_name_length;
        
        read(srcFile, &new_object.mHeader);
        read(srcFile, &object_name_length);
        char *object_name = reinterpret_cast<char*>(malloc(object_name_length + 1));
        read(srcFile, object_name, object_name_length);
        object_name[object_name_length] = '\0';
        new_object.mObjectName = std::string(object_name);

        read(srcFile, new_object.mBounds, 0x4C);

        size_t current_pos = srcFile.tellg();
        size_t header_size = sizeof(Header) + sizeof(uint32_t) + object_name_length + 0x4C;
        while(srcFile.tellg() < current_pos + new_object.mHeader.mSize - header_size)
        {
            Instance new_instance = {};

            read(srcFile, &new_instance.mHeader);

            read(srcFile, &model_name_length);
            char *model_name = reinterpret_cast<char*>(malloc(model_name_length + 1));
            read(srcFile, model_name, model_name_length);
            model_name[model_name_length - 4] = '\0';
            sprintf(model_name, "%s.4ds", model_name);
            model_name[model_name_length] = '\0';
            new_instance.mModelName = std::string(model_name);

            read(srcFile, &new_instance.mPos);
            read(srcFile, &new_instance.mRot);
            read(srcFile, &new_instance.mScale);
            read(srcFile, &new_instance.mUnk0);
            read(srcFile, &new_instance.mScale2);
            new_object.mInstances.push_back(new_instance);
        }

        mObjects.push_back(new_object);
    }

    return true;
}

std::vector<DataFormatCacheBIN::Object> DataFormatCacheBIN::getObjects()
{
    return mObjects;
}
 
size_t DataFormatCacheBIN::getNumObjects()
{
    return mObjects.size();
}
 
DataFormatCacheBIN::Object* DataFormatCacheBIN::getObject(size_t index) 
{
    return &mObjects.at(index);
}

}

#endif
