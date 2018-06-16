#ifndef ENTITY_H
#define ENTITY_H

#include <utils/math.hpp>
#include <string>
#include <memory>

#define DEFAULT_ID_MANAGER BackingIDManager

namespace MFGame
{

    class Engine;

/**
  Abstract interface for something that exists in 3D world and has a graphical and/or
  physical and/or sound etc. representations.
 */

class Entity
{
public:
    typedef enum
    {
        STATIC,            ///< The entity cannot move.
        KINEMATIC,         ///< The entity can move, but doesn't react to collisions.
        RIGID,             ///< The entity can move and react to collisions as a rigid body.
        RIGID_PAWN       ///< Same as RIGID, but cannot rotate (e.g. player capsule collision).
    } PhysicsBehavior;

    typedef uint32_t Id;
    static const Id NullId = 0;

    Entity();
    virtual void update(double dt)=0;
    virtual void think() {};

    /**
      Initializes the entity with currently set data (OSG node, Bullet body, ...) so that it
      is ready to work.
    */
    virtual void ready()=0;
    virtual void destroy()=0;
    void setEngine(MFGame::Engine *engine)              { mEngine = engine;            };
    MFGame::Engine *getEngine() const { return mEngine; }
    virtual std::string toString()                      { return "";                   };
    MFMath::Vec3 getPosition()                          { return mPosition;            };
    virtual void setPosition(MFMath::Vec3 position)     { mPosition = position;        };   ///< Sets position without collisions.
    virtual void setVelocity(MFMath::Vec3 velocity)=0;
    virtual MFMath::Vec3 getVelocity()=0;
    virtual void setAngularVelocity(MFMath::Vec3 velocity)=0;
    virtual void setDamping(float lin, float ang) = 0;
    virtual MFMath::Vec3 getAngularVelocity() = 0;
    virtual MFMath::Vec2 getDamping() = 0;
    MFMath::Quat getRotation()                          { return mRotation;            };
    virtual void setRotation(MFMath::Quat rotation)     { mRotation = rotation;        };
    virtual bool hasVisual()=0;
    virtual bool hasPhysics()=0;
    virtual bool canBeMoved()=0;                        ///< Says whether the entity can be transformed with setPosition(...) etc.
    virtual MFMath::Vec3 getSize()=0;                   ///< Get the AABBox size in x, y and z.

//  detach()
//  mergeWithChildren()

    virtual bool hasCollision()=0;
    void setName(std::string name)                      { mName = name;                };
    std::string getName()                               { return mName;                };
    bool isReady()                                      { return mReady;               };
    Id getId()                                          { return mId;                  };

    void setNextThink(int time) { mNextthink = time; }
    int  getNextThink() const { return mNextthink; }

    virtual void setFriction(double factor)=0;

    virtual void setPhysicsBehavior(PhysicsBehavior behavior)=0;
    int  getPhysicsBehavior()                           { return mPhysicsBehavior;     };

protected:
    MFMath::Vec3 mPosition;
    MFMath::Vec3 mScale;
    MFMath::Quat mRotation;
    MFGame::Engine *mEngine;

    std::string mName;
    bool mReady;
    Id mId;
    int mPhysicsBehavior;
    int mNextthink;

    static Id sNextId;
};

}

#endif
