#include <4ds/4ds_osg.hpp>

namespace MFFormat
{

class TextureSwitchCallback: public osg::StateSet::Callback
{
public:
    TextureSwitchCallback(unsigned framePeriod)
    {
        mCurrentTexture = 0;
        mFramePeriod = framePeriod;
    }

    virtual void operator()(osg::StateSet *s, osg::NodeVisitor *n)
    {
        unsigned int newTexture = ((unsigned int) (n->getFrameStamp()->getReferenceTime() * 1000.0 / mFramePeriod)) % mTextures.size();

        if (newTexture != mCurrentTexture)
        {
            mCurrentTexture = newTexture;

            s->setTextureAttributeAndModes(0,mTextures[mCurrentTexture].get(),
                osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
        }
    }

    void addTexture(osg::Texture2D *texture)
    {
        mTextures.push_back(texture);
    }

protected:
    std::vector<osg::ref_ptr<osg::Texture2D>> mTextures;
    unsigned int mCurrentTexture;
    unsigned int mFramePeriod;
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

    std::string logStr = "  Loading texture " + fileName;

    bool alphaTexture = fileNameAlpha.length() > 0;
    bool diffuseTexture = fileName.length() > 0;

    if (alphaTexture)
        logStr += " (alpha texture: " + fileNameAlpha + ")";

    logStr += ".";

    MFLogger::Logger::info(logStr, OSG4DS_MODULE_STR);

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

    if (diffuseTexture)
    {
        if (fileLocation.length() == 0)
        {
            MFLogger::Logger::warn("Could not load texture: " + fileName, OSG4DS_MODULE_STR);
        }
        else
        {
            img = osgDB::readImageFile(fileLocation);

            if (colorKey)
            {
                MFFormat::BMPInfo bmp;

                std::ifstream bmpFile;
                bmpFile.open(fileLocation, std::ios::binary);
                bmp.load(bmpFile);
                bmpFile.close();

                osg::Vec3f transparentColor = osg::Vec3f(     
                    bmp.mTransparentColor.r / 255.0,
                    bmp.mTransparentColor.g / 255.0,
                    bmp.mTransparentColor.b / 255.0);

                img = MFUtil::applyColorKey(img.get(), transparentColor, 0.04f );
            }
        }
    }

    if (alphaTexture)
    {
        if (fileLocationAlpha.length() == 0)
        {
            MFLogger::Logger::warn("Could not load alpha texture: " + fileNameAlpha, OSG4DS_MODULE_STR);
        }
        else
        {
            osg::ref_ptr<osg::Image> imgAlpha = osgDB::readImageFile(fileLocationAlpha);

            if (!diffuseTexture)
            {
                img->allocateImage(imgAlpha->s(),imgAlpha->t(),1,imgAlpha->getPixelFormat(),imgAlpha->getDataType());
            }

            img = MFUtil::addAlphaFromImage(img.get(),imgAlpha.get());
        }
    }

    tex->setImage(img);
    tex->setMaxAnisotropy(16.0f);

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
    MFLogger::Logger::info("      loading facegroup, material: " + std::to_string(faceGroup->mMaterialID) + ".", OSG4DS_MODULE_STR);

    osg::ref_ptr<osg::DrawElementsUInt> indices = new osg::DrawElementsUInt(GL_TRIANGLES);

    for (size_t i = 0; i < faceGroup->mFaceCount; ++i)
    {
        auto face = faceGroup->mFaces[i];
        indices->push_back(face.mA);
        indices->push_back(face.mB);
        indices->push_back(face.mC);
    }

    osg::ref_ptr<osg::Geometry> geom = new osg::Geometry();
    geom->setName("facegroup");
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
        billboard->setNodeMask(MFRender::MASK_GAME);
        return billboard;
    }

    osg::ref_ptr<osg::Geode> geode = new osg::Geode;
    geode->addDrawable(geom.get());
    geode->setNodeMask(MFRender::MASK_GAME);
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

    std::string meshName = std::string(mesh->mMeshName,0,mesh->mMeshNameLength);

    MFLogger::Logger::info("  loading mesh (" + std::string(meshName) + "), LOD level: " + std::to_string((int) standard.mLODLevel) +
        ", type: " + std::to_string((int) mesh->mMeshType) +
        ", parent: " + std::to_string((int) mesh->mParentID) +
        ", instanced: " + std::to_string(standard.mInstanced), 
        OSG4DS_MODULE_STR);

    const float maxDistance = 10000000.0;

    osg::ref_ptr<osg::LOD> nodeLOD = new osg::LOD();

