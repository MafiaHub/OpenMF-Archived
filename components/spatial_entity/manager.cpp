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

SpatialEntity *SpatialEntityManager::getEntityById(MFGame::SpatialEntity::Id id)
{
    if (id == MFGame::SpatialEntity::NullId)
        return nullptr;

    auto entity = mEntities[id];

    if (!entity.get())
    {
        MFLogger::Logger::warn("Can't retrieve invalid entity.", SPATIAL_ENTITY_MANAGER_MODULE_STR);
        return nullptr;
    }

    return entity.get();
}

bool SpatialEntityManager::isValid(MFGame::SpatialEntity::Id ident)
{
    auto entity = mEntities[ident];
    return entity.get() != nullptr;
}

MFGame::SpatialEntity::Id SpatialEntityManager::addEntity(std::shared_ptr<SpatialEntity> entity)
{
    auto id = entity->getId();
    auto ourEntity = mEntities[id];
    if (ourEntity.get())
    {
        MFLogger::Logger::warn("Entity with existing ID (" + std::to_string(id) +
            ") being added - ignoring.",SPATIAL_ENTITY_MANAGER_MODULE_STR);
        return MFGame::SpatialEntity::NullId;
    }

    mEntities[id] = entity;

    mNumEntities++;
    return id;
}

void SpatialEntityManager::removeEntity(MFGame::SpatialEntity::Id ident)
{
    auto entity = mEntities[ident];
    if (!entity.get())
    {
        MFLogger::Logger::warn("Can't remove invalid entity.", SPATIAL_ENTITY_MANAGER_MODULE_STR);
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
