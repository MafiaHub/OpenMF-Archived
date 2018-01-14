#ifndef LOADER_CACHE_H
#define LOADER_CACHE_H

#include <unordered_map>
#include <loggers/console.hpp>

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
    LoaderCache();
    void storeObject(std::string identifier, T obj);
    T getObject(std::string identifier);

    unsigned int getCacheHits()  { return mCacheHits;      };
    unsigned int getNumObjects() { return mObjects.size(); };

    /**
      Gets the size of the cache alone in bytes (NOT including allocated pointed to memory).
    */
    unsigned int getCacheSize()  { return mObjects.size() * sizeof(T); };

    void clear();

    void logStats();

protected:
    std::unordered_map<std::string,T> mObjects;
    unsigned int mCacheHits;
};

template<class T>
void LoaderCache<T>::logStats()
{
    MFLogger::ConsoleLogger::info("CACHE STATS:",LOADERCACHE_MODULE_STR);
    MFLogger::ConsoleLogger::info("  objects: " + std::to_string(getNumObjects()),LOADERCACHE_MODULE_STR);
    MFLogger::ConsoleLogger::info("  cache hits: " + std::to_string(getCacheHits()),LOADERCACHE_MODULE_STR);
    MFLogger::ConsoleLogger::info("  cache array memory size: " + std::to_string(getCacheSize()),LOADERCACHE_MODULE_STR);
}

template<class T>
void LoaderCache<T>::storeObject(std::string identifier, T obj)
{
    mObjects[identifier] = obj;
}

template<class T>
T LoaderCache<T>::getObject(std::string identifier)
{
    T result = mObjects[identifier];

    if (result)
    {
        mCacheHits++;
        MFLogger::ConsoleLogger::info("loading cached: " + identifier, LOADERCACHE_MODULE_STR);
    }

    return result;
}

template<class T>
LoaderCache<T>::LoaderCache()
{
    clear();
}

template<class T>
void LoaderCache<T>::clear()
{
    mObjects.clear();
    mCacheHits = 0;
}

}

#endif
