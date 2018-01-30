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

void EntityController::moveByRelativeVector(MFMath::Vec3 vector)
{
    auto rot = mEntity->getRotation();
    MFMath::Mat4 m = MFMath::rotationMatrix(rot);
    vector = m.preMult(vector);
    move(vector);
}

void EntityController::moveLeft()
{
    moveByRelativeVector(MFMath::Vec3(-1,0,0));
}

void EntityController::moveRight()
{
    moveByRelativeVector(MFMath::Vec3(1,0,0));
}

void EntityController::moveForward()
{
    moveByRelativeVector(MFMath::Vec3(0,1,0));
}

void EntityController::moveBackward()
{
    moveByRelativeVector(MFMath::Vec3(0,-1,0));
}

void EntityController::moveUp()
{
    moveByRelativeVector(MFMath::Vec3(0,0,1));
}

}
