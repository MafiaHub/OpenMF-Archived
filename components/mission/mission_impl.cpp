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
    std::string treeKlzPath = missionDir + "/tree.klz";

    std::ifstream file4DS;
    std::ifstream fileScene2Bin;
    std::ifstream fileCacheBin;
    std::ifstream fileCheckBin;
    std::ifstream fileTreeKlz;

    MFFormat::OSGModelLoader l4ds;
    MFFormat::OSGStaticSceneLoader lScene2;
    MFFormat::OSGCachedCityLoader lCache;

    l4ds.setLoaderCache(mRenderer->getLoaderCache());
    lScene2.setLoaderCache(mRenderer->getLoaderCache());
    lScene2.setObjectFactory(mEngine->getSpatialEntityFactory());
    lCache.setLoaderCache(mRenderer->getLoaderCache());
    lCache.setObjectFactory(mEngine->getSpatialEntityFactory());

    
    l4ds.setNodeMap(&mNodeMap);
    lScene2.setNodeMap(&mNodeMap);

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

    if (mFileSystem->open(fileTreeKlz, treeKlzPath)) {
        mStaticColsData.load(fileTreeKlz);
        fileTreeKlz.close();
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

class CreateEntitiesFromSceneVisitor : public osg::NodeVisitor
{
public:
    CreateEntitiesFromSceneVisitor(std::vector<MFUtil::NamedRigidBody> *treeKlzBodies, MFGame::SpatialEntityFactory *entityFactory) : osg::NodeVisitor()
    {
        mTreeKlzBodies = treeKlzBodies;
        mEntityFactory = entityFactory;
        mModelName = "";

        for (int i = 0; i < (int)treeKlzBodies->size(); ++i)
        {
            std::string name = (*treeKlzBodies)[i].mName;
            mNameToBody.insert(std::pair<std::string, MFUtil::NamedRigidBody *>(name, &((*treeKlzBodies)[i])));
        }
    }

    virtual void apply(osg::Node &n) override
    {
        MFUtil::traverse(this, n);
    }

    virtual void apply(osg::MatrixTransform &n) override
    {
        std::string modelName = "";

        if (n.getUserDataContainer())
        {
            std::vector<std::string> descriptions = n.getUserDataContainer()->getDescriptions();

            if (descriptions.size() > 0)
            {
                if (descriptions[0].compare("scene2.bin model") == 0)
                {
                    modelName = n.getName();
                }
                else if (descriptions[0].compare("4ds mesh") == 0)
                {
                    std::vector<MFUtil::NamedRigidBody *> matches = findCollisions(n.getName());
                    std::string fullName = mModelName.length() > 0 ? mModelName + "." + n.getName() : n.getName();

                    if (matches.size() == 0)
                    {
                        MFLogger::Logger::warn("Could not find matching collision for visual node \"" + n.getName() + "\" (model: \"" + mModelName + "\").", SPATIAL_ENTITY_FACTORY_MODULE_STR);
                        mEntityFactory->createEntity(&n, 0, 0, fullName);
                    }
                    else
                    {
                        for (int i = 0; i < (int)matches.size(); ++i)
                        {
                            mEntityFactory->createEntity(&n, matches[i]->mRigidBody.mBody, matches[i]->mRigidBody.mMotionState, fullName);
                            mMatchedBodies.insert(matches[i]->mName);
                        }
                    }
                }
            }
        }

        if (modelName.size() > 0)
            mModelName = modelName;          // traverse downwards with given name prefix

        MFUtil::traverse(this, n);

        if (modelName.size() > 0)
            mModelName = "";                 // going back up => clear the name prefix
    }

    std::set<std::string> mMatchedBodies;

protected:
    std::vector<MFUtil::NamedRigidBody> *mTreeKlzBodies;
    MFGame::SpatialEntityFactory *mEntityFactory;
    std::string mModelName;                  // when traversing into a model loaded from scene2.bin, this will contain the model name (needed as the name prefix)
    std::multimap<std::string, MFUtil::NamedRigidBody *> mNameToBody;

    std::vector<MFUtil::NamedRigidBody *> findCollisions(std::string visualName)
    {
        std::vector<MFUtil::NamedRigidBody *> result;

        auto found = mNameToBody.equal_range(visualName);

        for (auto it = found.first; it != found.second; ++it)
            result.push_back(it->second);

        found = mNameToBody.equal_range(mModelName);

        for (auto it = found.first; it != found.second; ++it)
            result.push_back(it->second);

        found = mNameToBody.equal_range(mModelName + "." + visualName);

        for (auto it = found.first; it != found.second; ++it)
            result.push_back(it->second);

        return result;
    }
};

void MissionImpl::createMissionEntities()
{
    auto treeKlzBodies = mEngine->getPhysicsWorld()->getTreeKlzBodies();

    CreateEntitiesFromSceneVisitor v(&treeKlzBodies, mEngine->getSpatialEntityFactory());
    mRenderer->getRootNode()->accept(v);

    // process the unmatched rigid bodies:

    for (int i = 0; i < (int)treeKlzBodies.size(); ++i)
    {
        if (v.mMatchedBodies.find(treeKlzBodies[i].mName) != v.mMatchedBodies.end())
            continue;

        mEngine->getSpatialEntityFactory()->createEntity(0,
            treeKlzBodies[i].mRigidBody.mBody,
            treeKlzBodies[i].mRigidBody.mMotionState,
            treeKlzBodies[i].mName);
    }

    for (auto pair : mSceneData.getObjects()) {
        auto object = pair.second;

        MFGame::SpatialEntityImpl *entity = nullptr;

        switch (object.mType) {
            case MFFormat::DataFormatScene2BIN::OBJECT_TYPE_MODEL:
            {
                switch (object.mSpecialType) {
                    case MFFormat::DataFormatScene2BIN::SPECIAL_OBJECT_TYPE_PHYSICAL:
                    {
                        auto entityId = mEngine->getSpatialEntityFactory()->createDynamicEntity(&object);
                        entity = (MFGame::SpatialEntityImpl *)mEngine->getSpatialEntityManager()->getEntityById(entityId);
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

        if (!entity) continue;

        auto it = mNodeMap.find(object.mParentName);


        if (it != mNodeMap.end()) {
            auto parent = (osg::MatrixTransform *)it->second.get();
            osg::Vec3f oPos = parent->getMatrix().getTrans();
            osg::Quat oRot = parent->getMatrix().getRotate();
            osg::Vec3f oScale = parent->getMatrix().getScale();

            // TODO make sure entity starts at transform calculated from his parent
            //auto pos = MFMath::Vec3(oPos.x(), oPos.y(), oPos.z());
            //auto rot = MFMath::Quat(oRot.x(), oRot.y(), oRot.z(), oRot.w());
            //auto scale = MFMath::Vec3(oScale.x(), oScale.y(), oScale.z());

            auto pos = object.mPos;
            auto rot = object.mRot;

            entity->setPosition(pos);
            entity->setRotation(rot);
        }
        else {
            MFFormat::OSGStaticSceneLoader l;
            auto pos = l.toOSG(object.mPos);
            auto rot = l.toOSG(object.mRot);

            entity->setPosition(MFMath::Vec3(pos.x(), pos.y(), pos.z()));
            entity->setRotation(MFMath::Quat(rot.x(), rot.y(), rot.z(), rot.w()));
        }
    }
}

}
