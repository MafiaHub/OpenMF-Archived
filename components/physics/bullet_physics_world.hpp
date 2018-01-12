#ifndef BULLET_PHYSICS_WORLD_H
#define BULLET_PHYSICS_WORLD_H

#include <physics/base_physics_world.hpp>
#include <loggers/console.hpp>
#include <klz/bullet.hpp>
#include <4ds/parser.hpp>
#include <btBulletDynamicsCommon.h>
#include <vfs/vfs.hpp>

#define BULLET_PHYSICS_WORLD_MODULE_STR "bullet world"

namespace MFPhysics
{

class BulletPhysicsWorld: public MFPhysicsWorld
{
public:
    BulletPhysicsWorld();
    ~BulletPhysicsWorld();
    virtual void frame(double dt) override;
    virtual bool loadMission(std::string mission) override;
    virtual int pointCollision(double x, double y, double z) override;
    btDiscreteDynamicsWorld *getWorld() { return mWorld; }

    std::vector<MFUtil::NamedRigidBody> getTreeKlzBodies();

    class ContactSensorCallback : public btCollisionWorld::ContactResultCallback
    {
    public:
        ContactSensorCallback(btRigidBody *body): btCollisionWorld::ContactResultCallback()
        {
            mBody = body;
            mResult = 0;
        }

        virtual btScalar addSingleResult(btManifoldPoint& cp,
            const btCollisionObjectWrapper* colObj0,int partId0,int index0,
            const btCollisionObjectWrapper* colObj1,int partId1,int index1) override
        {
            const btCollisionObjectWrapper *colObj = colObj0->getCollisionObject() == mBody ? colObj1 : colObj0;
            mResult = colObj->getCollisionObject();

            return 0;
        }

        const btCollisionObject *mResult;

    protected:
        btRigidBody *mBody;
    };

protected:
    btDiscreteDynamicsWorld             *mWorld;
    btBroadphaseInterface               *mBroadphaseInterface;
    btDefaultCollisionConfiguration     *mConfiguration;
    btCollisionDispatcher               *mCollisionDispatcher;
    btSequentialImpulseConstraintSolver *mSolver;

    std::vector<MFUtil::NamedRigidBody> mTreeKlzBodies;

    MFFile::FileSystem *mFileSystem;
};

BulletPhysicsWorld::BulletPhysicsWorld()
{
    mBroadphaseInterface = new btDbvtBroadphase();
    mConfiguration       = new btDefaultCollisionConfiguration();
    mCollisionDispatcher = new btCollisionDispatcher(mConfiguration);
    mSolver              = new btSequentialImpulseConstraintSolver;
    mWorld               = new btDiscreteDynamicsWorld(mCollisionDispatcher, mBroadphaseInterface, mSolver, mConfiguration);

    mWorld->setGravity(btVector3(0.0f, -9.81f, 0.0f));

    mFileSystem = MFFile::FileSystem::getInstance();
}

BulletPhysicsWorld::~BulletPhysicsWorld()
{
    delete mWorld;
    delete mSolver;
    delete mCollisionDispatcher;
    delete mConfiguration;
    delete mBroadphaseInterface;
}

int BulletPhysicsWorld::pointCollision(double x, double y, double z)
{
    btCollisionShape* sphere = new btSphereShape(0.01);  // can't make a point, so make a small sphere
    btRigidBody::btRigidBodyConstructionInfo ci(0,0,sphere,btVector3(0,0,0));
    btRigidBody* pointRigidBody = new btRigidBody(ci);
    pointRigidBody->translate(btVector3(x,y,z));

    ContactSensorCallback cb(pointRigidBody);

    mWorld->contactTest(pointRigidBody,cb);

    delete pointRigidBody;
    delete sphere;

    if (cb.mResult)
        return cb.mResult->getUserIndex();

    return -1;
}

void BulletPhysicsWorld::frame(double dt)
{
    mWorld->stepSimulation(dt, 1);
}

std::vector<MFUtil::NamedRigidBody> BulletPhysicsWorld::getTreeKlzBodies()
{
    return mTreeKlzBodies;
}

bool BulletPhysicsWorld::loadMission(std::string mission)
{
    MFLogger::ConsoleLogger::info("Loading physics world for mission " + mission + ".",BULLET_PHYSICS_WORLD_MODULE_STR);

    std::string missionDir = "missions/" + mission;
    std::string treeKlzPath = missionDir + "/tree.klz";
    std::string scene4dsPath = missionDir + "/scene.4ds";

    std::ifstream fileTreeKlz;
    std::ifstream fileScene4ds;

    BulletTreeKlzLoader treeKlzLoader;
    MFFormat::DataFormat4DS loader4DS;

    if (!mFileSystem->open(fileScene4ds,scene4dsPath))    // FIXME: parsed files should be retrieved from LoaderCache to avoid parsing the same file twice
    {
        MFLogger::ConsoleLogger::warn("Couldn't open scene.4ds file: " + treeKlzPath + ".",BULLET_PHYSICS_WORLD_MODULE_STR);
        return false;
    }

    if (mFileSystem->open(fileTreeKlz,treeKlzPath))
    {
        loader4DS.load(fileScene4ds);
        treeKlzLoader.load(fileTreeKlz,loader4DS);
        mTreeKlzBodies = treeKlzLoader.mRigidBodies; 
        fileTreeKlz.close();

        for (int i = 0; i < (int) mTreeKlzBodies.size(); ++i)
        {
            mTreeKlzBodies[i].mRigidBody.mBody->setActivationState(0);
            mWorld->addRigidBody(mTreeKlzBodies[i].mRigidBody.mBody.get()); 
        }
    }
    else
    {
        fileScene4ds.close();
        MFLogger::ConsoleLogger::warn("Couldn't open tree.klz file: " + treeKlzPath + ".",BULLET_PHYSICS_WORLD_MODULE_STR);
        return false;
    }

    return true;
}

}

#endif
