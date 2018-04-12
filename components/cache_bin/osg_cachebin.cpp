#include <cache_bin/osg_cachebin.hpp>
#include "spatial_entity/factory.hpp"

namespace MFFormat
{

osg::ref_ptr<osg::Node> OSGCachedCityLoader::load(MFFormat::DataFormatCacheBIN *format, std::string fileName)
{
    osg::ref_ptr<osg::Group> group = new osg::Group();
    group->setName("cache.bin");
    MFLogger::Logger::info("loading cache.bin", OSGCACHEBIN_MODULE_STR);
    MFFormat::OSGModelLoader loader4DS;
    
    for (auto object : format->getObjects())
    {
        MFLogger::Logger::info("Loading object " + object.mObjectName + ".", OSGCACHEBIN_MODULE_STR);
        
        osg::ref_ptr<osg::Group> objectGroup = new osg::Group();
        group->setName("object group");

        for (auto instance : object.mInstances)
        {
            osg::ref_ptr<osg::Node> objectNode = mObjectFactory->loadModel(instance.mModelName);
                
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
