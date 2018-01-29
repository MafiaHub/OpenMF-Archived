#include <spatial_entity/spatial_entity.hpp>

namespace MFGame
{

SpatialEntity::Id SpatialEntity::sNextId = 1;

SpatialEntity::SpatialEntity()
{
    mId = SpatialEntity::sNextId;
    SpatialEntity::sNextId++;
    mPosition = MFMath::Vec3();
    mScale = MFMath::Vec3(1,1,1);
    mReady = true;
    mPhysicsBehavior = ENTITY_MOVABLE;
}

}
