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
#include <4ds/parser.hpp>
#include <loggers/console.hpp>
#include <utils.hpp>
#include <osg_masks.hpp>
#include <osg_utils.hpp>
#include <base_loader.hpp>
#include <osg/FrontFace>
#include <osg/BlendFunc>
#include <osg/BlendEquation>
#include <osg/AlphaFunc>
#include <bmp_analyser.hpp>
#include <osg/TexGen>
#include <osg/TexEnv>
#include <osg/CullFace>

#define OSG4DS_MODULE_STR "loader 4ds"

namespace MFFormat
{

class OSG4DSLoader: public OSGLoader
{
public:
    virtual osg::ref_ptr<osg::Node> load(std::ifstream &srcFile, std::string fileName="") override;

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
