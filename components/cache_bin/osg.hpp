#ifndef OSG_CACHE_BIN_LOADER_H
#define OSG_CACHE_BIN_LOADER_H

#include <cache_bin/parser.hpp>

#define OSGCACHEBIN_MOSULE_STR "loader cache.bin"

namespace MFFormat
{

class OSGCacheBinLoader : public OSGLoader
{
public:
    osg::ref_ptr<osg::Node> load(std::ifstream &srcFile, std::string fileName = "");
};

osg::ref_ptr<osg::Node> OSGCacheBinLoader::load(std::ifstream &srcFile, std::string fileName)
{
    osg::ref_ptr<osg::Group> group = new osg::Group();
    group->setName("cache.bin");
    MFLogger::ConsoleLogger::info("loading cache.bin", OSGCACHEBIN_MOSULE_STR);
    MFFormat::DataFormatCacheBIN parser;
    MFFormat::OSG4DSLoader loader4DS;
    bool success = parser.load(srcFile);

    if (success)
    {
        for (auto object : parser.getObjects())
        {
            MFLogger::ConsoleLogger::info("Loading object " + object.mObjectName + ".", OSGCACHEBIN_MOSULE_STR);
        
            osg::ref_ptr<osg::Group> objectGroup = new osg::Group();
            group->setName("object group");

            for (auto instance : object.mInstances)
            {
                osg::ref_ptr<osg::Node> objectNode = (osg::Node *) getFromCache(instance.mModelName).get();

                if (!objectNode)
                {
                    MFLogger::ConsoleLogger::info("Loading model " + instance.mModelName + ". (" + instance.mPos.str() + ")",OSGCACHEBIN_MOSULE_STR);
                    std::ifstream f;
                    
                    if (!mFileSystem->open(f,"MODELS/" + instance.mModelName))
                    {
                        MFLogger::ConsoleLogger::warn("Could not load model " + instance.mModelName + ".", OSGCACHEBIN_MOSULE_STR);
                    }
                    else
                    {
                        objectNode = loader4DS.load(f, instance.mModelName);
                        storeToCache(instance.mModelName, objectNode);
                        f.close();
                    }
                }
                
                if (objectNode.get())
                {
                    osg::ref_ptr<osg::MatrixTransform> objectTransform = new osg::MatrixTransform();
                    objectTransform->setName("object transform");

                    osg::Matrixd m = makeTransformMatrix(instance.mPos, instance.mScale, instance.mRot);
                    m.preMult( osg::Matrixd::rotate(osg::PI,osg::Vec3f(1,0,0)) );
                    objectTransform->setMatrix(m);

                    objectTransform->addChild(objectNode);
                    objectGroup->addChild(objectTransform);
                }
            }   // for instances

            group->addChild(objectGroup);
        }       // for objects
    }

    return group;
}

}

#endif
