#ifndef OSG_LOADER_H
#define OSG_LOADER_H

#include <osg/MatrixTransform>
#include <fstream>
#include <utils/logger.hpp>
#include <vfs/vfs.hpp>
#include <loader_cache.hpp>
#include <utils/math.hpp>
#define OSGLOADER_MODULE_STR "OSG loader"

namespace MFFormat
{

    class DataFormat4DS;
    typedef MFFormat::LoaderCache<MFFormat::DataFormat4DS*> ModelCache;

class OSGLoader
{
public:
    typedef osg::ref_ptr<osg::Referenced> OSGCached;
    typedef std::map<std::string,osg::ref_ptr<osg::Group>> NodeMap;

    OSGLoader();

    osg::ref_ptr<osg::Node> loadFile(std::string fileName);                                   // overloading load() didn't work somehow - why?
    void setBaseDir(std::string baseDir);
    void setLoaderCache(LoaderCache<OSGCached> *cache) { mLoaderCache = cache; };
    void setModelCache(MFFormat::ModelCache *cache) { mModelCache = cache; }

    osg::Vec3f toOSG(MFMath::Vec3 v);
    osg::Quat toOSG(MFMath::Quat q);

    void setNodeMap(NodeMap *nodeMap);

protected:
    osg::Matrixd makeTransformMatrix(
        MFMath::Vec3 p,
        MFMath::Vec3 s,
        MFMath::Quat r);

    std::string getTextureDir();
    std::string getModelDir();

    std::string mBaseDir;

    MFFile::FileSystem *mFileSystem;

    osg::Matrixd mMafiaToOSGMatrix;
    osg::Matrixd mMafiaToOSGMatrixInvert;

    LoaderCache<OSGCached> *mLoaderCache;   ///< Derived classes should make use of the cache with getFromChache/storeToCache.
    MFFormat::ModelCache  *mModelCache;

    osg::ref_ptr<osg::Referenced> getFromCache(std::string identifier);
    void storeToCache(std::string identifier,OSGCached obj);
    NodeMap *mNodeMap;
};

}

#endif
