#include "mission/mission_impl.hpp"
#include "engine/engine.hpp"

namespace MFGame
{

MissionImpl::MissionImpl(std::string missionName, MFGame::Engine *engine): Mission(missionName)
{
    mFileSystem = MFFile::FileSystem::getInstance();
    mRenderer = (MFRender::OSGRenderer *)engine->getRenderer();
    mEngine = engine;
}

MissionImpl::~MissionImpl()
{
}

bool MissionImpl::load()
{
    std::string missionDir = "missions/" + mMissionName;
    std::string scene4dsPath = missionDir + "/scene.4ds";
    std::string scene2BinPath = missionDir + "/scene2.bin";
    std::string cacheBinPath = missionDir + "/cache.bin";
    std::string checkBinPath = missionDir + "/check.bin";

    std::ifstream file4DS;
    std::ifstream fileScene2Bin;
    std::ifstream fileCacheBin;
    std::ifstream fileCheckBin;

    MFFormat::OSG4DSLoader l4ds;
    MFFormat::OSGStaticSceneLoader lScene2;
    MFFormat::OSGCachedCityLoader lCache;

    l4ds.setLoaderCache(mRenderer->getLoaderCache());
    lScene2.setLoaderCache(mRenderer->getLoaderCache());
    lScene2.setModelCache(&mModelCache);
    lCache.setLoaderCache(mRenderer->getLoaderCache());
    lCache.setModelCache(&mModelCache);

    MFFormat::OSGLoader::NodeMap nodeMap;
    l4ds.setNodeMap(&nodeMap);
    lScene2.setNodeMap(&nodeMap);

    if (mFileSystem->open(file4DS, scene4dsPath)) {
        if (!mSceneModel.load(file4DS)) return false;
        file4DS.close();

        osg::ref_ptr<osg::Node> n = l4ds.load(&mSceneModel);
        mRenderer->getRootNode()->addChild(n);
    }
    else
        MFLogger::Logger::warn("Could not open 4ds file: " + scene4dsPath + ".", OSGRENDERER_MODULE_STR);
    
    bool lightsAreSet = false;

    float viewDistance = 2000;    // default value

    if (mFileSystem->open(fileScene2Bin, scene2BinPath))
    {
        if (!mSceneData.load(fileScene2Bin)) return false;
        osg::ref_ptr<osg::Node> n = lScene2.load(&mSceneData);

        if (!n)
            MFLogger::Logger::warn("Could not parse scene2.bin file: " + scene2BinPath + ".", OSGRENDERER_MODULE_STR);
        else
        {
            mRenderer->getRootNode()->addChild(n);
            std::vector<osg::ref_ptr<osg::LightSource>> lightNodes = lScene2.getLightNodes();
            mRenderer->setUpLights(&lightNodes);
            lightsAreSet = true;
            viewDistance = lScene2.getViewDistance();
        }

        fileScene2Bin.close();
    }

    if (!lightsAreSet)
        mRenderer->setUpLights(nullptr);

    if (mFileSystem->open(fileCacheBin, cacheBinPath))
    {
        if (!mCacheData.load(fileCacheBin)) return false;
        osg::ref_ptr<osg::Node> n = lCache.load(&mCacheData);

        if (!n)
            MFLogger::Logger::warn("Could not parse cache.bin file: " + cacheBinPath + ".", OSGRENDERER_MODULE_STR);
        else
            mRenderer->getRootNode()->addChild(n);

        fileCacheBin.close();
    }

    ////NOTE(DavoSK): Only for debug 
    //if (mFileSystem->open(fileCheckBin, checkBinPath))
    //{
    //    osg::ref_ptr<osg::Node> n = lCheck.load(fileCheckBin);
    //    if (!n)
    //        MFLogger::Logger::warn("Couldn't not parse check.bin file: " + checkBinPath + ".", OSGRENDERER_MODULE_STR);
    //    else
    //        mRootNode->addChild(n);

    //    fileCheckBin.close();
    //}

    createMissionEntities();

    mRenderer->setViewDistance(viewDistance);
    mRenderer->optimize();
    mRenderer->getLoaderCache()->logStats();
    return true;
}

bool MissionImpl::unload()
{
    return false;
}

bool MissionImpl::importFile()
{
    return false;
}

bool MissionImpl::exportFile()
{
    return false;
}

void MissionImpl::createMissionEntities()
{
    for (auto pair : mSceneData.getObjects()) {
        auto object = pair.second;

        switch (object.mType) {
            case MFFormat::DataFormatScene2BIN::OBJECT_TYPE_MODEL:
            {
                switch (object.mSpecialType) {
                    case MFFormat::DataFormatScene2BIN::SPECIAL_OBJECT_TYPE_PHYSICAL:
                    {
                        MFFormat::OSG4DSLoader model;
                    }
                    break;

                    default:
                        break;
                }
            }
            break;

            default:
                break;
        }
    }
}

}
