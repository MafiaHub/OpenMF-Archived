#include <base_loader.hpp>

namespace MFFormat
{

void OSGLoader::setNodeMap(NodeMap *nodeMap)
{
    mNodeMap = nodeMap;
}

osg::ref_ptr<osg::Referenced> OSGLoader::getFromCache(std::string identifier)
{
    if (mLoaderCache)
        return mLoaderCache->getObject(identifier);

    osg::ref_ptr<osg::Referenced> result;

    return result;
}

void OSGLoader::storeToCache(std::string identifier,OSGCached obj)
{
    if (mLoaderCache)
        mLoaderCache->storeObject(identifier,obj);
}

osg::Vec3f OSGLoader::toOSG(MFMath::Vec3 v)
{
    return mMafiaToOSGMatrix.preMult(osg::Vec3(v.x,v.y,v.z));
}

osg::Quat OSGLoader::toOSG(MFMath::Quat q)
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
    mNodeMap = 0;
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
    MFMath::Vec3 p,
    MFMath::Vec3 s,
    MFMath::Quat r)
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
        MFLogger::Logger::warn("Could not open file " + fileName + ".", OSGLOADER_MODULE_STR);

    return n;
}

}
