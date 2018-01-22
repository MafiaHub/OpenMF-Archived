#ifndef SPATIAL_ENTITY_MANAGER_H
#define SPATIAL_ENTITY_MANAGER_H

#include <spatial_entity/spatial_entity.hpp>
#include <loggers/console.hpp>

#include <unordered_map>

#define SPATIAL_ENTITY_MANAGER_MODULE_STR "spatial entity manager"

namespace MFGame
{

class SpatialEntityManager
{
public:
    typedef std::unordered_map<uint64_t, std::shared_ptr<SpatialEntity>> EntityMap;

    SpatialEntityManager() {}

    SpatialEntity *getEntityById(MFGame::Id id);

    /**
     * \brief Returns valid entity by a raw index,
     * if entity is invalid (e.g. removed) or index points outside the bounds of the pool
     * it returns nullptr.
     */
    SpatialEntity *getEntityByIndex(unsigned int index);

    Id addEntity(std::shared_ptr<SpatialEntity> entity);
    void removeEntity(Id ident);
    bool isValid(Id ident);

    /**
      Update all managed entities.
    */
    void update(double dt);

    /**
     * \brief Returns the number of active entities
     **/
    unsigned int getNumEntities();

    /**
     * \brief Returns the number of entity slots
     **/
    unsigned int getNumEntitySlots();

protected:
    EntityMap mEntities;
    size_t mNumEntities;
};

SpatialEntity *SpatialEntityManager::getEntityByIndex(unsigned int index)
{
    if (index < 0 || index > mEntities.size())
    {
        return nullptr;
    }

    // TODO return entity by index
    return nullptr;
}

SpatialEntity *SpatialEntityManager::getEntityById(MFGame::Id id)
{
    auto entity = mEntities[id.mValue];
    if (!entity.get())
    {
        MFLogger::ConsoleLogger::warn("Can't retrieve invalid entity.", SPATIAL_ENTITY_MANAGER_MODULE_STR);
        return nullptr;
    }

    return entity.get();
}

bool SpatialEntityManager::isValid(Id ident)
{
    auto entity = mEntities[ident.mValue];
    return entity.get() != nullptr;
}

Id SpatialEntityManager::addEntity(std::shared_ptr<SpatialEntity> entity)
{
    auto id = entity->getId();
    auto ourEntity = mEntities[id.mValue];
    if (ourEntity.get())
    {
        MFLogger::ConsoleLogger::warn("Entity with existing ID (" + std::to_string(id.mIndex) +
            ") being added - ignoring.",SPATIAL_ENTITY_MANAGER_MODULE_STR);
        return NullId;
    }

    mEntities[id.mValue] = entity;

    mNumEntities++;
    return id;
}

void SpatialEntityManager::removeEntity(Id ident)
{
    auto entity = mEntities[ident.mValue];
    if (!entity.get())
    {
        MFLogger::ConsoleLogger::warn("Can't remove invalid entity.", SPATIAL_ENTITY_MANAGER_MODULE_STR);
        return;
    }
    mEntities[ident.mValue].reset();
    mEntities.erase(ident.mValue);
    mNumEntities--;
}

void SpatialEntityManager::update(double dt)
{
    for (auto pair : mEntities)
    {
        if (pair.second && pair.second.get())
            pair.second->update(dt);
    }
}

unsigned int SpatialEntityManager::getNumEntities()
{
    return mNumEntities;
}

unsigned int SpatialEntityManager::getNumEntitySlots()
{
    return mEntities.size();
}

}

#endif
