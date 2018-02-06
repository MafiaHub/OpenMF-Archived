#include <klz/klz_parser.hpp>

namespace MFFormat
{

DataFormatTreeKLZ::~DataFormatTreeKLZ()
{
    for (unsigned int i = 0; i < mDataHeader.mGridWidth * mDataHeader.mGridWidth; ++i)
    {
        if (mGridCellsMemory[i].mNumObjects)
        {
            free(mGridCellsMemory[i].mReferences);
            free(mGridCellsMemory[i].mFlags);
        }
    }

    free(mGridCellsMemory);

    for (unsigned int i = 0; i < mLinkTables.size(); ++i)
        free(mLinkTables[i].mName);

    free(mLinkNameOffsetTable);
    free(mCellBoundariesX); 
    free(mCellBoundariesY); 
}

bool DataFormatTreeKLZ::load(std::ifstream &srcFile)
{
    read(srcFile, &mHeader);
    
    mLinkNameOffsetTable = reinterpret_cast<uint32_t*>(malloc(sizeof(uint32_t)*mHeader.mNumLinks));
    read(srcFile, mLinkNameOffsetTable, sizeof(uint32_t)*mHeader.mNumLinks);

    for (unsigned int i = 0; i < mHeader.mNumLinks; i++) 
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
  
    for (unsigned int i = 0; i < mDataHeader.mNumFaces; i++)
    {
        FaceCol newCol = {};
        read(srcFile, &newCol);
        mFaceCols.push_back(newCol);
    }

    for (unsigned int i = 0; i < mDataHeader.mNumAABBs; i++)
    {
        AABBCol newCol = {};
        read(srcFile, &newCol);
        mAABBCols.push_back(newCol);
    }

    for (unsigned int i = 0; i < mDataHeader.mNumXTOBBs; i++)
    {
        XTOBBCol newCol = {};
        read(srcFile, &newCol);
        mXTOBBCols.push_back(newCol);
    }

    for (unsigned int i = 0; i < mDataHeader.mNumCylinders; i++)
    {
        CylinderCol newCol = {};
        read(srcFile, &newCol);
        mCylinderCols.push_back(newCol);
    }

    for (unsigned int i = 0; i < mDataHeader.mNumOBBs; i++)
    {
        OBBCol newCol = {};
        read(srcFile, &newCol);
        mOBBCols.push_back(newCol);
    }

    for (unsigned int i = 0; i < mDataHeader.mNumSpheres; i++)
    {
        SphereCol newCol = {};
        read(srcFile, &newCol);
        mSphereCols.push_back(newCol);
    }
   
    read(srcFile, &mCollisionGridMagic);
    uint32_t gridSize = mDataHeader.mGridWidth * mDataHeader.mGridWidth;
    mGridCellsMemory = reinterpret_cast<Cell*>(malloc(sizeof(Cell) * gridSize));

    for (unsigned int i = 0; i < gridSize; i++)
    {
        read(srcFile, &mGridCellsMemory[i].mNumObjects);
        read(srcFile, mGridCellsMemory[i].mReserved, sizeof(uint32_t)*2);
        read(srcFile, &mGridCellsMemory[i].mHeight);

        if (mGridCellsMemory[i].mNumObjects)
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

std::vector<std::string> DataFormatTreeKLZ::getLinkStrings()
{
    std::vector<std::string> result;

    for (int i = 0; i < (int) mLinkTables.size(); ++i)
    {
        char buffer[255];
        std::memcpy(buffer,mLinkTables[i].mName, mLinkTables[i].mNameLength);
        buffer[mLinkTables[i].mNameLength] = 0;
        result.push_back(buffer);
    }

    return result;
}

}
