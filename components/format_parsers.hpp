#ifndef FORMAT_PARSERS_H
#define FORMAT_PARSERS_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdint>
#include <vector>
#include <unordered_map>
#include <string>
#include <logger_console.hpp>

namespace MFFormat
{

/// Abstract class representing a game data format.

class DataFormat
{
public:
    virtual bool load(std::ifstream &srcFile)=0;
    virtual bool save(std::ofstream &dstFile) { return false; /* optional */ };

    #pragma pack(push, 1)
    typedef struct
    {
        float x;
        float y;
        float z;

        inline std::string str()
        {
            std::stringstream sstream;

            sstream << x << ", " << y << ", " << z;

            return sstream.str();
        }
    } Vec3;

    typedef struct
    {
        float x;
        float y;

        inline std::string str()
        {
            std::stringstream sstream;

            sstream << x << ", " << y;

            return sstream.str();
        }
    } Vec2;

    typedef struct
    {
        float x;
        float y;
        float z;
        float w;

        inline std::string str()
        {
            std::stringstream sstream;

            sstream << x << ", " << y << ", " << z << ", " << w;

            return sstream.str();
        }
    } Quat;

    typedef struct
    {
        float a0, a1, a2, a3;
        float b0, b1, b2, b3;
        float c0, c1, c2, c3;
        float d0, d1, d2, d3;
    } Mat4;

    #pragma pack(pop)
protected:
    template<typename T>
    void read(std::ifstream & stream, T* a, size_t size = sizeof(T))
    {
        stream.read((char*)a, size);
    }

    std::streamsize fileLength(std::ifstream &f);
};

class DataFormatScene2BIN: public DataFormat
{
public:
    typedef enum {
        // top nodes
        NODE_MISSION = 0x4c53,
        NODE_META = 0x0001,
        NODE_UNK_FILE = 0xAFFF,
        NODE_UNK_FILE2 = 0x3200,
        NODE_FOV = 0x3010,
        NODE_VIEW_DISTANCE = 0x3011,
        NODE_CLIPPING_PLANES = 0x3211,
        NODE_WORLD = 0x4000,
        NODE_ENTITIES = 0xAE20,
        NODE_INIT = 0xAE50,
        // WORLD subnode
        NODE_OBJECT = 0x4010
    } NodeType;

    typedef enum {
        OBJECT_TYPE = 0x4011,
        OBJECT_POSITION = 0x0020,
        OBJECT_ROTATION = 0x0022,
        OBJECT_POSITION_2 = 0x002C,
        OBJECT_SCALE = 0x002D,
        OBJECT_PARENT = 0x4020,
        OBJECT_NAME = 0x0010, 
        OBJECT_MODEL = 0x2012,
        OBJECT_LIGHT_TYPE = 0x4041,
        OBJECT_LIGHT_COLOUR = 0x0026,
        OBJECT_LIGHT_POWER = 0x4042,
        OBJECT_LIGHT_UNK_1 = 0x4043,
        OBJECT_LIGHT_RANGE = 0x4044,
        OBJECT_LIGHT_FLAGS = 0x4045,
        OBJECT_LIGHT_SECTOR= 0x4046
    } ObjectProperty;

    typedef enum {
        OBJECT_TYPE_LIGHT = 0x02,
        OBJECT_TYPE_CAMERA = 0x03,
        OBJECT_TYPE_SOUND = 0x04,
        OBJECT_TYPE_MODEL = 0x09,
        OBJECT_TYPE_OCCLUDER = 0x0C,
        OBJECT_TYPE_SECTOR = 0x99,
        OBJECT_TYPE_SCRIPT = 0x9B
    } ObjectType;

    #pragma pack(push, 1)
    typedef struct
    {
        uint16_t mType;
        uint32_t mSize;
    } Node;
    #pragma pack(pop)

    typedef struct _Object
    {
        uint32_t mType;
        Vec3 mPos;
        Quat mRot;
        Vec3 mPos2; // NOTE(zaklaus): Final world transform position?
        Vec3 mScale;
        std::string mName;
        std::string mModelName;
        std::string mParentName;

        // Light properties
        float mLightType;
        Vec3 mLightColour;
        int32_t mLightFlags;
        float mLightPower;
        float mLightUnk0;
        float mLightUnk1;
        float mLightNear;
        float mLightFar;
        char mLightSectors[5000];
    } Object;

    virtual bool load(std::ifstream &srcFile);
    
    size_t  getNumObjects();
    Object* getObject(size_t index);
    Object* getObject(std::string name);
    std::unordered_map<std::string, Object> getObjects();

    float getFov();
    void  setFov(float value);

    float getViewDistance();
    void  setViewDistance(float value);

    Vec2  getClippingPlanes();
    void  setClippingPlanes(Vec2 value);
private:
    void readNode(std::ifstream &srcFile, Node* node, uint32_t offset);
    void readObject(std::ifstream &srcFile, Node* node, Object* object);
    
    std::unordered_map<std::string, Object> mObjects;
    float mFov;
    float mViewDistance;
    Vec2  mClippingPlanes;
};

class DataFormatCacheBIN: public DataFormat
{
public:
    #pragma pack(push, 1)
    typedef struct
    {
        uint16_t mType;
        uint32_t mSize;
    } Header;
    #pragma pack(pop)

