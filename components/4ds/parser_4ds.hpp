#ifndef FORMAT_PARSERS_4DS_H
#define FORMAT_PARSERS_4DS_H

#include <base_parser.hpp>
#include <loader_cache.hpp>
#include <cstring>
#include <utils/math.hpp>

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
        uint32_t mOneWeightedVertCount; // amount of vertices that should have a weight of 1.0f
        uint32_t mWeightCount; // amount of vertices whose weights are stored in mWeights
        uint32_t mBoneID; // this is likely a reference to a paired bone, which takes the remainder (1.0f - w) of weight
        MFMath::Vec3 mMinBox;
        MFMath::Vec3 mMaxBox;
        std::vector<float> mWeights;
    } SingleMeshLodJoint;

    typedef struct
    {
        uint8_t mJointCount;
        uint32_t mNonWeightedVertCount;
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

        MFMath::Mat4 computeWorldTransform(uint16_t meshIndex)
        {
            meshIndex += 1;  // convert to 1-based

            MFMath::Mat4 result = MFMath::identity;

            while (meshIndex > 0 && meshIndex <= mMeshCount)
            {
                Mesh *m = &(mMeshes[meshIndex - 1]);
                MFMath::Mat4 meshTransform = MFMath::translationMatrix(MFMath::Vec3(m->mPos.x,m->mPos.y,m->mPos.z));

                meshTransform = MFMath::mul(meshTransform,MFMath::rotationMatrix(MFMath::Quat(m->mRot.x,m->mRot.y,m->mRot.z,m->mRot.w)));
                meshTransform = MFMath::mul(meshTransform,MFMath::scalingMatrix(MFMath::Vec3(m->mScale.x,m->mScale.y,m->mScale.z)));

                result = MFMath::mul(meshTransform,result);
                meshIndex = m->mParentID;
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

}

#endif
