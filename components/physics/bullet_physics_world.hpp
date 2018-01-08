#ifndef BULLET_PHYSICS_WORLD_H
#define BULLET_PHYSICS_WORLD_H

#include <physics/base_physics_world.hpp>
#include <loggers/console.hpp>
#include <klz/bullet.hpp>
#include <btBulletDynamicsCommon.h>

#define BULLET_PHYSICS_WORLD_MODULE_STR "bullet world"

namespace MFPhysics
{

class BulletPhysicsWorld: public MFPhysicsWorld
{
public:
	BulletPhysicsWorld(float time);
	~BulletPhysicsWorld();
    virtual void frame(double dt) override;
    virtual bool loadMission(std::string mission) override;

	btDiscreteDynamicsWorld *getWorld() { return mWorld; }

	float getFrameTime()           { return mFrameTime; }
	void  setFrameTime(float time) { mFrameTime = time; }

private:
	btDiscreteDynamicsWorld             *mWorld;
	btBroadphaseInterface               *mBroadphaseInterface;
	btDefaultCollisionConfiguration     *mConfiguration;
	btCollisionDispatcher               *mCollisionDispatcher;
	btSequentialImpulseConstraintSolver *mSolver;

	float mFrameTime;
};

BulletPhysicsWorld::BulletPhysicsWorld(float time)
{
	mBroadphaseInterface = new btDbvtBroadphase();
	mConfiguration       = new btDefaultCollisionConfiguration();
	mCollisionDispatcher = new btCollisionDispatcher(mConfiguration);
	mSolver              = new btSequentialImpulseConstraintSolver;
	mWorld               = new btDiscreteDynamicsWorld(mCollisionDispatcher, mBroadphaseInterface, mSolver, mConfiguration);

	mWorld->setGravity(btVector3(0.0f, -9.81f, 0.0f));

	setFrameTime(time);
}

BulletPhysicsWorld::~BulletPhysicsWorld()
{
	delete mWorld;
	delete mSolver;
	delete mCollisionDispatcher;
	delete mConfiguration;
	delete mBroadphaseInterface;
}

void BulletPhysicsWorld::frame(double dt)
{
	mWorld->stepSimulation(mFrameTime, 1);

	size_t numManifolds = mWorld->getDispatcher()->getNumManifolds();
	for (size_t i = 0; i < numManifolds; i++)
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

bool BulletPhysicsWorld::loadMission(std::string mission)
{
    MFLogger::ConsoleLogger::info("Loading physics world for mission " + mission + ".",BULLET_PHYSICS_WORLD_MODULE_STR);
    return true;
}

};

#endif
