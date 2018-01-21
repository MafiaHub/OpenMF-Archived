#ifndef SPATIAL_ENTITY_MANAGER_H
#define SPATIAL_ENTITY_MANAGER_H

#include <spatial_entity/spatial_entity.hpp>
#include <spatial_entity/id_manager.hpp>
#include <loggers/console.hpp>

#include <vector>

#define SPATIAL_ENTITY_MANAGER_MODULE_STR "spatial entity manager"

namespace MFGame
{

class SpatialEntityManager
{
public:
    SpatialEntityManager()
    {
        mIDManager = std::make_shared<BackingIDManager>();
    }

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

    void setIDManager(std::shared_ptr<IDManager> manager) { mIDManager = manager; }
    IDManager *getIDManager()                       const { return mIDManager.get(); }

protected:
    std::vector<std::shared_ptr<SpatialEntity>> mEntities;
    size_t mNumEntities;
    std::shared_ptr<IDManager> mIDManager;
};

SpatialEntity *SpatialEntityManager::getEntityByIndex(unsigned int index)
{
    if (index < 0 || index > mEntities.size())
    {
        return nullptr;
    }

    auto e = mEntities[index].get();

    if (MFGame::Id(e->getId()) == NullId)
    {
        return nullptr;
    }

    return e;
}

SpatialEntity *SpatialEntityManager::getEntityById(MFGame::Id id)
{
    auto index = mIDManager->getSlot(MFGame::Id(id));

    if (index == NullId.Index)
    {
        MFLogger::ConsoleLogger::warn("Can't retrieve invalid entity.", SPATIAL_ENTITY_MANAGER_MODULE_STR);
        return nullptr;
    }

    return mEntities.at(index).get();
}

bool SpatialEntityManager::isValid(Id ident)
{
    return mIDManager->isValid(ident);
}

Id SpatialEntityManager::addEntity(std::shared_ptr<SpatialEntity> entity)
{
    if (getEntityById(entity->getId()) != 0)
    {
        MFLogger::ConsoleLogger::warn("Entity with existing ID (" + std::to_string(entity->getId()) +
            ") being added - ignoring.",SPATIAL_ENTITY_MANAGER_MODULE_STR);
        return NullId;
    }

    auto id = mIDManager->allocate();
    entity->setId(id.Value);

    if (id.Index < mEntities.size())
        mEntities.at(id.Index) = entity;
    else
        mEntities.push_back(entity);

    mNumEntities++;
    return id;
}

void SpatialEntityManager::removeEntity(Id ident)
{
    auto index = mIDManager->getSlot(ident);
    if (index == NullId.Index)
    {
        MFLogger::ConsoleLogger::warn("Can't remove invalid entity.", SPATIAL_ENTITY_MANAGER_MODULE_STR);
        return;
    }

    auto entity = mEntities.at(index);
    entity->setId(NullId.Value);
    mIDManager->deallocate(ident);
    mNumEntities--;
}

void SpatialEntityManager::update(double dt)
{
    for (int i = 0; i < (int) mEntities.size(); ++i)
        if (mEntities[i]->getId() != NullId.Value)
            mEntities[i]->update(dt);
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
