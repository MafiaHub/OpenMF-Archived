#ifndef OSG_SCENE2_LOADER_H
#define OSG_SCENE2_LOADER_H

#include <osg/Node>
#include <osg/Geometry>
#include <osg/MatrixTransform>
#include <osg/Geode>
#include <osg/Texture2D>
#include <osg/ShapeDrawable>
#include <fstream>
#include <algorithm>
#include <4ds/parser.hpp>
#include <4ds/osg.hpp>
#include <scene2_bin/parser.hpp>
#include <loggers/console.hpp>
#include <utils.hpp>
#include <base_loader.hpp>
#include <osgText/Text3D>
#include <osgText/Font3D>
#include <osg/Billboard>

namespace MFFormat
{

class OSGScene2BinLoader : public OSGLoader
{
public:
    osg::ref_ptr<osg::Node> load(std::ifstream &srcFile);
};

osg::ref_ptr<osg::Node> OSGScene2BinLoader::load(std::ifstream &srcFile)
{
    osg::ref_ptr<osg::Group> group = new osg::Group();

    MFLogger::ConsoleLogger::info("loading scene2.bin");

    MFFormat::DataFormatScene2BIN parser;

    bool success = parser.load(srcFile);

    if (success)
    {
        std::map<std::string,osg::ref_ptr<osg::Group>> nodeMap;

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
                    logStr += "light";

                    #if 1
                        // for debug
                        osg::ref_ptr<osg::ShapeDrawable> lightNode = new osg::ShapeDrawable(
                        new osg::Sphere(osg::Vec3f(0,0,0),0.1));
                    #else
                        osg::ref_ptr<osg::LightSource> lightNode = new osg::LightSource();

                        MFFormat::DataFormat::Vec3 c = object.mLightColour;
                        osg::Vec3f lightColor = osg::Vec3f(c.x,c.z,c.z);
                        osg::Vec3f lightColor = osg::Vec3f(1,1,1);

                        lightNode->getLight()->setDiffuse(osg::Vec4(lightColor,1));
                        lightNode->getLight()->setAmbient(osg::Vec4(lightColor * 0.5,1));
                    #endif

                    objectNode = lightNode;
                    break;
                }


                default:
                {
                    logStr += "unknown";
                    hasTransform = false;
                    break;
                }
            }
            
            MFLogger::ConsoleLogger::info(logStr);

            if (objectNode.get())
            {
                osg::ref_ptr<osg::MatrixTransform> objectTransform = new osg::MatrixTransform();

                if (hasTransform)
                    objectTransform->setMatrix(makeTransformMatrix(
                        object.mPos,
                        object.mScale,
                        object.mRot));

                objectTransform->addChild(objectNode);
                objectTransform->setName(object.mParentName);    // hack: store the parent name in node name
                nodeMap.insert(nodeMap.begin(),std::pair<std::string,osg::ref_ptr<osg::Group>>(object.mName,objectTransform));
            }
        }   // for

        for (auto pair : nodeMap)   // set parents
        {
            std::string parentName = pair.second->getName();

            if ( nodeMap.find(parentName) != nodeMap.end() )
                nodeMap[parentName]->addChild(pair.second);
            else
                group->addChild(pair.second);

            pair.second->setName("");
        }
    }

    return group;
}

}

#endif
