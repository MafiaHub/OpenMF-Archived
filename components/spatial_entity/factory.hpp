#ifndef SPATIAL_ENTITY_FACTORY
#define SPATIAL_ENTITY_FACTORY

#include <osg/Group>
#include <physics/bullet_physics_world.hpp>
#include <spatial_entity/spatial_entity_implementation.hpp>

namespace MFGame
{

class SpatialEntityFactory
{
public:
    SpatialEntityFactory(osg::Group *sceneOSGRoot, MFPhysics::BulletPhysicsWorld *bulletWorld);

    std::shared_ptr<SpatialEntityImplementation> createEntity(std::string name="");
protected:
    osg::Group *mOSGRoot;
    MFPhysics::BulletPhysicsWorld *mBulletWorld;
};

SpatialEntityFactory::SpatialEntityFactory(osg::Group *sceneOSGRoot, MFPhysics::BulletPhysicsWorld *bulletWorld)
{
    mOSGRoot = sceneOSGRoot;
    mBulletWorld = bulletWorld;
}

std::shared_ptr<SpatialEntityImplementation> SpatialEntityFactory::createEntity(std::string name)
{
    std::shared_ptr<MFGame::SpatialEntityImplementation> newEntity = std::make_shared<MFGame::SpatialEntityImplementation>();
    newEntity->setName(name);
    newEntity->setOSGRootNode(mOSGRoot);
    return newEntity;
}

}

#endif
