#ifndef ENTITY_CONTROLLER_H
#define ENTITY_CONTROLLER_H

#include <spatial_entity/spatial_entity.hpp>

namespace MFGame
{

typedef enum {
    MOVE_WALK,
    MOVE_RUN,
    MOVE_CROUCH,
} MoveBehavior;

class EntityController
{
public:
    EntityController(SpatialEntity *entity);

    void setEntity(SpatialEntity *entity) { mEntity = entity; }

    void setSpeed(float walk, float run, float crouch)
    {
        mSpeed[MOVE_WALK]   = walk;
        mSpeed[MOVE_RUN]    = run;
        mSpeed[MOVE_CROUCH] = crouch;
    }

    void setMoveBehavior(int moveBehavior)
    {
        mMoveBehavior = moveBehavior;
    }

    void move(MFMath::Vec3 offset);
    void moveLeft();
    void moveRight();
    void moveForward();
    void moveBackward();
    void jump();
    void setCrouch(bool state);
protected:
    SpatialEntity *mEntity;
    bool mIsCrouching;
    int mMoveBehavior;
    float mSpeed[3];
};

EntityController::EntityController(SpatialEntity *entity)
{
    mEntity = entity;
    mIsCrouching = false;
    mMoveBehavior = MOVE_WALK;

    mSpeed[MOVE_WALK]   = 1.0f;
    mSpeed[MOVE_RUN]    = 5.0f;
    mSpeed[MOVE_CROUCH] = 0.5f;
}

void EntityController::move(MFMath::Vec3 offset)
{
    float speed = mSpeed[mMoveBehavior];
    auto fin = offset*speed;
    auto physicsBehavior = mEntity->getPhysicsBehavior();

    if (physicsBehavior == SpatialEntity::ENTITY_MOVABLE)
    {
        mEntity->setPosition(mEntity->getPosition() + fin);
    }
    else if (physicsBehavior == SpatialEntity::ENTITY_RIGID)
    {
        auto prevVel = mEntity->getVelocity();
        auto vel = (0.4f * prevVel + 0.6f * 65.f * fin);
        mEntity->setVelocity(vel);
    }
}

void EntityController::moveLeft()
{
    auto rot = mEntity->getRotation();
    auto right = MFMath::Vec3(1,0,0);
    MFMath::Mat4 m = MFMath::rotationMatrix(rot);
    right *= m;

    move(-right);
}

void EntityController::moveRight()
{
    auto rot = mEntity->getRotation();
    auto right = MFMath::Vec3(1, 0, 0);
    MFMath::Mat4 m = MFMath::rotationMatrix(rot);
    right *= m;

    move(right);
}

void EntityController::moveForward()
{
    auto rot = mEntity->getRotation();
    auto forward = MFMath::Vec3(0, 1, 0);
    MFMath::Mat4 m = MFMath::rotationMatrix(rot);
    forward *= m;

    move(forward);
}
void EntityController::moveBackward()
{
    auto rot = mEntity->getRotation();
    auto forward = MFMath::Vec3(0, 1, 0);
    MFMath::Mat4 m = MFMath::rotationMatrix(rot);
    forward *= m;

    move(-forward);
}

void EntityController::jump()
{
    if (mEntity->getPhysicsBehavior() == SpatialEntity::ENTITY_RIGID)
    {
        // TODO apply force to jump
    }
}

}

#endif // ENTITY_CONTROLLER_H