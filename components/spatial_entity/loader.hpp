#ifndef SPATIAL_ENTITY_LOADER_H
#define SPATIAL_ENTITY_LOADER_H

#include <vector>
#include <spatial_entity/spatial_entity_implementation.hpp>
#include <osg/Node>

namespace MFFormat
{

/**
  Handles creating spatial entities from different loaded files.
 */

class SpatialEntityLoaderImplementation
{
public:
    typedef std::vector<MFGame::SpatialEntity> SpatialEntityList;

    SpatialEntityList loadFromScene(osg::Group *sceneRoot);
    // void loadModel();
protected:
};

SpatialEntityLoaderImplementation::SpatialEntityList SpatialEntityLoaderImplementation::loadFromScene(osg::Group *sceneRoot)
{
    SpatialEntityLoaderImplementation::SpatialEntityList result;
    return result;
}

};

#endif
