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

    MFGame::Entity::Id createEntity(
        osg::MatrixTransform *graphicNode,
        std::shared_ptr<btRigidBody> physicsBody=0,
        std::shared_ptr<btDefaultMotionState> physicsMotionsState=0, 
        std::string name="",
        Entity::PhysicsBehavior physicsBehavior=Entity::RIGID);

    MFGame::Entity::Id createTestBallEntity();
    MFGame::Entity::Id createTestBoxEntity();
    MFGame::Entity::Id createPawnEntity(std::string modelName="", btScalar mass=150.0f);
    MFGame::Entity::Id createCameraEntity();
    MFGame::Entity::Id createTestShapeEntity(btCollisionShape *colShape, osg::ShapeDrawable *visualNode);
    MFGame::Entity::Id createPropEntity(MFFormat::DataFormatScene2BIN::Object *object);
    MFGame::Entity::Id createPropEntity(std::string modelName, btScalar mass=20.0f);

protected: 
    MFGame::EntityManager *mEntityManager;
};

}

#endif
