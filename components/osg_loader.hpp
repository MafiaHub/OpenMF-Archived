#include <osg/Node>
#include <osg/Geometry>
#include <osg/Geode>
#include <fstream>
#include <format_parsers.hpp>

namespace MFFormat
{

class Loader
{
public:
    osg::ref_ptr<osg::Node> load4ds(std::ifstream &srcFile);
};

osg::ref_ptr<osg::Node> Loader::load4ds(std::ifstream &srcFile)
{
    MFformat::DataFormat4DS format;
    osg::ref_ptr<osg::Geode> geode = new osg::Geode;

    if (format.load(srcFile))
    {
        auto model = format.getModel();
        auto lod = model->mMeshes[0].mStandard.mLODs[0];

        osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
        osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array;
        osg::ref_ptr<osg::Vec2Array> uvs = new osg::Vec2Array;

        for (size_t i = 0; i < lod.mVertexCount; ++i)
        {
            auto vertex = lod.mVertices[i];

            vertices->push_back(osg::Vec3f(vertex.mPos.x, vertex.mPos.y, vertex.mPos.z));
            normals->push_back(osg::Vec3f(vertex.mNormal.x, vertex.mNormal.y, vertex.mNormal.z));
            uvs->push_back(osg::Vec2f(vertex.mUV.x, vertex.mUV.y));
        }

        osg::ref_ptr<osg::DrawElementsUInt> indices = new osg::DrawElementsUInt(GL_TRIANGLES, 6);
        for (size_t i = 0; i < lod.mFaceGroups[0].mFaceCount; ++i)
        {
            auto face = lod.mFaceGroups[0].mFaces[i];
            indices->push_back(face.mA);
            indices->push_back(face.mB);
            indices->push_back(face.mC);
        }

        osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
        geom->setVertexArray(vertices.get());
        geom->setNormalArray(normals.get());
        geom->setTexCoordArray(0, uvs.get());

        geom->addPrimitiveSet(indices.get());
        geode->addDrawable(geom.get());
    }

    return geode;
}

}
