#include <cache_bin/parser_cachebin.hpp>

namespace MFFormat
{

bool DataFormatCacheBIN::load(std::ifstream &srcFile) 
{
    Header newHeader = {};
    read(srcFile, &newHeader);

    Chunk newChunk = {};
    read(srcFile, &newChunk.mVersion);

    while((uint32_t)srcFile.tellg() < newHeader.mSize - sizeof(uint32_t)) 
    {
        Object newObject = {};
        uint32_t objectNameLength, modelNameLength;
        
        read(srcFile, &newObject.mHeader);
        read(srcFile, &objectNameLength);
        char *objectName = reinterpret_cast<char*>(malloc(objectNameLength + 1));
        read(srcFile, objectName, objectNameLength);
        objectName[objectNameLength] = '\0';
        newObject.mObjectName = std::string(objectName);
        free(objectName);

        read(srcFile, newObject.mBounds, 0x4C);

        size_t current_pos = srcFile.tellg();
        size_t header_size = sizeof(Header) + sizeof(uint32_t) + objectNameLength + 0x4C;
        while((uint32_t)srcFile.tellg() < current_pos + newObject.mHeader.mSize - header_size)
        {
            Instance newInstance = {};

            read(srcFile, &newInstance.mHeader);

            read(srcFile, &modelNameLength);
            char *modelName = reinterpret_cast<char*>(malloc(modelNameLength + 1));
            read(srcFile, modelName, modelNameLength);
            modelName[modelNameLength - 4] = '\0';
            sprintf(modelName, "%s.4ds", modelName);
            modelName[modelNameLength] = '\0';
            newInstance.mModelName = std::string(modelName);
            free(modelName);

            read(srcFile, &newInstance.mPos);

            read(srcFile, &newInstance.mRot);
            newInstance.mRot.fromMafia();

            read(srcFile, &newInstance.mScale);
            read(srcFile, &newInstance.mUnk0);
            read(srcFile, &newInstance.mScale2);
            newObject.mInstances.push_back(newInstance);
        }

        mObjects.push_back(newObject);
    }

    return true;
}

}
