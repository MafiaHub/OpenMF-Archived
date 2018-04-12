#ifndef SPATIAL_ENTITY_MANAGER_H
#define SPATIAL_ENTITY_MANAGER_H

#include <spatial_entity/spatial_entity.hpp>
#include <utils/logger.hpp>

#include <unordered_map>

#define SPATIAL_ENTITY_MANAGER_MODULE_STR "spatial entity manager"

namespace MFGame
{

class SpatialEntityManager
{
public:
    typedef std::unordered_map<uint64_t, std::shared_ptr<SpatialEntity>> EntityMap;
    SpatialEntityManager() {};
    SpatialEntity *getEntityById(MFGame::SpatialEntity::Id id);

    /**
     * Retrieves entity by its name.
     * Note that there might be multiple entities with the same name and this method
     * retrieves only the first one found. This makes the use case much more restricted.
     */
    SpatialEntity *getEntityByName(std::string name);

    MFGame::SpatialEntity::Id addEntity(std::shared_ptr<SpatialEntity> entity);
    void removeEntity(MFGame::SpatialEntity::Id ident);
    bool isValid(MFGame::SpatialEntity::Id ident);

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
};

}

#endif
