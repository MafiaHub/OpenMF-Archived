#ifndef FORMAT_PARSERS_5DS_H
#define FORMAT_PARSERS_5DS_H

#include <base_parser.hpp>
#include <math.hpp>

namespace MFFormat
{

class DataFormat5DS: public DataFormat
{
public:
    virtual bool load(std::ifstream &srcFile) override;

    typedef enum
    {
        SEQUENCE_MOVEMENT = 0x2,
        SEQUENCE_ROTATION = 0x4,
        SEQUENCE_SCALE = 0x8
    } TypeOfSequence;

    #pragma pack(push,1)
    typedef struct
    {
        // should be "5DS\0" 
        uint32_t mMagicByte; 
        // should be 0x14
        uint16_t mAnimationType;
        uint32_t mUnk1;
        uint32_t mUnk2;
        uint32_t mLengthOfAnimationData;
    } Header;
   
    typedef struct
    {
        uint16_t mNumberOfAnimatedObjects;
        // Note: 25 frames = 1 seconds
        uint16_t mOverallCountOfFrames;
    } Description;
    
    typedef struct
    {
        uint32_t mPointerToString;
        uint32_t mPointerToData;
    } PointerTable;

    #pragma pop()

    class AnimationSequence
    {
    public:
        void setNumberOfSequences(uint16_t numberOfSequences);
        void setType(uint16_t type);
        void setName(const std::string& str);
        void addTimestamp(uint32_t time);
        void addMovement(MFMath::Vec3& data);
        void addRotation(MFMath::Vec3& data);
        void addScale(MFMath::Vec3& data);
        const std::string getName();
        uint16_t getCount() const;
        const uint32_t& getTimestamp(uint16_t id) const;
        const MFMath::Vec3& getMovement(uint16_t id) const;
        const MFMath::Vec3& getRotation(uint16_t id) const;
        const MFMath::Vec3& getScale(uint16_t id) const;
        bool hasMovement() const;
        bool hasRotation() const;
        bool hasScale() const;
    private:
        std::string mObjectName;
        std::vector<uint32_t> mTimestamps;
        std::vector<MFMath::Vec3> mMovements;
        std::vector<MFMath::Vec3> mRotations;
        std::vector<MFMath::Vec3> mScale;
        uint16_t mNumberOfSequences;
        TypeOfSequence mType;
    };

    const AnimationSequence& getSequence(unsigned int id) const;
    unsigned int getTotalFrameCount() const;
private:
    void addAnimatedObject(AnimationSequence& seq);
    bool parseAnimationSequence(std::ifstream& inputFile, uint32_t pointerData, uint32_t pointerName, AnimationSequence& result);
    std::vector<AnimationSequence> mSequences;
    unsigned int mTotalFrameCount;
};

}

#endif
