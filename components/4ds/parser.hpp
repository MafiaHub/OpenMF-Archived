#ifndef FORMAT_PARSERS_4DS_H
#define FORMAT_PARSERS_4DS_H

#include <base_parser.hpp>
#include <cstring>
#include <math.hpp>

namespace MFFormat
{

class DataFormat4DS: public DataFormat
{
public:
    typedef enum
    {
        MATERIALFLAG_TEXTUREDIFFUSE = 0x00040000,          // whether diffuse texture is present
        MATERIALFLAG_COLORED = 0x08000000,                 // whether to use diffuse color (only applies with diffuse texture)
        MATERIALFLAG_MIPMAPPING = 0x00800000,
        MATERIALFLAG_ANIMATEDTEXTUREDIFFUSE = 0x04000000,
        MATERIALFLAG_ANIMATEXTEXTUREALPHA = 0x02000000,
        MATERIALFLAG_DOUBLESIDEDMATERIAL = 0x10000000,     // whether backface culling should be off
        MATERIALFLAG_ENVIRONMENTMAP = 0x00080000,          // simulates glossy material with environment texture
        MATERIALFLAG_NORMALTEXTUREBLEND = 0x00000100,      // blend between diffuse and environment texture normally
        MATERIALFLAG_MULTIPLYTEXTUREBLEND = 0x00000200,    // blend between diffuse and environment texture by multiplying
        MATERIALFLAG_ADDITIVETEXTUREBLEND = 0x00000400,    // blend between diffuse and environment texture by addition
        MATERIALFLAG_CALCREFLECTTEXTUREY = 0x00001000,
        MATERIALFLAG_PROJECTREFLECTTEXTUREY = 0x00002000,
        MATERIALFLAG_PROJECTREFLECTTEXTUREZ = 0x00004000,
        MATERIALFLAG_ADDITIONALEFFECT = 0x00008000,        // should be ALPHATEXTURE | COLORKEY | ADDITIVEMIXING
        MATERIALFLAG_ALPHATEXTURE = 0x40000000,
        MATERIALFLAG_COLORKEY = 0x20000000,
        MATERIALFLAG_ADDITIVEMIXING = 0x80000000           // the object is blended against the world by adding RGB (see street lamps etc.)
    } MaterialFlag;

    typedef enum
    {
        MESHTYPE_STANDARD = 0x01,           // visual mesh
        MESHTYPE_COLLISION = 0x02,          // NOTE(zaklaus): Imaginary type based on mesh name "wcol*"
        MESHTYPE_SECTOR = 0x05,             // part of space, used for culling, effective lighting etc.
        MESHTYPE_DUMMY = 0x06,              // invisible bounding box
        MESHTYPE_TARGET = 0x07,             // used in human models (as a shooting target?)
        MESHTYPE_BONE = 0x0a                // for skeletal animation
    } MeshType;

    typedef enum
    {
        VISUALMESHTYPE_STANDARD = 0x0,      // normal mesh
        VISUALMESHTYPE_SINGLEMESH = 0x02,   // mesh with bones
        VISUALMESHTYPE_SINGLEMORPH = 0x03,  // combination of morph (for face) and skeletal (for body) animation
        VISUALMESHTYPE_BILLBOARD = 0x04,    // billboarding mesh (rotates towards camera
        VISUALMESHTYPE_MORPH = 0x05,        // mesh with morphing (non-skeletal) animation, e.g. curtains in wind
        VISUALMESHTYPE_GLOW = 0x06,         // has no geometry, only shows glow texture
        VISUALMESHTYPE_MIRROR = 0x08        // reflects the scene
    } VisualMeshType;                       // subtype of mesh, when MeshType == MESHTYPE_STANDARD

    typedef enum
    {
        MESHRENDERFLAG_USEDEPTHBIAS = 0x0001,  // whether to receive shadows
        MESHRENDERFLAG_USESHADOWS = 0x0002,
        MESHRENDERFLAG_UNKNOWN = 0x0008,       // always 1
        MESHRENDERFLAG_USEPROJECTION = 0x0020, // used for projecting textures, such as blood
        MESHRENDERFLAG_FORBIDFOG = 0x0080
    } MeshRenderFlag;