    typedef struct
    {
        Header mHeader;
        std::string mModelName;
        Vec3 mPos;
        Quat mRot;
        Vec3 mScale;
        uint32_t mUnk0;

        // NOTE(zaklaus): This field is rarely used for some instances
        // around the docks or right next to Central Hoboken station.
        Vec3 mScale2;
    } Instance;

    typedef struct
    {
        Header mHeader;
        std::string mObjectName;
        int8_t mBounds[0x4C];
        std::vector<Instance> mInstances;
    } Object;

    typedef struct
    {
        uint32_t mVersion; // NOTE(zaklaus): Should always be 1.
    } Chunk;

    virtual bool load(std::ifstream &srcFile) override;
    std::vector<Object> getObjects();
    size_t              getNumObjects();
    Object*             getObject(size_t index);
private:
    std::vector<Object> mObjects;
};

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
        float mTransparency; // 0.0 - invisible; 1.0 - solid

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
    Model* getModel();

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
    Model* mLodedModel;
};

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
        void addMovement(Vec3& data);
        void addRotation(Vec3& data);
        void addScale(Vec3& data);
        const std::string getName();
        uint16_t getCount() const;
        const uint32_t& getTimestamp(uint16_t id) const;
        const Vec3& getMovement(uint16_t id) const;
        const Vec3& getRotation(uint16_t id) const;
        const Vec3& getScale(uint16_t id) const;
        bool hasMovement() const;
        bool hasRotation() const;
        bool hasScale() const;
    private:
        std::string mObjectName;
        std::vector<uint32_t> mTimestamps;
        std::vector<Vec3> mMovements;
        std::vector<Vec3> mRotations;
        std::vector<Vec3> mScale;
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

class DataFormatDTA: public DataFormat
{
public:
    virtual bool load(std::ifstream &srcFile) override; ///< Loads the file table from the DTA file.
    void setDecryptKeys(uint32_t key1, uint32_t key2);  ///< Decrypting keys have to be set before load(...) is called.
    void setDecryptKeys(uint32_t keys[2]);
    unsigned int getNumFiles();                         ///< Get the number of files inside the DTA.
    unsigned int getFileSize(unsigned int index);
    std::string getFileName(unsigned int index);
    void getFile(std::ifstream &srcFile, unsigned int index, char **dstBuffer, unsigned int &length);   ///< Get the concrete file from within the DST file into a buffer.

    static uint32_t A0_KEYS[2];   // decrypting keys
    static uint32_t A1_KEYS[2];
    static uint32_t A2_KEYS[2];
    static uint32_t A3_KEYS[2];
    static uint32_t A4_KEYS[2];
    static uint32_t A5_KEYS[2];
    static uint32_t A6_KEYS[2];
    static uint32_t A7_KEYS[2];
    // TODO: A8 ?
    static uint32_t A9_KEYS[2];
    static uint32_t AA_KEYS[2];
    static uint32_t AB_KEYS[2];
    static uint32_t AC_KEYS[2];

    typedef struct
    {
        uint32_t mFileCount;
        uint32_t mContentOffset;
        uint32_t mContentSize;
        uint32_t mUnknown;
    } FileHeader;

    typedef struct
    {
        uint32_t mUnknown;         // ID or seq number?
        uint32_t mDataOffset;
        uint32_t mDataEnd;
        char mName[16];
    } ContentHeader;

    typedef struct
    {
        uint32_t mUnknown;
        uint32_t mUnknown2;
        uint32_t mUnknown3;
        uint32_t mUnknown4;
        uint32_t mSize;
        uint32_t mUnknown6;
        unsigned char mNameLength;
        unsigned char mUnknown7[7];
        unsigned char mName[256];
    } DataHeader;

protected:
    void decrypt(char *buffer, unsigned int bufferLen, uint32_t key1, uint32_t key2);

    FileHeader mFileHeader;
    std::vector<ContentHeader> mContentHeaders;
    std::vector<DataHeader> mDataHeaders;
    uint32_t mKey1;
    uint32_t mKey2;
}; 

uint32_t DataFormatDTA::A0_KEYS[2] = {0x7f3d9b74, 0xec48fe17};
uint32_t DataFormatDTA::A1_KEYS[2] = {0xe7375f59, 0x900210e};
uint32_t DataFormatDTA::A2_KEYS[2] = {0x1417d340, 0xb6399e19};
uint32_t DataFormatDTA::A3_KEYS[2] = {0xa94b8d3c, 0x771f3888};
uint32_t DataFormatDTA::A4_KEYS[2] = {0xa94b8d3c, 0x771f3888};
uint32_t DataFormatDTA::A5_KEYS[2] = {0x4f4bb0c6, 0xea340420};
uint32_t DataFormatDTA::A6_KEYS[2] = {0x728e2db9, 0x5055da68};
uint32_t DataFormatDTA::A7_KEYS[2] = {0xf4f03a72, 0xe266fe62};
// TODO: A8
uint32_t DataFormatDTA::A9_KEYS[2] = {0x959d1117, 0x5b763446};
uint32_t DataFormatDTA::AA_KEYS[2] = {0xd4ad90c6, 0x67da216e};
uint32_t DataFormatDTA::AB_KEYS[2] = {0x7f3d9b74, 0xec48fe17};
uint32_t DataFormatDTA::AC_KEYS[2] = {0xa94b8d3c, 0x771f3888};

}

#endif
