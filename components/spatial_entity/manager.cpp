#include <spatial_entity/manager.hpp>

namespace MFGame
{

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
    if (id == MFGame::NullId)
        return 0;

    auto entity = mEntities[id];

    if (!entity.get())
    {
        MFLogger::ConsoleLogger::warn("Can't retrieve invalid entity.", SPATIAL_ENTITY_MANAGER_MODULE_STR);
        return nullptr;
    }

    return entity.get();
}

bool SpatialEntityManager::isValid(Id ident)
{
    auto entity = mEntities[ident];
    return entity.get() != nullptr;
}

Id SpatialEntityManager::addEntity(std::shared_ptr<SpatialEntity> entity)
{
    auto id = entity->getId();
    auto ourEntity = mEntities[id];
    if (ourEntity.get())
    {
        MFLogger::ConsoleLogger::warn("Entity with existing ID (" + std::to_string(id) +
            ") being added - ignoring.",SPATIAL_ENTITY_MANAGER_MODULE_STR);
        return NullId;
    }

    mEntities[id] = entity;

    mNumEntities++;
    return id;
}

void SpatialEntityManager::removeEntity(Id ident)
{
    auto entity = mEntities[ident];
    if (!entity.get())
    {
        MFLogger::ConsoleLogger::warn("Can't remove invalid entity.", SPATIAL_ENTITY_MANAGER_MODULE_STR);
        return;
    }
    mEntities[ident].reset();
    mEntities.erase(ident);
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
