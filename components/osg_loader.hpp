#include <osg/Node>
#include <osg/Geometry>
#include <osg/MatrixTransform>
#include <osg/Geode>
#include <osg/Texture2D>
#include <fstream>
#include <algorithm>
#include <format_parsers.hpp>

namespace MFFormat
{

class Loader
{
public:
    osg::ref_ptr<osg::Node> load4ds(std::ifstream &srcFile);

    void setTextureDir(std::string textureDir);

protected:
    typedef std::vector<osg::ref_ptr<osg::StateSet>> MaterialList;

    osg::ref_ptr<osg::Node> make4dsMesh(MFFormat::DataFormat4DS::Mesh *mesh, MaterialList &materials);
    osg::ref_ptr<osg::Node> make4dsMeshLOD(MFFormat::DataFormat4DS::Lod *meshLOD, MaterialList &materials);
    osg::ref_ptr<osg::Node> make4dsFaceGroup(
        osg::Vec3Array *vertices,
        osg::Vec3Array *normals,
        osg::Vec2Array *uvs,
        MFFormat::DataFormat4DS::FaceGroup *faceGroup);

    std::string mTextureDir;
};

void Loader::setTextureDir(std::string textureDir)
{
    mTextureDir = textureDir;
}

osg::ref_ptr<osg::Node> Loader::make4dsFaceGroup(
        osg::Vec3Array *vertices,
        osg::Vec3Array *normals,
        osg::Vec2Array *uvs,
        MFFormat::DataFormat4DS::FaceGroup *faceGroup)
{
    std::cout << "      loading facegroup";
    std::cout << ", material: " << faceGroup->mMaterialID << std::endl;

    osg::ref_ptr<osg::Geode> geode = new osg::Geode;

    osg::ref_ptr<osg::DrawElementsUInt> indices = new osg::DrawElementsUInt(GL_TRIANGLES);

    for (size_t i = 0; i < faceGroup->mFaceCount; ++i)
    {
        auto face = faceGroup->mFaces[i];
        indices->push_back(face.mA);
        indices->push_back(face.mB);
        indices->push_back(face.mC);
    }

    osg::ref_ptr<osg::Geometry> geom = new osg::Geometry();
    geom->setVertexArray(vertices);
    geom->setNormalArray(normals);
    geom->setTexCoordArray(0,uvs);

    geom->addPrimitiveSet(indices.get());
    geode->addDrawable(geom.get());

    return geode;
}

osg::ref_ptr<osg::Node> Loader::make4dsMesh(DataFormat4DS::Mesh *mesh, MaterialList &materials)
{
    std::cout << "  loading mesh";
    std::cout << ", LOD level: " << ((int) mesh->mStandard.mLODLevel);
    std::cout << ", type: " << ((int) mesh->mMeshType) << std::endl;

    const float maxDistance = 100.0;
    const float stepLOD = maxDistance / mesh->mStandard.mLODLevel;

    osg::ref_ptr<osg::LOD> nodeLOD = new osg::LOD();

    for (int i = 0; i < mesh->mStandard.mLODLevel; ++i)
    {
        nodeLOD->addChild(make4dsMeshLOD(&(mesh->mStandard.mLODs[i]),materials));
        nodeLOD->setRange(i,i * stepLOD, (i + 1) * stepLOD);
    }

    return nodeLOD; 
}

osg::ref_ptr<osg::Node> Loader::make4dsMeshLOD(DataFormat4DS::Lod *meshLOD, MaterialList &materials)
{
    std::cout << "    loading LOD";
    std::cout << ", vertices: " << meshLOD->mVertexCount << ", face groups: " << ((int) meshLOD->mFaceGroupCount) << std::endl;

    osg::ref_ptr<osg::Group> group = new osg::Group();

    osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
    osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array;
    osg::ref_ptr<osg::Vec2Array> uvs = new osg::Vec2Array;

    for (size_t i = 0; i < meshLOD->mVertexCount; ++i)
    {
        auto vertex = meshLOD->mVertices[i];

        vertices->push_back(osg::Vec3f(vertex.mPos.x, vertex.mPos.y, vertex.mPos.z));
        normals->push_back(osg::Vec3f(vertex.mNormal.x, vertex.mNormal.y, vertex.mNormal.z));
        uvs->push_back(osg::Vec2f(vertex.mUV.x, vertex.mUV.y));
    }

    osg::ref_ptr<osg::DrawElementsUInt> indices = new osg::DrawElementsUInt(GL_TRIANGLES);

    for (size_t i = 0; i < meshLOD->mFaceGroupCount; ++i)
    {
        osg::ref_ptr<osg::Node> faceGroup = make4dsFaceGroup(
            vertices.get(),
            normals.get(),
            uvs.get(),
            &(meshLOD->mFaceGroups[i]));

        const int materialID = std::max(0,std::min(
            static_cast<int>(materials.size() - 1),
            meshLOD->mFaceGroups[i].mMaterialID - 1));

        faceGroup->setStateSet(materials[materialID]);

        group->addChild(faceGroup);
    }

    return group;
}

osg::ref_ptr<osg::Node> Loader::load4ds(std::ifstream &srcFile)
{
    std::cout << "loading model";

    MFFormat::DataFormat4DS format;

    osg::ref_ptr<osg::Group> group = new osg::Group();

    if (format.load(srcFile))
    {
        auto model = format.getModel();
        
        std::cout << ", meshes: " << model->mMeshCount;
        std::cout << ", materials: " << model->mMaterialCount << std::endl;

        MaterialList materials;

        for (int i = 0; i < model->mMaterialCount; ++i)  // load materials
        {
            materials.push_back(new osg::StateSet());

            osg::StateSet *stateSet = materials[i].get();

            char diffuseTextureName[255];
            memcpy(diffuseTextureName,model->mMaterials[i].mDiffuseMapName,255);
            diffuseTextureName[model->mMaterials[i].mDiffuseMapNameLength] = 0;  // terminate the string

            std::string texturePath = mTextureDir + "/" + diffuseTextureName;    // FIXME: platform independent path concat

            std::cout << "  loading texture: " << texturePath << std::endl;
    
            osg::ref_ptr<osg::Texture2D> tex = new osg::Texture2D();
            osg::ref_ptr<osg::Image> img = osgDB::readImageFile( texturePath );
            tex->setImage(img);

            stateSet->setTextureAttributeAndModes(0,tex.get(), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
        }

        for (int i = 0; i < model->mMeshCount; ++i)      // load meshes
        {
            osg::ref_ptr<osg::MatrixTransform> transform = new osg::MatrixTransform();
            osg::Matrixd mat;

            MFFormat::DataFormat4DS::Vec3 p, s;
            MFFormat::DataFormat4DS::Quat r;

            p = model->mMeshes[i].mPos;
            s = model->mMeshes[i].mScale;
            r = model->mMeshes[i].mRot;

            mat.preMultTranslate(osg::Vec3f(p.x,p.y,p.z));
            mat.preMultScale(osg::Vec3f(s.x,s.y,s.z));
            mat.preMultRotate(osg::Quat(r.x,r.y,r.z,r.w)); 

            transform->setMatrix(mat);

            transform->addChild(make4dsMesh(&(model->mMeshes[i]),materials));
            group->addChild(transform);
        }
    }

    return group;
}

}
