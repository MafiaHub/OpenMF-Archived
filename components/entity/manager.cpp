#include <entity/manager.hpp>
#include <engine/engine.hpp>

namespace MFGame
{

Entity *EntityManager::getEntityById(MFGame::Entity::Id id)
{
    if (id == MFGame::Entity::NullId)
        return nullptr;

    auto entity = mEntities[id];

    if (!entity.get())
    {
        MFLogger::Logger::warn("Can't retrieve invalid entity.", ENTITY_MANAGER_MODULE_STR);
        return nullptr;
    }

    return entity.get();
}

Entity *EntityManager::getEntityByName(std::string name)
{
    std::shared_ptr<Entity> entity = nullptr;

    for (auto const pair : mEntities) {
        if (pair.second && !pair.second->getName().compare(name)) {
            entity = pair.second;
            break;
        }
    }

    if (!entity.get())
    {
        MFLogger::Logger::warn("Can't retrieve invalid entity.", ENTITY_MANAGER_MODULE_STR);
        return nullptr;
    }

    return entity.get();
}

bool EntityManager::isValid(MFGame::Entity::Id ident)
{
    auto entity = mEntities[ident];
    return entity.get() != nullptr;
}

MFGame::Entity::Id EntityManager::addEntity(std::shared_ptr<Entity> entity)
{
    auto id = entity->getId();
    auto ourEntity = mEntities[id];
    if (ourEntity.get())
    {
        MFLogger::Logger::warn("Entity with existing ID (" + std::to_string(id) +
            ") being added - ignoring.",ENTITY_MANAGER_MODULE_STR);
        return MFGame::Entity::NullId;
    }

    entity->setEngine(mEngine);
    mEntities[id] = entity;

    mNumEntities++;
    return id;
}

void EntityManager::removeEntity(MFGame::Entity::Id ident)
{
    auto entity = mEntities[ident];
    if (!entity.get())
    {
        MFLogger::Logger::warn("Can't remove invalid entity.", ENTITY_MANAGER_MODULE_STR);
        return;
    }
    entity->destroy();
    entity.reset();

    mEntities.erase(ident);
    mNumEntities--;
}

void EntityManager::update(double dt)
{
    for (auto pair : mEntities)
    {
        if (pair.second && pair.second.get())
            pair.second->update(dt);

        if (pair.second->getNextThink() < mEngine->getTime())
            pair.second->think();
    }
}

unsigned int EntityManager::getNumEntities()
{
    return mNumEntities;
}

unsigned int EntityManager::getNumEntitySlots()
{
    return mEntities.size();
}

}
