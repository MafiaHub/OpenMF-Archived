#include <osg/Node>
#include <osg/Geometry>
#include <osg/Geode>
#include <fstream>

namespace MFFormat
{

class Loader
{
public:
    osg::ref_ptr<osg::Node> load4ds(std::ifstream &srcFile);
};

namespace MaterialFlag
{

	enum
	{
		TextureDiffuse = 0x00040000,
		Colored = 0x08000000,
		MipMapping = 0x00800000,
		AnimatedTextureDiffuse = 0x04000000,
		AnimatedTextureAlpha = 0x02000000,
		DoubleSidedMaterial = 0x10000000,
		EnvironmentMap = 0x00080000,
		NormalTextureBlend = 0x00000100,
		MultiplyTextureBlend = 0x00000200,
		AdditiveTextureBlend = 0x00000400,
		CalcReflectTextureY = 0x00001000,
		ProjectReflectTextureY = 0x00002000,
		ProjectReflectTextureZ = 0x00004000,
		AdditionalEffect = 0x00008000,
		AlphaTexture = 0x40000000,
		ColorKey = 0x20000000,
		AdditiveMixing = 0x80000000
	};

};

namespace MeshType
{
	enum
	{
		Standard = 0x01,
		Sector = 0x05,
		Dummy = 0x06,
		Target = 0x07,
		Bone = 0x0a
	};
};

namespace VisualMeshType
{
	enum
	{
		Standard = 0x0,
		SingleMesh = 0x02,
		SingleMorph = 0x03,
		Billboard = 0x04,
		Morph = 0x05,
		Glow = 0x06,
		Mirror = 0x08
	};
};


namespace MeshRenderFlag
{
	enum
	{

		UseDepthBias = 0x0001,
		UseShadows = 0x0002,
		Unknown = 0x0008,
		UseProjection = 0x0020,
		ForbidFog = 0x0080
	};
};

namespace MeshOccludingFlags
{
	enum 
	{
		Normal = 0x09,
		Sector = 0x7D,
		Wall = 0x3D, // NOTE(zaklaus): Mesh in sector (walls).
		Portal = 0x1D,   // NOTE(zaklaus): Mesh in portal.
		Inactive = 0x11
	};
};

struct Material
{
    uint32_t mFlags;
    
    osg::Vec3f mAmbient;
    osg::Vec3f mDiffuse;
    osg::Vec3f mEmission;
    
    float mTransparency; // 0.0 - invisible; 1.0 - solid
    
    // NOTE(zaklaus): Environment map
    float mEnvRatio;
    char mEnvMapNameLength;
    char mEnvMapName[255];
    
    char mDiffuseMapNameLength;
    char mDiffuseMapName[255];
    
    // NOTE(zaklaus): Alpha map
    char mAlphaMapNameLength;
    char mAlphaMapName[255];
    
    // NOTE(zaklaus): Anim map
    uint32_t mAnimSequenceLength;
    uint16_t mUnk0;
    uint32_t mFramePeriod;
    uint32_t mUnk1;
    uint32_t mUnk2;
    
};

#pragma pack(push, 1)
struct Vertex
{
    osg::Vec3f mPos;
    osg::Vec3f mNormal;
    osg::Vec2f mUV;
};

struct Face
{
    uint16_t mA;
    uint16_t mB;
    uint16_t mC;
};
#pragma pack(pop)

struct FaceGroup
{
    uint16_t mFaceCount;
    Face *mFaces;
    uint16_t mMaterialID;
};

struct Lod
{
    float mRelativeDistance;
    uint16_t mVertexCount;
    Vertex *mVertices;
    uint8_t mFaceGroupCount;
    FaceGroup *mFaceGroups;
};

struct Standard
{
    uint16_t mInstanced;
    
