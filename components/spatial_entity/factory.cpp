#include <spatial_entity/factory.hpp>
#include <4ds/osg_4ds.hpp>
#include <4ds/parser_4ds.hpp>

#define SPATIAL_ENTITY_FACTORY_MODULE_STR "spatial entity factory"

namespace MFGame
{

SpatialEntityFactory::SpatialEntityFactory(MFRender::OSGRenderer *renderer, MFPhysics::BulletPhysicsWorld *physicsWorld, MFGame::SpatialEntityManager *entityManager):
    ObjectFactory(renderer, physicsWorld)
{
    mEntityManager = entityManager;
}

MFGame::SpatialEntity::Id SpatialEntityFactory::createEntity(
    osg::MatrixTransform *graphicNode,
    std::shared_ptr<btRigidBody> physicsBody,
    std::shared_ptr<btDefaultMotionState> physicsMotionsState, 
    std::string name,
    SpatialEntity::PhysicsBehavior physicsBehavior)
{
    if (physicsBody && physicsMotionsState && physicsBody->getMotionState() != physicsMotionsState.get())
        physicsBody->setMotionState(physicsMotionsState.get());

    std::shared_ptr<MFGame::SpatialEntityImpl> newEntity = std::make_shared<MFGame::SpatialEntityImpl>();
    newEntity->setName(name);
    newEntity->setDebugMode(mDebugMode);
    newEntity->setOSGRootNode(mRenderer->getRootNode());
    newEntity->setVisualNode(graphicNode);
    newEntity->setPhysicsBody(physicsBody);
    newEntity->setPhysicsMotionState(physicsMotionsState);
    newEntity->ready();
    mEntityManager->addEntity(newEntity);
    newEntity->setPhysicsBehavior(physicsBehavior);
    return newEntity->getId();
}

MFGame::SpatialEntity::Id SpatialEntityFactory::createPawnEntity(std::string modelName)
{
    MFUtil::FullRigidBody body;

    btScalar mass = 150.0f;
    body.mMotionState = std::make_shared<btDefaultMotionState>(
        btTransform(btQuaternion(0, 0, 0, mass),
        btVector3(0, 0, 0)));

    btVector3 inertia;
    mPhysicalCapsuleShape->calculateLocalInertia(mass,inertia);
    body.mBody = std::make_shared<btRigidBody>(mass, body.mMotionState.get(), mPhysicalCapsuleShape.get(), inertia);
    body.mBody->setActivationState(DISABLE_DEACTIVATION);
    body.mBody->setFriction(0);
    mPhysicsWorld->getWorld()->addRigidBody(body.mBody.get());

    osg::ref_ptr<osg::MatrixTransform> visualTransform = new osg::MatrixTransform();

    if (modelName.length() == 0)
    {
        visualTransform->addChild(mCapsuleNode);
    }
    else
    {
        auto cache = mRenderer->getLoaderCache();
        auto node = (osg::Node *)cache->getObject(modelName).get();

        if (!node) {
            node = loadModel(modelName);
        }

        visualTransform->addChild(node);

        // TMP: shift a little down
        visualTransform->setMatrix(osg::Matrixd::translate(osg::Vec3(0, 0, -1)));
    }

    mRenderer->getRootNode()->addChild(visualTransform);

    return createEntity(visualTransform.get(), body.mBody, body.mMotionState, "capsule", SpatialEntity::RIGID_PAWN);
}

MFGame::SpatialEntity::Id SpatialEntityFactory::createCameraEntity()
{
    MFUtil::FullRigidBody body;

    btScalar mass = 1.0f;
    body.mMotionState = std::make_shared<btDefaultMotionState>(
        btTransform(btQuaternion(0, 0, 0, mass), 
        btVector3(0,0,0)));

    btVector3 inertia;
    mCameraShape->calculateLocalInertia(mass, inertia);
    body.mBody = std::make_shared<btRigidBody>(mass, body.mMotionState.get(), mCameraShape.get(), inertia);
    body.mBody->setActivationState(DISABLE_DEACTIVATION);
    mPhysicsWorld->getWorld()->addRigidBody(body.mBody.get());
    body.mBody->setGravity(btVector3(0, 0, 0));

    osg::ref_ptr<osg::MatrixTransform> visualTransform = new osg::MatrixTransform();
    mRenderer->getRootNode()->addChild(visualTransform);

    return createEntity(visualTransform.get(), body.mBody, body.mMotionState, "camera", SpatialEntity::RIGID_PAWN);
}

MFGame::SpatialEntity::Id SpatialEntityFactory::createTestShapeEntity(btCollisionShape *colShape, osg::ShapeDrawable *visualNode)
{
    osg::ref_ptr<osg::MatrixTransform> visualTransform = new osg::MatrixTransform();
    visualTransform->addChild(visualNode);
    mRenderer->getRootNode()->addChild(visualTransform);

    std::shared_ptr<btDefaultMotionState> motionState = std::make_shared<btDefaultMotionState>();

    btScalar mass = 1;
    btVector3 fallInertia(0,0,0);
    mTestPhysicalSphereShape->calculateLocalInertia(mass,fallInertia);
    btRigidBody::btRigidBodyConstructionInfo ci(mass,motionState.get(),colShape,fallInertia);

    std::shared_ptr<btRigidBody> physicalBody = std::make_shared<btRigidBody>(ci);
    physicalBody->setActivationState(DISABLE_DEACTIVATION);
    mPhysicsWorld->getWorld()->addRigidBody(physicalBody.get());

    return createEntity(visualTransform.get(),physicalBody,motionState,"test");
}

MFGame::SpatialEntity::Id SpatialEntityFactory::createDynamicEntity(MFFormat::DataFormatScene2BIN::Object * object)
{
    btScalar mass = object->mSpecialProps.mWeight;
    btTransform transform;
    transform.setIdentity();
    auto motionState = std::make_shared<btDefaultMotionState>(transform);

    btVector3 inertia = btVector3(0,0,0);

    auto btMesh = loadFaceCols(object->mModelName);

    auto shape = new btConvexTriangleMeshShape(btMesh, true);
    shape->setMargin(0.05f);
    shape->calculateLocalInertia(mass, inertia);
    auto body = std::make_shared<btRigidBody>(mass, motionState.get(), shape, inertia);
    body->setActivationState(DISABLE_DEACTIVATION);
    mPhysicsWorld->getWorld()->addRigidBody(body.get());
    
    osg::ref_ptr<osg::MatrixTransform> visualTransform = new osg::MatrixTransform();

    auto cache = mRenderer->getLoaderCache();
    auto node = (osg::Node *)cache->getObject(object->mModelName).get();

    if (!node) {
        node = loadModel(object->mModelName);
    }

    visualTransform->addChild(node);

    mRenderer->getRootNode()->addChild(visualTransform);

    return createEntity(visualTransform.get(), body, motionState, "dynamic " + object->mName, SpatialEntity::RIGID);
}

osg::ref_ptr<osg::Node> ObjectFactory::loadModel(std::string modelName)
{
    osg::ref_ptr<osg::Node> node = nullptr;
    auto cache = mRenderer->getLoaderCache();
    MFFormat::OSGModelLoader l4ds;
    l4ds.setLoaderCache(cache);
    auto model = loadModelData(modelName);

    node = l4ds.load(model, modelName);
    node->setName(modelName);

    return node;
}

MFFormat::DataFormat4DS * ObjectFactory::loadModelData(std::string modelName)
{
    MFFormat::DataFormat4DS *model = nullptr;
    
    model = mModelCache.getObject(modelName);

    if (!model) {
        model = new MFFormat::DataFormat4DS();
        std::ifstream file4DS;
        if (!mFileSystem->open(file4DS, "models/" + modelName)) {
            MFLogger::Logger::warn("Couldn't not open 4ds file: " + modelName + ".", SPATIAL_ENTITY_FACTORY_MODULE_STR);
        }
        else {
            model->load(file4DS);
            file4DS.close();
        }

        mModelCache.storeObject(modelName, model);
    }
    
    return model;
}

btTriangleMesh *ObjectFactory::loadFaceCols(std::string modelName, int meshId)
{
    auto btMesh = mFaceColsCache.getObject(modelName);

    if (!btMesh) {
        auto model = loadModelData(modelName);

        if (!model) {
            return nullptr;
        }

        btMesh = new btTriangleMesh();

        auto mesh = model->getModel().mMeshes[meshId];
        // TODO support more types?
        if (mesh.mMeshType == MFFormat::DataFormat4DS::MESHTYPE_STANDARD && mesh.mVisualMeshType == MFFormat::DataFormat4DS::VISUALMESHTYPE_STANDARD) {
            auto lod = mesh.mStandard.mLODs[0];
            for (auto faceGroup : lod.mFaceGroups) {
                for (auto face : faceGroup.mFaces) {
                    auto i1 = face.mA;
                    auto i2 = face.mB;
                    auto i3 = face.mC;

                    auto p1 = lod.mVertices[i1].mPos;
                    auto p2 = lod.mVertices[i2].mPos;
                    auto p3 = lod.mVertices[i3].mPos;

                    btVector3 v1 = MFUtil::mafiaVec3ToBullet(p1.x, p1.y, p1.z);
                    btVector3 v2 = MFUtil::mafiaVec3ToBullet(p2.x, p2.y, p2.z);
                    btVector3 v3 = MFUtil::mafiaVec3ToBullet(p3.x, p3.y, p3.z);
                    btMesh->addTriangle(v1, v2, v3);
                }
            }
        }

        mFaceColsCache.storeObject(modelName, btMesh);
    }

    return btMesh;
}

MFGame::SpatialEntity::Id SpatialEntityFactory::createTestBallEntity()
{
    return createTestShapeEntity(mTestPhysicalSphereShape.get(),mTestSphereNode.get());
}

MFGame::SpatialEntity::Id SpatialEntityFactory::createTestBoxEntity()
{
    return createTestShapeEntity(mTestPhysicalBoxShape.get(),mTestBoxNode.get());
}

ObjectFactory::ObjectFactory(MFRender::OSGRenderer *renderer, MFPhysics::BulletPhysicsWorld *physicsWorld)
{
    mDebugMode = false;

    mFileSystem = MFFile::FileSystem::getInstance();
    mRenderer = renderer;
    mPhysicsWorld = physicsWorld;

    const double r = 0.7;
    const double l = 1.0;

    mTestMaterial = new osg::Material();
    mTestMaterial->setEmission(osg::Material::FRONT_AND_BACK, osg::Vec4(0, 0, 0, 0));

    mTestStateSet = new osg::StateSet();
    mTestStateSet->setAttributeAndModes(mTestMaterial);

    mTestVisualSphereShape = new osg::Sphere(osg::Vec3f(0, 0, 0), r);
    mTestSphereNode = new osg::ShapeDrawable(mTestVisualSphereShape);
    mTestSphereNode->setStateSet(mTestStateSet);
    mTestPhysicalSphereShape = std::make_shared<btSphereShape>(r);

    mTestVisualBoxShape = new osg::Box(osg::Vec3f(0, 0, 0), l);
    mTestBoxNode = new osg::ShapeDrawable(mTestVisualBoxShape);
    mTestBoxNode->setStateSet(mTestStateSet);
    mTestPhysicalBoxShape = std::make_shared<btBoxShape>(btVector3(l / 2.0, l / 2.0, l / 2.0));

    const double capsuleRadius = 0.3;
    const double capsuleHeight = 1.3;

    mCapsuleShape = new osg::Capsule(osg::Vec3f(0, 0, 0), capsuleRadius, capsuleHeight);
    mCapsuleNode = new osg::ShapeDrawable(mCapsuleShape);
    mCapsuleNode->setStateSet(mTestStateSet);
    mPhysicalCapsuleShape = std::make_shared<btCapsuleShapeZ>(capsuleRadius, capsuleHeight);

    mCameraShape = std::make_shared<btSphereShape>(1.0f);
}

}
