#ifndef OSG_CHECK_BIN_LOADER_H
#define OSG_CHECK_BIN_LOADER_H

#include <check_bin/parser.hpp>
#include <osg/Node>
#include <osg/Geometry>
#include <osg/MatrixTransform>
#include <osg/ShapeDrawable>
#include <utils.hpp>
#include <base_loader.hpp>
#include <osg_masks.hpp>

#define OSGCHECKBIN_MODULE_STR "loader check.bin"

namespace MFFormat
{

class OSGCheckBinLoader : public OSGLoader
{
public:
    typedef struct 
    {
        osg::Vec3f mStart;
        osg::Vec3f mEnd;
        uint16_t mType;
    } Line;

    osg::ref_ptr<osg::Node> load(std::ifstream &srcFile, std::string fileName = "");
    std::vector<Line> resolveLinks(MFFormat::DataFormatCheckBIN parser);
    size_t getColorFromIndexOfType(uint16_t type);
};

}

#endif
