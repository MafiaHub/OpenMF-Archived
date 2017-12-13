#include <osg/Node>
#include <osg/Geometry>
#include <osg/Geode>
#include <fstream>

namespace MFFormat
{

class Loader
{
public:
    osg::ref_ptr<osg::Node> load4ds(std::ifstream &srcFile);
};

osg::ref_ptr<osg::Node> Loader::load4ds(std::ifstream &srcFile)
{
    osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
    osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array;

    normals->push_back(osg::Vec3(0.0f,-1.0f, 0.0f));

    vertices->push_back( osg::Vec3f(0,0,0) );
    vertices->push_back( osg::Vec3f(0,0,1) );
    vertices->push_back( osg::Vec3f(1,0,0) );

    osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;

    geom->setVertexArray(vertices.get());
    geom->setNormalArray(normals.get());
    geom->setNormalBinding(osg::Geometry::BIND_OVERALL);

    geom->addPrimitiveSet(new osg::DrawArrays(GL_TRIANGLES,0,vertices->size()));

    osg::ref_ptr<osg::Geode> geode = new osg::Geode;
    geode->addDrawable(geom.get());

    return geode;
}

}
