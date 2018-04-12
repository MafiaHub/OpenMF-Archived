#include <scene2_bin/osg_scene2bin.hpp>
#include <4ds/osg_4ds.hpp>
#include "spatial_entity/factory.hpp"

namespace MFFormat
{

class OverrideTextureWrapVisitor: public osg::NodeVisitor
{
public:
    OverrideTextureWrapVisitor(osg::Texture::WrapMode mode): osg::NodeVisitor()
    {
        mWrapMode = mode;
    }

    virtual void apply(osg::Node &n) override
    {
        osg::StateSet *stateSet = n.getStateSet();

        if (stateSet)
        {
            osg::StateAttribute *stateAttribute = stateSet->getTextureAttribute(0,osg::StateAttribute::TEXTURE);

            if (stateAttribute && stateAttribute->asTexture())
            {
                stateAttribute->asTexture()->setWrap(osg::Texture::WRAP_S,osg::Texture::MIRROR);    
                stateAttribute->asTexture()->setWrap(osg::Texture::WRAP_T,osg::Texture::MIRROR);    
            }
        }

        MFUtil::traverse(this,n);
    }

protected:
    osg::Texture::WrapMode mWrapMode;
};

OSGStaticSceneLoader::OSGStaticSceneLoader(): OSGLoader()
{
    mViewDistance = 0;
    mDebugPointLightNode = new osg::ShapeDrawable(new osg::Sphere(osg::Vec3f(0,0,0),1));
    mDebugPointLightNode->setNodeMask(MFRender::MASK_DEBUG);
    mDebugPointLightNode->setName("debug point light node");

    mDebugDirectionalLightNode = new osg::ShapeDrawable(new osg::Cone(osg::Vec3f(0,0,0),1,1));
    mDebugDirectionalLightNode->setNodeMask(MFRender::MASK_DEBUG);
    mDebugDirectionalLightNode->setName("debug directional light node");

    mDebugOtherLightNode = new osg::ShapeDrawable(new osg::Box(osg::Vec3f(0,0,0),1));
    mDebugOtherLightNode->setNodeMask(MFRender::MASK_DEBUG);
    mDebugOtherLightNode->setName("debug other light node");
}

osg::ref_ptr<osg::Node> OSGStaticSceneLoader::makeLightNode(MFFormat::DataFormatScene2BIN::Object object)
{
    osg::ref_ptr<osg::Group> lightGroup = new osg::Group;
    lightGroup->setName(MFFormat::DataFormatScene2BIN::lightTypeToStr(object.mLightType));

    float debugNodeSize = 0.01 + object.mLightPower * 0.1;

    osg::ref_ptr<osg::MatrixTransform> debugNodeTransform = new osg::MatrixTransform;

    switch (object.mLightType)
    {
        case MFFormat::DataFormatScene2BIN::LIGHT_TYPE_POINT:
            debugNodeTransform->addChild(mDebugPointLightNode);
            break;

        case MFFormat::DataFormatScene2BIN::LIGHT_TYPE_DIRECTIONAL:
            debugNodeTransform->addChild(mDebugDirectionalLightNode);
            break;

        default:
            debugNodeTransform->addChild(mDebugOtherLightNode);
            break;
    }

    debugNodeTransform->setMatrix(osg::Matrixd::scale(osg::Vec3f(debugNodeSize,debugNodeSize,debugNodeSize)));
    lightGroup->addChild(debugNodeTransform);

    MFMath::Vec3 c = object.mLightColour;
    osg::Vec4f lightColor = osg::Vec4f(c.x,c.y,c.z,1);

    osg::ref_ptr<osg::Material> debugMat = new osg::Material;
    debugMat->setDiffuse(osg::Material::FRONT_AND_BACK,lightColor);
    debugMat->setAmbient(osg::Material::FRONT_AND_BACK,lightColor);
    debugMat->setEmission(osg::Material::FRONT_AND_BACK,lightColor);
    debugNodeTransform->getOrCreateStateSet()->setAttributeAndModes(debugMat);

    osg::ref_ptr<osg::LightSource> lightNode = new osg::LightSource();

    lightNode->setNodeMask(MFRender::MASK_GAME);

    if (object.mLightType == MFFormat::DataFormatScene2BIN::LIGHT_TYPE_AMBIENT)
    {
        lightNode->getLight()->setDiffuse(osg::Vec4(0,0,0,0));
        lightNode->getLight()->setAmbient(lightColor * object.mLightPower);
        lightNode->getLight()->setSpecular(osg::Vec4(0,0,0,0));
    }
    else
    {
        lightNode->getLight()->setDiffuse(lightColor * object.mLightPower);
        lightNode->getLight()->setAmbient(osg::Vec4(0,0,0,0));
        lightNode->getLight()->setSpecular(osg::Vec4(0,0,0,0));
    }

    bool isPositional =
        object.mLightType == MFFormat::DataFormatScene2BIN::LIGHT_TYPE_POINT ||
        object.mLightType == MFFormat::DataFormatScene2BIN::LIGHT_TYPE_POINT_AMBIENT;

    osg::Vec3f lightPos = isPositional ?
        osg::Vec3f(0,0,0) :             // position will be set via transform node
        toOSG(object.mPos);             // for directional lights position usually defines direction - is this the right field though?

    lightNode->getLight()->setPosition( osg::Vec4f(lightPos,
        isPositional ? 1.0f : 0.0f) );  // see OpenGL light types

    lightGroup->addChild(lightNode);

    lightNode->setName(MFFormat::DataFormatScene2BIN::lightTypeToStr(object.mLightType));
    mLightNodes.push_back(lightNode);

    return lightGroup;
}

osg::ref_ptr<osg::Node> OSGStaticSceneLoader::load(MFFormat::DataFormatScene2BIN *format, std::string fileName)
{
    osg::ref_ptr<osg::Group> group = new osg::Group();
    group->setName("scene2.bin");
    MFLogger::Logger::info("loading scene2.bin", OSGSCENE2BIN_MODULE_STR);
    MFFormat::OSGModelLoader loader4DS;
    loader4DS.setBaseDir(mBaseDir);

    
    mViewDistance = format->getViewDistance();

    NodeMap emptyNodeMap;
    NodeMap *nodeMap = &emptyNodeMap;
    std::vector<osg::Node *> loadedNodes;    
    std::vector<std::string> loadedNames;

    if (!mNodeMap)
        MFLogger::Logger::warn("loading scene2.bin without node map set, objects' parents may be wrong.");
    else
        nodeMap = mNodeMap;

    mCameraRelative = new MFUtil::SkyboxNode();   // for Backdrop sector (camera relative placement)
    group->addChild(mCameraRelative);
 
    for (auto pair : format->getObjects())
    {
        auto object = pair.second;
        osg::ref_ptr<osg::Node> objectNode;
        std::string logStr = object.mName + ": ";
        bool hasTransform = true;

        switch (object.mType)
        {
            case MFFormat::DataFormatScene2BIN::OBJECT_TYPE_LIGHT:
            {
                logStr += "light (" + MFFormat::DataFormatScene2BIN::lightTypeToStr(object.mLightType) + ")";
                objectNode = makeLightNode(object);
                break;
            }

            case MFFormat::DataFormatScene2BIN::OBJECT_TYPE_MODEL:
            {
                // Check if object is special (is not static)
                if (object.mSpecialType) continue;

                logStr += "model: " + object.mModelName;

                objectNode = (osg::Node *) mObjectFactory->loadModel(object.mModelName);

                break;
            }

            default:
            {
                logStr += "unknown";
                hasTransform = false;
                break;
            }
        }
            
        MFLogger::Logger::info(logStr, OSGSCENE2BIN_MODULE_STR);

        if (objectNode.get())
        {
            osg::ref_ptr<osg::MatrixTransform> objectTransform = new osg::MatrixTransform();

            if (hasTransform)
            {
                osg::Matrixd m = makeTransformMatrix(object.mPos,object.mScale,object.mRot);
                objectTransform->setMatrix(m);
            }

            objectTransform->addChild(objectNode);
            objectTransform->getOrCreateUserDataContainer()->addDescription("scene2.bin model"); // mark the node as a model loaded from scene2.bin
            objectTransform->setName(object.mParentName);    // hack: store the parent name in node name
            nodeMap->insert(nodeMap->begin(),std::pair<std::string,osg::ref_ptr<osg::Group>>(object.mName,objectTransform));
            loadedNodes.push_back(objectTransform.get());
            loadedNames.push_back(object.mName);
        }
    }   // for

    for (int i = 0; i < (int) loadedNodes.size(); ++i)   // set parents
    {
        std::string parentName = loadedNodes[i]->getName();
        loadedNodes[i]->setName(loadedNames[i]);

        if (parentName.compare("Backdrop sector") == 0)      // backdrop is for camera-relative stuff
            mCameraRelative->addChild(loadedNodes[i]);
        else if (parentName.compare("Primary sector") == 0 || parentName.length() == 0)
            group->addChild(loadedNodes[i]);
        else if ( nodeMap->find(parentName) != nodeMap->end() )
            (*nodeMap)[parentName]->addChild(loadedNodes[i]);
        else
        {
            MFLogger::Logger::warn("Parent \"" + parentName + "\" not found.",OSGSCENE2BIN_MODULE_STR);
            group->addChild(loadedNodes[i]);
        }
    }

    /* TODO: Skybox cannot have texture wrap set to repeat, otherwise its edges are visible, therefore
       use the below visitor to correct the texture wrap for everything in the backdrop sector, but
       there probably is some better way. */

    OverrideTextureWrapVisitor textureCorrector(osg::Texture::MIRROR);
    mCameraRelative->accept(textureCorrector);

    return group;
}

}
