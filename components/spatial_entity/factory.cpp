#include <spatial_entity/factory.hpp>

namespace MFGame
{

SpatialEntityFactory::SpatialEntityFactory(MFRender::OSGRenderer *renderer, MFPhysics::BulletPhysicsWorld *physicsWorld, MFGame::SpatialEntityManager *entityManager)
{
    mDebugMode = false;

    mRenderer = renderer;
    mPhysicsWorld = physicsWorld;
    mEntityManager = entityManager;

    const double r = 0.7;
    const double l = 1.0;

    mTestMaterial = new osg::Material();
    mTestMaterial->setEmission(osg::Material::FRONT_AND_BACK,osg::Vec4(0,0,0,0));

    mTestStateSet = new osg::StateSet();
    mTestStateSet->setAttributeAndModes(mTestMaterial);

    mTestVisualSphereShape = new osg::Sphere(osg::Vec3f(0,0,0),r);
    mTestSphereNode = new osg::ShapeDrawable(mTestVisualSphereShape);
    mTestSphereNode->setStateSet(mTestStateSet);
    mTestPhysicalSphereShape = std::make_shared<btSphereShape>(r);

    mTestVisualBoxShape = new osg::Box(osg::Vec3f(0,0,0),l);
    mTestBoxNode = new osg::ShapeDrawable(mTestVisualBoxShape);
    mTestBoxNode->setStateSet(mTestStateSet);
    mTestPhysicalBoxShape = std::make_shared<btBoxShape>(btVector3(l/2.0,l/2.0,l/2.0));

    mCapsuleShape = new osg::Capsule(osg::Vec3f(0,0,0),1,3);
    mCapsuleNode = new osg::ShapeDrawable(mCapsuleShape);
    mPhysicalCapsuleShape = std::make_shared<btCapsuleShapeZ>(1.0f,3.0f);

    mCameraShape = std::make_shared<btSphereShape>(1.0f);
}

MFGame::SpatialEntity::Id SpatialEntityFactory::createEntity(
    osg::MatrixTransform *graphicNode,
    std::shared_ptr<btRigidBody> physicsBody,
    std::shared_ptr<btDefaultMotionState> physicsMotionsState, 
    std::string name,
    SpatialEntity::PhysicsBehavior physicsBehavior)
{
    if (physicsBody && physicsMotionsState && physicsBody->getMotionState() != physicsMotionsState.get())
        physicsBody->setMotionState(physicsMotionsState.get());

    std::shared_ptr<MFGame::SpatialEntityImplementation> newEntity = std::make_shared<MFGame::SpatialEntityImplementation>();
    newEntity->setName(name);
    newEntity->setDebugMode(mDebugMode);
    newEntity->setOSGRootNode(mRenderer->getRootNode());
    newEntity->setVisualNode(graphicNode);
    newEntity->setPhysicsBody(physicsBody);
    newEntity->setPhysicsMotionState(physicsMotionsState);
    newEntity->ready();
    mEntityManager->addEntity(newEntity);
    newEntity->setPhysicsBehavior(physicsBehavior);
    return newEntity->getId();
}

MFGame::SpatialEntity::Id SpatialEntityFactory::createCapsuleEntity()
{
    MFUtil::FullRigidBody body;

    btScalar mass = 1.0f;
    body.mMotionState = std::make_shared<btDefaultMotionState>(
        btTransform(btQuaternion(0, 0, 0, mass),
        btVector3(0, 0, 0)));

    btVector3 inertia;
    mPhysicalCapsuleShape->calculateLocalInertia(mass,inertia);
    body.mBody = std::make_shared<btRigidBody>(mass, body.mMotionState.get(), mPhysicalCapsuleShape.get(), inertia);
    body.mBody->setActivationState(DISABLE_DEACTIVATION);
    mPhysicsWorld->getWorld()->addRigidBody(body.mBody.get());

    osg::ref_ptr<osg::MatrixTransform> visualTransform = new osg::MatrixTransform();
    visualTransform->addChild(mCapsuleNode);
    mRenderer->getRootNode()->addChild(visualTransform);

    return createEntity(visualTransform.get(), body.mBody, body.mMotionState, "capsule", SpatialEntity::RIGID_PLAYER);
}

MFGame::SpatialEntity::Id SpatialEntityFactory::createCameraEntity()
{
    MFUtil::FullRigidBody body;

    btScalar mass = 1.0f;
    body.mMotionState = std::make_shared<btDefaultMotionState>(
        btTransform(btQuaternion(0, 0, 0, mass), 
        btVector3(0,0,0)));

    btVector3 inertia;
    mCameraShape->calculateLocalInertia(mass, inertia);
    body.mBody = std::make_shared<btRigidBody>(mass, body.mMotionState.get(), mCameraShape.get(), inertia);
    body.mBody->setActivationState(DISABLE_DEACTIVATION);
    mPhysicsWorld->getWorld()->addRigidBody(body.mBody.get());
    body.mBody->setGravity(btVector3(0, 0, 0));

    osg::ref_ptr<osg::MatrixTransform> visualTransform = new osg::MatrixTransform();
    mRenderer->getRootNode()->addChild(visualTransform);

    return createEntity(visualTransform.get(), body.mBody, body.mMotionState, "camera", SpatialEntity::RIGID_PLAYER);
}

MFGame::SpatialEntity::Id SpatialEntityFactory::createTestShapeEntity(btCollisionShape *colShape, osg::ShapeDrawable *visualNode)
{
    osg::ref_ptr<osg::MatrixTransform> visualTransform = new osg::MatrixTransform();
    visualTransform->addChild(visualNode);
    mRenderer->getRootNode()->addChild(visualTransform);

    std::shared_ptr<btDefaultMotionState> motionState = std::make_shared<btDefaultMotionState>();

    btScalar mass = 1;
    btVector3 fallInertia(0,0,0);
    mTestPhysicalSphereShape->calculateLocalInertia(mass,fallInertia);
    btRigidBody::btRigidBodyConstructionInfo ci(mass,motionState.get(),colShape,fallInertia);

    std::shared_ptr<btRigidBody> physicalBody = std::make_shared<btRigidBody>(ci);
    physicalBody->setActivationState(DISABLE_DEACTIVATION);
    mPhysicsWorld->getWorld()->addRigidBody(physicalBody.get());

    return createEntity(visualTransform.get(),physicalBody,motionState,"test");
}

MFGame::SpatialEntity::Id SpatialEntityFactory::createTestBallEntity()
{
    return createTestShapeEntity(mTestPhysicalSphereShape.get(),mTestSphereNode.get());
}

MFGame::SpatialEntity::Id SpatialEntityFactory::createTestBoxEntity()
{
    return createTestShapeEntity(mTestPhysicalBoxShape.get(),mTestBoxNode.get());
}

class CreateEntitiesFromSceneVisitor: public osg::NodeVisitor
{
public:
    CreateEntitiesFromSceneVisitor(std::vector<MFUtil::NamedRigidBody> *treeKlzBodies, MFGame::SpatialEntityFactory *entityFactory): osg::NodeVisitor()
    {
        mTreeKlzBodies = treeKlzBodies;
        mEntityFactory = entityFactory;
        mModelName = "";

        for (int i = 0; i < (int) treeKlzBodies->size(); ++i)
        {
            std::string name = (*treeKlzBodies)[i].mName;
            mNameToBody.insert(std::pair<std::string,MFUtil::NamedRigidBody *>(name,&((*treeKlzBodies)[i])));
        }
    }

