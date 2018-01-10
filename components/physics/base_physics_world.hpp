#ifndef MF_PHYSICS_WORLD_H
#define MF_PHYSICS_WORLD_H

#include <spatial_entity/spatial_entity.hpp>

namespace MFPhysics
{

/**
  Abstract interface for the physics world.
 */

class MFPhysicsWorld
{
public:
    virtual void frame(double dt)=0;
    virtual bool loadMission(std::string mission)=0;

    /**
      Perform collision query on given point in the world, return colliding entity ID or -1.
    */
    virtual int pointCollision(double x, double y, double z)=0;
};

};

#endif
