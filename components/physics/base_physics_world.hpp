#ifndef MF_PHYSICS_WORLD_H
#define MF_PHYSICS_WORLD_H

#include <entity/entity.hpp>
#include <utils/math.hpp>

namespace MFPhysics
{

/**
  Abstract interface for the physics world.
 */

class PhysicsWorld
{
public:
    virtual ~PhysicsWorld() {};
    virtual void frame(double dt)=0;
    virtual void getWorldAABBox(MFMath::Vec3 &min, MFMath::Vec3 &max)=0;

    /**
      Perform collision query on given point in the world, return colliding entity ID or -1.
    */
    virtual MFGame::Entity::Id pointCollision(MFMath::Vec3 position)=0;

    /**
      Perform a ray cast from given point in given direction. Returns the distance of the first
      intersection, or a negative value if there is none.
    */

    virtual double castRay(MFMath::Vec3 origin, MFMath::Vec3 direction)=0;
};

}

#endif
