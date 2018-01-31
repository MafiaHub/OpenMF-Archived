#ifndef BULLET_PHYSICS_WORLD_H
#define BULLET_PHYSICS_WORLD_H

#include <physics/base_physics_world.hpp>
#include <utils/logger.hpp>
#include <klz/bullet.hpp>
#include <4ds/parser.hpp>
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

}

#endif
