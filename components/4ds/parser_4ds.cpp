#include <4ds/parser_4ds.hpp>

namespace MFFormat
{

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
    read(file, &newJoint.mOneWeightedVertCount);
    read(file, &newJoint.mWeightCount);
    read(file, &newJoint.mBoneID);
    read(file, &newJoint.mMinBox);
    read(file, &newJoint.mMaxBox);
  
    for (size_t i = 0; i < newJoint.mWeightCount; ++i)
    {
        float f;
        read(file, &f, sizeof(f));
        newJoint.mWeights.push_back(f);
    }

    return newJoint;
}

DataFormat4DS::SingleMeshLod DataFormat4DS::loadSingleMeshLod(std::ifstream &file)
{
    // Every LOD's vertext buffer is sorted in the following order:
    // - non-weighted vertices
    // - BONE0's fully-weighted vertices (1.0f weight)
    // - BONE0's weighted vertices
    // - BONE1's fully-weighted vertices (1.0f weight)
    // - BONE1's weighted vertices
    // and so on
    SingleMeshLod newLod = {};
    read(file, &newLod.mJointCount);
    read(file, &newLod.mNonWeightedVertCount);
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
