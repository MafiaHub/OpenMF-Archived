#ifndef SPATIAL_ENTITY_LOADER_H
#define SPATIAL_ENTITY_LOADER_H

#include <iostream>
#include <vector>
#include <string>
#include <spatial_entity/spatial_entity_implementation.hpp>
#include <osg/Node>
#include <bullet_utils.hpp>

namespace MFFormat
{

/**
  Handles creating spatial entities from different loaded files.
 */

class SpatialEntityLoaderImplementation
{
public:
    typedef std::vector<MFGame::SpatialEntityImplementation> SpatialEntityList;
    /* FIXME: SpatialEntityList should be a list of SpatialEntity, but can't because
       it's abstract => find a way to do this */

    SpatialEntityList loadFromScene(osg::Group *sceneRoot);
    // void loadModel();

protected:
    class CreateEntitiesFromSceneVisitor: public osg::NodeVisitor
    {
    public:
        virtual void apply(osg::Node &n) override
        {
            if (n.getUserDataContainer())
            {
                std::vector<std::string> descriptions = n.getUserDataContainer()->getDescriptions();
 
                if (descriptions.size() > 0 && descriptions[0].compare("4ds mesh") == 0)
                {
                    MFGame::SpatialEntityImplementation newEntity;

                    newEntity.setName(n.getName());
std::cout << n.getName() << std::endl;
                    mEntities.push_back(newEntity);
                }
            }

            if (n.asGroup())         // FIXME: why does traverse() not work?
                for (int i = 0; i < n.asGroup()->getNumChildren(); ++i)
                    n.asGroup()->getChild(i)->accept(*this);
        }

        SpatialEntityList mEntities;
    };
};

SpatialEntityLoaderImplementation::SpatialEntityList SpatialEntityLoaderImplementation::loadFromScene(osg::Group *sceneRoot)
{
    SpatialEntityLoaderImplementation::SpatialEntityList result;

    CreateEntitiesFromSceneVisitor v;
    sceneRoot->accept(v);

    return result;
}

};

#endif
