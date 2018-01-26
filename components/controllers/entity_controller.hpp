#ifndef ENTITY_CONTROLLER_H
#define ENTITY_CONTROLLER_H

#include <spatial_entity/spatial_entity.hpp>

namespace MFGame
{

class EntityController
{
public:
    EntityController(SpatialEntity *entity);

    void setEntity(SpatialEntity *entity) { mEntity = entity; }


    virtual void move(MFMath::Vec3 offset);
    virtual void moveLeft();
    virtual void moveRight();
    virtual void moveForward();
    virtual void moveBackward();

  protected:
    SpatialEntity *mEntity;
};

}

#endif // ENTITY_CONTROLLER_H
