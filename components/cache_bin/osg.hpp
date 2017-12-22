#ifndef OSG_CACHE_BIN_LOADER_H
#define OSG_CACHE_BIN_LOADER_H

#include <cache_bin/parser.hpp>

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

    MFLogger::ConsoleLogger::info("loading cache.bin");

    MFFormat::DataFormatCacheBIN parser;

    MFFormat::OSG4DSLoader loader4DS;

    bool success = parser.load(srcFile);

	std::map<std::string, osg::ref_ptr<osg::Group>> nodeMap;

    if (success)
    {
        for (auto object : parser.getObjects())
        {
            osg::ref_ptr<osg::Node> objectNode;

            std::string logStr = object.mObjectName + ": ";

            bool hasTransform = true;

        
            for(auto instance : object.mInstances)
            {
				objectNode = (osg::Node *) getFromCache(instance.mModelName).get();

                if (!objectNode)
                {
					logStr += "model: " + instance.mModelName + " " + std::to_string(instance.mPos.x) + " " + std::to_string(instance.mPos.y) + " " + std::to_string(instance.mPos.z) + "\n";
                    std::ifstream f;
                    
                    if (!mFileSystem->open(f,"MODELS/" + instance.mModelName))
                    {
                        MFLogger::ConsoleLogger::warn("Could not load model " + instance.mModelName + ".");
                    }
                    else
                    {
						objectNode = loader4DS.load(f, instance.mModelName);
						storeToCache(instance.mModelName, objectNode);
						f.close();
                    }
                }
                
                MFLogger::ConsoleLogger::info(logStr);

                if (objectNode.get())
                {
                    osg::ref_ptr<osg::MatrixTransform> objectTransform = new osg::MatrixTransform();

                    if (hasTransform)
                    {
                        osg::Matrixd m = makeTransformMatrix(instance.mPos, instance.mScale, instance.mRot);
                        m.preMult( osg::Matrixd::rotate(osg::PI,osg::Vec3f(1,0,0)) );
                        objectTransform->setMatrix(m);
                    }

                    objectTransform->addChild(objectNode);
                    group->addChild(objectTransform);
					nodeMap.insert(nodeMap.begin(), std::pair<std::string, osg::ref_ptr<osg::Group>>(object.mObjectName, objectTransform));
                }
            } // for
        }   // for      
    }

    return group;
}

}

#endif
