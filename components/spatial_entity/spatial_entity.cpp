#include <spatial_entity/spatial_entity.hpp>

namespace MFGame
{

SpatialEntity::SpatialEntity()
{
    mId = sIDManager->allocate();
    mPosition = MFMath::Vec3();
    mScale = MFMath::Vec3(1,1,1);
    mReady = true;
    mPhysicsBehavior = ENTITY_MOVABLE;
}

SpatialEntity::~SpatialEntity()
{
    sIDManager->deallocate(mId);
}

}
