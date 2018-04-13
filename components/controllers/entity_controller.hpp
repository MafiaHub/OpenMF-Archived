#ifndef ENTITY_CONTROLLER_H
#define ENTITY_CONTROLLER_H

#include <entity/entity.hpp>

namespace MFGame
{

class EntityController
{
public:
    EntityController(Entity *entity);

    /**
      Set the velocity vector relatively to where the entity is facing.
    */

    void setRelativeVelocityVector(MFMath::Vec3 vector);
    Entity *getEntity()    { return mEntity;   };

protected:
    Entity *mEntity;
};

}

#endif // ENTITY_CONTROLLER_H
