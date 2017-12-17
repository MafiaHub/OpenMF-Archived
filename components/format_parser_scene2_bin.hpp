#ifndef FORMAT_PARSERS_SCENE2_BIN_H
#define FORMAT_PARSERS_SCENE2_BIN_H

#include <algorithm>
#include <cstring>

#include <format_parsers.hpp>
#include <logger_console.hpp>

namespace MFFormat
{

bool DataFormatScene2BIN::load(std::ifstream &srcFile)
{
    Node new_node = {};
    read(srcFile, &new_node);
    uint32_t position = 6;

    while(position + 6 < new_node.mSize)
    {
        srcFile.seekg(position, srcFile.beg);
        Node current_node = {};
        read(srcFile, &current_node);
        readNode(srcFile, &current_node, position + 6);
        position += current_node.mSize;
    }

    return true;
}

void DataFormatScene2BIN::readNode(std::ifstream &srcFile, Node* node, uint32_t offset)
{
    switch(node->mType)
    {
        case NODE_WORLD:
        {
            uint32_t position = offset;
            while(position + 6 < offset + node->mSize)
            {
                Node next_node = {};
                srcFile.seekg(position, srcFile.beg);
                read(srcFile, &next_node);
                readNode(srcFile, &next_node, position + 6);
                position += next_node.mSize;
            }
        }
        break;

        case NODE_VIEW_DISTANCE:
        {
            read(srcFile, &mViewDistance);
        } 
        break;

        case NODE_CLIPPING_PLANES:
        {
            read(srcFile, &mClippingPlanes);
        }
        break;

        case NODE_FOV:
        {
            read(srcFile, &mFov);
        } 
        break;

        case NODE_OBJECT:
        {
            uint32_t position = offset;
            Object new_object = {};
            while(position + 6 < offset + node->mSize)
            {
                Node next_node = {};
                srcFile.seekg(position, srcFile.beg);
                read(srcFile, &next_node);
                readObject(srcFile, &next_node, &new_object);
                position += next_node.mSize;
            }

            mObjects.insert(make_pair(new_object.mName, new_object));
        } 
        break;
    }
}

void DataFormatScene2BIN::readObject(std::ifstream &srcFile, Node* node, Object* object)
{
    switch(node->mType)
    {
        case OBJECT_TYPE:
        {
            read(srcFile, &object->mType);
        }
        break;

        case OBJECT_NAME:
        {
            char *name = reinterpret_cast<char*>(malloc(node->mSize + 1));
            read(srcFile, name, node->mSize - 1);
            name[node->mSize] = '\0';

            object->mName = std::string(name);
        }
        break;

        case OBJECT_MODEL:
        {
            char *model_name = reinterpret_cast<char*>(malloc(node->mSize + 1));
            read(srcFile, model_name, node->mSize);
            model_name[strlen(model_name) - 4] = '\0';
            sprintf(model_name, "%s.4ds", model_name);
            model_name[node->mSize] = '\0';

            object->mModelName = std::string(model_name);
        }
        break;

        case OBJECT_POSITION:
        {
            Vec3 new_position = {};
            read(srcFile, &new_position);
            object->mPos = new_position;
        } 
        break;

        case OBJECT_ROTATION:
        {
            Quat new_rotation = {};
            read(srcFile, &new_rotation);
            object->mRot = new_rotation;
        } 
        break;

        case OBJECT_POSITION_2:
        {
            Vec3 new_position = {};
            read(srcFile, &new_position);
            object->mPos2 = new_position;
        } 
        break;

        case OBJECT_SCALE:
        {
            Vec3 new_scale = {};
            read(srcFile, &new_scale);
            object->mScale = new_scale;
        } 
        break;

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
            read(srcFile, object->mLightSectors, node->mSize);
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

        case OBJECT_PARENT:
        {
            Node parent_node = {};
            read(srcFile, &parent_node);
            Object parent_object = {};
            readObject(srcFile, &parent_node, &parent_object);

            object->mParentName = parent_object.mName;
        }
        break;
    }
}

size_t  DataFormatScene2BIN::getNumObjects()
{
    return mObjects.size();
}

DataFormatScene2BIN::Object* DataFormatScene2BIN::getObject(std::string name)
{
    return &mObjects.at(name);
}

std::unordered_map<std::string, DataFormatScene2BIN::Object> DataFormatScene2BIN::getObjects()
{
    return mObjects;
}

float DataFormatScene2BIN::getFov()
{
    return mFov;
}

void DataFormatScene2BIN::setFov(float value)
{
    mFov = value;
}

float DataFormatScene2BIN::getViewDistance()
{
    return mViewDistance;
}

void DataFormatScene2BIN::setViewDistance(float value)
{
    mViewDistance = value;
}

DataFormat::Vec2 DataFormatScene2BIN::getClippingPlanes()
{
    return mClippingPlanes;
}

void DataFormatScene2BIN::setClippingPlanes(DataFormat::Vec2 value)
{
    mClippingPlanes = value;
}

}

#endif