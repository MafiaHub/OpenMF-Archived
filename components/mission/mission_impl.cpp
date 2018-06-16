#include "mission/mission_impl.hpp"
#include "engine/engine.hpp"

namespace MFGame
{

MissionImpl::MissionImpl(std::string missionName, MFGame::Engine *engine): Mission(missionName),
                                                                           mSceneModel(),
                                                                           mSceneData(), mStaticColsData()
    {
        mFileSystem = MFFile::FileSystem::getInstance();
        mRenderer = static_cast<MFRender::OSGRenderer *>(engine->getRenderer());
        mEngine = engine;
    }

MissionImpl::~MissionImpl() = default;

bool MissionImpl::load()
{
    const std::string missionDir = "missions/" + mMissionName;
    const std::string scene4dsPath = missionDir + "/scene.4ds";
    const std::string scene2BinPath = missionDir + "/scene2.bin";
    const std::string cacheBinPath = missionDir + "/cache.bin";
    const std::string checkBinPath = missionDir + "/check.bin";
    const std::string treeKlzPath = missionDir + "/tree.klz";

    std::ifstream file4DS;
    std::ifstream fileScene2Bin;
    std::ifstream fileCacheBin;
    std::ifstream fileCheckBin;
    std::ifstream fileTreeKlz;

    MFFormat::OSGModelLoader l4ds;
    MFFormat::OSGStaticSceneLoader lScene2;
    MFFormat::OSGCachedCityLoader lCache;
    MFPhysics::BulletStaticCollisionLoader lTreeKlz;

    l4ds.setLoaderCache(mRenderer->getLoaderCache());
    lScene2.setLoaderCache(mRenderer->getLoaderCache());
    lScene2.setObjectFactory(mEngine->getEntityFactory());
    lCache.setLoaderCache(mRenderer->getLoaderCache());
    lCache.setObjectFactory(mEngine->getEntityFactory());
    
    l4ds.setNodeMap(&mNodeMap);
    lScene2.setNodeMap(&mNodeMap);

    if (mFileSystem->open(file4DS, scene4dsPath)) {   
        file4DS.close();

        osg::ref_ptr<osg::Node> n = l4ds.load(&mSceneModel);
        mSceneModelNode = n->asGroup();
        mRenderer->getRootNode()->addChild(n);
    }
    else
        MFLogger::Logger::warn("Could not open 4ds file: " + scene4dsPath + ".", OSGRENDERER_MODULE_STR);
    
    bool lightsAreSet = false;

    float viewDistance = 2000;    // default value

    if (mFileSystem->open(fileScene2Bin, scene2BinPath))
    {
        osg::ref_ptr<osg::Node> n = lScene2.load(&mSceneData);
        mSceneNode = n->asGroup();

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
        osg::ref_ptr<osg::Node> n = lCache.load(&mCacheData);
        mCachedCityNode = n->asGroup();

        if (!n)
            MFLogger::Logger::warn("Could not parse cache.bin file: " + cacheBinPath + ".", OSGRENDERER_MODULE_STR);
        else
            mRenderer->getRootNode()->addChild(n);

        fileCacheBin.close();
    }

    lTreeKlz.load(&mStaticColsData, mSceneModel);
    mEngine->getPhysicsWorld()->setTreeKlzBodies(lTreeKlz.mRigidBodies);
    auto treeKlzBodies = lTreeKlz.mRigidBodies;
    for (int i = 0; i < (int)treeKlzBodies.size(); ++i)
    {
        treeKlzBodies[i].mRigidBody.mBody->setActivationState(0);
        mEngine->getPhysicsWorld()->getWorld()->addRigidBody(treeKlzBodies[i].mRigidBody.mBody.get());
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
    mRenderer->getRootNode()->removeChild(mSceneModelNode);
    mRenderer->getRootNode()->removeChild(mSceneNode);
    mRenderer->getRootNode()->removeChild(mCachedCityNode);


    auto phys = static_cast<MFPhysics::BulletPhysicsWorld *>(mEngine->getPhysicsWorld());
    auto bodies = phys->getTreeKlzBodies();
    for (auto body : bodies) {
        phys->getWorld()->removeRigidBody(body.mRigidBody.mBody.get());
    }

    for (auto entity : mLoadedEntities) {
        mEngine->getEntityManager()->removeEntity(entity->getId());
    }

    mLoadedEntities.clear();

    mNodeMap.clear();

    return true;
}

bool MissionImpl::importFile()
{
    const std::string missionDir = "missions/" + mMissionName;
    const std::string scene4dsPath = missionDir + "/scene.4ds";
    const std::string scene2BinPath = missionDir + "/scene2.bin";
    const std::string cacheBinPath = missionDir + "/cache.bin";
    const std::string checkBinPath = missionDir + "/check.bin";
    const std::string treeKlzPath = missionDir + "/tree.klz";

    std::ifstream file4DS;
    std::ifstream fileScene2Bin;
    std::ifstream fileCacheBin;
    std::ifstream fileCheckBin;
    std::ifstream fileTreeKlz;

    if (mFileSystem->open(file4DS, scene4dsPath)) {
        if (!mSceneModel.load(file4DS)) return false;
        file4DS.close();
    }
    if (mFileSystem->open(fileScene2Bin, scene2BinPath))
    {
        if (!mSceneData.load(fileScene2Bin)) return false;
        fileScene2Bin.close();
    }

    if (mFileSystem->open(fileCacheBin, cacheBinPath))
    {
        if (!mCacheData.load(fileCacheBin)) return false;
        fileCacheBin.close();
    }

    if (mFileSystem->open(fileTreeKlz, treeKlzPath)) {
        if (!mStaticColsData.load(fileTreeKlz)) return false;
        fileTreeKlz.close();
    }

    return true;
}

bool MissionImpl::exportFile()
{
    return false;
}

class CreateEntitiesFromSceneVisitor : public osg::NodeVisitor
{
public:
    CreateEntitiesFromSceneVisitor(std::vector<MFUtil::NamedRigidBody> *treeKlzBodies, MFGame::EntityFactory *entityFactory) : osg::NodeVisitor()
    {
        mTreeKlzBodies = treeKlzBodies;
        mEntityFactory = entityFactory;
        mModelName = "";

        for (auto& treeKlzBody : *treeKlzBodies)
        {
            const std::string name = treeKlzBody.mName;
            mNameToBody.insert(std::pair<std::string, MFUtil::NamedRigidBody *>(name, &treeKlzBody));
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

            if (!descriptions.empty())
            {
                if (descriptions[0] == "scene2.bin model")
                {
                    modelName = n.getName();
                }
                else if (descriptions[0] == "4ds mesh")
                {
                    std::vector<MFUtil::NamedRigidBody *> matches = findCollisions(n.getName());
                    const std::string fullName = mModelName.length() > 0 ? mModelName + "." + n.getName() : n.getName();

                    if (matches.empty())
                    {
                        MFLogger::Logger::warn("Could not find matching collision for visual node \"" + n.getName() + "\" (model: \"" + mModelName + "\").", ENTITY_FACTORY_MODULE_STR);
                        mEntityFactory->createEntity(&n, 0, 0, fullName);
                    }
                    else
                    {
                        for (auto& match : matches)
                        {
                            mEntityFactory->createEntity(&n, match->mRigidBody.mBody, match->mRigidBody.mMotionState, fullName);
                            mMatchedBodies.insert(match->mName);
                        }
                    }
                }
            }
        }

        if (!modelName.empty())
            mModelName = modelName;          // traverse downwards with given name prefix

        MFUtil::traverse(this, n);

        if (!modelName.empty())
            mModelName = "";                 // going back up => clear the name prefix
    }

    std::set<std::string> mMatchedBodies;

protected:
    std::vector<MFUtil::NamedRigidBody> *mTreeKlzBodies;
    MFGame::EntityFactory *mEntityFactory;
    std::string mModelName;                  // when traversing into a model loaded from scene2.bin, this will contain the model name (needed as the name prefix)
    std::multimap<std::string, MFUtil::NamedRigidBody *> mNameToBody;

    std::vector<MFUtil::NamedRigidBody *> findCollisions(const std::string& visualName)
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

    CreateEntitiesFromSceneVisitor v(&treeKlzBodies, mEngine->getEntityFactory());
    mRenderer->getRootNode()->accept(v);

    // process the unmatched rigid bodies:

    for (int i = 0; i < (int)treeKlzBodies.size(); ++i)
    {
        if (v.mMatchedBodies.find(treeKlzBodies[i].mName) != v.mMatchedBodies.end())
            continue;

        mEngine->getEntityFactory()->createEntity(0,
            treeKlzBodies[i].mRigidBody.mBody,
            treeKlzBodies[i].mRigidBody.mMotionState,
            treeKlzBodies[i].mName);
    }

    for (const auto pair : mSceneData.getObjects()) {
        auto object = pair.second;

        MFGame::EntityImpl *entity = nullptr;

        switch (object.mType) {
            case MFFormat::DataFormatScene2BIN::OBJECT_TYPE_MODEL:
            {
                switch (object.mSpecialType) {
                    case MFFormat::DataFormatScene2BIN::SPECIAL_OBJECT_TYPE_PHYSICAL:
                    {
                        const auto entityId = mEngine->getEntityFactory()->createPropEntity(&object);
                        entity = static_cast<MFGame::EntityImpl *>(mEngine->getEntityManager()->getEntityById(entityId));
                    }
                    break;

                    case MFFormat::DataFormatScene2BIN::SPECIAL_OBJECT_TYPE_CHARACTER:
                    {
                        // TODO real character support

                        const auto entityId = mEngine->getEntityFactory()->createPawnEntity(object.mModelName, 1000.0f);
                        entity = static_cast<MFGame::EntityImpl *>(mEngine->getEntityManager()->getEntityById(entityId));
                    }
                    break;

                    case MFFormat::DataFormatScene2BIN::SPECIAL_OBJECT_TYPE_PLAYER:
                    {
                        osg::ref_ptr<osg::MatrixTransform> transform = new osg::MatrixTransform();
                        auto node = mEngine->getEntityFactory()->loadModel(object.mModelName);
                        transform->addChild(node);
                        mRenderer->getRootNode()->addChild(transform);

                        const auto entityId = mEngine->getEntityFactory()->createEntity(transform, nullptr, nullptr, "player start");
                        entity = static_cast<MFGame::EntityImpl *>(mEngine->getEntityManager()->getEntityById(entityId));
                    }
                    break;

                    default: 
                    {
                        if (!object.mSpecialType)continue;
                        MFLogger::Logger::info("Unsupported special object: " + object.mName + " with type: " + std::to_string(object.mSpecialType), MISSION_MANAGER_MODULE_STR);
                        
                        if (object.mModelName.length() == 0) continue;
                        auto node = mEngine->getEntityFactory()->loadModel(object.mModelName);

                        osg::ref_ptr<osg::MatrixTransform> transform = new osg::MatrixTransform();
                        transform->addChild(node);
                        mRenderer->getRootNode()->addChild(transform);

                        const auto entityId = mEngine->getEntityFactory()->createEntity(transform, nullptr, nullptr, object.mName);
                        entity = static_cast<MFGame::EntityImpl *>(mEngine->getEntityManager()->getEntityById(entityId));
                    }
                    break;
                }
            }
            break;

            default:
                if (!object.mSpecialType)continue;
                MFLogger::Logger::info("Unsupported special object: " + object.mName + " with type: " + std::to_string(object.mSpecialType), MISSION_MANAGER_MODULE_STR);
        }

        if (!entity) continue;

        const auto it = mNodeMap.find(object.mParentName);


        if (it != mNodeMap.end()) {
            const auto parent = dynamic_cast<osg::MatrixTransform *>(it->second.get());
            osg::Vec3f oPos = parent->getMatrix().getTrans();
            osg::Quat oRot = parent->getMatrix().getRotate();
            osg::Vec3f oScale = parent->getMatrix().getScale();
            
            // TODO make sure entity starts at transform calculated from his parent
            const auto pos = MFMath::Vec3(oPos.x() + object.mPos.x, oPos.y() + object.mPos.z, oPos.z() + object.mPos.y);
            const auto rot = MFMath::Quat(oRot.x(), oRot.y(), oRot.z(), oRot.w());
            //rot *= MFMath::Quat(object.mRot.x, object.mRot.x, object.mRot.x, object.mRot.w);
            //auto scale = MFMath::Vec3(oScale.x(), oScale.y(), oScale.z());

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

        mLoadedEntities.push_back(entity);
    }
}

}
