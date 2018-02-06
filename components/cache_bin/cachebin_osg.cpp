#include <cache_bin/cachebin_osg.hpp>

namespace MFFormat
{

osg::ref_ptr<osg::Node> OSGCacheBinLoader::load(std::ifstream &srcFile, std::string fileName)
{
    osg::ref_ptr<osg::Group> group = new osg::Group();
    group->setName("cache.bin");
    MFLogger::Logger::info("loading cache.bin", OSGCACHEBIN_MODULE_STR);
    MFFormat::DataFormatCacheBIN parser;
    MFFormat::OSG4DSLoader loader4DS;
    bool success = parser.load(srcFile);

    if (success)
    {
        for (auto object : parser.getObjects())
        {
            MFLogger::Logger::info("Loading object " + object.mObjectName + ".", OSGCACHEBIN_MODULE_STR);
        
            osg::ref_ptr<osg::Group> objectGroup = new osg::Group();
            group->setName("object group");

            for (auto instance : object.mInstances)
            {
                osg::ref_ptr<osg::Node> objectNode = (osg::Node *) getFromCache(instance.mModelName).get();

                if (!objectNode)
                {
                    MFLogger::Logger::info("Loading model " + instance.mModelName + ". (" + instance.mPos.str() + ")",OSGCACHEBIN_MODULE_STR);
                    std::ifstream f;
                    
                    if (!mFileSystem->open(f,"MODELS/" + instance.mModelName))
                    {
                        MFLogger::Logger::warn("Could not load model " + instance.mModelName + ".", OSGCACHEBIN_MODULE_STR);
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
