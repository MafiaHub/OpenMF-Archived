#ifndef OSG_CHECK_BIN_LOADER_H
#define OSG_CHECK_BIN_LOADER_H

#include <base_loader.hpp>

namespace MFFormat
{

class OSGCheckBinLoader : public OSGLoader
{
public:
    osg::ref_ptr<osg::Node> load(std::ifstream &srcFile, std::string fileName = "");
};

}

#endif
