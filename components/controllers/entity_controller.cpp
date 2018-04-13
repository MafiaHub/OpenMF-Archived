#include <controllers/entity_controller.hpp>

namespace MFGame
{

EntityController::EntityController(Entity *entity)
{
    mEntity = entity;
}

void EntityController::setRelativeVelocityVector(MFMath::Vec3 vector)
{
    auto rot = mEntity->getRotation();
    MFMath::Mat4 m = MFMath::rotationMatrix(rot);
    vector = m.preMult(vector);
    mEntity->setVelocity(vector);
}

}
