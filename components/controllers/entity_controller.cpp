#include <controllers/entity_controller.hpp>

namespace MFGame
{

EntityController::EntityController(SpatialEntity *entity)
{
    mEntity = entity;
}

void EntityController::setRelativeVelocityVector(MFMath::Vec3 vector)
{
    auto rot = mEntity->getRotation();
    MFMath::Mat4 m = MFMath::rotationMatrix(rot);
    vector = m.preMult(vector);

    auto physicsBehavior = mEntity->getPhysicsBehavior();

    if (physicsBehavior == SpatialEntity::RIGID || physicsBehavior == SpatialEntity::RIGID_PLAYER)
    {
        auto prevVel = mEntity->getVelocity();
        auto vel = (0.4f * prevVel + 0.6f * 65.f * vector);
        mEntity->setVelocity(vel);
    }
}

}
