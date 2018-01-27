#include <spatial_entity/spatial_entity.hpp>

namespace MFGame
{

std::shared_ptr<IDManager> SpatialEntity::sIDManager = std::make_shared<DEFAULT_ID_MANAGER>();

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
