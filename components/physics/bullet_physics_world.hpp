#ifndef BULLET_PHYSICS_WORLD_H
#define BULLET_PHYSICS_WORLD_H

#include <physics/base_physics_world.hpp>
#include <loggers/console.hpp>
#include <klz/bullet.hpp>
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
    virtual MFGame::SpatialEntity *pointCollision(double x, double y, double z) override;
    btDiscreteDynamicsWorld *getWorld() { return mWorld; }

    std::vector<MFUtil::NamedRigidBody> getTreeKlzBodies();

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

MFGame::SpatialEntity *BulletPhysicsWorld::pointCollision(double x, double y, double z)
{
    return 0;
}

void BulletPhysicsWorld::frame(double dt)
{
    mWorld->stepSimulation(dt, 1);

    size_t numManifolds = mWorld->getDispatcher()->getNumManifolds();

    for (size_t i = 0; i < numManifolds; ++i)
    {
        btPersistentManifold *c = mWorld->getDispatcher()->getManifoldByIndexInternal(i);
        btCollisionObject const *a = c->getBody0();
        btCollisionObject const *b = c->getBody1();

        size_t co = c->getNumContacts();
        for (size_t j = 0; j < co; j++)
        {
            btManifoldPoint &pt = c->getContactPoint(j);
            if (pt.getDistance() < 0.0f)
            {
                    // TODO collision has happened, do something smart.
            }
        }
    }
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

    std::ifstream fileTreeKlz;

    BulletTreeKlzLoader treeKlzLoader;

    if (mFileSystem->open(fileTreeKlz,treeKlzPath))
    {
        mTreeKlzBodies = treeKlzLoader.load(fileTreeKlz);
        fileTreeKlz.close();

        for (int i = 0; i < mTreeKlzBodies.size(); ++i)
        {
            mTreeKlzBodies[i].mRigidBody.mBody->setActivationState(0);
            mWorld->addRigidBody(mTreeKlzBodies[i].mRigidBody.mBody.get()); 
        }
    }
    else
    {
        MFLogger::ConsoleLogger::warn("Couldn't open tree.klz file: " + treeKlzPath + ".",BULLET_PHYSICS_WORLD_MODULE_STR);
        return false;
    }

    return true;
}

};

#endif
