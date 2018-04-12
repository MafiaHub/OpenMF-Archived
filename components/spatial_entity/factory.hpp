#ifndef SPATIAL_ENTITY_FACTORY
#define SPATIAL_ENTITY_FACTORY

#include <osg/Group>
#include <physics/bullet_physics_world.hpp>
#include <spatial_entity/spatial_entity_impl.hpp>
#include <renderer/osg_renderer.hpp>
#include <spatial_entity/manager.hpp>
#include <vfs/vfs.hpp>

#define SPATIAL_ENTITY_FACTORY_MODULE_STR "spatial entity factory"

namespace MFGame
{

class ObjectFactory {
public:
    ObjectFactory(MFRender::OSGRenderer *renderer, MFPhysics::BulletPhysicsWorld *physicsWorld);
    
    osg::ref_ptr<osg::Node> loadModel(std::string modelName);
    MFFormat::DataFormat4DS *loadModelData(std::string modelName);
    
    void setDebugMode(bool enable) { mDebugMode = enable; };

protected:
    MFFormat::ModelCache mModelCache;
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

class SpatialEntityFactory : public ObjectFactory
{
public:
    SpatialEntityFactory(MFRender::OSGRenderer *renderer, MFPhysics::BulletPhysicsWorld *physicsWorld, MFGame::SpatialEntityManager *entityManager);

    MFGame::SpatialEntity::Id createEntity(
        osg::MatrixTransform *graphicNode,
        std::shared_ptr<btRigidBody> physicsBody=0,
        std::shared_ptr<btDefaultMotionState> physicsMotionsState=0, 
        std::string name="",
        SpatialEntity::PhysicsBehavior physicsBehavior=SpatialEntity::RIGID);

    MFGame::SpatialEntity::Id createTestBallEntity();
    MFGame::SpatialEntity::Id createTestBoxEntity();
    MFGame::SpatialEntity::Id createPawnEntity(std::string modelName="");
    MFGame::SpatialEntity::Id createCameraEntity();
    MFGame::SpatialEntity::Id createTestShapeEntity(btCollisionShape *colShape, osg::ShapeDrawable *visualNode);
    MFGame::SpatialEntity::Id createDynamicEntity(MFFormat::DataFormatScene2BIN::Object *object);

protected: 
    MFGame::SpatialEntityManager *mEntityManager;
};

}

#endif
