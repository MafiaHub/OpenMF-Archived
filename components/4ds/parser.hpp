#ifndef FORMAT_PARSERS_4DS_H
#define FORMAT_PARSERS_4DS_H

#include <base_parser.hpp>
#include <cstring>

namespace MFFormat
{

class DataFormat4DS: public DataFormat
{
public:
    typedef enum
    {
        MATERIALFLAG_TEXTUREDIFFUSE = 0x00040000,
        MATERIALFLAG_COLORED = 0x08000000,
        MATERIALFLAG_MIPMAPPING = 0x00800000,
        MATERIALFLAG_ANIMATEDTEXTUREDIFFUSE = 0x04000000,
        MATERIALFLAG_ANIMATEXTEXTUREALPHA = 0x02000000,
        MATERIALFLAG_DOUBLESIDEDMATERIAL = 0x10000000,
        MATERIALFLAG_ENVIRONMENTMAP = 0x00080000,
        MATERIALFLAG_NORMALTEXTUREBLEND = 0x00000100,
        MATERIALFLAG_MULTIPLYTEXTUREBLEND = 0x00000200,
        MATERIALFLAG_ADDITIVETEXTUREBLEND = 0x00000400,
        MATERIALFLAG_CALCREFLECTTEXTUREY = 0x00001000,
        MATERIALFLAG_PROJECTREFLECTTEXTUREY = 0x00002000,
        MATERIALFLAG_PROJECTREFLECTTEXTUREZ = 0x00004000,
        MATERIALFLAG_ADDITIONALEFFECT = 0x00008000,
        MATERIALFLAG_ALPHATEXTURE = 0x40000000,
        MATERIALFLAG_COLORKEY = 0x20000000,
        MATERIALFLAG_ADDITIVEMIXING = 0x80000000
    } MaterialFlag;

    typedef enum
    {
        MESHTYPE_STANDARD = 0x01,
        MESHTYPE_SECTOR = 0x05,
        MESHTYPE_DUMMY = 0x06,
        MESHTYPE_TARGET = 0x07,
        MESHTYPE_BONE = 0x0a
    } MeshType;

    typedef enum
    {
        VISUALMESHTYPE_STANDARD = 0x0,
        VISUALMESHTYPE_SINGLEMESH = 0x02,
        VISUALMESHTYPE_SINGLEMORPH = 0x03,
        VISUALMESHTYPE_BILLBOARD = 0x04,
        VISUALMESHTYPE_MORPH = 0x05,
        VISUALMESHTYPE_GLOW = 0x06,
        VISUALMESHTYPE_MIRROR = 0x08
    } VisualMeshType;

    typedef enum
    {
        MESHRENDERFLAG_USEDEPTHBIAS = 0x0001,
        MESHRENDERFLAG_USESHADOWS = 0x0002,
        MESHRENDERFLAG_UNKNOWN = 0x0008,
        MESHRENDERFLAG_USEPROJECTION = 0x0020,
        MESHRENDERFLAG_FORBIDFOG = 0x0080
    } MeshRenderFlag;

    typedef enum
    {
        MESHOCCLUDINGFLAG_NORMAL = 0x09,
        MESHOCCLUDINGFLAG_SECTOR = 0x7D,
        MESHOCCLUDINGFLAG_WALL = 0x3D,     // mesh in sector (walls)
        MESHOCCLUDINGFLAG_PORTAL = 0x1D,   // mesh in portal
        MESHOCCLUDINGFLAG_INACTIVE = 0x11
    } MeshOccludingFlag;

    typedef struct
    {
        uint32_t mFlags;
        Vec3 mAmbient;
        Vec3 mDiffuse;
        Vec3 mEmission;
        float mTransparency; // 0.0 - invisible; 1.0 - opaque

        // environment map
        float mEnvRatio;
        char mEnvMapNameLength;
        char mEnvMapName[255];

        char mDiffuseMapNameLength;
        char mDiffuseMapName[255];

        // alpha map
        char mAlphaMapNameLength;
        char mAlphaMapName[255];

        // anim map
        uint32_t mAnimSequenceLength;
        uint16_t mUnk0;
        uint32_t mFramePeriod;
        uint32_t mUnk1;
        uint32_t mUnk2;
    } Material;

    typedef struct
    {
        Vec3 mPos;
        Vec3 mNormal;
        Vec2 mUV;
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
        Face *mFaces;
        uint16_t mMaterialID;
    } FaceGroup;

    typedef struct
    {
        float mRelativeDistance;
        uint16_t mVertexCount;
        Vertex *mVertices;
        uint8_t mFaceGroupCount;
        FaceGroup *mFaceGroups;
    } Lod;

