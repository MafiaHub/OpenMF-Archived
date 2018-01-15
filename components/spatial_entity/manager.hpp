#ifndef SPATIAL_ENTITY_MANAGER_H
#define SPATIAL_ENTITY_MANAGER_H

#include <spatial_entity/spatial_entity.hpp>
#include <vector>
#include <loggers/console.hpp>

#define SPATIAL_ENTITY_MANAGER_MODULE_STR "spatial entity manager"

namespace MFGame
{

class SpatialEntityManager
{
public:
    SpatialEntity *getEntityById(SpatialEntity::Id id);
    void addEntity(std::shared_ptr<SpatialEntity> entity);

    /**
      Update all managed entities.
    */
    void update(double dt);
    unsigned int getNumEntities();

protected:
    std::vector<std::shared_ptr<SpatialEntity>> mEntities;
};

SpatialEntity *SpatialEntityManager::getEntityById(SpatialEntity::Id id)
{
    // TODO: implement some fast, indexed search (hash, tree, map, ...)

    for (int i = 0; i < (int) mEntities.size(); ++i)
        if (mEntities[i]->getId() == id)
            return mEntities[i].get();

    return 0;
}

void SpatialEntityManager::addEntity(std::shared_ptr<SpatialEntity> entity)
{
    if (getEntityById(entity->getId()) != 0)
    {
        MFLogger::ConsoleLogger::warn("Entity with existing ID (" + std::to_string(entity->getId()) +
            ") being added - ignoring.",SPATIAL_ENTITY_MANAGER_MODULE_STR);
        return;
    }

    mEntities.push_back(entity);
}

void SpatialEntityManager::update(double dt)
{
    for (int i = 0; i < (int) mEntities.size(); ++i)
        mEntities[i]->update(dt);
}

unsigned int SpatialEntityManager::getNumEntities()
{
    return mEntities.size();
}

}

#endif
