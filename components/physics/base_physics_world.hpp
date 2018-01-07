#ifndef MF_PHYSICS_WORLD_H
#define MF_PHYSICS_WORLD_H

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
};

};

#endif
