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
    osg::ref_ptr<osg::Vec2Array> uvs = new osg::Vec2Array;

    osg::ref_ptr<osg::DrawElementsUInt> indices = new osg::DrawElementsUInt(GL_TRIANGLES,6);

    osg::Vec3 defaultNormal = osg::Vec3(-1,-1,-1);

    // create vertices:

    vertices->push_back( osg::Vec3f(0,0,0) );
    normals->push_back( defaultNormal );
    uvs->push_back( osg::Vec2f(0,0) );

    vertices->push_back( osg::Vec3f(0,0,1) );
    normals->push_back( defaultNormal );
    uvs->push_back( osg::Vec2f(0,1) );

    vertices->push_back( osg::Vec3f(1,0,0) );
    normals->push_back( defaultNormal );
    uvs->push_back( osg::Vec2f(1,0) );

    vertices->push_back( osg::Vec3f(1,0,1) );
    normals->push_back( defaultNormal );
    uvs->push_back( osg::Vec2f(1,1) );

    // indices for indexed drawing:

    (*indices)[0] = 0;
    (*indices)[1] = 1;
    (*indices)[2] = 2;
    (*indices)[3] = 1;
    (*indices)[4] = 2;
    (*indices)[5] = 3;

    osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;

    geom->setVertexArray(vertices.get());
    geom->setNormalArray(normals.get());
    geom->setTexCoordArray(0,uvs.get());

    geom->addPrimitiveSet(indices.get());

    osg::ref_ptr<osg::Geode> geode = new osg::Geode;
    geode->addDrawable(geom.get());

    return geode;
}

}
