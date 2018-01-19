#ifndef OSG_CHECK_BIN_LOADER_H
#define OSG_CHECK_BIN_LOADER_H

#include <check_bin/parser.hpp>
#include <osg/Node>
#include <osg/Geometry>
#include <osg/MatrixTransform>
#include <osg/ShapeDrawable>
#include <utils.hpp>
#include <base_loader.hpp>
#include <osg_masks.hpp>

#define OSGCHECKBIN_MODULE_STR "loader check.bin"

namespace MFFormat
{

class OSGCheckBinLoader : public OSGLoader
{
public:
    typedef struct 
    {
        osg::Vec3f mStart;
        osg::Vec3f mEnd;
        uint16_t mType;
    } Line;

    osg::ref_ptr<osg::Node> load(std::ifstream &srcFile, std::string fileName = "");
    std::vector<Line> resolveLinks(MFFormat::DataFormatCheckBIN parser);
    size_t getColorFromIndexOfType(uint16_t type);
};

std::vector<OSGCheckBinLoader::Line> OSGCheckBinLoader::resolveLinks(MFFormat::DataFormatCheckBIN parser)
{
    std::vector<Line> mPointsConnections;
    auto links = parser.getLinks();
    auto points = parser.getPoints();

    size_t linkIndex = 0;
    for(auto point : points) 
    {
        if(point.mEnterLinks > 0) 
        {
            for(size_t i = linkIndex; i < linkIndex + point.mEnterLinks; i++)
            {
                auto currentLink = links.at(i);
                if(currentLink.mTargetPoint > 0 && currentLink.mTargetPoint < points.size()) 
                {
                    auto targetPoint = points.at(currentLink.mTargetPoint);
                    Line lineToPush = 
                    {
                        toOSG(point.mPos),
                        toOSG(targetPoint.mPos),
                        targetPoint.mType
                    };
                   
                    mPointsConnections.push_back(lineToPush);     
                }
            }

            linkIndex += point.mEnterLinks;
        }
    }
    return mPointsConnections;
}

size_t OSGCheckBinLoader::getColorFromIndexOfType(uint16_t type)
{
    switch(type) 
    {
        case MFFormat::DataFormatCheckBIN::POINTTYPE_PEDESTRIAN:
        {
            return 0;
        } break;

        case MFFormat::DataFormatCheckBIN::POINTTYPE_AI:
        {
            return 1;
        } break;

        case MFFormat::DataFormatCheckBIN::POINTTYPE_VEHICLE:
        {
            return 2;
        } break;

        case MFFormat::DataFormatCheckBIN::POINTTYPE_TRAM_STATION:
        {
            return 3;
        } break;

        default: 
        {
            return 4;
        } break;
    }
}
osg::ref_ptr<osg::Node> OSGCheckBinLoader::load(std::ifstream &srcFile, std::string fileName)
{
    osg::ref_ptr<osg::Group> group = new osg::Group();
    group->setNodeMask(MFRender::MASK_DEBUG);
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
            osg::Vec4f(0,1,1,0),
            osg::Vec4f(1,1,0,0),
            osg::Vec4f(1,0,0,0),
            osg::Vec4f(0,0,0,0)
        };
        
        for (size_t i = 0; i < debugNodesColors.size(); i++) 
        {
            debugNodes[i] = new osg::ShapeDrawable(new osg::Sphere(osg::Vec3f(0,0,0),0.1));
            debugNodes[i]->setColor(debugNodesColors.at(i));
            debugNodes[i]->setUseDisplayList(false);
        }

        for (auto line : resolveLinks(parser))
        {
            osg::ref_ptr<osg::Vec3Array> points = new osg::Vec3Array;
            osg::ref_ptr<osg::Vec4Array> color = new osg::Vec4Array;
            osg::ref_ptr<osg::Geometry> geometry(new osg::Geometry);

            points->push_back(line.mStart);
            points->push_back(line.mEnd);
            color->push_back(debugNodesColors.at(getColorFromIndexOfType(line.mType)));
            geometry->setVertexArray(points.get());
            geometry->setColorArray(color.get());
            geometry->setColorBinding(osg::Geometry::BIND_OVERALL);
            geometry->addPrimitiveSet(new osg::DrawArrays(GL_LINES,0, points->size())); 
            group->addChild(geometry);

        } //for links

        for (auto point : parser.getPoints())
        {   
            osg::ref_ptr<osg::MatrixTransform> objectTransform = new osg::MatrixTransform();    
            osg::Matrixd m;
            m.preMultTranslate(toOSG(point.mPos));
            objectTransform->setMatrix(m);
            objectTransform->addChild(debugNodes[getColorFromIndexOfType(point.mType)]);
            group->addChild(objectTransform);
            
        } //for points
    }

    return group;
}

}

#endif
