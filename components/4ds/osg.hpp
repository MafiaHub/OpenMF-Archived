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
#include <osg_utils.hpp>
#include <base_loader.hpp>
#include <osg/FrontFace>
#include <osg/BlendFunc>
#include <osg/AlphaFunc>
#include <bmp_analyser.hpp>

#define OSG4DS_MODULE_STR "loader 4ds"

namespace MFFormat
{

class TextureSwitchCallback: public osg::StateSet::Callback
{
public:
    virtual void operator()(osg::StateSet *s, osg::NodeVisitor *n)
    {
        if (mTmpCoundown <= 0)   // TODO: do this based on time, not frames
            mTmpCoundown = 60;
        else
        {
            mTmpCoundown--;
            return;
        }

        mCurrentTexture = (mCurrentTexture + 1) % mTextures.size();

        s->setTextureAttributeAndModes(0,mTextures[mCurrentTexture].get(),
            osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
    }

    void addTexture(osg::Texture2D *texture)
    {
        mTextures.push_back(texture);
    }

protected:
    std::vector<osg::ref_ptr<osg::Texture2D>> mTextures;
    unsigned int mCurrentTexture;
    unsigned int mTmpCoundown;
};

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

std::vector<std::string> OSG4DSLoader::makeAnimationNames(std::string baseFileName, unsigned int frames)
{
    // FIXME: make this nicer 

    std::vector<std::string> result;

    if (baseFileName.length() == 0)
    {
        for (unsigned int i = 0; i < frames; ++i)
            result.push_back("");

        return result;
    }

    size_t dotPos = baseFileName.find('.');

    std::string preStr, postStr;

    postStr = baseFileName.substr(dotPos);
    preStr = baseFileName.substr(0,dotPos - 2);

    for (unsigned int i = 0; i < frames; ++i)
    {
        std::string numStr = std::to_string(i);

        if (numStr.length() == 1)
            numStr = "0" + numStr;

        result.push_back(preStr + numStr + postStr);
    }

    return result;
}

osg::ref_ptr<osg::Texture2D> OSG4DSLoader::loadTexture(std::string fileName, std::string fileNameAlpha, bool colorKey)
{
    std::string textureIdentifier = fileName + ";" + fileNameAlpha + ";" + (colorKey ? "1" : "0");

    osg::ref_ptr<osg::Texture2D> cached = (osg::Texture2D *) getFromCache(textureIdentifier).get();   
    
    if (cached)
        return cached;

    std::string logStr = "loading texture " + fileName;

    bool alphaTexture = fileNameAlpha.length() > 0;

    if (alphaTexture)
        logStr += " (alpha texture: " + fileNameAlpha + ")";

    logStr += ".";

    MFLogger::ConsoleLogger::info(logStr, OSG4DS_MODULE_STR);

    osg::ref_ptr<osg::Texture2D> tex = new osg::Texture2D();
     
    tex->setWrap(osg::Texture::WRAP_S,osg::Texture::REPEAT);
    tex->setWrap(osg::Texture::WRAP_T,osg::Texture::REPEAT);

    std::string filePath = MFFile::convertPathToCanonical(getTextureDir() + fileName);
    std::string fileLocation = mFileSystem->getFileLocation(filePath);

    std::string filePathAlpha;
    std::string fileLocationAlpha;

    if (alphaTexture)
    {
        filePathAlpha = MFFile::convertPathToCanonical(getTextureDir() + fileNameAlpha);
        fileLocationAlpha = mFileSystem->getFileLocation(filePathAlpha);
    } 

    osg::ref_ptr<osg::Image> img;

    if (fileLocation.length() == 0 && fileName.length() != 0)
    {
        MFLogger::ConsoleLogger::warn("Could not load texture: " + fileName, OSG4DS_MODULE_STR);
    }
    else if (fileName.length() != 0)
    {
        img = osgDB::readImageFile(fileLocation);

        if (colorKey)
        {
            MFFormat::BMPInfo bmp;

            std::ifstream bmpFile;
            bmpFile.open(fileLocation);
            bmp.load(bmpFile);
            bmpFile.close();

            osg::Vec3f transparentColor = osg::Vec3f(     
                bmp.mTransparentColor.r / 255.0,
                bmp.mTransparentColor.g / 255.0,
                bmp.mTransparentColor.b / 255.0);

            img = MFUtil::applyColorKey(img.get(), transparentColor, 0.04f );
        }

        if (alphaTexture)
        {
            if (fileLocationAlpha.length() == 0)
            {
                MFLogger::ConsoleLogger::warn("Could not load alpha texture: " + fileNameAlpha, OSG4DS_MODULE_STR);
            }
            else
            {
                osg::ref_ptr<osg::Image> imgAlpha = osgDB::readImageFile(fileLocationAlpha);
                img = MFUtil::addAlphaFromImage(img.get(),imgAlpha.get());
            }
        }

        tex->setImage(img);
		tex->setMaxAnisotropy(16.0f);
    }

    storeToCache(textureIdentifier,tex);

    return tex;
}

osg::ref_ptr<osg::Node> OSG4DSLoader::make4dsFaceGroup(
        osg::Vec3Array *vertices,
        osg::Vec3Array *normals,
        osg::Vec2Array *uvs,
        MFFormat::DataFormat4DS::FaceGroup *faceGroup,
        bool isBillboard,
        osg::Vec3f billboardAxis)
{
    MFLogger::ConsoleLogger::info("      loading facegroup, material: " + std::to_string(faceGroup->mMaterialID) + ".", OSG4DS_MODULE_STR);

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

    geom->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);

