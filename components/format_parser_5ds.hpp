#ifndef FORMAT_PARSERS_5DS_H
#define FORMAT_PARSERS_5DS_H

#include <format_parsers.hpp>

namespace MFFormat
{

void DataFormat5DS::AnimationSequence::setNumberOfSequences(uint16_t numberOfSequences)
{
    mNumberOfSequences = numberOfSequences;    
}

void DataFormat5DS::AnimationSequence::setType(uint16_t type)
{
    mType = static_cast<TypeOfSequence>(type); 
}

void DataFormat5DS::AnimationSequence::setName(const std::string& str)
{
	mObjectName = str;
}

void DataFormat5DS::AnimationSequence::addTimestamp(uint32_t time)
{
    mTimestamps.push_back(time);
}

void DataFormat5DS::AnimationSequence::addMovement(DataFormat5DS::Vec3& data)
{
    mMovements.push_back(data);
}

void DataFormat5DS::AnimationSequence::addRotation(DataFormat5DS::Vec3& data)
{
    mRotations.push_back(data);
}

void DataFormat5DS::AnimationSequence::addScale(DataFormat5DS::Vec3& data)
{
    mScale.push_back(data);
}

const std::string DataFormat5DS::AnimationSequence::getName()
{
    return mObjectName;
}

uint16_t DataFormat5DS::AnimationSequence::getCount() const
{
    return mNumberOfSequences;
}

const uint32_t& DataFormat5DS::AnimationSequence::getTimestamp(uint16_t id) const
{ 
    return mTimestamps[id];
}

const DataFormat5DS::Vec3& DataFormat5DS::AnimationSequence::getMovement(uint16_t id) const
{ 
    return mMovements[id];
}

const DataFormat5DS::Vec3& DataFormat5DS::AnimationSequence::getRotation(uint16_t id) const
{ 
    return mRotations[id];
}

const DataFormat5DS::Vec3& DataFormat5DS::AnimationSequence::getScale(uint16_t id) const 
{ 
    return mScale[id];
}

bool DataFormat5DS::AnimationSequence::hasMovement() const
{ 
    return (mType & SEQUENCE_MOVEMENT);
}

bool DataFormat5DS::AnimationSequence::hasRotation() const
{ 
    return (mType & SEQUENCE_ROTATION);
}

bool DataFormat5DS::AnimationSequence::hasScale() const
{ 
    return (mType & SEQUENCE_SCALE);
}

void DataFormat5DS::addAnimatedObject(DataFormat5DS::AnimationSequence& seq)
{
    mSequences.push_back(seq); 
}

bool DataFormat5DS::parseAnimationSequence(std::ifstream& inputFile, uint32_t pointerData, uint32_t pointerName, AnimationSequence& result)
{
    //seek to destination
    inputFile.seekg(pointerData);

    // read block type; 
    uint32_t typeOfBlock;
    read(inputFile, &typeOfBlock);
    result.setType(typeOfBlock);

    uint16_t animationCount;
    read(inputFile, &animationCount);
    result.setNumberOfSequences(animationCount);

    uint16_t timeFrame;
    uint16_t timeFrameCountMax = (animationCount % 2 == 0)?animationCount+1: animationCount;
    for(uint32_t timeFrameCount = 0; timeFrameCount < timeFrameCountMax; timeFrameCount++)
    {
        read(inputFile, &timeFrame);
        
        // Add frame time
        result.addTimestamp(timeFrame);
    }

    // Read moving data
    for(uint16_t dataIndex = 0; dataIndex < animationCount; dataIndex++)
    {
        if(typeOfBlock & SEQUENCE_MOVEMENT)
        {
            // read movement
            Vec3 movementData;
            read(inputFile, &movementData);
            result.addMovement(movementData);

        }
        if(typeOfBlock & SEQUENCE_ROTATION)
        {
            // read movement
            Vec3 rotationData;
            read(inputFile, &rotationData);
            result.addRotation(rotationData);

        }
        if(typeOfBlock & SEQUENCE_SCALE)
        {
            // read movement
            Vec3 scaleData;
            read(inputFile, &scaleData);
            result.addScale(scaleData);
        }
    }
    
    // get string
    inputFile.seekg(pointerName); 

    std::string objectName;
    char objectNameChar;
    inputFile.get(objectNameChar);
    while(objectNameChar)
    {
        objectName.push_back(objectNameChar);
        inputFile.get(objectNameChar);
    }
    
    result.setName(objectName); 
    return true;
}

bool DataFormat5DS::load(std::ifstream &srcFile) 
{
    Header new_header = {};
    read(srcFile, &new_header);

    if(new_header.mMagicByte != 0x00534435)
    {
        //NOTE(DavoSK): add event handler 
        return false;
    }

    auto begginingOfData = srcFile.tellg();
    Description new_desc = {};
    read(srcFile, &new_desc);
    mTotalFrameCount = new_desc.mOverallCountOfFrames;
    PointerTable new_pointer_table = {};

    for(unsigned int i = 0; i < new_desc.mNumberOfAnimatedObjects; i++)
    {
        read(srcFile, &new_pointer_table);
        auto nextBlock = srcFile.tellg();

        uint32_t pointerToName = static_cast<uint32_t>(begginingOfData + new_pointer_table.mPointerToString);
        uint32_t pointerToData = static_cast<uint32_t>(begginingOfData + new_pointer_table.mPointerToData);

        AnimationSequence new_sequence = {};
        parseAnimationSequence(srcFile, pointerToData, pointerToName, new_sequence);
        addAnimatedObject(new_sequence);
    
        srcFile.seekg(nextBlock);
    }
}

const DataFormat5DS::AnimationSequence& DataFormat5DS::getSequence(unsigned int id) const
{
    return mSequences[id];
}

unsigned int DataFormat5DS::getTotalFrameCount() const
{
    return mTotalFrameCount;
}

}

#endif
