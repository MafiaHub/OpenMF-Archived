#ifndef OSG_CHECK_BIN_LOADER_H
#define OSG_CHECK_BIN_LOADER_H

#include <check_bin/parser.hpp>
#include <osg/Node>
#include <osg/Geometry>
#include <osg/MatrixTransform>
#include <osg/ShapeDrawable>
#include <utils.hpp>
#include <base_loader.hpp>

#define OSGCHECKBIN_MODULE_STR "loader check.bin"

namespace MFFormat
{

class OSGCheckBinLoader : public OSGLoader
{
public:
    osg::ref_ptr<osg::Node> load(std::ifstream &srcFile, std::string fileName = "");
};

osg::ref_ptr<osg::Node> OSGCheckBinLoader::load(std::ifstream &srcFile, std::string fileName)
{
    osg::ref_ptr<osg::Group> group = new osg::Group();
    group->setName("check.bin");
    MFLogger::ConsoleLogger::info("loading check.bin", OSGCHECKBIN_MODULE_STR);
    MFFormat::DataFormatCheckBIN parser;
   
    bool success = parser.load(srcFile);
    if (success)
    {
        osg::ref_ptr<osg::ShapeDrawable> debugNodes[5];
        std::vector<osg::Vec4f> debugNodesColors = 
        {
            osg::Vec4f(0,0,1,0),
            osg::Vec4f(0,1,0,0),
            osg::Vec4f(1,1,0,0),
            osg::Vec4f(1,0,0,0),
            osg::Vec4f(0,0,0,0)
        };
        
        for(size_t i = 0; i < debugNodesColors.size(); i++) 
        {
            debugNodes[i] = new osg::ShapeDrawable(new osg::Sphere(osg::Vec3f(0,0,0),0.1));
            debugNodes[i]->setColor(debugNodesColors.at(i));
            debugNodes[i]->setUseDisplayList(false);
        }
      
        for (auto point : parser.getPoints())
        {   
            osg::ref_ptr<osg::MatrixTransform> objectTransform = new osg::MatrixTransform();
            osg::ref_ptr<osg::ShapeDrawable> nodeToAdd;

            switch(point.mType) 
            {
                case MFFormat::DataFormatCheckBIN::POINTTYPE_PEDESTRIAN:
                {
                    nodeToAdd = debugNodes[0];
                } break;

                case MFFormat::DataFormatCheckBIN::POINTTYPE_AI:
                {
                    nodeToAdd = debugNodes[1];
                } break;

                case MFFormat::DataFormatCheckBIN::POINTTYPE_TRAINSANDSALINAS:
                {
                    nodeToAdd = debugNodes[2];
                } break;

                case MFFormat::DataFormatCheckBIN::POINTTYPE_SALINASTOP:
                {
                    nodeToAdd = debugNodes[3];
                } break;

                default: 
                {
                    nodeToAdd = debugNodes[4];
                } break;
            }
    
            osg::Matrixd m;
            m.preMultTranslate(toOSG(point.mPos));
            objectTransform->setMatrix(m);
            objectTransform->addChild(nodeToAdd);
            group->addChild(objectTransform);
            
        } //for points
    }

    return group;
}

}

#endif
