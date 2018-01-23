#ifndef MF_PHYSICS_WORLD_H
#define MF_PHYSICS_WORLD_H

#include <spatial_entity/spatial_entity.hpp>
#include <math.hpp>

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
    virtual void getWorldAABBox(MFMath::Vec3 &min, MFMath::Vec3 &max)=0;

    /**
      Perform collision query on given point in the world, return colliding entity ID or -1.
    */
    virtual MFGame::Id pointCollision(MFMath::Vec3 position)=0;
};

}

#endif
