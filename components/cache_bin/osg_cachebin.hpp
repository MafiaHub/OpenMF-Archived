#ifndef OSG_CACHE_BIN_LOADER_H
#define OSG_CACHE_BIN_LOADER_H

#include <cache_bin/parser_cachebin.hpp>
#include <4ds/osg_4ds.hpp>
#include <base_loader.hpp>

#define OSGCACHEBIN_MODULE_STR "loader cache.bin"

namespace MFFormat
{

class OSGCacheBinLoader : public OSGLoader
{
public:
    osg::ref_ptr<osg::Node> load(MFFormat::DataFormatCacheBIN *format, std::string fileName = "");
};

}

#endif
