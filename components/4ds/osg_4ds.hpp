#ifndef OSG_4DS_LOADER_H
#define OSG_4DS_LOADER_H

#include <osg/Node>
#include <osg/Geometry>
#include <osg/MatrixTransform>
#include <osg/Material>
#include <osg/Geode>
#include <osg/Texture2D>
#include <vfs/vfs.hpp>
#include <fstream>
#include <algorithm>
#include <4ds/parser_4ds.hpp>
#include <utils/logger.hpp>
#include <utils/openmf.hpp>
#include <renderer/osg_masks.hpp>
#include <utils/osg.hpp>
#include <base_loader.hpp>
#include <osg/FrontFace>
#include <osg/BlendFunc>
#include <osg/BlendEquation>
#include <osg/AlphaFunc>
#include <utils/bmp_analyser.hpp>
#include <osg/TexGen>
#include <osg/TexEnv>
#include <osg/CullFace>
#include <osgDB/ReadFile>
#include <osg/Billboard>
#include <osg/LOD>

#define OSG4DS_MODULE_STR "loader 4ds"

namespace MFFormat
{

class OSG4DSLoader: public OSGLoader
{
public:
    osg::ref_ptr<osg::Node> load(MFFormat::DataFormat4DS *format, std::string fileName="");

protected:
    typedef std::vector<osg::ref_ptr<osg::StateSet>> MaterialList;

    osg::ref_ptr<osg::Node> make4dsMesh(MFFormat::DataFormat4DS::Mesh *mesh, MaterialList &materials);
    osg::ref_ptr<osg::StateSet> make4dsMaterial(MFFormat::DataFormat4DS::Material *material);
    osg::ref_ptr<osg::Node> make4dsMeshLOD(
        MFFormat::DataFormat4DS::Lod *meshLOD,
        MaterialList &materials,
        bool isBillboard=false,
        osg::Vec3f billboardAxis=osg::Vec3f(0,0,1));
    osg::ref_ptr<osg::Node> make4dsFaceGroup(
        osg::Vec3Array *vertices,
        osg::Vec3Array *normals,
        osg::Vec2Array *uvs,
        MFFormat::DataFormat4DS::FaceGroup *faceGroup,
        bool isBillboard=false,
        osg::Vec3f billboardAxis=osg::Vec3f(0,0,1));
    osg::ref_ptr<osg::Texture2D> loadTexture(std::string fileName, std::string fileNameAlpha="", bool colorKey=false);

    std::vector<std::string> makeAnimationNames(std::string baseFileName, unsigned int frames);
};

}

#endif