    // NOTE(zaklaus): For non-instanced mesh
    uint8_t mLODLevel;
    Lod *mLODs;
};

struct Target
{
    uint16_t mUnk0;
    uint8_t mTargetCount;
    uint16_t *mTargets;
};

#pragma pack(push, 1)
struct Bone
{
    osg::Matrixf mTransform;
    uint32_t mBoneID;
};
#pragma pack(pop)

struct Portal
{
    uint8_t mVertexCount;
    uint32_t mUnk0; // NOTE(zaklaus): Always 4.
    uint32_t mUnk1[6];
    osg::Vec3f *mVertices;   
};

struct Sector
{
    uint32_t mUnk0; // NOTE(zaklaus): Always 2049.
    uint32_t mUnk1; // NOTE(zaklaus): Always 0.
    uint32_t mVertexCount;
    uint32_t mFaceCount;
    osg::Vec3f *mVertices;
    Face *mFaces;
    osg::Vec3f mMinBox;
    osg::Vec3f mMaxBox;
    uint8_t mPortalCount;
    Portal *mPortals;
    
};

struct Billboard
{
    Standard mStandard;
    uint32_t mRotationAxis;
    uint8_t mIgnoreCamera;
};

struct Dummy
{
    // NOTE(zaklaus): Bounding box
    osg::Vec3f mMinBox;
    osg::Vec3f mMaxBox;
};

struct GlowData
{
    float mPosition;
    uint16_t mMaterialID;
};

struct Glow
{
    uint8_t mGlowCount;
    GlowData *mGlowData;
};

struct Mirror
{
    osg::Vec3f mMinBox;
    osg::Vec3f mMaxBox;
    float mUnk[4];
    osg::Matrixf mReflectionMatrix;
    osg::Vec3f mBackgroundColor;
    float mViewDistance;
    uint32_t mVertexCount;
    uint32_t mFaceCount;
    osg::Vec3f *mVertices;
    Face *mFaces;
};

struct MorphLodVertex
{
    osg::Vec3f mPosition;
    osg::Vec3f mNormals;
};

struct MorphLod
{
    uint16_t mVertexCount;
    MorphLodVertex *mVertices;  
    uint8_t mUnk0;
    uint16_t *mVertexLinks; // NOTE(zaklaus): Addresses vertices from Standard's LOD mesh.
};

struct Morph
{
    Standard mStandard;
    uint8_t mFrameCount;
    uint8_t mLODLevel; // NOTE(zaklaus): Should be equal to Standard.LODLevel.
    uint8_t mUnk0;
    MorphLod *mLODs;
    osg::Vec3f mMinBox;
    osg::Vec3f mMaxBox;
    float mUnk1[4];
};

struct SingleMeshLodJoint
{
    osg::Matrixf mTransform;
    uint32_t mUnk0;
    uint32_t mAdditionalValuesCount;
    uint32_t mBoneID;
    osg::Vec3f mMinBox;
    osg::Vec3f mMaxBox;
    float *mAdditionalValues;
};

struct SingleMeshLod
{
    uint8_t mJointCount;
    uint32_t mUnk0;
    osg::Vec3f mMinBox;
    osg::Vec3f mMaxBox;
    SingleMeshLodJoint *mJoints;
};

struct SingleMesh
{
    Standard mStandard;
    SingleMeshLod *mLODs; // NOTE(zaklaus): LODLevel == Standard.LODLevel.
};

struct SingleMorph
{
    SingleMesh mSingleMesh;
    Morph mMorph; // NOTE(zaklaus): Morph without Standard Mesh!
};

struct Mesh
{
    uint8_t mMeshType;
    
    // NOTE(zaklaus): Standard mesh type
    uint8_t mVisualMeshType;
    uint16_t mMeshRenderFlags;
    uint16_t mParentID; // 0 - not connected
    
    osg::Vec3f mPos;
    osg::Vec3f mScale;
    osg::Quat mRot;
    
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
    
    
};

struct Header
{
    uint8_t mSignature[4];
    uint16_t mFormatVersion; // PC : 0x1D (29)
    uint64_t mTimestamp;
    
    uint16_t mMaterialCount;
    Material *mMaterials;
    
