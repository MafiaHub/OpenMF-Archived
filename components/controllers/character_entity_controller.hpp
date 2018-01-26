#ifndef CHARACTER_ENTITY_CONTROLLER_H
#define CHARACTER_ENTITY_CONTROLLER_H

#include <controllers/entity_controller.hpp>

namespace MFGame
{

typedef enum
{
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

}

#endif // CHARACTER_ENTITY_CONTROLLER_H
