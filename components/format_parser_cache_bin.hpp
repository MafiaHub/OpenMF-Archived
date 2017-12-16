#ifndef FORMAT_PARSERS_CACHE_BIN_H
#define FORMAT_PARSERS_CACHE_BIN_H

#include <format_parsers.hpp>

namespace MFFormat
{

bool DataFormatCacheBIN::load(std::ifstream &srcFile) 
{
    Header new_header = {};
    read(srcFile, &new_header);

    Chunk new_chunk = {};
    read(srcFile, &new_chunk.mVersion);

    while(srcFile.tellg() < new_header.mSize - sizeof(uint32_t)) 
    {
        Object new_object = {};
        read(srcFile, &new_object.mHeader);
        read(srcFile, &new_object.mObjectNameLength);
        new_object.mObjectName = reinterpret_cast<char*>(malloc(new_object.mObjectNameLength + 1));
        read(srcFile, new_object.mObjectName, new_object.mObjectNameLength);
        new_object.mObjectName[new_object.mObjectNameLength] = '\0';
        read(srcFile, new_object.mBounds, 0x4C);

        size_t current_pos = srcFile.tellg();
        size_t header_size = sizeof(Header) + sizeof(uint32_t) + new_object.mObjectNameLength + 0x4C;
        while(srcFile.tellg() < current_pos + new_object.mHeader.mSize - header_size)
        {
            Instance new_instance = {};

            read(srcFile, &new_instance.mHeader);
            read(srcFile, &new_instance.mModelNameLength);

            new_instance.mModelName = reinterpret_cast<char*>(malloc(new_instance.mModelNameLength + 1));
            read(srcFile, new_instance.mModelName, new_instance.mModelNameLength);
            new_instance.mModelName[new_instance.mModelNameLength - 4] = '\0';

            sprintf(new_instance.mModelName, "%s.4ds", new_instance.mModelName);
            new_instance.mModelName[new_instance.mModelNameLength] = '\0';

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
