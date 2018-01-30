#ifndef CHARACTER_ENTITY_CONTROLLER_H
#define CHARACTER_ENTITY_CONTROLLER_H

#include <controllers/entity_controller.hpp>
#include <input/base_input_manager.hpp>

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

    CharacterEntityController(SpatialEntity *entity);

    virtual void move(MFMath::Vec3 offset) override;

    void setSpeeds(float walk, float run, float crouch)
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
    virtual void moveLeft() override;
    virtual void moveRight() override;
    virtual void moveForward() override;
    virtual void moveBackward() override;

  protected:
    int mMovementState;
    float mSpeeds[3];
};

class TestCharacterController: public CharacterEntityController
{
public:
    TestCharacterController(SpatialEntity *entity, MFInput::InputManager *inputManager);
};

}

#endif // CHARACTER_ENTITY_CONTROLLER_H
