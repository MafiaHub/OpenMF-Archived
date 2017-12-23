#ifndef LOADER_CACHE_H
#define LOADER_CACHE_H

#include <unordered_map>
#include <osg/Referenced>
#include <string>
#include <loggers/console.hpp>

namespace MFFormat    // TODO: MFFormat or MFRender?
{

/**
  \brief Serves to only load resources (models, textures) at most once.
*/

class OSGLoaderCache
{
public:
    void storeObject(std::string identifier, osg::ref_ptr<osg::Referenced> obj);
    osg::ref_ptr<osg::Referenced> getObject(std::string identifier);

    // TODO: functions for getting stats like number of loaded textures etc.?

protected:
    std::unordered_map<std::string,osg::ref_ptr<osg::Referenced>> mObjects;
};

void OSGLoaderCache::storeObject(std::string identifier, osg::ref_ptr<osg::Referenced> obj)
{
    mObjects[identifier] = obj;
}

osg::ref_ptr<osg::Referenced> OSGLoaderCache::getObject(std::string identifier)
{
    osg::ref_ptr<osg::Referenced> result = mObjects[identifier];

    if (result)
        MFLogger::ConsoleLogger::info("asset", "loading cached: " + identifier);

    return result;
}

}

#endif
