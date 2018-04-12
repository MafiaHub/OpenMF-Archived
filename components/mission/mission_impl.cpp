#include "mission/mission_impl.hpp"

namespace MFGame
{

MissionImpl::MissionImpl(std::string missionName, MFRender::OSGRenderer *renderer): Mission(missionName)
{
    mFileSystem = MFFile::FileSystem::getInstance();
    mRenderer = renderer;
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
    MFFormat::OSGScene2BinLoader lScene2;
    MFFormat::OSGCacheBinLoader lCache;

    l4ds.setLoaderCache(mRenderer->getLoaderCache());
    lScene2.setLoaderCache(mRenderer->getLoaderCache());
    lCache.setLoaderCache(mRenderer->getLoaderCache());

    MFFormat::OSGLoader::NodeMap nodeMap;
    l4ds.setNodeMap(&nodeMap);
    lScene2.setNodeMap(&nodeMap);

    if (mFileSystem->open(file4DS, scene4dsPath)) {
        mSceneModel.load(file4DS);
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
        mSceneData.load(fileScene2Bin);
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
        mCacheData.load(fileCacheBin);
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

    //setViewDistance(viewDistance);
    //optimize();
    //mLoaderCache.logStats();
    return false;
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

}