    typedef struct
    {
        uint16_t mInstanced;
        // for non-instanced mesh
        uint8_t mLODLevel;
        Lod *mLODs;
    } Standard;

    typedef struct
    {
        uint16_t mUnk0;
        uint8_t mTargetCount;
        uint16_t *mTargets;
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
        Vec3 *mVertices;
    } Portal;

    typedef struct
    {
        uint32_t mUnk0; // always 2049.
        uint32_t mUnk1; // always 0.
        uint32_t mVertexCount;
        uint32_t mFaceCount;
        Vec3 *mVertices;
        Face *mFaces;
        Vec3 mMinBox;
        Vec3 mMaxBox;
        uint8_t mPortalCount;
        Portal *mPortals;
    } Sector;

    typedef struct
    {
        Standard mStandard;
        uint32_t mRotationAxis;
        uint8_t mIgnoreCamera;
    } Billboard;

    typedef struct
    {
        // bounding box
        Vec3 mMinBox;
        Vec3 mMaxBox;
    } Dummy;

    typedef struct
    {
        float mPosition;
        uint16_t mMaterialID;
    } GlowData;

    typedef struct
    {
        uint8_t mGlowCount;
        GlowData *mGlowData;
    } Glow;

    typedef struct
    {
        Vec3 mMinBox;
        Vec3 mMaxBox;
        float mUnk[4];
        Mat4 mReflectionMatrix;
        Vec3 mBackgroundColor;
        float mViewDistance;
        uint32_t mVertexCount;
        uint32_t mFaceCount;
        Vec3 *mVertices;
        Face *mFaces;
    } Mirror;

    typedef struct
    {
        Vec3 mPosition;
        Vec3 mNormals;
    } MorphLodVertex;

    typedef struct
    {
        uint16_t mVertexCount;
        MorphLodVertex *mVertices;
        uint8_t mUnk0;
        uint16_t *mVertexLinks; // addresses vertices from Standard's LOD mesh  
    } MorphLod;

    typedef struct
    {
        Standard mStandard;
        uint8_t mFrameCount;
        uint8_t mLODLevel;      // should be equal to Standard.LODLevel
        uint8_t mUnk0;
        MorphLod *mLODs;
        Vec3 mMinBox;
        Vec3 mMaxBox;
        float mUnk1[4];
    } Morph;

    typedef struct
    {
        float mTransform[16];
        uint32_t mUnk0;
        uint32_t mAdditionalValuesCount;
        uint32_t mBoneID;
        Vec3 mMinBox;
        Vec3 mMaxBox;
        float *mAdditionalValues;
    } SingleMeshLodJoint;

    typedef struct
    {
        uint8_t mJointCount;
        uint32_t mUnk0;
        Vec3 mMinBox;
        Vec3 mMaxBox;
        SingleMeshLodJoint *mJoints;
    } SingleMeshLod;