    typedef enum
    {
        MESHOCCLUDINGFLAG_NORMAL = 0x09,
        MESHOCCLUDINGFLAG_SECTOR = 0x7D,
        MESHOCCLUDINGFLAG_WALL = 0x3D,       // mesh in sector (walls)
        MESHOCCLUDINGFLAG_PORTAL = 0x1D,     // mesh in portal
        MESHOCCLUDINGFLAG_INACTIVE = 0x11
    } MeshOccludingFlag;

    typedef struct
    {
        uint32_t mFlags;
        MFMath::Vec3 mAmbient;
        MFMath::Vec3 mDiffuse;                     // only used if there is no diffuse texture, or if COLORED flag is set
        MFMath::Vec3 mEmission;                    // always used
        float mTransparency; // 0.0 - invisible; 1.0 - opaque

        // environment map
        float mEnvRatio;                   // parameter for interpolating between env. and diffuse map (only for NORMAL blending flag)
        char mEnvMapNameLength;
        char mEnvMapName[255];

        char mDiffuseMapNameLength;
        char mDiffuseMapName[255];

        // alpha map
        char mAlphaMapNameLength;
        char mAlphaMapName[255];

        // anim map
        uint32_t mAnimSequenceLength;      // how many frames animated texture has
        uint16_t mUnk0;
        uint32_t mFramePeriod;
        uint32_t mUnk1;
        uint32_t mUnk2;
    } Material;

    typedef struct
    {
        MFMath::Vec3 mPos;
        MFMath::Vec3 mNormal;
        MFMath::Vec2 mUV;
    } Vertex;

    typedef struct
    {
        uint16_t mA;
        uint16_t mB;
        uint16_t mC;
    } Face;

    typedef struct
    {
        uint16_t mFaceCount;
        std::vector<Face> mFaces;
        uint16_t mMaterialID;      // 1-based, 0 = default material
    } FaceGroup;

    typedef struct
    {
        float mRelativeDistance;
        uint16_t mVertexCount;
        std::vector<Vertex> mVertices;
        uint8_t mFaceGroupCount;
        std::vector<FaceGroup> mFaceGroups;
    } Lod;

    typedef struct
    {
        uint16_t mInstanced;
        // for non-instanced mesh
        uint8_t mLODLevel;
        std::vector<Lod> mLODs;
    } Standard;

    typedef struct
    {
        uint16_t mUnk0;
        uint8_t mTargetCount;
        std::vector<uint16_t> mTargets;
    } Target;

    typedef struct
    {
        float mTransform[16];
        uint32_t mBoneID;
    } Bone;

    typedef struct
    {
        uint8_t mVertexCount;
        uint32_t mUnk0; // always 4.
        uint32_t mUnk1[6];
        std::vector<MFMath::Vec3> mVertices;
    } Portal;

    typedef struct
    {
        uint32_t mUnk0; // always 2049.
        uint32_t mUnk1; // always 0.
        uint32_t mVertexCount;
        uint32_t mFaceCount;
        std::vector<MFMath::Vec3> mVertices;
        std::vector<Face> mFaces;
        MFMath::Vec3 mMinBox;
        MFMath::Vec3 mMaxBox;
        uint8_t mPortalCount;
        std::vector<Portal> mPortals;
    } Sector;

    typedef struct
    {
        Standard mStandard;
        uint32_t mRotationAxis;  // 0 - X, 1 - Y, 2 - Z
        uint8_t mIgnoreCamera;   // 0 - rotate around center point, 1 - rotate around mRotationAxis
    } Billboard;

    typedef struct
    {
        // bounding box
        MFMath::Vec3 mMinBox;
        MFMath::Vec3 mMaxBox;
    } Dummy;

    typedef struct
    {
        float mPosition;
        uint16_t mMaterialID;
    } GlowData;

    typedef struct
    {
        uint8_t mGlowCount;
        std::vector<GlowData> mGlowData;
    } Glow;

