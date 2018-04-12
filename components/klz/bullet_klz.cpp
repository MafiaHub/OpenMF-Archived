#include <klz/bullet_klz.hpp>

namespace MFPhysics
{

void BulletStaticCollisionLoader::load(MFFormat::DataFormatTreeKLZ *klz, MFFormat::DataFormat4DS &scene4ds)
{
    std::vector<std::string> linkStrings = klz->getLinkStrings();

    #define loopBegin(getFunc)\
    {\
        auto cols = klz->getFunc(); \
        for (int i = 0; i < (int) cols.size(); ++i)\
        {\
            auto col = cols[i];\
            MFUtil::NamedRigidBody newBody;\

    #define loopEnd \
            newBody.mName = linkStrings[cols[i].mLink];\
            mRigidBodies.push_back(newBody);\
        }\
    }

    loopBegin(getAABBCols)
        btVector3 p1 = MFUtil::mafiaVec3ToBullet(col.mMin.x,col.mMin.y,col.mMin.z);
        btVector3 p2 = MFUtil::mafiaVec3ToBullet(col.mMax.x,col.mMax.y,col.mMax.z);

        btVector3 center = (p1 + p2) / 2.0f;
        btVector3 bboxCorner = p2 - center;

        newBody.mRigidBody.mShape = std::make_shared<btBoxShape>(bboxCorner);

        btRigidBody::btRigidBodyConstructionInfo ci(0,0,newBody.mRigidBody.mShape.get());
        newBody.mRigidBody.mBody = std::make_shared<btRigidBody>(ci);
        newBody.mRigidBody.mBody->setCollisionFlags(newBody.mRigidBody.mBody->getCollisionFlags() | btCollisionObject::CF_STATIC_OBJECT);
        newBody.mRigidBody.mBody->translate(center);
    loopEnd

    loopBegin(getSphereCols)
        btVector3 center = MFUtil::mafiaVec3ToBullet(col.mPosition.x,col.mPosition.y,col.mPosition.z);
        float radius = col.mRadius;

        newBody.mRigidBody.mShape = std::make_shared<btSphereShape>(radius);

        btRigidBody::btRigidBodyConstructionInfo ci(0,0,newBody.mRigidBody.mShape.get());
        newBody.mRigidBody.mBody = std::make_shared<btRigidBody>(ci);
        newBody.mRigidBody.mBody->setCollisionFlags(newBody.mRigidBody.mBody->getCollisionFlags() | btCollisionObject::CF_STATIC_OBJECT);
        newBody.mRigidBody.mBody->translate(center);
    loopEnd

    #define loadOBBOrXTOBB \
        btVector3 p1 = MFUtil::mafiaVec3ToBullet(col.mExtends[0].x,col.mExtends[0].y,col.mExtends[0].z); \
        btVector3 p2 = MFUtil::mafiaVec3ToBullet(col.mExtends[1].x,col.mExtends[1].y,col.mExtends[1].z); \
        btVector3 center = (p1 + p2) / 2.0f; \
        btVector3 bboxCorner = p2 - center; \
        btTransform transform = MFUtil::mafiaMat4ToBullet(col.mTransform); \
        newBody.mRigidBody.mShape = std::make_shared<btBoxShape>(bboxCorner); \
        btRigidBody::btRigidBodyConstructionInfo ci(0,0,newBody.mRigidBody.mShape.get()); \
        newBody.mRigidBody.mBody = std::make_shared<btRigidBody>(ci); \
        newBody.mRigidBody.mBody->setCollisionFlags(newBody.mRigidBody.mBody->getCollisionFlags() | btCollisionObject::CF_STATIC_OBJECT); \
        newBody.mRigidBody.mBody->setWorldTransform(transform);

    loopBegin(getOBBCols)
        loadOBBOrXTOBB
    loopEnd

    loopBegin(getXTOBBCols)
        loadOBBOrXTOBB
    loopEnd
    
    loopBegin(getCylinderCols)
        btVector3 center = btVector3(col.mPosition.x,col.mPosition.y,0);
        float radius = col.mRadius;

        newBody.mRigidBody.mShape = std::make_shared<btCylinderShapeZ>(btVector3(radius,0,200.0));  // FIXME: cylinder height infinite?
        btRigidBody::btRigidBodyConstructionInfo ci(0,0,newBody.mRigidBody.mShape.get());
        newBody.mRigidBody.mBody = std::make_shared<btRigidBody>(ci);
        newBody.mRigidBody.mBody->setCollisionFlags(newBody.mRigidBody.mBody->getCollisionFlags() | btCollisionObject::CF_STATIC_OBJECT);
        newBody.mRigidBody.mBody->translate(center);
    loopEnd

    // load face collisions:

    auto cols = klz->getFaceCols();

    int currentLink = -1;
    MeshFaceCollision faceCol;

    // TODO: the following loop supposes the cols are grouped by link, find out whether that is always true

    for (int i = 0; i < (int) cols.size(); ++i)
    {
        auto col = cols[i];

        FaceIndices face;
        face.mI1 = col.mIndices[0].mIndex;
        face.mI2 = col.mIndices[1].mIndex;
        face.mI3 = col.mIndices[2].mIndex;

        if (currentLink < 0 || currentLink != col.mIndices[0].mLink)
        {
            // start loading a new mesh
            
            if (currentLink >= 0)
                mFaceCollisions.push_back(faceCol);

            faceCol.mFaces.clear();
            currentLink = col.mIndices[0].mLink;

            faceCol.mMeshName = linkStrings[currentLink];
        }

        faceCol.mFaces.push_back(face);

        if (i == ((int) cols.size()) - 1)   // last one => push
            mFaceCollisions.push_back(faceCol);
    }

    // make the bodies now:
    
    auto model = scene4ds.getModel();

    for (int i = 0; i < (int) mFaceCollisions.size(); ++i)
    {
        MFFormat::DataFormat4DS::Mesh *m = 0;

        // find the corresponding mesh

        unsigned int meshIndex = 0;

        for (int j = 0; j < (int) model.mMeshCount; ++j)
        {
            MFFormat::DataFormat4DS::Mesh *mesh = &(model.mMeshes[j]);            
            std::string meshName = std::string(mesh->mMeshName,0,mesh->mMeshNameLength);

            if (meshName.compare(mFaceCollisions[i].mMeshName) == 0)
            {
                m = mesh;
                meshIndex = j;
                break;
            }
        }

        if (m == 0)
        {
            MFLogger::Logger::warn("Could not load face collisions for \"" + mFaceCollisions[i].mMeshName + "\" - link not found.",TREE_KLZ_BULLET_LOADER_MODULE_STR);
            continue;
        }

        if (m->mMeshType != MFFormat::DataFormat4DS::MESHTYPE_STANDARD &&
            m->mMeshType != MFFormat::DataFormat4DS::MESHTYPE_COLLISION)
        {
            MFLogger::Logger::warn("Could not load face collisions for \"" + mFaceCollisions[i].mMeshName + "\" - unsupported mesh type: " + std::to_string(m->mMeshType) + ".",TREE_KLZ_BULLET_LOADER_MODULE_STR);
            continue;
        }

        if (m->mStandard.mLODs.size() == 0)
        {
            MFLogger::Logger::warn("Could not load face collisions for \"" + mFaceCollisions[i].mMeshName + "\" - no LODs.",TREE_KLZ_BULLET_LOADER_MODULE_STR);
            continue;
        }

        auto vertices = &(m->mStandard.mLODs[0].mVertices);

        MFUtil::NamedRigidBody newBody;
        newBody.mRigidBody.mMesh = std::make_shared<btTriangleMesh>();

        for (int j = 0; j < (int) mFaceCollisions[i].mFaces.size(); ++j)
        {
            auto indices = mFaceCollisions[i].mFaces[j];
            auto v = (*vertices)[indices.mI1].mPos;
            btVector3 v0 = MFUtil::mafiaVec3ToBullet(v.x,v.y,v.z);

            v = (*vertices)[indices.mI2].mPos;
            btVector3 v1 = MFUtil::mafiaVec3ToBullet(v.x,v.y,v.z);

            v = (*vertices)[indices.mI3].mPos;
            btVector3 v2 = MFUtil::mafiaVec3ToBullet(v.x,v.y,v.z);
            newBody.mRigidBody.mMesh->addTriangle(v0,v1,v2);
        }

        newBody.mRigidBody.mShape = std::make_shared<btBvhTriangleMeshShape>(newBody.mRigidBody.mMesh.get(),true);

        newBody.mName = mFaceCollisions[i].mMeshName;

        btRigidBody::btRigidBodyConstructionInfo ci(0,0,newBody.mRigidBody.mShape.get());
        newBody.mRigidBody.mBody = std::make_shared<btRigidBody>(ci);
        newBody.mRigidBody.mBody->setCollisionFlags(newBody.mRigidBody.mBody->getCollisionFlags() | btCollisionObject::CF_STATIC_OBJECT);
        newBody.mRigidBody.mBody->setWorldTransform(MFUtil::mafiaMat4ToBullet(model.computeWorldTransform(meshIndex)));
        mRigidBodies.push_back(newBody);
    }
}

}
