#ifndef ENTITY_MANAGER_H
#define ENTITY_MANAGER_H

#include <entity/entity.hpp>
#include <utils/logger.hpp>

#include <unordered_map>

#define ENTITY_MANAGER_MODULE_STR "spatial entity manager"

namespace MFGame
{

    class Engine;

class EntityManager
{
public:
    typedef std::unordered_map<uint64_t, std::shared_ptr<Entity>> EntityMap;
    EntityManager(MFGame::Engine *engine) {
        mEngine = engine;
    };
    ~EntityManager() {
        for (auto pair : mEntities)
        {
            if (pair.second && pair.second.get())
            {
                auto entity = pair.second.get();
                entity->destroy();
                pair.second.reset();
            }
        }

        mEntities.clear();
    }
    Entity *getEntityById(MFGame::Entity::Id id);

    /**
     * Retrieves entity by its name.
     * Note that there might be multiple entities with the same name and this method
     * retrieves only the first one found. This makes the use case much more restricted.
     */
    Entity *getEntityByName(std::string name);

    MFGame::Entity::Id addEntity(std::shared_ptr<Entity> entity);
    void removeEntity(MFGame::Entity::Id ident);
    bool isValid(MFGame::Entity::Id ident);

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

    EntityMap *getEntities() { return &mEntities; };

protected:
    EntityMap mEntities;
    size_t mNumEntities;

    MFGame::Engine *mEngine;
};

}

#endif
