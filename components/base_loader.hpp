#ifndef OSG_LOADER_H
#define OSG_LOADER_H

#include <osg/MatrixTransform>
#include <fstream>
#include <loggers/console.hpp>
#include <vfs/vfs.hpp>

namespace MFFormat
{

class OSGLoader
{
public:
    OSGLoader();
    virtual osg::ref_ptr<osg::Node> load(std::ifstream &srcFile)=0;
    osg::ref_ptr<osg::Node> loadFile(std::string fileName);          // overloading load() didn't work somehow - why?
    void setBaseDir(std::string baseDir);

    osg::Vec3f toOSG(MFFormat::DataFormat::Vec3 v);
    osg::Quat toOSG(MFFormat::DataFormat::Quat q);

protected:
    osg::Matrixd makeTransformMatrix(
        MFFormat::DataFormat::Vec3 p,
        MFFormat::DataFormat::Vec3 s,
        MFFormat::DataFormat::Quat r);

    std::string getTextureDir();
    std::string getModelDir();

    std::string mBaseDir;

    MFFile::FileSystem *mFileSystem;

    osg::Matrixd mMafiaToOSGMatrix;
    osg::Matrixd mMafiaToOSGMatrixInvert;
};

osg::Vec3f OSGLoader::toOSG(MFFormat::DataFormat::Vec3 v)
{
    return mMafiaToOSGMatrix.preMult(osg::Vec3(v.x,v.y,v.z));
}

osg::Quat OSGLoader::toOSG(MFFormat::DataFormat::Quat q)
{
    osg::Matrixd transform;
    transform.preMult(mMafiaToOSGMatrix);
    transform.preMult(osg::Matrixd::rotate(osg::Quat(q.x,q.y,q.z,q.w)));
    transform.preMult(mMafiaToOSGMatrixInvert);
    return transform.getRotate();
}

OSGLoader::OSGLoader()
{
    mBaseDir = "";
    mFileSystem = MFFile::FileSystem::getInstance();

    mMafiaToOSGMatrix.makeScale(osg::Vec3f(1,1,-1));
    mMafiaToOSGMatrix.postMult( osg::Matrixd::rotate(osg::PI / 2.0,osg::Vec3f(1,0,0)) );

    mMafiaToOSGMatrixInvert = mMafiaToOSGMatrix;
    mMafiaToOSGMatrixInvert.invert(mMafiaToOSGMatrixInvert);
}

void OSGLoader::setBaseDir(std::string baseDir)
{
    mBaseDir = baseDir;
}

std::string OSGLoader::getTextureDir()
{
    return mBaseDir + "MAPS/";
}

std::string OSGLoader::getModelDir()
{
    return mBaseDir + "MODELS/";
}

osg::Matrixd OSGLoader::makeTransformMatrix(
    MFFormat::DataFormat::Vec3 p,
    MFFormat::DataFormat::Vec3 s,
    MFFormat::DataFormat::Quat r)
{
    osg::Matrixd mat;

    mat.preMultTranslate(toOSG(p));
    mat.preMultScale(toOSG(s));
    mat.preMultRotate(toOSG(r)); 

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
