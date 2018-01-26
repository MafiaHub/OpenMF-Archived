#ifndef OSG_SCENE2_LOADER_H
#define OSG_SCENE2_LOADER_H

#include <osg/Node>
#include <osg/Geometry>
#include <osg_masks.hpp>
#include <osg/MatrixTransform>
#include <osg/Texture2D>
#include <osg/ShapeDrawable>
#include <osg/Light>
#include <osg/LightSource>
#include <fstream>
#include <algorithm>
#include <scene2_bin/parser.hpp>
#include <loggers/console.hpp>
#include <osg/Material>
#include <utils.hpp>
#include <osg_utils.hpp>
#include <base_loader.hpp>
#include <osgText/Text3D>
#include <osgText/Font3D>
#include <osg/Billboard>
#include <osg/Fog>

#define OSGSCENE2BIN_MODULE_STR "loader scene2.bin"

namespace MFFormat
{

class OSGScene2BinLoader : public OSGLoader
{
public:
    typedef std::vector<osg::ref_ptr<osg::LightSource>> LightList;

    OSGScene2BinLoader();
    osg::ref_ptr<osg::Node> load(std::ifstream &srcFile, std::string fileName="");
    LightList getLightNodes() { return mLightNodes; };
    osg::Group *getCameraRelativeGroup() { return mCameraRelative.get(); };

protected:
    LightList mLightNodes;
    osg::ref_ptr<osg::Node> mDebugPointLightNode;
    osg::ref_ptr<osg::Node> mDebugDirectionalLightNode;
    osg::ref_ptr<osg::Node> mDebugOtherLightNode;

    osg::ref_ptr<MFUtil::MoveEarthSkyWithEyePointTransform> mCameraRelative;   ///< children of this node move relatively with the camera
    osg::ref_ptr<osg::Node> makeLightNode(MFFormat::DataFormatScene2BIN::Object object);
};

}

#endif
