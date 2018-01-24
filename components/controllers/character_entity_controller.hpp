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

  protected:
    bool mIsCrouching;
    int mMoveBehavior;
    float mSpeed[3];
};

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