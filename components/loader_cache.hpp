#ifndef LOADER_CACHE_H
#define LOADER_CACHE_H

#include <unordered_map>
#include <utils/logger.hpp>

#define LOADERCACHE_MODULE_STR "loader cache"

namespace MFFormat
{

/**
  \brief Serves to only load resources (models, textures, parsed files, ...) at most once.
*/

template <class T>
class LoaderCache
{
public:
    LoaderCache()
    {
        clear();
    }

    void storeObject(std::string identifier, T obj)
    {
        mObjects[identifier] = obj;
    }

    T getObject(std::string identifier)
    {
        T result = mObjects[identifier];

        if (result)
        {
            mCacheHits++;
            MFLogger::Logger::info("loading cached: " + identifier, LOADERCACHE_MODULE_STR);
        }

        return result;
    }

    unsigned int getCacheHits()  { return mCacheHits;      };
    unsigned int getNumObjects() { return mObjects.size(); };

    /**
      Gets the size of the cache alone in bytes (NOT including allocated pointed to memory).
    */
    unsigned int getCacheSize()  { return mObjects.size() * sizeof(T); };

    void clear()
    {
        mObjects.clear();
        mCacheHits = 0;
    }

    void logStats()
    {
        MFLogger::Logger::info("CACHE STATS:",LOADERCACHE_MODULE_STR);
        MFLogger::Logger::info("  objects: " + std::to_string(getNumObjects()),LOADERCACHE_MODULE_STR);
        MFLogger::Logger::info("  cache hits: " + std::to_string(getCacheHits()),LOADERCACHE_MODULE_STR);
        MFLogger::Logger::info("  cache array memory size: " + std::to_string(getCacheSize()),LOADERCACHE_MODULE_STR);
    }

protected:
    std::unordered_map<std::string,T> mObjects;
    unsigned int mCacheHits;
};

}

#endif
