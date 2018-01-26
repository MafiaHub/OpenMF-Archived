#include <loader_cache.hpp>

namespace MFFormat
{

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