    uint16_t mMeshCount;
    Mesh *mMeshes;
    uint8_t mUse5DS;
};

template<typename T>
void read(std::ifstream & stream , T a, size_t size = 0) 
{
    size_t current_size = !size ? sizeof(T) : size;
    stream.read(reinterpret_cast<char*>(a), current_size);
}

void loadMaterial(Header *model, std::ifstream & file)
{
    read(file, &model->mMaterialCount);
    model->mMaterials = reinterpret_cast<Material*>(malloc(sizeof(Material) * model->mMaterialCount));
    
    for(size_t i = 0; i < model->mMaterialCount; ++i) 
    {
        Material mat;
        read(file, &mat.mFlags);
        read(file, &mat.mAmbient);
        read(file, &mat.mDiffuse);
        read(file, &mat.mEmission);
        read(file, &mat.mTransparency);

        if(mat.mFlags & MaterialFlag::EnvironmentMap) 
        {
            read(file, &mat.mEnvRatio);
            read(file, &mat.mEnvMapNameLength);
            read(file, &mat.mEnvMapName, mat.mEnvMapNameLength);
        }

        read(file, &mat.mDiffuseMapNameLength);
        read(file, mat.mDiffuseMapName, mat.mDiffuseMapNameLength);
       
            
        if(mat.mFlags & MaterialFlag::AlphaTexture)
        {
            read(file, &mat.mAlphaMapNameLength);
            read(file, mat.mAlphaMapName, mat.mAlphaMapNameLength);
        }
        
        if(mat.mFlags & MaterialFlag::AnimatedTextureDiffuse)
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

Lod loadLod(std::ifstream & file)
{
    Lod new_lod = {};
    read(file, &new_lod.mRelativeDistance);
    read(file, &new_lod.mVertexCount);

    new_lod.mVertices = reinterpret_cast<Vertex*>(malloc(sizeof(Vertex)* new_lod.mVertexCount));
    read(file, new_lod.mVertices, sizeof(Vertex) * new_lod.mVertexCount);
    read(file, &new_lod.mFaceGroupCount); 

    new_lod.mFaceGroups = reinterpret_cast<FaceGroup*>(malloc(sizeof(FaceGroup)*new_lod.mFaceGroupCount));
    
    for(size_t i = 0; i < new_lod.mFaceGroupCount; ++i)
    {
        FaceGroup new_face_group = {};
        read(file, &new_face_group.mFaceCount);
        new_face_group.mFaces = reinterpret_cast<Face*>(malloc(sizeof(Face)*new_face_group.mFaceCount));
        read(file, new_face_group.mFaces, sizeof(Face)*new_face_group.mFaceCount);
        read(file, &new_face_group.mMaterialID);

        new_lod.mFaceGroups[i] = new_face_group;
    }

    return new_lod;
}

Standard loadStandard(std::ifstream & file)
{
    Standard new_standard = {};
    read(file, &new_standard.mInstanced);

    if(!new_standard.mInstanced)
    {
        read(file, &new_standard.mLODLevel);
        new_standard.mLODs = reinterpret_cast<Lod*>(malloc(sizeof(Lod)*new_standard.mLODLevel));

        for(size_t i = 0; i < new_standard.mLODLevel; ++i)
        {
            Lod new_lod = {};
            new_lod = loadLod(file);
            new_standard.mLODs[i] = new_lod;
        }
    }

    return new_standard;
}

Mirror loadMirror(std::ifstream & file)
{
    Mirror new_mirror = {};
    read(file, &new_mirror.mMinBox);
    read(file, &new_mirror.mMaxBox);
    read(file, &new_mirror.mUnk, sizeof(float) * 4);
    read(file, &new_mirror.mReflectionMatrix);
    read(file, &new_mirror.mBackgroundColor);
    read(file, &new_mirror.mViewDistance);
    read(file, &new_mirror.mVertexCount);
    read(file, &new_mirror.mFaceCount);

    new_mirror.mVertices = reinterpret_cast<osg::Vec3f*>(malloc(sizeof(osg::Vec3f)*new_mirror.mVertexCount));
    new_mirror.mFaces = reinterpret_cast<Face*>(malloc(sizeof(Face)*new_mirror.mFaceCount));
    
    read(file, new_mirror.mVertices, sizeof(osg::Vec3f)*new_mirror.mVertexCount);
    read(file, new_mirror.mFaces, sizeof(Face)*new_mirror.mFaceCount);
    
    return new_mirror;
}

Glow loadGlow(std::ifstream & file)
{
    Glow new_glow = {};
    read(file, &new_glow.mGlowCount);
    
    new_glow.mGlowData = reinterpret_cast<GlowData*>(malloc(sizeof(GlowData)*new_glow.mGlowCount));
    
    for(size_t i = 0; i < new_glow.mGlowCount; ++i)
    {
        GlowData new_glow_data = {};
        read(file, &new_glow_data.mPosition);
        read(file, &new_glow_data.mMaterialID);
        
        new_glow.mGlowData[i] = new_glow_data;
    }
    
    return new_glow;
}

Portal loadPortal(std::ifstream & file)
{
    Portal new_portal = {};
    read(file, &new_portal.mVertexCount);
    read(file, &new_portal.mUnk0);
    read(file, new_portal.mUnk1, sizeof(float) * 6);
    new_portal.mVertices = reinterpret_cast<osg::Vec3f*>(malloc(sizeof(osg::Vec3f)*new_portal.mVertexCount));
    read(file, new_portal.mVertices, sizeof(osg::Vec3f)*new_portal.mVertexCount); 
   
    return new_portal;
}

Sector loadSector(std::ifstream & file)
{
    Sector new_sector = {};
    read(file, &new_sector.mUnk0);
    read(file, &new_sector.mUnk1);
    read(file, &new_sector.mVertexCount);
    read(file, &new_sector.mFaceCount);

    new_sector.mVertices = reinterpret_cast<osg::Vec3f*>(malloc(sizeof(osg::Vec3f)*new_sector.mVertexCount));
    read(file, new_sector.mVertices, sizeof(osg::Vec3f)*new_sector.mVertexCount);

    new_sector.mFaces = reinterpret_cast<Face*>(malloc(sizeof(Face)*new_sector.mFaceCount));
    read(file, new_sector.mFaces, sizeof(Face) *new_sector.mFaceCount);

    read(file, &new_sector.mMinBox);
    read(file, &new_sector.mMaxBox);
    read(file, &new_sector.mPortalCount);

    new_sector.mPortals = reinterpret_cast<Portal*>(malloc(sizeof(Portal)*new_sector.mPortalCount));

    for(size_t i = 0; i < new_sector.mPortalCount; ++i) 
    {
        Portal new_portal = {};
        new_portal = loadPortal(file);
        new_sector.mPortals[i] = new_portal;
    }

    return new_sector;
}

Target loadTarget(std::ifstream & file)
{
    Target new_target = {};
    read(file, &new_target.mUnk0);
    read(file, &new_target.mTargetCount);
    
    new_target.mTargets = reinterpret_cast<uint16_t*>(malloc(sizeof(uint16_t)*new_target.mTargetCount));
    read(file, new_target.mTargets, sizeof(uint16_t)*new_target.mTargetCount);
    
    return new_target;
}

Morph loadMorph(std::ifstream & file, bool ignoreStandar)
{
    Morph new_morph = {0};
    // NOTE(zaklaus): Single Morph contains Standard Mesh in Single Mesh already.
    if(!ignoreStandar) 
    {
        new_morph.mStandard = loadStandard(file);
    } 
    // NOTE(zaklaus): ELSE ignore Standard Mesh, since Single Mesh has it.
    
    read(file, &new_morph.mFrameCount);
    
    if(new_morph.mFrameCount)
    {
        read(file, &new_morph.mLODLevel);
        read(file, &new_morph.mUnk0);

        new_morph.mLODs = reinterpret_cast<MorphLod*>(malloc(sizeof(MorphLod)*new_morph.mLODLevel));

        for(size_t i = 0; i < new_morph.mLODLevel; ++i) 
        {
            MorphLod new_morph_lod = {};
            read(file, &new_morph_lod.mVertexCount);

            new_morph_lod.mVertices = reinterpret_cast<MorphLodVertex*>(malloc(sizeof(MorphLodVertex)*new_morph_lod.mVertexCount * new_morph.mFrameCount));
            read(file, new_morph_lod.mVertices, sizeof(MorphLodVertex)*new_morph_lod.mVertexCount * new_morph.mFrameCount);

            if(new_morph_lod.mVertexCount * new_morph.mFrameCount) 
            {
                read(file, &new_morph_lod.mUnk0);
            }

            new_morph_lod.mVertexLinks = reinterpret_cast<uint16_t*>(malloc(sizeof(uint16_t)*new_morph_lod.mVertexCount));
            read(file, new_morph_lod.mVertexLinks, sizeof(uint16_t)*new_morph_lod.mVertexCount);

            new_morph.mLODs[i] = new_morph_lod;
        }
        
        read(file, &new_morph.mMinBox);
        read(file, &new_morph.mMaxBox);
        read(file, new_morph.mUnk1, sizeof(float) * 4);

    }
    return new_morph;
}

SingleMeshLodJoint loadSingleMeshLodJoint(std::ifstream & file)
{
    SingleMeshLodJoint new_joint = {};
    read(file, &new_joint.mTransform);
    read(file, &new_joint.mUnk0, sizeof(uint32_t));
    read(file, &new_joint.mAdditionalValuesCount);
    read(file, &new_joint.mBoneID);
    read(file, &new_joint.mMinBox);
    read(file, &new_joint.mMaxBox);

    new_joint.mAdditionalValues = reinterpret_cast<float*>(malloc(sizeof(float)*new_joint.mAdditionalValuesCount));
    read(file, new_joint.mAdditionalValues, sizeof(float)*new_joint.mAdditionalValuesCount);
   
    return new_joint;
}

SingleMeshLod loadSingleMeshLod(std::ifstream & file)
{
    SingleMeshLod new_lod = {};
    read(file, &new_lod.mJointCount);
    read(file, &new_lod.mUnk0);
    read(file, &new_lod.mMinBox);
    read(file, &new_lod.mMaxBox);

    new_lod.mJoints = reinterpret_cast<SingleMeshLodJoint*>(malloc(sizeof(SingleMeshLodJoint)*new_lod.mJointCount));
    for(size_t i = 0; i < new_lod.mJointCount; ++i) 
    {
        SingleMeshLodJoint new_joint = {};
        new_joint = loadSingleMeshLodJoint(file);
        new_lod.mJoints[i] = new_joint;
    }  
      
    return(new_lod);
}

SingleMesh loadSingleMesh(std::ifstream & file)
{
    SingleMesh new_mesh = {};
    
    new_mesh.mStandard = loadStandard(file);
    
    new_mesh.mLODs = reinterpret_cast<SingleMeshLod*>(malloc(sizeof(SingleMeshLod)*new_mesh.mStandard.mLODLevel));
    
    for(size_t i = 0; i < new_mesh.mStandard.mLODLevel; ++i) 
    {
        SingleMeshLod new_lod = {};
        new_lod = loadSingleMeshLod(file);
        new_mesh.mLODs[i] = new_lod;
    }
    
    return new_mesh;
}

SingleMorph loadSingleMorph(std::ifstream & file)
{
    SingleMorph new_morph = {};
    new_morph.mSingleMesh = loadSingleMesh(file);

    new_morph.mMorph = loadMorph(file, 1);
    return new_morph;
}

void loadMesh(Header *model, std::ifstream & file)
{
    read(file, &model->mMeshCount);
    model->mMeshes = reinterpret_cast<Mesh*>(malloc(sizeof(Mesh)*model->mMeshCount));

    for(size_t i = 0; i < model->mMeshCount; ++i)
    {
        Mesh new_mesh = {};
        read(file, &new_mesh.mMeshType);

        if(new_mesh.mMeshType == MeshType::Standard)
        {
            read(file, &new_mesh.mVisualMeshType);
            read(file, &new_mesh.mMeshRenderFlags);
        }

        read(file, &new_mesh.mParentID);
        read(file, &new_mesh.mPos);
        read(file, &new_mesh.mScale);

		float r[4];
        read(file, r, sizeof(float) * 4);
        new_mesh.mRot = osg::Quat(r[1], r[2], r[3], -r[0]);

        read(file, &new_mesh.mCullingFlags);
        read(file, &new_mesh.mMeshNameLength);
        read(file, new_mesh.mMeshName, new_mesh.mMeshNameLength);
        read(file, &new_mesh.mMeshParamsLength);
        read(file, new_mesh.mMeshParams, new_mesh.mMeshParamsLength);

        switch(new_mesh.mMeshType)
        {
			case MeshType::Standard:
            {
                switch(new_mesh.mVisualMeshType)
                {
                    case VisualMeshType::Standard:
                    {
                        Standard new_standard = {};
                        new_standard = loadStandard(file);
                        new_mesh.mStandard = new_standard;
                    } 
                    break;

                    case VisualMeshType::Mirror:
                    {
                        Mirror new_mirror = {};
						new_mirror = loadMirror(file);
                        new_mesh.mMirror = new_mirror;
                    } 
                    break;

                    case VisualMeshType::Glow:
                    {
                        Glow new_glow = {};
                        new_glow = loadGlow(file);
                        new_mesh.mGlow = new_glow;
                    } 
                    break;

                    case VisualMeshType::Billboard:
                    {
                        Billboard new_billboard = {};
                        new_billboard.mStandard = loadStandard(file);
                        read(file, &new_billboard.mRotationAxis);
                        read(file, &new_billboard.mIgnoreCamera);
                        new_mesh.mBillboard = new_billboard;
                    } 
                    break;
                    
                    case VisualMeshType::Morph:
                    {
                        Morph new_morph = {};
                        new_morph = loadMorph(file, 0);
                        new_mesh.mMorph = new_morph;
                    }
                    break;
                                
                    case VisualMeshType::SingleMesh:
                    {
                        SingleMesh new_single_mesh = {};
                        new_single_mesh = loadSingleMesh(file);
                        new_mesh.mSingleMesh = new_single_mesh;
                    }
                    break;
                    
                    case VisualMeshType::SingleMorph:
                    {
                        SingleMorph new_single_morph = {0};
                        new_single_morph = loadSingleMorph(file);
                        new_mesh.mSingleMorph = new_single_morph;
                    }
                    break;
                }
            }

            case MeshType::Dummy:
            {
                Dummy new_dummy = {};
                read(file, &new_dummy.mMinBox);
                read(file, &new_dummy.mMaxBox);
                new_mesh.mDummy = new_dummy;
            }
            break;
            
            case MeshType::Sector:
            {
                Sector new_sector = {};
                new_sector = loadSector(file);
                new_mesh.mSector = new_sector;
            }
            break;
            
            case MeshType::Target:
            {
                Target new_target = {};
                new_target = loadTarget(file);
                new_mesh.mTarget = new_target;
            }
            break;
            
            case MeshType::Bone:
            {
                Bone new_bone = {};
                read(file, &new_bone.mTransform);
                read(file, &new_bone.mBoneID);
                new_mesh.mBone = new_bone;
            }
            break;
        }

        model->mMeshes[i] = new_mesh;
    }
}

Header* loadModel(std::ifstream & file)
{
    Header *model = reinterpret_cast<Header*>(malloc(sizeof(Header)));
    read(file, &model->mSignature, 4);
    //TODO(DavoSK): Add check for proper format signature contains 4DS word in ASCII 
    
    read(file, &model->mFormatVersion);
    read(file, &model->mTimestamp);

    loadMaterial(model, file);
    loadMesh(model, file);
    read(file, &model->mUse5DS);
    
    return model;
}

osg::ref_ptr<osg::Node> Loader::load4ds(std::ifstream & srcFile)
{
	auto ulozene = loadModel(srcFile);
	//ulozene->mMeshes[0].mStandard.mLODs[0].
	auto lod = ulozene->mMeshes[0].mStandard.mLODs[0];
	
	osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
    osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array;
    osg::ref_ptr<osg::Vec2Array> uvs = new osg::Vec2Array;

	printf("Vertex Count: %d\n", lod.mVertexCount);
	for (size_t i = 0; i < lod.mVertexCount; ++i)
	{
		auto vertex = lod.mVertices[i];
		vertices->push_back(vertex.mPos);
		normals->push_back(vertex.mNormal);
		uvs->push_back(vertex.mUV);
	}



    osg::ref_ptr<osg::DrawElementsUInt> indices = new osg::DrawElementsUInt(GL_TRIANGLES,6);

	printf("Vertex Count: %d\n", lod.mFaceGroups[0].mFaceCount);
	for (size_t i = 0; i < lod.mFaceGroups[0].mFaceCount; ++i) 
	{
		auto face = lod.mFaceGroups[0].mFaces[i];
		indices->push_back(face.mA);
		indices->push_back(face.mB);
		indices->push_back(face.mC);

	}
    osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;

    geom->setVertexArray(vertices.get());
    geom->setNormalArray(normals.get());
    geom->setTexCoordArray(0,uvs.get());

    geom->addPrimitiveSet(indices.get());

    osg::ref_ptr<osg::Geode> geode = new osg::Geode;
    geode->addDrawable(geom.get());

    return geode;
}

}
