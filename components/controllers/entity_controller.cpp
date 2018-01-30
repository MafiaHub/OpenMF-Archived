#include <controllers/entity_controller.hpp>

namespace MFGame
{

EntityController::EntityController(SpatialEntity *entity)
{
    mEntity = entity;
}

void EntityController::move(MFMath::Vec3 offset)
{
    auto physicsBehavior = mEntity->getPhysicsBehavior();

    if (physicsBehavior == SpatialEntity::RIGID || physicsBehavior == SpatialEntity::RIGID_PLAYER)
    {
        auto prevVel = mEntity->getVelocity();
        auto vel = (0.4f * prevVel + 0.6f * 65.f * offset);
        mEntity->setVelocity(vel);
    }
    else if (physicsBehavior != SpatialEntity::STATIC)
    {
        mEntity->setPosition(mEntity->getPosition() + offset);
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
