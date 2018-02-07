#ifndef BULLET_PHYSICS_WORLD_H
#define BULLET_PHYSICS_WORLD_H

#include <physics/base_physics_world.hpp>
#include <utils/logger.hpp>
#include <klz/klz_bullet.hpp>
#include <4ds/parser_4ds.hpp>
#include <btBulletDynamicsCommon.h>
#include <vfs/vfs.hpp>
#include <utils/bullet.hpp>

#define BULLET_PHYSICS_WORLD_MODULE_STR "physics world"

namespace MFPhysics
{

class BulletPhysicsWorld: public PhysicsWorld
{
public:
    BulletPhysicsWorld();
    virtual ~BulletPhysicsWorld();
    virtual void frame(double dt) override;
    virtual bool loadMission(std::string mission) override;
    virtual MFGame::SpatialEntity::Id pointCollision(MFMath::Vec3 position) override;
    virtual double castRay(MFMath::Vec3 origin, MFMath::Vec3 direction) override;
    virtual void getWorldAABBox(MFMath::Vec3 &min, MFMath::Vec3 &max) override;
    btDiscreteDynamicsWorld *getWorld() { return mWorld; }

    std::vector<MFUtil::NamedRigidBody> getTreeKlzBodies();
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

}

#endif
