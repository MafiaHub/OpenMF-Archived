#ifndef BULLET_PHYSICS_WORLD_H
#define BULLET_PHYSICS_WORLD_H

#include <physics/base_physics_world.hpp>
#include <loggers/console.hpp>
#include <klz/bullet.hpp>

#define BULLET_PHYSICS_WORLD_MODULE_STR "bullet world"

namespace MFPhysics
{

class BulletPhysicsWorld: public MFPhysicsWorld
{
public:
    virtual void frame(double dt) override;
    virtual bool loadMission(std::string mission) override;
};

void BulletPhysicsWorld::frame(double dt)
{
}

bool BulletPhysicsWorld::loadMission(std::string mission)
{
    MFLogger::ConsoleLogger::info("Loading physics world for mission " + mission + ".",BULLET_PHYSICS_WORLD_MODULE_STR);
    return true;
}

};

#endif
