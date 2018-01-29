#ifndef SPATIAL_ENTITY_H
#define SPATIAL_ENTITY_H

#include <utils/math.hpp>
#include <string>
#include <memory>

#define DEFAULT_ID_MANAGER BackingIDManager

namespace MFGame
{

/**
  Abstract interface for something that exists in 3D world and has a graphical and/or
  physical and/or sound etc. representations.
 */

class SpatialEntity
{
public:
    SpatialEntity();

    typedef uint32_t Id;
    static const Id NullId = 0;

    virtual void update(double dt)=0;

    /**
      Initialises the entity with currently set data (OSG node, Bullet body, ...) so that it
      is ready to work.
    */
    virtual void ready()=0;
    virtual std::string toString()                      { return "";            };
    MFMath::Vec3 getPosition()                          { return mPosition;     };
    virtual void setPosition(MFMath::Vec3 position)     { mPosition = position; };   ///< Sets position without collisions.
    virtual void setVelocity(MFMath::Vec3 velocity)=0;
    virtual void setAngularVelocity(MFMath::Vec3 velocity)=0;
    virtual void setDamping(float lin, float ang) = 0;
    virtual MFMath::Vec3 getVelocity() = 0;
    virtual MFMath::Vec3 getAngularVelocity() = 0;
    virtual MFMath::Vec2 getDamping() = 0;
    MFMath::Quat getRotation()                          { return mRotation;     };
    virtual void setRotation(MFMath::Quat rotation)     { mRotation = rotation; };
    virtual bool hasVisual()=0;
    virtual bool canBeMoved()=0;                        ///< Says whether the entity can be transformed with setPosition(...) etc.
//  detach()
//  mergeWithChildren()

    virtual bool hasCollision()=0;
    void setName(std::string name)                      { mName = name;         };
    std::string getName()                               { return mName;         };
    bool isReady()                                      { return mReady;        };
    Id getId()                                          { return mId;           };

    void setPhysicsBehavior(int behavior)               { mPhysicsBehavior = behavior; };
    int  getPhysicsBehavior()                           { return mPhysicsBehavior;     };

    typedef enum
    {
        ENTITY_STATIC,
        ENTITY_MOVABLE,
        ENTITY_RIGID,
    } PhysicsBehavior;

  protected:
    MFMath::Vec3 mPosition;
    MFMath::Vec3 mScale;
    MFMath::Quat mRotation;

    std::string mName;
    bool mReady;
    Id mId;
    int mPhysicsBehavior;

    static Id sNextId;
};

}

#endif
