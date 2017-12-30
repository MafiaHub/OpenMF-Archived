#ifndef FORMAT_PARSERS_TREE_KLZ_H
#define FORMAT_PARSERS_TREE_KLZ_H

#include <base_parser.hpp>
#include <utils.hpp>

namespace MFFormat
{

class DataFormatTreeKLZ: public DataFormat
{
public:
    typedef enum 
    {   
        REFERENCE_FACE = 0x00,
        REFERENCE_XTOBB = 0x80,
        REFERENCE_AABB = 0x81,
        REFERENCE_SPHERE = 0x82,
        REFERENCE_OBB = 0x83,
        REFERENCE_CYLINDER = 0x84
    } GridReference;

    typedef struct
    {
        uint32_t mSignature;
        uint32_t mVersion;
        uint32_t mCollisionDataOffset;
        uint32_t mNumLinks;
        uint32_t mUnknown[2];
    } Header;

    typedef struct
    {
        uint32_t mFlags;
        uint32_t mNameLength;
        char* mName;
    } Link;

    #pragma pack(push, 1)
    typedef struct
    {
        float mGridMinX;
        float mGridMinY;
        float mGridMaxX;
        float mGridMaxY;
        float mCellWidth;
        float mCellHeight;
        uint32_t mGridWidth;
        uint32_t mGridHeight;
        uint32_t mUnknown0;
        uint32_t mReserved0[2];
        uint32_t mReserved1;
        uint32_t mNumFaces;
        uint32_t mReserved2;
        uint32_t mNumXTOBBs;
        uint32_t mReserved3;
        uint32_t mNumAAABBs;
        uint32_t mReserved4;
        uint32_t mNumSpheres;
        uint32_t mReserved5;
        uint32_t mNumOBBs;
        uint32_t mReserved6;
        uint32_t mNumCylinders;
        uint32_t mReserved7;
        uint32_t mNumUnknownType;  // always 0
        uint32_t mUnknown1;
    } DataHeader;
    #pragma pack(pop)

    typedef struct
    {
        unsigned char mMaterial;
        unsigned char mFlags;
        unsigned char mSortInfo;   // only used with FaceCol
        unsigned char mUnknown;    // different values for different collision types of objects
    } Properties;

    #pragma pack(push, 1)
    typedef struct 
    {    
        Properties mProperties; // NOTE(ASM): Material (8 bit) | Flags (8 bit) | SortInfo (8 bit) | 0 (8 bit)
        uint32_t mIndices[3];   // NOTE(ASM): (Link (index into LinkNameOffsetTable) (16bit) | Index of vertex in mesh's vertex buffer (16 bit))					// plane the triangle lies in
        Vec3 mNormal;           // NOTE(ASM): needs to point in opposite direction compared to the mesh face normal (IIRC!), i.e. if the mesh face normal is (1 0 0), the col face normal needs to be (-1 0 0)
        float mDistance;
    } FaceCol;

    typedef struct 
    {    
        uint32_t mProperties;   // NOTE(ASM): Material(8 bit) | Flags (8 bit) | 0 (8 bit) | 0x81 (8 bit)
        uint32_t mLink;         // NOTE(ASM): index into LinkNameOffsetTable
        Vec3 mMin;
        Vec3 mMax;
    } ABBCol;

    typedef struct 
    {    
        uint32_t mProperties;   // NOTE(ASM): Material(8 bit) | Flags (8 bit) | 0 (8 bit) | 0x80 (8 bit)
        uint32_t mLink;
        // NOTE(ASM): AABB
        Vec3 mMin;
        Vec3 mMax;
        Vec3 mExtends[2];
        Mat4 mTransform;
        Mat4 mInverseTransform;
    } XTOBBCol;

    typedef struct 
    {    
        int32_t mProperties;    // NOTE(ASM): Material(8 bit) | Flags (8 bit) | 0 (8 bit) | 0x84 (8 bit)
        uint32_t mLink;
        Vec2 mPosition;         // NOTE(ASM): cylinders only have a 2d position!
        float mRadius;
    } CylinderCol;
  
    typedef struct 
    {    
        uint32_t mProperties;   // NOTE(ASM): Material(8 bit) | Flags (8 bit) | 0 (8 bit) | 0x83 (8 bit)
        uint32_t mLink;
        Vec3 Extends[2];
        Mat4 mTransform;
        Mat4 mInverseTransform;    
    } OBBCol;

    typedef struct 
    {    
        uint32_t mProperties;   // NOTE(ASM): Material(8 bit) | Flags (8 bit) | 0 (8 bit) | 0x82 (8 bit)
        uint32_t mLink;
        Vec3 mPosition;
        float mRadius;    
    } SphereCol;

    #pragma pack(pop)

    typedef struct 
    {
        uint32_t mNumObjects;
        uint32_t mReserved[2];
        float mHeight;
        uint32_t* mReferences;  // NOTE(ASM): (Type (8 bit)) | (Offset into array of Type (24 bit)))
        uint8_t* mFlags;
    } Cell;

