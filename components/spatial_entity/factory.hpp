#ifndef SPATIAL_ENTITY_FACTORY
#define SPATIAL_ENTITY_FACTORY

#include <osg/Group>
#include <physics/bullet_physics_world.hpp>
#include <spatial_entity/spatial_entity_implementation.hpp>
#include <renderer/osg_renderer.hpp>
#include <spatial_entity/manager.hpp>

#define SPATIAL_ENTITY_FACTORY_MODULE_STR "spatial entity factory"

namespace MFGame
{

class SpatialEntityFactory
{
public:
    SpatialEntityFactory(MFRender::OSGRenderer *renderer, MFPhysics::BulletPhysicsWorld *physicsWorld, MFGame::SpatialEntityManager *entityManager);
    void createMissionEntities();
    void setDebugMode(bool enable)    { mDebugMode = enable; };

    MFGame::SpatialEntity::Id createEntity(
        osg::MatrixTransform *graphicNode,
        std::shared_ptr<btRigidBody> physicsBody=0,
        std::shared_ptr<btDefaultMotionState> physicsMotionsState=0, 
        std::string name="");

    MFGame::SpatialEntity::Id createTestBallEntity();
    MFGame::SpatialEntity::Id createTestBoxEntity();
    MFGame::SpatialEntity::Id createCapsuleEntity();
    MFGame::SpatialEntity::Id createCameraEntity();

    MFGame::SpatialEntity::Id createTestShapeEntity(btCollisionShape *colShape, osg::ShapeDrawable *visualNode);

protected: 
    MFGame::SpatialEntityManager *mEntityManager;
    MFPhysics::BulletPhysicsWorld *mPhysicsWorld;
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

}

#endif
