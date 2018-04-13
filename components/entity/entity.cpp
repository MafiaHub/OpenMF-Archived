#include <entity/entity.hpp>

namespace MFGame
{

Entity::Id Entity::sNextId = 1;

Entity::Entity()
{
    mId = Entity::sNextId;
    Entity::sNextId++;
    mPosition = MFMath::Vec3();
    mScale = MFMath::Vec3(1,1,1);
    mReady = true;
    mPhysicsBehavior = KINEMATIC;
}

}
