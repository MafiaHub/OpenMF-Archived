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

}

#endif