    nodeLOD->setName(meshName);

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
    MFLogger::Logger::info("    loading LOD, vertices: " + std::to_string(meshLOD->mVertexCount) +
        ", face groups: " + std::to_string((int) meshLOD->mFaceGroupCount),
 
        OSG4DS_MODULE_STR);

    osg::ref_ptr<osg::Group> group = new osg::Group();
    group->setName("LOD level");

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
        if (materials.size() > 0)
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
    bool envMap = material->mFlags & MFFormat::DataFormat4DS::MATERIALFLAG_ENVIRONMENTMAP;
    bool diffuseMap = material->mFlags & MFFormat::DataFormat4DS::MATERIALFLAG_TEXTUREDIFFUSE;
    bool alphaMap = material->mFlags & MFFormat::DataFormat4DS::MATERIALFLAG_ALPHATEXTURE;

    bool mixAdd = material->mFlags & MFFormat::DataFormat4DS::MATERIALFLAG_ADDITIVETEXTUREBLEND;
    bool mixMultiply = material->mFlags & MFFormat::DataFormat4DS::MATERIALFLAG_MULTIPLYTEXTUREBLEND;

    bool colored = material->mFlags & MFFormat::DataFormat4DS::MATERIALFLAG_COLORED;
    bool hide = !diffuseMap && !alphaMap && !envMap && material->mTransparency == 1;
    bool isTransparent = false;
    bool additiveBlend = material->mFlags & MFFormat::DataFormat4DS::MATERIALFLAG_ADDITIVEMIXING;

    unsigned int diffuseUnit = 0;
    unsigned int envUnit = diffuseMap ? 1 : 0;

    MFMath::Vec3 dif = material->mDiffuse;
    MFMath::Vec3 amb = material->mAmbient;
    MFMath::Vec3 emi = material->mEmission;

    if (!diffuseMap || colored)
    {
        /* source: http://djbozkosz.wz.cz/index.php?id=18
           "Valid only if diffuse texture is used, materials without texture are colored always,
            coloring is used for ambient and diffuse colors, emission is used always" */

        mat->setDiffuse(osg::Material::FRONT_AND_BACK,osg::Vec4f(dif.x,dif.y,dif.z,1.0));
        mat->setAmbient(osg::Material::FRONT_AND_BACK,osg::Vec4f(amb.x,amb.y,amb.z,1.0));
    }

    mat->setEmission(osg::Material::FRONT_AND_BACK,osg::Vec4f(emi.x,emi.y,emi.z,1.0));

    if (material->mTransparency < 1.0 && !alphaMap)
    {
        isTransparent = true;
        osg::Vec4f d = mat->getDiffuse(osg::Material::FRONT_AND_BACK);
        mat->setDiffuse(osg::Material::FRONT_AND_BACK,osg::Vec4f(d.x(),d.y(),d.z(),material->mTransparency));
    }

    std::string diffuseTextureName = std::string(material->mDiffuseMapName,0,material->mDiffuseMapNameLength);
    std::string alphaTextureName;
    std::string envTextureName; 

    if (envMap)
    {
        osg::ref_ptr<osg::TexEnv> texEnv = new osg::TexEnv;

        if (mixAdd)
            texEnv->setMode(osg::TexEnv::ADD);
        else if (mixMultiply)
            texEnv->setMode(osg::TexEnv::MODULATE);
//      else if (mixNormal)
//          texEnv->setMode(osg::TexEnv::BLEND);   // FIXME: this doesn't look good for some reason, maybe texture format, see TexEnv docs
            
        stateSet->setTextureAttributeAndModes(diffuseUnit,texEnv);

        envTextureName = std::string(material->mEnvMapName,0,material->mEnvMapNameLength);

        osg::ref_ptr<osg::TexGen> texGen = new osg::TexGen();
        texGen->setMode(osg::TexGen::SPHERE_MAP);
        stateSet->setTextureAttributeAndModes(envUnit,texGen);
    }

    if (alphaMap)
    {
        isTransparent = true;
        alphaTextureName = std::string(material->mAlphaMapName,0,material->mAlphaMapNameLength);
    }
    else
    {
        alphaTextureName[0] = 0;

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
        osg::ref_ptr<TextureSwitchCallback> cb = new TextureSwitchCallback(material->mFramePeriod);
        std::vector<std::string> diffuseAnimationNames = makeAnimationNames(diffuseTextureName,material->mAnimSequenceLength);
        std::vector<std::string> alphaAnimationNames = makeAnimationNames(alphaTextureName,material->mAnimSequenceLength);

        for (int i = 0; i < (int) diffuseAnimationNames.size(); ++i)
            cb->addTexture(loadTexture(diffuseAnimationNames[i],alphaAnimationNames[i],colorKey));

        stateSet->setUpdateCallback(cb);
    }