    virtual bool load(std::ifstream &srcFile) override;
    std::vector<FaceCol> getFaceCols()             { return mFaceCols; }
    std::vector<ABBCol> getABBCols()               { return mABBCols; }
    std::vector<XTOBBCol> getXTOBBCols()           { return mXTOBBCols; }
    std::vector<CylinderCol> getCylinderCols()     { return mCylinderCols; }
    std::vector<OBBCol> getOBBCol()                { return mOBBCols; }
    std::vector<SphereCol> getSphereCols()         { return mSphereCols; }
    std::vector<Link> getLinks()                   { return mLinkTables; }
private:
    Header mHeader;
    uint32_t* mLinkNameOffsetTable;
    std::vector<Link> mLinkTables;
    float* mCellBoundariesX;
    float* mCellBoundariesY;
    DataHeader mDataHeader;
    uint32_t mCollisionDataMagic;
    std::vector<FaceCol> mFaceCols;
    std::vector<ABBCol> mABBCols;
    std::vector<XTOBBCol> mXTOBBCols;
    std::vector<CylinderCol> mCylinderCols;
    std::vector<OBBCol> mOBBCols;
    std::vector<SphereCol> mSphereCols;
    uint32_t mCollisionGridMagic;
    Cell* mGridCellsMemory;
};

bool DataFormatTreeKLZ::load(std::ifstream &srcFile)
{
    read(srcFile, &mHeader);
    
    mLinkNameOffsetTable = reinterpret_cast<uint32_t*>(malloc(sizeof(uint32_t)*mHeader.mNumLinks));
    read(srcFile, mLinkNameOffsetTable, sizeof(uint32_t)*mHeader.mNumLinks);

    for(unsigned int i = 0; i < mHeader.mNumLinks; i++) 
    {
        Link newLink = {};
        srcFile.seekg(mLinkNameOffsetTable[i], srcFile.beg);
        read(srcFile, &newLink.mFlags);
      
        newLink.mNameLength = MFUtil::peekLength(srcFile);
        newLink.mName = reinterpret_cast<char*>(malloc(newLink.mNameLength));
        read(srcFile, newLink.mName, newLink.mNameLength);
        mLinkTables.push_back(newLink);
    }

    srcFile.seekg(mHeader.mCollisionDataOffset, srcFile.beg);
    read(srcFile, &mDataHeader);

    mCellBoundariesX = reinterpret_cast<float*>(malloc(sizeof(float)*(mDataHeader.mGridWidth + 1)));
    read(srcFile, mCellBoundariesX, sizeof(float)* (mDataHeader.mGridWidth + 1));
   
    mCellBoundariesY = reinterpret_cast<float*>(malloc(sizeof(float)*(mDataHeader.mGridHeight + 1)));
    read(srcFile, mCellBoundariesY, sizeof(float)* (mDataHeader.mGridHeight + 1));
    read(srcFile, &mCollisionDataMagic);
  
    for(unsigned int i = 0; i < mDataHeader.mNumFaces; i++)
    {
        FaceCol newCol = {};
        read(srcFile, &newCol);
        mFaceCols.push_back(newCol);
    }

    for(unsigned int i = 0; i < mDataHeader.mNumAAABBs; i++)
    {
        ABBCol newCol = {};
        read(srcFile, &newCol);
        mABBCols.push_back(newCol);
    }

    for(unsigned int i = 0; i < mDataHeader.mNumXTOBBs; i++)
    {
        XTOBBCol newCol = {};
        read(srcFile, &newCol);
        mXTOBBCols.push_back(newCol);
    }

    for(unsigned int i = 0; i < mDataHeader.mNumCylinders; i++)
    {
        CylinderCol newCol = {};
        read(srcFile, &newCol);
        mCylinderCols.push_back(newCol);
    }

    for(unsigned int i = 0; i < mDataHeader.mNumOBBs; i++)
    {
        OBBCol newCol = {};
        read(srcFile, &newCol);
        mOBBCols.push_back(newCol);
    }

    for(unsigned int i = 0; i < mDataHeader.mNumSpheres; i++)
    {
        SphereCol newCol = {};
        read(srcFile, &newCol);
        mSphereCols.push_back(newCol);
    }
   
    read(srcFile, &mCollisionGridMagic);
    uint32_t gridSize = mDataHeader.mGridWidth * mDataHeader.mGridWidth;
    mGridCellsMemory = reinterpret_cast<Cell*>(malloc(sizeof(Cell) * gridSize));

    for(unsigned int i = 0; i < gridSize; i++)
    {
        read(srcFile, &mGridCellsMemory[i].mNumObjects);
        read(srcFile, mGridCellsMemory[i].mReserved, sizeof(uint32_t)*2);
        read(srcFile, &mGridCellsMemory[i].mHeight);

        if(mGridCellsMemory[i].mNumObjects)
        {
            mGridCellsMemory[i].mReferences = reinterpret_cast<uint32_t*>(malloc(sizeof(uint32_t) * mGridCellsMemory[i].mNumObjects));
            read(srcFile, mGridCellsMemory[i].mReferences, sizeof(uint32_t) * mGridCellsMemory[i].mNumObjects);

            // NOTE(ASM): needs to be aligned to 4 bytes, purpose unknown
            mGridCellsMemory[i].mFlags = reinterpret_cast<uint8_t*>(malloc((mGridCellsMemory[i].mNumObjects + 3) /4 * sizeof(uint32_t)));
            read(srcFile, mGridCellsMemory[i].mFlags, (mGridCellsMemory[i].mNumObjects + 3) /4 * sizeof(uint32_t));
        }
    }
    return true;
}

}

#endif
