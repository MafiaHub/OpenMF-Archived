#include <zpl.h>
#include <zpl_math.h>
#include <common.h>
#include <vfs.h>
#include <logger.h>
#include <utils/osg_utils.hpp>
#include <formats/check_bin/check_bin.h>
#include <formats/check_bin/osg.hpp>

#include <osg/Node>
#include <osg/Geometry>
#include <osg/MatrixTransform>
#include <osg/ShapeDrawable>
#include <utils/openmf.hpp>
#include <osg_masks.hpp>

#define omf_logger_info(fmt, ...) \
        omf_logger_ext(NULL, OMF_LOGGER_INFO, fmt, #__VA_ARGS__)

#define omf_logger_fatal(fmt, ...) \
        omf_logger_ext(NULL, OMF_LOGGER_FATAL, fmt, #__VA_ARGS__)

namespace MFFormat
{

typedef struct {
    osg::Vec3f mStart;
    osg::Vec3f mEnd;
    uint16_t mType;
} omf_checkbin__line_t;

std::vector<omf_checkbin__line_t> omf_checkbin__resolve_links(omf_checkbin_t *format) {
    std::vector<omf_checkbin__line_t> mPointsConnections;

    size_t linkIndex = 0;
    for (usize k = 0; k < zpl_array_count(format->points); ++k) {
        if (format->points[k].mEnterLinks < 1) {
            continue;
        }

        auto point = format->points[k];

        for(size_t i = linkIndex; i < linkIndex + point.mEnterLinks; i++) {
            omf_assert(i < zpl_array_count(format->links));
            auto currentLink = format->links[i];

            if (currentLink.mTargetPoint > 0 && currentLink.mTargetPoint < zpl_array_count(format->points)) {
                auto targetPoint = format->points[currentLink.mTargetPoint];
                omf_checkbin__line_t lineToPush = {
                    // TODO: remove old math lib
                    omf_osg_utils_v3(point.mPos),
                    omf_osg_utils_v3(targetPoint.mPos),
                    targetPoint.mType
                };

                mPointsConnections.push_back(lineToPush);
            }
        }

        linkIndex += point.mEnterLinks;
    }

    return mPointsConnections;
}

u8 omf_checkbin__color_from_type(u16 type) {
    switch(type) {
        case OMF_CHECKBIN_POINTTYPE_PEDESTRIAN:
            return 0;

        case OMF_CHECKBIN_POINTTYPE_AI:
            return 1;

        case OMF_CHECKBIN_POINTTYPE_VEHICLE:
            return 2;

        case OMF_CHECKBIN_POINTTYPE_TRAM_STATION:
            return 3;

        default:
            return 4;
    }
}


osg::ref_ptr<osg::Node> OSGCheckBinLoader::load(std::ifstream &srcFile, std::string fileName)
{
    osg::ref_ptr<osg::Group> group = new osg::Group();

    group->setNodeMask(MFRender::MASK_DEBUG);
    group->setName("check.bin");

    omf_logger_info("loading check.bin: %s", fileName.c_str());

    // TODO: remove file opening from there
    // TODO: change singnature from ifstream to zpl_file_t

    zpl_file_t file = {0};
    if (!omf_vfs_open(&file, fileName.c_str())) {
        omf_logger_fatal("could not open file %s", input.c_str());
        return nullptr;
    }

    omf_checkbin_t format = {0};
    if (!omf_checkbin_parse(&format, &file)) {
        omf_logger_fatal("could not parse file %f", fileName.c_str());
        return nullptr;
    }

    osg::ref_ptr<osg::ShapeDrawable> debugNodes[5];
    std::vector<osg::Vec4f> debugNodesColors = {
        osg::Vec4f(0,0,1,0),
        osg::Vec4f(0,1,1,0),
        osg::Vec4f(1,1,0,0),
        osg::Vec4f(1,0,0,0),
        osg::Vec4f(0,0,0,0)
    };

    for (size_t i = 0; i < debugNodesColors.size(); i++) {
        debugNodes[i] = new osg::ShapeDrawable(new osg::Sphere(osg::Vec3f(0,0,0),0.1));
        debugNodes[i]->setColor(debugNodesColors.at(i));
        debugNodes[i]->setUseDisplayList(false);
    }

    for (auto line : omf_checkbin__resolve_links(&format)) {
        osg::ref_ptr<osg::Vec3Array> points = new osg::Vec3Array;
        osg::ref_ptr<osg::Vec4Array> color = new osg::Vec4Array;
        osg::ref_ptr<osg::Geometry> geometry(new osg::Geometry);

        points->push_back(line.mStart);
        points->push_back(line.mEnd);
        color->push_back(debugNodesColors.at(omf_checkbin__color_from_type(line.mType)));
        geometry->setVertexArray(points.get());
        geometry->setColorArray(color.get());
        geometry->setColorBinding(osg::Geometry::BIND_OVERALL);
        geometry->addPrimitiveSet(new osg::DrawArrays(GL_LINES, 0, points->size()));
        group->addChild(geometry);

    } //for links

    for (usize i = 0; i < zpl_array_count(format.points); ++i) {
        auto point = format.points[i];
        osg::ref_ptr<osg::MatrixTransform> objectTransform = new osg::MatrixTransform();
        osg::Matrixd m; m.preMultTranslate(omf_osg_utils_v3(point.mPos));
        objectTransform->setMatrix(m);
        objectTransform->addChild(debugNodes[omf_checkbin__color_from_type(point.mType)]);
        group->addChild(objectTransform);

    } //for points

    omf_checkbin_free(&format);
    omf_vfs_close(&file); // TODO: remove
    return group;
}

}
