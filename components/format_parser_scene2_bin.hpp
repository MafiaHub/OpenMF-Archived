#ifndef FORMAT_PARSERS_SCENE2_BIN_H
#define FORMAT_PARSERS_SCENE2_BIN_H

#include <format_parsers.hpp>

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
            uint32_t position = offset;
            float viewdist = 0;
            read(srcFile, &viewdist);
        } 
        break;

        case NODE_FOV:
        {
            uint32_t position = offset;
            float fov = 0;
            read(srcFile, &fov);
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
            mObjects.push_back(new_object);
        } 
        break;

        default:
        {
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
            object->mName = reinterpret_cast<char*>(malloc(node->mSize + 1));
            read(srcFile, object->mName, node->mSize - 1);
            object->mName[node->mSize] = '\0';
        }
        break;

        case OBJECT_MODEL:
        {
            object->mModelName = reinterpret_cast<char*>(malloc(node->mSize + 1));
            read(srcFile, object->mModelName, node->mSize - 1);
            object->mModelName[node->mSize] = '\0';
        }
        break;

        case OBJECT_POSITION_2:
        {
            Vec3 new_position = {};
            read(srcFile, &new_position);
            object->mPos = new_position;
        } 
        break;
    }
}

size_t  DataFormatScene2BIN::getNumObjects()
{
    return mObjects.size();
}

DataFormatScene2BIN::Object* DataFormatScene2BIN::getObject(size_t index)
{
    return &mObjects.at(index);
}

std::vector<DataFormatScene2BIN::Object>* DataFormatScene2BIN::getObjects()
{
    return &mObjects;
}

}

#endif