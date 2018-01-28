#ifndef OSG_CACHE_BIN_LOADER_H
#define OSG_CACHE_BIN_LOADER_H

#include <formats/cache_bin/parser.hpp>
#include <formats/4ds/osg.hpp>
#include <base_loader.hpp>

#define OSGCACHEBIN_MODULE_STR "loader cache.bin"

namespace MFFormat
{

class OSGCacheBinLoader : public OSGLoader
{
public:
    osg::ref_ptr<osg::Node> load(std::ifstream &srcFile, std::string fileName = "");
};

}

#endif