    if (additiveBlend)
        isTransparent = true;        

    if (isTransparent)
    {
        stateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
        osg::ref_ptr<osg::BlendFunc> blendFunc = new osg::BlendFunc;

        if (additiveBlend)
        {
            blendFunc->setFunction(osg::BlendFunc::SRC_COLOR,osg::BlendFunc::DST_COLOR);

            osg::ref_ptr<osg::BlendEquation> blendEq = new osg::BlendEquation;
            blendEq->setEquation(osg::BlendEquation::RGBA_MAX);    // FIXME: should be FUNC_ADD, but doesn't work for some reason

            stateSet->setAttributeAndModes(blendEq.get(),osg::StateAttribute::ON);
        }
        else
            blendFunc->setFunction(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        stateSet->setMode(GL_BLEND, osg::StateAttribute::ON);
        stateSet->setAttributeAndModes(blendFunc.get(),osg::StateAttribute::ON);
    }
    else   // opaque
    {
        stateSet->setRenderingHint(osg::StateSet::OPAQUE_BIN);
    }

    if (diffuseMap)
    {
        osg::ref_ptr<osg::Texture2D> tex = loadTexture(diffuseTextureName,alphaTextureName,colorKey);
        stateSet->setTextureAttributeAndModes(diffuseUnit,tex.get(), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
    }

    if (envMap)
    {
        osg::ref_ptr<osg::Texture2D> tex = loadTexture(envTextureName,"",false);
        stateSet->setTextureAttributeAndModes(envUnit,tex.get(), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
    }

    stateSet->setAttributeAndModes(new osg::FrontFace(osg::FrontFace::CLOCKWISE));

    if (!(material->mFlags & MFFormat::DataFormat4DS::MATERIALFLAG_DOUBLESIDEDMATERIAL))
        stateSet->setMode(GL_CULL_FACE,osg::StateAttribute::ON);

    if (hide)
    {
        osg::ref_ptr<osg::CullFace> cullFace = new osg::CullFace;
        cullFace->setMode(osg::CullFace::FRONT_AND_BACK);
        stateSet->setMode(GL_CULL_FACE,osg::StateAttribute::ON);
        stateSet->setAttributeAndModes(cullFace,osg::StateAttribute::ON);
    }

    stateSet->setAttribute(mat);

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
    group->setName("4DS model");

    if (format.load(srcFile))
    {
        auto model = format.getModel();
        
        logStr += ", meshes: " + std::to_string(model.mMeshCount);
        logStr += ", materials: " + std::to_string(model.mMaterialCount);

        MFLogger::Logger::info(logStr,OSG4DS_MODULE_STR);

        MaterialList materials;

        for (int i = 0; i < model.mMaterialCount; ++i)  // load materials
        {
            MFLogger::Logger::info("  Loading material " + std::to_string(i) + ".",OSG4DS_MODULE_STR);
            materials.push_back(make4dsMaterial(&(model.mMaterials[i])));
        }

        std::vector<osg::ref_ptr<osg::MatrixTransform>> meshes;

        for (int i = 0; i < model.mMeshCount; ++i)      // load meshes
        {
            osg::ref_ptr<osg::MatrixTransform> transform = new osg::MatrixTransform();
            std::string meshName = MFUtil::charArrayToStr(model.mMeshes[i].mMeshName,model.mMeshes[i].mMeshNameLength);
            transform->setName(meshName);  // don't mess with this name, it's needed to link with collisions etc.

            transform->getOrCreateUserDataContainer()->addDescription("4ds mesh");    // mark the node as a 4DS mesh

            osg::Matrixd mat;

            MFMath::Vec3 p, s;
            MFMath::Quat r;

            p = model.mMeshes[i].mPos;
            s = model.mMeshes[i].mScale;
            r = model.mMeshes[i].mRot;

            transform->setMatrix(makeTransformMatrix(p,s,r));
            transform->addChild(make4dsMesh(&(model.mMeshes[i]),materials));
                
            meshes.push_back(transform);

            if (mNodeMap)
                mNodeMap->insert(mNodeMap->begin(),std::pair<std::string,osg::ref_ptr<osg::Group>>(meshName,transform));
        }

        for (int i = 0; i < model.mMeshCount; ++i)     // parent meshes
        {
            unsigned int parentID = model.mMeshes[i].mParentID;

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
