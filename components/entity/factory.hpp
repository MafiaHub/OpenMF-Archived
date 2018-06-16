#ifndef ENTITY_FACTORY
#define ENTITY_FACTORY

#include <osg/Group>
#include <physics/bullet_physics_world.hpp>
#include <entity/entity_impl.hpp>
#include <renderer/osg_renderer.hpp>
#include <entity/manager.hpp>
#include <vfs/vfs.hpp>

#define ENTITY_FACTORY_MODULE_STR "spatial entity factory"

namespace MFGame
{

    typedef MFFormat::LoaderCache<MFFormat::DataFormat4DS*> ModelCache;
    typedef MFFormat::LoaderCache<btTriangleMesh*> FaceColsCache;

class ObjectFactory {
public:
    ObjectFactory(MFRender::OSGRenderer *renderer, MFPhysics::BulletPhysicsWorld *physicsWorld);
    
    osg::ref_ptr<osg::Node> loadModel(std::string modelName);
    MFFormat::DataFormat4DS *loadModelData(std::string modelName);
    btTriangleMesh *loadFaceCols(std::string modelName, int meshId=0);
    
    void setDebugMode(bool enable) { mDebugMode = enable; };

protected:
    ModelCache mModelCache;
    FaceColsCache mFaceColsCache;
    MFPhysics::BulletPhysicsWorld *mPhysicsWorld;
    MFFile::FileSystem *mFileSystem;
    MFRender::OSGRenderer *mRenderer;

    osg::ref_ptr<osg::StateSet> mTestStateSet;
    osg::ref_ptr<osg::Material> mTestMaterial;

    osg::ref_ptr<osg::Shape> mTestVisualSphereShape;
    osg::ref_ptr<osg::ShapeDrawable> mTestSphereNode;
    std::shared_ptr<btCollisionShape> mTestPhysicalSphereShape;

    osg::ref_ptr<osg::Shape> mTestVisualBoxShape;
    osg::ref_ptr<osg::ShapeDrawable> mTestBoxNode;
    std::shared_ptr<btCollisionShape> mTestPhysicalBoxShape;

    osg::ref_ptr<osg::Shape> mCapsuleShape;
    osg::ref_ptr<osg::ShapeDrawable> mCapsuleNode;
    std::shared_ptr<btCollisionShape> mPhysicalCapsuleShape;

    std::shared_ptr<btCollisionShape> mCameraShape;

    bool mDebugMode;
};

class EntityFactory : public ObjectFactory
{
public:
    EntityFactory(MFRender::OSGRenderer *renderer, MFPhysics::BulletPhysicsWorld *physicsWorld, MFGame::EntityManager *entityManager);

    template <class T = MFGame::EntityImpl>
    MFGame::Entity::Id createEntity(
        osg::MatrixTransform *graphicNode,
        std::shared_ptr<btRigidBody> physicsBody=0,
        std::shared_ptr<btDefaultMotionState> physicsMotionsState=0, 
        std::string name="",
        Entity::PhysicsBehavior physicsBehavior=Entity::RIGID)
    {
        if (physicsBody && physicsMotionsState && physicsBody->getMotionState() != physicsMotionsState.get())
            physicsBody->setMotionState(physicsMotionsState.get());

        std::shared_ptr<T> newEntity = std::make_shared<T>();
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

    MFGame::Entity::Id createTestBallEntity();
    MFGame::Entity::Id createTestBoxEntity();

    template<class T = MFGame::EntityImpl>
    MFGame::Entity::Id createPawnEntity(std::string modelName="", btScalar mass=150.0f)
    {
        MFUtil::FullRigidBody body;

        btTransform transform;
        transform.setIdentity();

        body.mMotionState = std::make_shared<btDefaultMotionState>(transform);

        btVector3 inertia(0, 0, 0);

        if (mass)
            mPhysicalCapsuleShape->calculateLocalInertia(mass, inertia);

        body.mBody = std::make_shared<btRigidBody>(mass, body.mMotionState.get(), mPhysicalCapsuleShape.get(), inertia);

        if (mass) {
            body.mBody->setActivationState(DISABLE_DEACTIVATION);
            body.mBody->setFriction(0);
        }
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

        return createEntity<T>(visualTransform.get(), body.mBody, body.mMotionState, "capsule", Entity::RIGID_PAWN);
    }

    MFGame::Entity::Id createCameraEntity();
    MFGame::Entity::Id createTestShapeEntity(btCollisionShape *colShape, osg::ShapeDrawable *visualNode);
    MFGame::Entity::Id createPropEntity(MFFormat::DataFormatScene2BIN::Object *object);
    MFGame::Entity::Id createPropEntity(std::string modelName, btScalar mass=20.0f);

protected: 
    MFGame::EntityManager *mEntityManager;
};

}

#endif
