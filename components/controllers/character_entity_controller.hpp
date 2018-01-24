#ifndef CHARACTER_ENTITY_CONTROLLER_H
#define CHARACTER_ENTITY_CONTROLLER_H

#include <controllers/entity_controller.hpp>

namespace MFGame
{

typedef enum {
    MOVE_WALK,
    MOVE_RUN,
    MOVE_CROUCH,
} MoveBehavior;

class CharacterEntityController: public EntityController
{
public:
    CharacterEntityController(SpatialEntity *entity): EntityController(entity)
    {
        mIsCrouching = false;
        mMoveBehavior = MOVE_WALK;

        mSpeed[MOVE_WALK] = 1.0f;
        mSpeed[MOVE_RUN] = 5.0f;
        mSpeed[MOVE_CROUCH] = 0.5f;
    }

    virtual void move(MFMath::Vec3 offset) override;

    void setSpeed(float walk, float run, float crouch)
    {
        mSpeed[MOVE_WALK] = walk;
        mSpeed[MOVE_RUN] = run;
        mSpeed[MOVE_CROUCH] = crouch;
    }

    void setMoveBehavior(int moveBehavior)
    {
        mMoveBehavior = moveBehavior;
    }

    void jump();
    void setCrouch(bool state);
    virtual void moveLeft() override;
    virtual void moveRight() override;
    virtual void moveForward() override;
    virtual void moveBackward() override;

  protected:
    bool mIsCrouching;
    int mMoveBehavior;
    float mSpeed[3];
};

void CharacterEntityController::moveLeft()
{
    // TODO check if we can move (if we're not standing at ground, don't move)
    EntityController::moveLeft();
}

void CharacterEntityController::moveRight()
{
    // TODO check if we can move (if we're not standing at ground, don't move)
    EntityController::moveRight();
}

void CharacterEntityController::moveForward()
{
    // TODO check if we can move (if we're not standing at ground, don't move)
    EntityController::moveForward();
}

void CharacterEntityController::moveBackward()
{
    // TODO check if we can move (if we're not standing at ground, don't move)
    EntityController::moveBackward();
}

void CharacterEntityController::move(MFMath::Vec3 offset)
{
    float speed = mSpeed[mMoveBehavior];
    auto fin = offset * speed;

    EntityController::move(fin);
}

void CharacterEntityController::jump()
{
    if (mEntity->getPhysicsBehavior() == SpatialEntity::ENTITY_RIGID)
    {
        // TODO apply force to jump
    }
}

void CharacterEntityController::setCrouch(bool state)
{
    // TODO deal with this
}

}

#endif // CHARACTER_ENTITY_CONTROLLER_H