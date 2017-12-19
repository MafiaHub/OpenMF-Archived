#ifndef OSG_LOADER_H
#define OSG_LOADER_H

#include <osg/MatrixTransform>
#include <base_parser.hpp>
#include <fstream>
#include <loggers/console.hpp>

namespace MFFormat
{

class OSGLoader
{
public:
    virtual osg::ref_ptr<osg::Node> load(std::ifstream &srcFile)=0;
    osg::ref_ptr<osg::Node> loadFile(std::string fileName);          // overloading load() didn't work somehow - why?

protected:
    osg::Matrixd makeTransformMatrix(
        MFFormat::DataFormat::Vec3 p,
        MFFormat::DataFormat::Vec3 s,
        MFFormat::DataFormat::Quat r);
};

osg::Matrixd OSGLoader::makeTransformMatrix(
    MFFormat::DataFormat::Vec3 p,
    MFFormat::DataFormat::Vec3 s,
    MFFormat::DataFormat::Quat r)
{
    osg::Matrixd mat;

    mat.preMultTranslate(osg::Vec3f(p.x,p.y,p.z));
    mat.preMultScale(osg::Vec3f(s.x,s.y,s.z));
    mat.preMultRotate(osg::Quat(r.x,r.y,r.z,r.w)); 

    return mat;
}

osg::ref_ptr<osg::Node> OSGLoader::loadFile(std::string fileName)
{
    osg::ref_ptr<osg::Node> n;

    std::ifstream f;
    f.open(fileName);

    if (f.is_open())
    {
        n = load(f);
        f.close();
    }
    else
        MFLogger::ConsoleLogger::warn("Could not open file " + fileName + ".");

    return n;
}

}

#endif