    typedef struct
    {
        MFMath::Vec3 mMinBox;
        MFMath::Vec3 mMaxBox;
        float mUnk[4];
        MFMath::Mat4 mReflectionMatrix;
        MFMath::Vec3 mBackgroundColor;
        float mViewDistance;
        uint32_t mVertexCount;
        uint32_t mFaceCount;
        std::vector<MFMath::Vec3> mVertices;
        std::vector<Face> mFaces;
    } Mirror;

    typedef struct
    {
        MFMath::Vec3 mPosition;
        MFMath::Vec3 mNormals;
    } MorphLodVertex;

    typedef struct
    {
        uint16_t mVertexCount;
        std::vector<MorphLodVertex> mVertices;
        uint8_t mUnk0;
        std::vector<uint16_t> mVertexLinks; // addresses vertices from Standard's LOD mesh  
    } MorphLod;

    typedef struct
    {
        Standard mStandard;
        uint8_t mFrameCount;
        uint8_t mLODLevel;      // should be equal to Standard.LODLevel
        uint8_t mUnk0;
        std::vector<MorphLod> mLODs;
        MFMath::Vec3 mMinBox;
        MFMath::Vec3 mMaxBox;
        float mUnk1[4];
    } Morph;

    typedef struct
    {
        float mTransform[16];
        uint32_t mUnk0;
        uint32_t mAdditionalValuesCount;
        uint32_t mBoneID;
        MFMath::Vec3 mMinBox;
        MFMath::Vec3 mMaxBox;
        std::vector<float> mAdditionalValues;
    } SingleMeshLodJoint;

    typedef struct
    {
        uint8_t mJointCount;
        uint32_t mUnk0;
        MFMath::Vec3 mMinBox;
        MFMath::Vec3 mMaxBox;
        std::vector<SingleMeshLodJoint> mJoints;
    } SingleMeshLod;

    typedef struct
    {
        Standard mStandard;
        std::vector<SingleMeshLod> mLODs; // LODLevel == Standard.LODLevel.
    } SingleMesh;

    typedef struct
    {
        SingleMesh mSingleMesh;
        Morph mMorph;         // Morph without Standard Mesh!
    } SingleMorph;

    typedef struct
    {
        uint8_t mMeshType;
        // standard mesh type
        uint8_t mVisualMeshType;
        uint16_t mMeshRenderFlags;
        uint16_t mParentID; // 0 - not connected
        MFMath::Vec3 mPos;
        MFMath::Vec3 mScale;
        MFMath::Quat mRot;
        uint8_t mCullingFlags;
        uint8_t mMeshNameLength;
        char mMeshName[255];
        uint8_t mMeshParamsLength;
        char mMeshParams[255];
        Standard mStandard;
        Dummy mDummy;
        Mirror mMirror;
        Glow mGlow;
        Billboard mBillboard;
        Sector mSector;
        Target mTarget;
        Bone mBone;
        Morph mMorph;
        SingleMesh mSingleMesh;
        SingleMorph mSingleMorph;
    } Mesh;

    typedef struct sModel
    {
        uint8_t mSignature[4];
        uint16_t mFormatVersion; // PC : 0x1D (29)
        uint64_t mTimestamp;
        uint16_t mMaterialCount;
        std::vector<Material> mMaterials;
        uint16_t mMeshCount;
        std::vector<Mesh> mMeshes;
        uint8_t mUse5DS;

        MFMath::Vec3 computeWorldPos(uint16_t meshIndex)
        {
            MFMath::Vec3 result;

            meshIndex += 1;  // convert to 1-based

            while (meshIndex > 0 && meshIndex <= mMeshCount)
            {
                Mesh m = mMeshes[meshIndex - 1];

                result.x += m.mPos.x;
                result.y += m.mPos.y;
                result.z += m.mPos.z;

                meshIndex = m.mParentID;
            }

            return result;
        }
    } Model;

    virtual bool load(std::ifstream &srcFile) override;
    
    inline Model getModel()
    {
        return mLoadedModel;
    }

    typedef enum
    {
        ERROR_SUCCESS,
        ERROR_SIGNATURE,
    } ErrorCodes;