    geom->addPrimitiveSet(indices.get());

    if (isBillboard)
    {
        osg::ref_ptr<osg::Billboard> billboard = new osg::Billboard;
        billboard->addDrawable(geom);
        billboard->setAxis(billboardAxis);
        return billboard;
    }

    osg::ref_ptr<osg::Geode> geode = new osg::Geode;
    geode->addDrawable(geom.get());
    return geode;
}

osg::ref_ptr<osg::Node> OSG4DSLoader::make4dsMesh(DataFormat4DS::Mesh *mesh, MaterialList &materials)
{
    if (mesh->mMeshType != MFFormat::DataFormat4DS::MESHTYPE_STANDARD)
    {
        osg::ref_ptr<osg::Node> emptyNode;
        return emptyNode;
    }

    MFFormat::DataFormat4DS::Standard standard;

    bool isBillboard = false;
    bool unknown = false;

    switch (mesh->mVisualMeshType)
    {
        case MFFormat::DataFormat4DS::VISUALMESHTYPE_STANDARD:
            standard = mesh->mStandard;
            break;

        case MFFormat::DataFormat4DS::VISUALMESHTYPE_BILLBOARD:
            isBillboard = true;
            standard = mesh->mBillboard.mStandard;
            break;

        case MFFormat::DataFormat4DS::VISUALMESHTYPE_SINGLEMESH:
            standard = mesh->mSingleMesh.mStandard;
            break;

        case MFFormat::DataFormat4DS::VISUALMESHTYPE_SINGLEMORPH:
            standard = mesh->mSingleMorph.mSingleMesh.mStandard;
            break;

        case MFFormat::DataFormat4DS::VISUALMESHTYPE_MORPH:
            standard = mesh->mMorph.mStandard;
            break;

        default:
            unknown = true;
            break;
    }

    if (unknown)
    {
        osg::ref_ptr<osg::Node> emptyNode;
        return emptyNode;
    }

    MFLogger::ConsoleLogger::info("  loading mesh, LOD level: " + std::to_string((int) standard.mLODLevel) +
        ", type: " + std::to_string((int) mesh->mMeshType) +
        ", instanced: " + std::to_string(standard.mInstanced),
 
        OSG4DS_MODULE_STR);

    const float maxDistance = 10000000.0;
    const float stepLOD = maxDistance / standard.mLODLevel;

    osg::ref_ptr<osg::LOD> nodeLOD = new osg::LOD();

    float previousDist = 0.0;

    for (int i = 0; i < standard.mLODLevel; ++i)
    {
        float distLOD = standard.mLODLevel == 1 ? maxDistance : standard.mLODs[i].mRelativeDistance;

        nodeLOD->addChild(make4dsMeshLOD(&(standard.mLODs[i]),materials,isBillboard));  // TODO: add rotation axis
        nodeLOD->setRange(i,previousDist,distLOD);
        previousDist = distLOD;
    }

    return nodeLOD; 
}

osg::ref_ptr<osg::Node> OSG4DSLoader::make4dsMeshLOD(
    DataFormat4DS::Lod *meshLOD,
    MaterialList &materials,
    bool isBillboard,
    osg::Vec3 billboardAxis)
{
    MFLogger::ConsoleLogger::info("    loading LOD, vertices: " + std::to_string(meshLOD->mVertexCount) +
        ", face groups: " + std::to_string((int) meshLOD->mFaceGroupCount),
 
        OSG4DS_MODULE_STR);

    osg::ref_ptr<osg::Group> group = new osg::Group();

    osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
    osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array;
    osg::ref_ptr<osg::Vec2Array> uvs = new osg::Vec2Array;

    for (size_t i = 0; i < meshLOD->mVertexCount; ++i)
    {
        auto vertex = meshLOD->mVertices[i];

        osg::Vec3 normal = toOSG( vertex.mNormal );
        normal.normalize();

        vertices->push_back( toOSG(vertex.mPos) );
        normals->push_back(normal);
        uvs->push_back(osg::Vec2f(vertex.mUV.x, 1.0 - vertex.mUV.y));
    }

    osg::ref_ptr<osg::DrawElementsUInt> indices = new osg::DrawElementsUInt(GL_TRIANGLES);

    for (size_t i = 0; i < meshLOD->mFaceGroupCount; ++i)
    {
        osg::ref_ptr<osg::Node> faceGroup = make4dsFaceGroup(
            vertices.get(),
            normals.get(),
            uvs.get(),
            &(meshLOD->mFaceGroups[i]),
            isBillboard,
            billboardAxis);

        const int materialID = std::max(0,std::min(
            static_cast<int>(materials.size() - 1),
            meshLOD->mFaceGroups[i].mMaterialID - 1));

        // TODO: set default material when materialID = 0
        // or no materials are defined in .4ds file
        if(materials.size() > 0)
            faceGroup->setStateSet(materials[materialID]);

        group->addChild(faceGroup);
    }

    return group;
}

osg::ref_ptr<osg::StateSet> OSG4DSLoader::make4dsMaterial(MFFormat::DataFormat4DS::Material *material)
{
    osg::ref_ptr<osg::StateSet> stateSet = new osg::StateSet;

    auto mat = new osg::Material();

    bool colorKey = material->mFlags & MFFormat::DataFormat4DS::MATERIALFLAG_COLORKEY;

    MFFormat::DataFormat::Vec3 dif = material->mDiffuse;
    MFFormat::DataFormat::Vec3 amb = material->mAmbient;
    MFFormat::DataFormat::Vec3 emi = material->mEmission;

    // TODO:
    //mat->setDiffuse(osg::Material::FRONT_AND_BACK,osg::Vec4f(dif.x,dif.y,dif.z,1.0));
    //mat->setAmbient(osg::Material::FRONT_AND_BACK,osg::Vec4f(amb.x,amb.y,amb.z,1.0));
    //mat->setEmission(osg::Material::FRONT_AND_BACK,osg::Vec4f(emi.x,emi.y,emi.z,1.0));

    if (material->mTransparency < 1)
    {
        osg::Vec4f d = mat->getDiffuse(osg::Material::FRONT_AND_BACK);

        mat->setDiffuse(osg::Material::FRONT_AND_BACK,osg::Vec4f(d.x(),d.y(),d.z(),material->mTransparency));

        stateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
        osg::ref_ptr<osg::BlendFunc> blendFunc = new osg::BlendFunc;
        blendFunc->setFunction(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        stateSet->setMode(GL_BLEND, osg::StateAttribute::ON);
        stateSet->setAttributeAndModes(blendFunc.get(),osg::StateAttribute::ON);
    }

    char diffuseTextureName[255];
    memcpy(diffuseTextureName,material->mDiffuseMapName,255);
    diffuseTextureName[material->mDiffuseMapNameLength] = 0;  // terminate the string

    char alphaTextureName[255];

    if (material->mFlags & MFFormat::DataFormat4DS::MATERIALFLAG_ALPHATEXTURE)
    {
        memcpy(alphaTextureName,material->mAlphaMapName,255);
        alphaTextureName[material->mAlphaMapNameLength] = 0;  // terminate the string

        stateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);      // FIXME: copy-paste code from above
        osg::ref_ptr<osg::BlendFunc> blendFunc = new osg::BlendFunc;
        blendFunc->setFunction(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        stateSet->setMode(GL_BLEND, osg::StateAttribute::ON);
        stateSet->setAttributeAndModes(blendFunc.get(),osg::StateAttribute::ON);
    }
    else
    {
        alphaTextureName[0] = 0;
        stateSet->setRenderingHint(osg::StateSet::OPAQUE_BIN);

        if (colorKey)
        {
            osg::ref_ptr<osg::AlphaFunc> alphaFunc = new osg::AlphaFunc;
            alphaFunc->setFunction(osg::AlphaFunc::GREATER,0.5);
            stateSet->setMode(GL_ALPHA_TEST, osg::StateAttribute::ON);
            stateSet->setAttributeAndModes(alphaFunc,osg::StateAttribute::ON);
        }
    }

    if (material->mFlags & MFFormat::DataFormat4DS::MATERIALFLAG_ANIMATEDTEXTUREDIFFUSE)
    {
        osg::ref_ptr<TextureSwitchCallback> cb = new TextureSwitchCallback;
        std::vector<std::string> diffuseAnimationNames = makeAnimationNames(diffuseTextureName,material->mAnimSequenceLength);
        std::vector<std::string> alphaAnimationNames = makeAnimationNames(alphaTextureName,material->mAnimSequenceLength);

        for (int i = 0; i < diffuseAnimationNames.size(); ++i)
            cb->addTexture(loadTexture(diffuseAnimationNames[i],alphaAnimationNames[i],colorKey));

        stateSet->setUpdateCallback(cb);
    }

    osg::ref_ptr<osg::Texture2D> tex = loadTexture(diffuseTextureName,alphaTextureName,colorKey);

    stateSet->setAttributeAndModes(new osg::FrontFace(osg::FrontFace::CLOCKWISE));

    if (!(material->mFlags & MFFormat::DataFormat4DS::MATERIALFLAG_DOUBLESIDEDMATERIAL))
        stateSet->setMode(GL_CULL_FACE,osg::StateAttribute::ON);

    stateSet->setAttribute(mat);
    stateSet->setTextureAttributeAndModes(0,tex.get(), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);

    return stateSet;
}

osg::ref_ptr<osg::Node> OSG4DSLoader::load(std::ifstream &srcFile, std::string fileName)
{
    if (fileName.length() > 0)
    {
        osg::ref_ptr<osg::Node> cached = (osg::Node *) getFromCache(fileName).get();   

        if (cached)
            return cached;
    }

    std::string logStr = "loading model";

    MFFormat::DataFormat4DS format;

    osg::ref_ptr<osg::MatrixTransform> group = new osg::MatrixTransform();

    //  this transforms the scene to the correct coord system, but other things (scene.bin etc.) have to follow
    //  group->setMatrix( osg::Matrixd::scale(osg::Vec3f(1.0,1.0,-1.0)) );

    if (format.load(srcFile))
    {
        auto model = format.getModel();
        
        logStr += ", meshes: " + std::to_string(model->mMeshCount);
        logStr += ", materials: " + std::to_string(model->mMaterialCount);

        MFLogger::ConsoleLogger::info(logStr,OSG4DS_MODULE_STR);

        MaterialList materials;

        for (int i = 0; i < model->mMaterialCount; ++i)  // load materials
        {
            MFLogger::ConsoleLogger::info("Loading material " + std::to_string(i) + ".",OSG4DS_MODULE_STR);
            materials.push_back(make4dsMaterial(&(model->mMaterials[i])));
        }

        std::vector<osg::ref_ptr<osg::MatrixTransform>> meshes;

        for (int i = 0; i < model->mMeshCount; ++i)      // load meshes
        {
            osg::ref_ptr<osg::MatrixTransform> transform = new osg::MatrixTransform();
            osg::Matrixd mat;

            MFFormat::DataFormat4DS::Vec3 p, s;
            MFFormat::DataFormat4DS::Quat r;

            p = model->mMeshes[i].mPos;
            s = model->mMeshes[i].mScale;
            r = model->mMeshes[i].mRot;

            transform->setMatrix(makeTransformMatrix(p,s,r));

            // TODO(zaklaus): Improve this, either distinguish collision faces
            // in the world or skip them entirely.
            transform->addChild(make4dsMesh(&(model->mMeshes[i]),materials));

            meshes.push_back(transform);
        }

        for (int i = 0; i < model->mMeshCount; ++i)     // parent meshes
        {
            unsigned int parentID = model->mMeshes[i].mParentID;

            if (parentID == 0)
                group->addChild(meshes[i]);
            else
                meshes[parentID - 1]->addChild(meshes[i]);
        }
    }

    if (fileName.length() > 0)
        storeToCache(fileName,group);

    return group;
}

}

#endif
