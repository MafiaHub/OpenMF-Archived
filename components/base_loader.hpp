#ifndef OSG_LOADER_H
#define OSG_LOADER_H

#include <osg/MatrixTransform>
#include <fstream>
#include <loggers/console.hpp>
#include <vfs/vfs.hpp>
#include <loader_cache.hpp>

#define OSGLOADER_MODULE_STR "OSG loader"

namespace MFFormat
{

class OSGLoader
{
public:
    typedef osg::ref_ptr<osg::Referenced> OSGCached;

    OSGLoader();

    /**
        Load given object into OSG node.

        @param srcFile    file stream to load the object from
        @param fileName   optional file name that serves to reuse already loaded objects from cache
    */
    virtual osg::ref_ptr<osg::Node> load(std::ifstream &srcFile,std::string fileName="")=0;

    osg::ref_ptr<osg::Node> loadFile(std::string fileName);                                   // overloading load() didn't work somehow - why?
    void setBaseDir(std::string baseDir);
    void setLoaderCache(LoaderCache<OSGCached> *cache) { mLoaderCache = cache; };

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

    LoaderCache<OSGCached> *mLoaderCache;   ///< Derived classes should make use of the cache with getFromChache/storeToCache.

    osg::ref_ptr<osg::Referenced> getFromCache(std::string identifier);
    void storeToCache(std::string identifier,osg::ref_ptr<osg::Referenced> obj);
};

osg::ref_ptr<osg::Referenced> OSGLoader::getFromCache(std::string identifier)
{
    if (mLoaderCache)
        return mLoaderCache->getObject(identifier);

    osg::ref_ptr<osg::Referenced> result;

    return result;
}

void OSGLoader::storeToCache(std::string identifier,osg::ref_ptr<osg::Referenced> obj)
{
    if (mLoaderCache)
        mLoaderCache->storeObject(identifier,obj);
}

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
    mLoaderCache = 0;
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
    mat.preMultRotate(toOSG(r)); 
    mat.preMultScale(toOSG(s));

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
        n = load(f,fileName);
        f.close();
    }
    else
        MFLogger::ConsoleLogger::warn("Could not open file " + fileName + ".", OSGLOADER_MODULE_STR);

    return n;
}

}

#endif
