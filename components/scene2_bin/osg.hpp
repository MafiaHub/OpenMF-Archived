#ifndef OSG_SCENE2_LOADER_H
#define OSG_SCENE2_LOADER_H

#include <osg/Node>
#include <osg/Geometry>
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

          if (n.asGroup())  // traverse(n) didn't work somehow
          {
              for (int i = 0; i < n.asGroup()->getNumChildren(); ++i)
                  n.asGroup()->getChild(i)->accept(*this);
          }
    }

protected:
    osg::Texture::WrapMode mWrapMode;
};

class OSGScene2BinLoader : public OSGLoader
{
public:
    typedef std::vector<osg::ref_ptr<osg::LightSource>> LightList;

    osg::ref_ptr<osg::Node> load(std::ifstream &srcFile, std::string fileName="");
    LightList getLightNodes() { return mLightNodes; };
    osg::Group *getCameraRelativeGroup() { return mCameraRelative.get(); };

protected:
    LightList mLightNodes;
    osg::ref_ptr<MFUtil::MoveEarthSkyWithEyePointTransform> mCameraRelative;   ///< children of this node move relatively with the camera
};

osg::ref_ptr<osg::Node> OSGScene2BinLoader::load(std::ifstream &srcFile, std::string fileName)
{
    osg::ref_ptr<osg::Group> group = new osg::Group();
    group->setName("scene2.bin");
    MFLogger::ConsoleLogger::info("loading scene2.bin", OSGSCENE2BIN_MODULE_STR);
    MFFormat::DataFormatScene2BIN parser;
    MFFormat::OSG4DSLoader loader4DS;
    loader4DS.setBaseDir(mBaseDir);

    bool success = parser.load(srcFile);

    if (success)
    {
        NodeMap emptyNodeMap;
        NodeMap *nodeMap = &emptyNodeMap;
        std::vector<osg::Node *> loadedNodes;    

        if (!mNodeMap)
            MFLogger::ConsoleLogger::warn("loading scene2.bin without node map set, objects' parents may be wrong.");
        else
            nodeMap = mNodeMap;

        mCameraRelative = new MFUtil::MoveEarthSkyWithEyePointTransform();   // for Backdrop sector (camera relative placement)
        mCameraRelative->setCullingActive(false);

        mCameraRelative->setName("camera relative");

        // disable lights for backdrop sector:
        osg::ref_ptr<osg::Material> mat = new osg::Material;

        mat->setEmission(osg::Material::FRONT_AND_BACK,osg::Vec4f(1,1,1,1));
        mat->setColorMode(osg::Material::OFF);
        mCameraRelative->getOrCreateStateSet()->setAttributeAndModes(mat,osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);

        mCameraRelative->getOrCreateStateSet()->setMode(GL_FOG,osg::StateAttribute::OFF);
        mCameraRelative->getOrCreateStateSet()->setRenderBinDetails(-1,"RenderBin");              // render before the scene
        mCameraRelative->getOrCreateStateSet()->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);  // don't write to depth buffer

        // FIXME: disabling depth writing this way also disables depth test => bad (osg::ClearNode?)

        group->addChild(mCameraRelative);
 
        for (auto pair : parser.getObjects())
        {
            auto object = pair.second;
            osg::ref_ptr<osg::Node> objectNode;
            std::string logStr = object.mName + ": ";
            bool hasTransform = true;

            switch (object.mType)
            {
                case MFFormat::DataFormatScene2BIN::OBJECT_TYPE_LIGHT:
                {
                    logStr += "light (";
                    std::string lightTypeName;

                    switch (object.mLightType)
                    {
                        case MFFormat::DataFormatScene2BIN::LIGHT_TYPE_POINT: lightTypeName = "point"; break;
                        case MFFormat::DataFormatScene2BIN::LIGHT_TYPE_DIRECTIONAL: lightTypeName = "directional"; break;
                        case MFFormat::DataFormatScene2BIN::LIGHT_TYPE_AMBIENT: lightTypeName = "ambient"; break;
                        case MFFormat::DataFormatScene2BIN::LIGHT_TYPE_FOG: lightTypeName = "fog"; break;
                        case MFFormat::DataFormatScene2BIN::LIGHT_TYPE_POINT_AMBIENT: lightTypeName = "point ambient"; break;
                        case MFFormat::DataFormatScene2BIN::LIGHT_TYPE_LAYERED_FOG: lightTypeName = "layered fog"; break;
                        default: break;
                    }

                    logStr += lightTypeName + ")";

                    #if 0
                        // for debug
                        osg::ref_ptr<osg::ShapeDrawable> lightNode = new osg::ShapeDrawable(
                        new osg::Sphere(osg::Vec3f(0,0,0),0.1));
                    #else
                        osg::ref_ptr<osg::LightSource> lightNode = new osg::LightSource();

                        MFFormat::DataFormat::Vec3 c = object.mLightColour;
                        osg::Vec3f lightColor = osg::Vec3f(c.x,c.y,c.z) * object.mLightPower;

                        if (object.mLightType == MFFormat::DataFormatScene2BIN::LIGHT_TYPE_AMBIENT)
                        {
                            lightNode->getLight()->setDiffuse(osg::Vec4(0,0,0,0));
                            lightNode->getLight()->setAmbient(osg::Vec4(lightColor,1));
                            lightNode->getLight()->setSpecular(osg::Vec4(0,0,0,0));
                        }
                        else
                        {
                            lightNode->getLight()->setDiffuse(osg::Vec4(lightColor,1));
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
                    #endif

                    lightNode->setName(lightTypeName);
                    mLightNodes.push_back(lightNode);

                    objectNode = lightNode;
                    break;
                }

                case MFFormat::DataFormatScene2BIN::OBJECT_TYPE_MODEL:
                {
                    logStr += "model: " + object.mModelName;

                    objectNode = (osg::Node *) getFromCache(object.mModelName).get();

                    if (!objectNode)
                    {
                        std::ifstream f;
                        
                        if (!mFileSystem->open(f,"models/" + object.mModelName))
                        {
                            MFLogger::ConsoleLogger::warn("Could not load model " + object.mModelName + ".", OSGSCENE2BIN_MODULE_STR);
                        }
                        else
                        {
                            objectNode = loader4DS.load(f,object.mModelName);   
                            storeToCache(object.mModelName,objectNode);
                            f.close();
                        }
                    }

                    objectNode->setName(object.mModelName);

                    break;
                }

                default:
                {
                    logStr += "unknown";
                    hasTransform = false;
                    break;
                }
            }
            
            MFLogger::ConsoleLogger::info(logStr, OSGSCENE2BIN_MODULE_STR);

            if (objectNode.get())
            {
                osg::ref_ptr<osg::MatrixTransform> objectTransform = new osg::MatrixTransform();

                if (hasTransform)
                {
                    // TODO: when to use mPos vs mPos2?
                    osg::Matrixd m = makeTransformMatrix(object.mPos2,object.mScale,object.mRot);
                    objectTransform->setMatrix(m);
                }

                objectTransform->addChild(objectNode);
                objectTransform->setName(object.mParentName);    // hack: store the parent name in node name
                nodeMap->insert(nodeMap->begin(),std::pair<std::string,osg::ref_ptr<osg::Group>>(object.mName,objectTransform));
                loadedNodes.push_back(objectTransform.get());
            }
        }   // for

        for (auto i = 0; i < loadedNodes.size(); ++i)   // set parents
        {
            std::string parentName = loadedNodes[i]->getName();

            if (parentName.compare("Backdrop sector") == 0)      // backdrop is for camera-relative stuff
                mCameraRelative->addChild(loadedNodes[i]);
            else if (parentName.compare("Primary sector") == 0 || parentName.length() == 0)
                group->addChild(loadedNodes[i]);
            else if ( nodeMap->find(parentName) != nodeMap->end() )
                (*nodeMap)[parentName]->addChild(loadedNodes[i]);
            else
            {
                MFLogger::ConsoleLogger::warn("Parent \"" + parentName + "\" not found.",OSGSCENE2BIN_MODULE_STR);
                group->addChild(loadedNodes[i]);
            }

            loadedNodes[i]->setName("object transform");
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

#endif