    std::string getErrorStr()
    {
        switch (mErrorCode)
        {
            case ERROR_SIGNATURE: return "Wrong 4ds signature";
        }

        return "Unknown error";
    }

protected:
    void loadMaterial(Model *model, std::ifstream &file);
    Lod loadLod(std::ifstream &file);
    Standard loadStandard(std::ifstream &file);
    Mirror loadMirror(std::ifstream &file);
    Glow loadGlow(std::ifstream &file);
    Portal loadPortal(std::ifstream &file);
    Sector loadSector(std::ifstream &file);
    Target loadTarget(std::ifstream &file);
    Morph loadMorph(std::ifstream &file, bool ignoreStandard);
    SingleMeshLodJoint loadSingleMeshLodJoint(std::ifstream &file);
    SingleMeshLod loadSingleMeshLod(std::ifstream &file);
    SingleMesh loadSingleMesh(std::ifstream &file);
    SingleMorph loadSingleMorph(std::ifstream &file);
    void loadMesh(Model *model, std::ifstream &file);
    Model loadModel(std::ifstream &file);
    Model mLoadedModel;
};

void DataFormat4DS::loadMaterial(Model *model, std::ifstream &file)
{
    read(file, &model->mMaterialCount);
    
    for(size_t i = 0; i < model->mMaterialCount; ++i) 
    {
        Material mat = {};
        read(file, &mat.mFlags);
        read(file, &mat.mAmbient);
        read(file, &mat.mDiffuse);
        read(file, &mat.mEmission);
        read(file, &mat.mTransparency);

        if(mat.mFlags & MATERIALFLAG_ENVIRONMENTMAP) 
        {
            read(file, &mat.mEnvRatio);
            read(file, &mat.mEnvMapNameLength);
            read(file, &mat.mEnvMapName, mat.mEnvMapNameLength);
        }

        read(file, &mat.mDiffuseMapNameLength);
        read(file, mat.mDiffuseMapName, mat.mDiffuseMapNameLength);
       
            
        if(mat.mFlags & MATERIALFLAG_ALPHATEXTURE)
        {
            read(file, &mat.mAlphaMapNameLength);
            read(file, mat.mAlphaMapName, mat.mAlphaMapNameLength);
        }
        
        if(mat.mFlags & MATERIALFLAG_ANIMATEDTEXTUREDIFFUSE)
        {
            read(file, &mat.mAnimSequenceLength);
            read(file, &mat.mUnk0);
            read(file, &mat.mFramePeriod);
            read(file, &mat.mUnk1);
            read(file, &mat.mUnk2);
        }

        model->mMaterials.push_back(mat);
    }  
}

DataFormat4DS::Lod DataFormat4DS::loadLod(std::ifstream &file)
{
    Lod newLod = {};
    read(file, &newLod.mRelativeDistance);
    read(file, &newLod.mVertexCount);

    for (size_t i = 0; i < newLod.mVertexCount; ++i)
    {
        Vertex v;
        read(file,&v,sizeof(v));
        newLod.mVertices.push_back(v);
    }

    read(file, &newLod.mFaceGroupCount); 

    for (size_t i = 0; i < newLod.mFaceGroupCount; ++i)
    {
        FaceGroup newFaceGroup = {};
        read(file, &newFaceGroup.mFaceCount);

        for (size_t j = 0; j < newFaceGroup.mFaceCount; ++j)
        {
           Face f;
           read(file,&f,sizeof(f));
           newFaceGroup.mFaces.push_back(f);
        }

        read(file, &newFaceGroup.mMaterialID);
        newLod.mFaceGroups.push_back(newFaceGroup);
    }

    return newLod;
}

DataFormat4DS::Standard DataFormat4DS::loadStandard(std::ifstream &file)
{
    Standard newStandard = {};
    read(file, &newStandard.mInstanced);

    if(!newStandard.mInstanced)
    {
        read(file, &newStandard.mLODLevel);

        for(size_t i = 0; i < newStandard.mLODLevel; ++i)
        {
            Lod newLod = {};
            newLod = loadLod(file);
            newStandard.mLODs.push_back(newLod);
        }
    }

    return newStandard;
}

DataFormat4DS::Mirror DataFormat4DS::loadMirror(std::ifstream &file)
{
    Mirror newMirror = {};
    read(file, &newMirror.mMinBox);
    read(file, &newMirror.mMaxBox);
    read(file, &newMirror.mUnk, sizeof(float) * 4);
    read(file, &newMirror.mReflectionMatrix);
    read(file, &newMirror.mBackgroundColor);
    read(file, &newMirror.mViewDistance);
    read(file, &newMirror.mVertexCount);
    read(file, &newMirror.mFaceCount);

    for (size_t i = 0; i < newMirror.mVertexCount; ++i)
    {
        MFMath::Vec3 v;
        read(file,&v,sizeof(v));
        newMirror.mVertices.push_back(v);
    }

    for (size_t i = 0; i < newMirror.mFaceCount; ++i)
    {
        Face f;
        read(file,&f,sizeof(f));
        newMirror.mFaces.push_back(f);
    }

    return newMirror;
}

DataFormat4DS::Glow DataFormat4DS::loadGlow(std::ifstream &file)
{
    Glow newGlow = {};
    read(file, &newGlow.mGlowCount);
    
    for (size_t i = 0; i < newGlow.mGlowCount; ++i)
    {
        GlowData newGlow_data = {};
        read(file, &newGlow_data.mPosition);
        read(file, &newGlow_data.mMaterialID);
        newGlow.mGlowData.push_back(newGlow_data);
    }
    
    return newGlow;
}

DataFormat4DS::Portal DataFormat4DS::loadPortal(std::ifstream &file)
{
    Portal newPortal = {};
    read(file, &newPortal.mVertexCount);
    read(file, &newPortal.mUnk0);
    read(file, newPortal.mUnk1, sizeof(float) * 6);

    for (size_t i = 0; i < newPortal.mVertexCount; ++i)
    {
        MFMath::Vec3 v;
        read(file,&v,sizeof(v));
        newPortal.mVertices.push_back(v);
    }
   
    return newPortal;
}

DataFormat4DS::Sector DataFormat4DS::loadSector(std::ifstream &file)
{
    Sector newSector = {};
    read(file, &newSector.mUnk0);
    read(file, &newSector.mUnk1);
    read(file, &newSector.mVertexCount);
    read(file, &newSector.mFaceCount);

    for (size_t i = 0; i < newSector.mVertexCount; ++i)
    {
        MFMath::Vec3 v;
        read(file,&v,sizeof(v));
        newSector.mVertices.push_back(v);
    }

    for (size_t i = 0; i < newSector.mFaceCount; ++i)
    {
        Face f;
        read(file,&f,sizeof(f));
        newSector.mFaces.push_back(f);
    }

    read(file, &newSector.mMinBox);
    read(file, &newSector.mMaxBox);
    read(file, &newSector.mPortalCount);

    for(size_t i = 0; i < newSector.mPortalCount; ++i) 
    {
        Portal newPortal = {};
        newPortal = loadPortal(file);
        newSector.mPortals.push_back(newPortal);
    }

    return newSector;
}

DataFormat4DS::Target DataFormat4DS::loadTarget(std::ifstream &file)
{
    Target newTarget = {};
    read(file, &newTarget.mUnk0);
    read(file, &newTarget.mTargetCount);
   
    for (size_t i = 0; i < newTarget.mTargetCount; ++i)
    {
        uint16_t t;
        read(file,&t,sizeof(t));
        newTarget.mTargets.push_back(t);
    }
 
    return newTarget;
}

DataFormat4DS::Morph DataFormat4DS::loadMorph(std::ifstream &file, bool ignoreStandard)
{
    Morph newMorph = { };
    // NOTE(zaklaus): Single Morph contains Standard Mesh in Single Mesh already.
    if (!ignoreStandard) 
    {
        newMorph.mStandard = loadStandard(file);
    } 
    // NOTE(zaklaus): ELSE ignore Standard Mesh, since Single Mesh has it.
    
    read(file, &newMorph.mFrameCount);
    
    if (newMorph.mFrameCount)
    {
        read(file, &newMorph.mLODLevel);
        read(file, &newMorph.mUnk0);

        for (size_t i = 0; i < newMorph.mLODLevel; ++i) 
        {
            MorphLod newMorphLod = {};
            read(file, &newMorphLod.mVertexCount);

            for (size_t j = 0; j < newMorph.mFrameCount * newMorphLod.mVertexCount; ++j)
            {
                MorphLodVertex v;
                read(file,&v,sizeof(v));
                newMorphLod.mVertices.push_back(v);
            }

            if (newMorphLod.mVertexCount * newMorph.mFrameCount) 
            {
                read(file, &newMorphLod.mUnk0);
            }

            for (size_t j = 0; j < newMorphLod.mVertexCount; ++j)
            {
                uint16_t v;
                read(file,&v,sizeof(v));
                newMorphLod.mVertexLinks.push_back(v);
            }

            newMorph.mLODs.push_back(newMorphLod);
        }
        
        read(file, &newMorph.mMinBox);
        read(file, &newMorph.mMaxBox);
        read(file, newMorph.mUnk1, sizeof(float) * 4);

    }
    return newMorph;
}

DataFormat4DS::SingleMeshLodJoint DataFormat4DS::loadSingleMeshLodJoint(std::ifstream &file)
{
    SingleMeshLodJoint newJoint = {};
    read(file, &newJoint.mTransform);
    read(file, &newJoint.mUnk0, sizeof(uint32_t));
    read(file, &newJoint.mAdditionalValuesCount);
    read(file, &newJoint.mBoneID);
    read(file, &newJoint.mMinBox);
    read(file, &newJoint.mMaxBox);
  
    for (size_t i = 0; i < newJoint.mAdditionalValuesCount; ++i)
    {
        float f;
        read(file,&f,sizeof(f));
        newJoint.mAdditionalValues.push_back(f);
    }

    return newJoint;
}

DataFormat4DS::SingleMeshLod DataFormat4DS::loadSingleMeshLod(std::ifstream &file)
{
    SingleMeshLod newLod = {};
    read(file, &newLod.mJointCount);
    read(file, &newLod.mUnk0);
    read(file, &newLod.mMinBox);
    read(file, &newLod.mMaxBox);

    for (size_t i = 0; i < newLod.mJointCount; ++i) 
    {
        SingleMeshLodJoint newJoint = {};
        newJoint = loadSingleMeshLodJoint(file);
        newLod.mJoints.push_back(newJoint);
    }  
      
    return newLod;
}

DataFormat4DS::SingleMesh DataFormat4DS::loadSingleMesh(std::ifstream &file)
{
    SingleMesh newMesh = {};
    
    newMesh.mStandard = loadStandard(file);
    
    for(size_t i = 0; i < newMesh.mStandard.mLODLevel; ++i) 
    {
        SingleMeshLod newLod = {};
        newLod = loadSingleMeshLod(file);
        newMesh.mLODs.push_back(newLod);
    }
    
    return newMesh;
}

DataFormat4DS::SingleMorph DataFormat4DS::loadSingleMorph(std::ifstream &file)
{
    SingleMorph newMorph = {};
    newMorph.mSingleMesh = loadSingleMesh(file);

    newMorph.mMorph = loadMorph(file, 1);
    return newMorph;
}

void DataFormat4DS::loadMesh(Model *model, std::ifstream &file)
{
    read(file, &model->mMeshCount);

    for (size_t i = 0; i < model->mMeshCount; ++i)
    {
        Mesh newMesh = {};
        read(file, &newMesh.mMeshType);

        if(newMesh.mMeshType == MESHTYPE_STANDARD)
        {
            read(file, &newMesh.mVisualMeshType);
            read(file, &newMesh.mMeshRenderFlags);
        }

        read(file, &newMesh.mParentID);
        read(file, &newMesh.mPos);
        read(file, &newMesh.mScale);

        read(file, (char *) &(newMesh.mRot), sizeof(float) * 4);
        newMesh.mRot.fromMafia();

        read(file, &newMesh.mCullingFlags);
        read(file, &newMesh.mMeshNameLength);
        read(file, newMesh.mMeshName, newMesh.mMeshNameLength);
        read(file, &newMesh.mMeshParamsLength);
        read(file, newMesh.mMeshParams, newMesh.mMeshParamsLength);

        switch(newMesh.mMeshType)
        {
            case MESHTYPE_STANDARD:
            {
                switch(newMesh.mVisualMeshType)
                {
                    case VISUALMESHTYPE_STANDARD:
                    {
                        Standard newStandard = {};
                        newStandard = loadStandard(file);
                        newMesh.mStandard = newStandard;
                    } 
                    break;

                    case VISUALMESHTYPE_MIRROR:
                    {
                        Mirror newMirror = {};
                        newMirror = loadMirror(file);
                        newMesh.mMirror = newMirror;
                    } 
                    break;

                    case VISUALMESHTYPE_GLOW:
                    {
                        Glow newGlow = {};
                        newGlow = loadGlow(file);
                        newMesh.mGlow = newGlow;
                    } 
                    break;

                    case VISUALMESHTYPE_BILLBOARD:
                    {
                        Billboard new_billboard = {};
                        new_billboard.mStandard = loadStandard(file);
                        read(file, &new_billboard.mRotationAxis);
                        read(file, &new_billboard.mIgnoreCamera);
                        newMesh.mBillboard = new_billboard;
                    } 
                    break;
                    
                    case VISUALMESHTYPE_MORPH:
                    {
                        Morph newMorph = {};
                        newMorph = loadMorph(file, 0);
                        newMesh.mMorph = newMorph;
                    }
                    break;
                                
                    case VISUALMESHTYPE_SINGLEMESH:
                    {
                        SingleMesh new_single_mesh = {};
                        new_single_mesh = loadSingleMesh(file);
                        newMesh.mSingleMesh = new_single_mesh;
                    }
                    break;
                    
                    case VISUALMESHTYPE_SINGLEMORPH:
                    {
                        SingleMorph new_single_morph = {};
                        new_single_morph = loadSingleMorph(file);
                        newMesh.mSingleMorph = new_single_morph;
                    }
                    break;

                    default:
                    {
                    }
                    break;
                }
            }
            break;

            case MESHTYPE_DUMMY:
            {
                Dummy newDummy = {};
                read(file, &newDummy.mMinBox);
                read(file, &newDummy.mMaxBox);
                newMesh.mDummy = newDummy;
            }
            break;
            
            case MESHTYPE_SECTOR:
            {
                Sector newSector = {};
                newSector = loadSector(file);
                newMesh.mSector = newSector;
            }
            break;
            
            case MESHTYPE_TARGET:
            {
                Target newTarget = {};
                newTarget = loadTarget(file);
                newMesh.mTarget = newTarget;
            }
            break;
            
            case MESHTYPE_BONE:
            {
                Bone newBone = {};
                read(file, &newBone.mTransform);
                read(file, &newBone.mBoneID);
                newMesh.mBone = newBone;
            }
            break;

            default: 
            {
            }
            break;
        }


        // NOTE(zaklaus): Check whether this is a collision mesh.
        // happens AFTER we load the required content to skip it.
        std::string meshName = std::string(newMesh.mMeshName);
        if (meshName.find("wcol") != meshName.npos)
        {
            newMesh.mMeshType = MESHTYPE_COLLISION;
        }

        model->mMeshes.push_back(newMesh);
    }
}

DataFormat4DS::Model DataFormat4DS::loadModel(std::ifstream &file)
{
    Model model;
    read(file, &model.mSignature, 4);

    if (strncmp(reinterpret_cast<char*>(model.mSignature), "4DS", 3) != 0)
    {
        mErrorCode = DataFormat4DS::ERROR_SIGNATURE;
        return model;
    }

    read(file, &model.mFormatVersion);
    read(file, &model.mTimestamp);
    
    loadMaterial(&model, file);
    loadMesh(&model, file);
    read(file, &model.mUse5DS);
   
    mErrorCode = DataFormat4DS::ERROR_SUCCESS;
    return model;
}

bool DataFormat4DS::load(std::ifstream &srcFile)
{
    mLoadedModel = loadModel(srcFile);
    return mErrorCode == DataFormat4DS::ERROR_SUCCESS;
}

}

#endif
