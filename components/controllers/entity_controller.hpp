#ifndef ENTITY_CONTROLLER_H
#define ENTITY_CONTROLLER_H

#include <spatial_entity/spatial_entity.hpp>

namespace MFGame
{

class EntityController
{
public:
    EntityController(SpatialEntity *entity);

    void setEntity(SpatialEntity *entity) { mEntity = entity; }


    virtual void move(MFMath::Vec3 offset);
    void moveLeft();
    void moveRight();
    void moveForward();
    void moveBackward();
protected:
    SpatialEntity *mEntity;
};

EntityController::EntityController(SpatialEntity *entity)
{
    mEntity = entity;
}

void EntityController::move(MFMath::Vec3 offset)
{
    auto physicsBehavior = mEntity->getPhysicsBehavior();

    if (physicsBehavior == SpatialEntity::ENTITY_MOVABLE)
    {
        mEntity->setPosition(mEntity->getPosition() + offset);
    }
    else if (physicsBehavior == SpatialEntity::ENTITY_RIGID)
    {
        auto prevVel = mEntity->getVelocity();
        auto vel = (0.4f * prevVel + 0.6f * 65.f * offset);
        mEntity->setVelocity(vel);
    }
}

void EntityController::moveLeft()
{
    auto rot = mEntity->getRotation();
    auto right = MFMath::Vec3(1,0,0);
    MFMath::Mat4 m = MFMath::rotationMatrix(rot);
    right = m.preMult(right);

    move(-right);
}

void EntityController::moveRight()
{
    auto rot = mEntity->getRotation();
    auto right = MFMath::Vec3(1, 0, 0);
    MFMath::Mat4 m = MFMath::rotationMatrix(rot);
    right = m.preMult(right);

    move(right);
}

void EntityController::moveForward()
{
    auto rot = mEntity->getRotation();
    auto forward = MFMath::Vec3(0, 1, 0);
    MFMath::Mat4 m = MFMath::rotationMatrix(rot);
    forward = m.preMult(forward);

    move(forward);
}
void EntityController::moveBackward()
{
    auto rot = mEntity->getRotation();
    auto forward = MFMath::Vec3(0, 1, 0);
    MFMath::Mat4 m = MFMath::rotationMatrix(rot);
    forward = m.preMult(forward);

    move(-forward);
}

}

#endif // ENTITY_CONTROLLER_H