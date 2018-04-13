#ifndef CHARACTER_ENTITY_CONTROLLER_H
#define CHARACTER_ENTITY_CONTROLLER_H

#include <controllers/entity_controller.hpp>
#include <input/base_input_manager.hpp>
#include <physics/base_physics_world.hpp>

namespace MFGame
{

class CharacterEntityController: public EntityController
{
public:
    typedef enum
    {
        WALK = 0,
        RUN,
        CROUCH,
    } MovementState;

    CharacterEntityController(Entity *entity, MFPhysics::PhysicsWorld *physicsWorld);

    void setSpeeds(float crouch, float walk, float run)
    {
        mSpeeds[WALK] = walk;
        mSpeeds[RUN] = run;
        mSpeeds[CROUCH] = crouch;
    }

    void setMovementState(CharacterEntityController::MovementState newState)
    {
        mMovementState = newState;
    }

    void jump();
    bool isOnGround();
    virtual void moveLeft(bool start=true);
    virtual void moveRight(bool start=true);
    virtual void moveForward(bool start=true);
    virtual void moveBackward(bool start=true);

    void instantRotate(MFMath::Quat newRotation);
    void applyCurrentMotion();
    void setMafiaPhysicsEmulation(bool state);

protected:
    MFPhysics::PhysicsWorld *mPhysicsWorld;

    MovementState mMovementState;
    MFMath::Vec3 mMovementVector;
    MFMath::Vec3 mLastJumpMovementVector;
    bool mIsGrounded;
    bool mEmulatesMafiaPhysics;
    float mSpeeds[3];
};

}

#endif // CHARACTER_ENTITY_CONTROLLER_H
