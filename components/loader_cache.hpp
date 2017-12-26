#ifndef LOADER_CACHE_H
#define LOADER_CACHE_H

#include <unordered_map>
#include <osg/Referenced>
#include <string>
#include <loggers/console.hpp>

#define LOADERCACHE_MODULE_STR "loader cache"

namespace MFFormat // TODO: MFFormat or MFRender?
{

/**
  \brief Serves to only load resources (models, textures) at most once.
*/

class OSGLoaderCache
{
public:
    OSGLoaderCache();
    void storeObject(std::string identifier, osg::ref_ptr<osg::Referenced> obj);
    osg::ref_ptr<osg::Referenced> getObject(std::string identifier);

    unsigned int getCacheHits() { return mCacheHits; };
    unsigned int getNumObjects() { return mObjects.size(); };

    void clear();

    // TODO: functions for getting stats like number of loaded textures etc.?

protected:
    std::unordered_map<std::string,osg::ref_ptr<osg::Referenced>> mObjects;
    unsigned int mCacheHits;
};

void OSGLoaderCache::storeObject(std::string identifier, osg::ref_ptr<osg::Referenced> obj)
{
    mObjects[identifier] = obj;
}

osg::ref_ptr<osg::Referenced> OSGLoaderCache::getObject(std::string identifier)
{
    osg::ref_ptr<osg::Referenced> result = mObjects[identifier];

    if (result)
    {
        mCacheHits++;
        MFLogger::ConsoleLogger::info("loading cached: " + identifier, LOADERCACHE_MODULE_STR);
    }

    return result;
}

OSGLoaderCache::OSGLoaderCache()
{
    clear();
}

void OSGLoaderCache::clear()
{
    mObjects.clear();
    mCacheHits = 0;
}

}

#endif