    virtual void apply(osg::Node &n) override
    {
        MFUtil::traverse(this,n);
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
                        MFLogger::Logger::warn("Could not find matching collision for visual node \"" + n.getName() + "\" (model: \"" + mModelName + "\").",SPATIAL_ENTITY_FACTORY_MODULE_STR);
                        mEntityFactory->createEntity(&n,0,0,fullName);
                    }
                    else
                    {
                        for (int i = 0; i < (int) matches.size(); ++i)
                        {
                            mEntityFactory->createEntity(&n, matches[i]->mRigidBody.mBody,matches[i]->mRigidBody.mMotionState,fullName);
                            mMatchedBodies.insert(matches[i]->mName);
                        }
                    }
                }
            }
        }

        if (modelName.size() > 0)
            mModelName = modelName;          // traverse downwards with given name prefix

        MFUtil::traverse(this,n);

        if (modelName.size() > 0)
            mModelName = "";                 // going back up => clear the name prefix
    }

    std::set<std::string> mMatchedBodies;

protected:
    std::vector<MFUtil::NamedRigidBody> *mTreeKlzBodies;
    MFGame::SpatialEntityFactory *mEntityFactory;
    std::string mModelName;                  // when traversing into a model loaded from scene2.bin, this will contain the model name (needed as the name prefix)
    std::multimap<std::string,MFUtil::NamedRigidBody *> mNameToBody;

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

void SpatialEntityFactory::createMissionEntities()
{
    auto treeKlzBodies = mPhysicsWorld->getTreeKlzBodies();

    CreateEntitiesFromSceneVisitor v(&treeKlzBodies,this);
    mRenderer->getRootNode()->accept(v);

    // process the unmatched rigid bodies:

    for (int i = 0; i < (int) treeKlzBodies.size(); ++i)
    {
        if (v.mMatchedBodies.find(treeKlzBodies[i].mName) != v.mMatchedBodies.end())
            continue;

        createEntity(0,
            treeKlzBodies[i].mRigidBody.mBody,
            treeKlzBodies[i].mRigidBody.mMotionState,
            treeKlzBodies[i].mName);
    }

    // TODO: set the static flag to the loaded bodies here
}

}
