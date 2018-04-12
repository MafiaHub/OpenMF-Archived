#include <cache_bin/osg_cachebin.hpp>

namespace MFFormat
{

osg::ref_ptr<osg::Node> OSGCachedCityLoader::load(MFFormat::DataFormatCacheBIN *format, std::string fileName)
{
    osg::ref_ptr<osg::Group> group = new osg::Group();
    group->setName("cache.bin");
    MFLogger::Logger::info("loading cache.bin", OSGCACHEBIN_MODULE_STR);
    MFFormat::OSG4DSLoader loader4DS;
    
    for (auto object : format->getObjects())
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
                
                MFFormat::DataFormat4DS *model = nullptr;

                if (mModelCache) {
                    model = mModelCache->getObject(instance.mModelName);

                    loadModel:
                    if (!model) {
                        model = new MFFormat::DataFormat4DS();
                        std::ifstream f;

                        if (!mFileSystem->open(f, "MODELS/" + instance.mModelName))
                        {
                            MFLogger::Logger::warn("Could not load model " + instance.mModelName + ".", OSGCACHEBIN_MODULE_STR);
                        }
                        else
                        {
                            model->load(f);
                            objectNode = loader4DS.load(model, instance.mModelName);
                            storeToCache(instance.mModelName, objectNode);
                            f.close();
                        }

                        if (mModelCache)
                            mModelCache->storeObject(instance.mModelName, model);
                    }
                }
                else {
                    goto loadModel;
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
    

    return group;
}

}
