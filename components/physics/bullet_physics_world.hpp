#ifndef BULLET_PHYSICS_WORLD_H
#define BULLET_PHYSICS_WORLD_H

#include <physics/base_physics_world.hpp>
#include <loggers/console.hpp>
#include <klz/bullet.hpp>
#include <4ds/parser.hpp>
#include <btBulletDynamicsCommon.h>
#include <vfs/vfs.hpp>
#include <bullet_utils.hpp>

#define BULLET_PHYSICS_WORLD_MODULE_STR "physics world"

namespace MFPhysics
{

class BulletPhysicsWorld: public MFPhysicsWorld
{
public:
    BulletPhysicsWorld();
    ~BulletPhysicsWorld();
    virtual void frame(double dt) override;
    virtual bool loadMission(std::string mission) override;
    virtual MFGame::Id pointCollision(MFMath::Vec3 position) override;
    virtual void getWorldAABBox(MFMath::Vec3 &min, MFMath::Vec3 &max) override;
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
            const btCollisionObjectWrapper* colObj0,int /* partId0 */,int /* index0 */,
            const btCollisionObjectWrapper* colObj1,int /* partId1 */,int /* index1 */) override
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
    btOverlappingPairCache *mPairCache;
    std::vector<MFUtil::NamedRigidBody> mTreeKlzBodies;
    MFFile::FileSystem *mFileSystem;
};

BulletPhysicsWorld::BulletPhysicsWorld()
{
    MFLogger::ConsoleLogger::info("Initializing physics world.",BULLET_PHYSICS_WORLD_MODULE_STR);
    mPairCache           = new btSortedOverlappingPairCache();    // TODO: choose the right type of cache

    // TODO: How to determine the world AABB for Axis Sweep when the bodies aren't loaded yet? Maybe from tree.klz grid?
    mBroadphaseInterface = new bt32BitAxisSweep3(btVector3(-500,-500,-500),btVector3(500,500,500),16384,mPairCache); 
    mConfiguration       = new btDefaultCollisionConfiguration();
    mCollisionDispatcher = new btCollisionDispatcher(mConfiguration);
    mSolver              = new btSequentialImpulseConstraintSolver;
    mWorld               = new btDiscreteDynamicsWorld(mCollisionDispatcher,mBroadphaseInterface,mSolver,mConfiguration);

    mWorld->setGravity(btVector3(0.0f, 0.0f, -9.81f));

    mFileSystem = MFFile::FileSystem::getInstance();
}

BulletPhysicsWorld::~BulletPhysicsWorld()
{
    delete mWorld;
    delete mSolver;
    delete mCollisionDispatcher;
    delete mConfiguration;
    delete mBroadphaseInterface;
    delete mPairCache;
}

MFGame::Id BulletPhysicsWorld::pointCollision(MFMath::Vec3 position)
{
    btCollisionShape* sphere = new btSphereShape(0.01);  // can't make a point, so make a small sphere
    btRigidBody::btRigidBodyConstructionInfo ci(0,0,sphere,btVector3(0,0,0));
    btRigidBody* pointRigidBody = new btRigidBody(ci);
    pointRigidBody->translate(btVector3(position.x,position.y,position.z));

    ContactSensorCallback cb(pointRigidBody);

    mWorld->contactTest(pointRigidBody,cb);

    delete pointRigidBody;
    delete sphere;

    if (cb.mResult)
        return MFUtil::getCollisionObjectEntityId(cb.mResult);

    return MFGame::NullId;
}

void BulletPhysicsWorld::frame(double dt)
{
    mWorld->stepSimulation(dt,1);
}

void BulletPhysicsWorld::getWorldAABBox(MFMath::Vec3 &min, MFMath::Vec3 &max)
{
    // FIXME: find/implement a way to get a list of all rigid bodies in a world

    if (mTreeKlzBodies.size() > 0)
    {
        min = MFMath::Vec3(0,0,0);
        max = MFMath::Vec3(0,0,0);

        for (int i = 0; i < (int) mTreeKlzBodies.size(); ++i)
        {
            btVector3 p1,p2;

            mTreeKlzBodies[i].mRigidBody.mBody->getAabb(p1,p2);

            min.x = std::min(min.x,std::min(p1.x(),p2.x()));
            min.y = std::min(min.y,std::min(p1.y(),p2.y()));
            min.z = std::min(min.z,std::min(p1.z(),p2.z()));

            max.x = std::max(max.x,std::max(p1.x(),p2.x()));
            max.y = std::max(max.y,std::max(p1.y(),p2.y()));
            max.z = std::max(max.z,std::max(p1.z(),p2.z()));
        }
    }
    else
    {
        // TMP: assign some default values here, the above FIXME should get rid of this
        min = MFMath::Vec3(-100,-100,-100);
        max = MFMath::Vec3(100,100,100);
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