    typedef struct
    {
        Standard mStandard;
        SingleMeshLod *mLODs; // LODLevel == Standard.LODLevel.
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
        Vec3 mPos;
        Vec3 mScale;
        Quat mRot;
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

    typedef struct
    {
        uint8_t mSignature[4];
        uint16_t mFormatVersion; // PC : 0x1D (29)
        uint64_t mTimestamp;
        uint16_t mMaterialCount;
        Material *mMaterials;
        uint16_t mMeshCount;
        Mesh *mMeshes;
        uint8_t mUse5DS;
    } Model;

    virtual bool load(std::ifstream &srcFile) override;
    
    inline Model* getModel()
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
    Model* loadModel(std::ifstream &file);
    Model* mLoadedModel;
};

void DataFormat4DS::loadMaterial(Model *model, std::ifstream &file)
{
    read(file, &model->mMaterialCount);
    model->mMaterials = reinterpret_cast<Material*>(malloc(sizeof(Material) * model->mMaterialCount));
    
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

        model->mMaterials[i] = mat;
    }  
}

DataFormat4DS::Lod DataFormat4DS::loadLod(std::ifstream &file)
{
    Lod newLod = {};
    read(file, &newLod.mRelativeDistance);
    read(file, &newLod.mVertexCount);

    newLod.mVertices = reinterpret_cast<Vertex*>(malloc(sizeof(Vertex)* newLod.mVertexCount));
    read(file, newLod.mVertices, sizeof(Vertex)*newLod.mVertexCount);
    read(file, &newLod.mFaceGroupCount); 

    newLod.mFaceGroups = reinterpret_cast<FaceGroup*>(malloc(sizeof(FaceGroup)*newLod.mFaceGroupCount));
    
    for(size_t i = 0; i < newLod.mFaceGroupCount; ++i)
    {
        FaceGroup new_face_group = {};
        read(file, &new_face_group.mFaceCount);
        new_face_group.mFaces = reinterpret_cast<Face*>(malloc(sizeof(Face)*new_face_group.mFaceCount));
        read(file, new_face_group.mFaces, sizeof(Face)*new_face_group.mFaceCount);
        read(file, &new_face_group.mMaterialID);

        newLod.mFaceGroups[i] = new_face_group;
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
        newStandard.mLODs = reinterpret_cast<Lod*>(malloc(sizeof(Lod)*newStandard.mLODLevel));

        for(size_t i = 0; i < newStandard.mLODLevel; ++i)
        {
            Lod newLod = {};
            newLod = loadLod(file);
            newStandard.mLODs[i] = newLod;
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

    newMirror.mVertices = reinterpret_cast<Vec3*>(malloc(sizeof(osg::Vec3f)*newMirror.mVertexCount));
    newMirror.mFaces = reinterpret_cast<Face*>(malloc(sizeof(Face)*newMirror.mFaceCount));
    
    read(file, newMirror.mVertices, sizeof(Vec3)*newMirror.mVertexCount);
    read(file, newMirror.mFaces, sizeof(Face)*newMirror.mFaceCount);
    
    return newMirror;
}

DataFormat4DS::Glow DataFormat4DS::loadGlow(std::ifstream &file)
{
    Glow newGlow = {};
    read(file, &newGlow.mGlowCount);
    newGlow.mGlowData = reinterpret_cast<GlowData*>(malloc(sizeof(GlowData)*newGlow.mGlowCount));
    
    for(size_t i = 0; i < newGlow.mGlowCount; ++i)
    {
        GlowData newGlow_data = {};
        read(file, &newGlow_data.mPosition);
        read(file, &newGlow_data.mMaterialID);
        
        newGlow.mGlowData[i] = newGlow_data;
    }
    
    return newGlow;
}

DataFormat4DS::Portal DataFormat4DS::loadPortal(std::ifstream &file)
{
    Portal newPortal = {};
    read(file, &newPortal.mVertexCount);
    read(file, &newPortal.mUnk0);
    read(file, newPortal.mUnk1, sizeof(float) * 6);
    newPortal.mVertices = reinterpret_cast<Vec3*>(malloc(sizeof(osg::Vec3f)*newPortal.mVertexCount));
    read(file, newPortal.mVertices, sizeof(Vec3)*newPortal.mVertexCount); 
   
    return newPortal;
}

DataFormat4DS::Sector DataFormat4DS::loadSector(std::ifstream &file)
{
    Sector newSector = {};
    read(file, &newSector.mUnk0);
    read(file, &newSector.mUnk1);
    read(file, &newSector.mVertexCount);
    read(file, &newSector.mFaceCount);

    newSector.mVertices = reinterpret_cast<Vec3*>(malloc(sizeof(Vec3)*newSector.mVertexCount));
    read(file, newSector.mVertices, sizeof(Vec3)*newSector.mVertexCount);

    newSector.mFaces = reinterpret_cast<Face*>(malloc(sizeof(Face)*newSector.mFaceCount));
    read(file, newSector.mFaces, sizeof(Face) *newSector.mFaceCount);

    read(file, &newSector.mMinBox);
    read(file, &newSector.mMaxBox);
    read(file, &newSector.mPortalCount);

    newSector.mPortals = reinterpret_cast<Portal*>(malloc(sizeof(Portal)*newSector.mPortalCount));

    for(size_t i = 0; i < newSector.mPortalCount; ++i) 
    {
        Portal newPortal = {};
        newPortal = loadPortal(file);
        newSector.mPortals[i] = newPortal;
    }

    return newSector;
}

DataFormat4DS::Target DataFormat4DS::loadTarget(std::ifstream &file)
{
    Target newTarget = {};
    read(file, &newTarget.mUnk0);
    read(file, &newTarget.mTargetCount);
    
    newTarget.mTargets = reinterpret_cast<uint16_t*>(malloc(sizeof(uint16_t)*newTarget.mTargetCount));
    read(file, newTarget.mTargets, sizeof(uint16_t)*newTarget.mTargetCount);
    
    return newTarget;
}

DataFormat4DS::Morph DataFormat4DS::loadMorph(std::ifstream &file, bool ignoreStandard)
{
    Morph newMorph = { };
    // NOTE(zaklaus): Single Morph contains Standard Mesh in Single Mesh already.
    if(!ignoreStandard) 
    {
        newMorph.mStandard = loadStandard(file);
    } 
    // NOTE(zaklaus): ELSE ignore Standard Mesh, since Single Mesh has it.
    
    read(file, &newMorph.mFrameCount);
    
    if(newMorph.mFrameCount)
    {
        read(file, &newMorph.mLODLevel);
        read(file, &newMorph.mUnk0);

        newMorph.mLODs = reinterpret_cast<MorphLod*>(malloc(sizeof(MorphLod)*newMorph.mLODLevel));

        for(size_t i = 0; i < newMorph.mLODLevel; ++i) 
        {
            MorphLod newMorphLod = {};
            read(file, &newMorphLod.mVertexCount);

            newMorphLod.mVertices = reinterpret_cast<MorphLodVertex*>(malloc(sizeof(MorphLodVertex)*newMorphLod.mVertexCount * newMorph.mFrameCount));
            read(file, newMorphLod.mVertices, sizeof(MorphLodVertex)*newMorphLod.mVertexCount * newMorph.mFrameCount);

            if(newMorphLod.mVertexCount * newMorph.mFrameCount) 
            {
                read(file, &newMorphLod.mUnk0);
            }

            newMorphLod.mVertexLinks = reinterpret_cast<uint16_t*>(malloc(sizeof(uint16_t)*newMorphLod.mVertexCount));
            read(file, newMorphLod.mVertexLinks, sizeof(uint16_t)*newMorphLod.mVertexCount);

            newMorph.mLODs[i] = newMorphLod;
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

    newJoint.mAdditionalValues = reinterpret_cast<float*>(malloc(sizeof(float)*newJoint.mAdditionalValuesCount));
    read(file, newJoint.mAdditionalValues, sizeof(float)*newJoint.mAdditionalValuesCount);
   
    return newJoint;
}

DataFormat4DS::SingleMeshLod DataFormat4DS::loadSingleMeshLod(std::ifstream &file)
{
    SingleMeshLod newLod = {};
    read(file, &newLod.mJointCount);
    read(file, &newLod.mUnk0);
    read(file, &newLod.mMinBox);
    read(file, &newLod.mMaxBox);

    newLod.mJoints = reinterpret_cast<SingleMeshLodJoint*>(malloc(sizeof(SingleMeshLodJoint)*newLod.mJointCount));
    for(size_t i = 0; i < newLod.mJointCount; ++i) 
    {
        SingleMeshLodJoint newJoint = {};
        newJoint = loadSingleMeshLodJoint(file);
        newLod.mJoints[i] = newJoint;
    }  
      
    return newLod;
}

DataFormat4DS::SingleMesh DataFormat4DS::loadSingleMesh(std::ifstream &file)
{
    SingleMesh newMesh = {};
    
    newMesh.mStandard = loadStandard(file);
    
    newMesh.mLODs = reinterpret_cast<SingleMeshLod*>(malloc(sizeof(SingleMeshLod)*newMesh.mStandard.mLODLevel));
    
    for(size_t i = 0; i < newMesh.mStandard.mLODLevel; ++i) 
    {
        SingleMeshLod newLod = {};
        newLod = loadSingleMeshLod(file);
        newMesh.mLODs[i] = newLod;
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
    model->mMeshes = reinterpret_cast<Mesh*>(malloc(sizeof(Mesh)*model->mMeshCount));

    for(size_t i = 0; i < model->mMeshCount; ++i)
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

        float r[4];
        read(file, r, sizeof(float) * 4);
        newMesh.mRot = {r[1], r[2], r[3], -r[0]};

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

        model->mMeshes[i] = newMesh;
    }
}

DataFormat4DS::Model* DataFormat4DS::loadModel(std::ifstream &file)
{
    Model *model = reinterpret_cast<Model*>(malloc(sizeof(Model)));
    read(file, &model->mSignature, 4);

    if (strncmp(reinterpret_cast<char*>(model->mSignature), "4DS", 3) != 0)
    {
        mErrorCode = DataFormat4DS::ERROR_SIGNATURE;
        return nullptr;
    }

    read(file, &model->mFormatVersion);
    read(file, &model->mTimestamp);
    
    loadMaterial(model, file);
    loadMesh(model, file);
    read(file, &model->mUse5DS);
    
    return model;
}

bool DataFormat4DS::load(std::ifstream &srcFile)
{
    mLoadedModel = loadModel(srcFile);
    return mLoadedModel != nullptr;
}

}

#endif
