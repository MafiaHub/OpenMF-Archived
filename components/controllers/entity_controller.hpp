#ifndef ENTITY_CONTROLLER_H
#define ENTITY_CONTROLLER_H

#include <spatial_entity/spatial_entity.hpp>

namespace MFGame
{

class EntityController
{
public:
    EntityController(SpatialEntity *entity);

    /**
      Set the velocity vector relatively to where the entity is facing.
    */

    void setRelativeVelocityVector(MFMath::Vec3 vector);

protected:
    SpatialEntity *mEntity;
};

}

#endif // ENTITY_CONTROLLER_H
