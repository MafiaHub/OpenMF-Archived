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
    OSGLoader();
    virtual osg::ref_ptr<osg::Node> load(std::ifstream &srcFile)=0;
    osg::ref_ptr<osg::Node> loadFile(std::string fileName);          // overloading load() didn't work somehow - why?
    void setBaseDir(std::string baseDir);

protected:
    osg::Matrixd makeTransformMatrix(
        MFFormat::DataFormat::Vec3 p,
        MFFormat::DataFormat::Vec3 s,
        MFFormat::DataFormat::Quat r);

    std::string getTextureDir();
    std::string getModelDir();

    std::string mBaseDir;
};

OSGLoader::OSGLoader()
{
    mBaseDir = ".";
}

void OSGLoader::setBaseDir(std::string baseDir)
{
    mBaseDir = baseDir;
}

std::string OSGLoader::getTextureDir()
{
    return mBaseDir + "/MAPS/";
}

std::string OSGLoader::getModelDir()
{
    return mBaseDir + "/MODELS/";
}

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

    fileName = mBaseDir + fileName;
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
