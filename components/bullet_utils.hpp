#ifndef BULLET_UTILS_H
#define BULLET_UTILS_H

#include <string>
#include <id_manager.hpp>
#include <BulletDynamics/Dynamics/btRigidBody.h>
#include <BulletCollision/CollisionShapes/btTriangleMesh.h>
#include <btBulletDynamicsCommon.h>
#include <math.hpp>
#include <memory>

namespace MFUtil
{

inline btVector3 mafiaVec3ToBullet(double x, double y, double z);
btTransform mafiaMat4ToBullet(MFMath::Mat4 m);

/**
  Holds together a bullet rigid body and its collision shape.
 */

typedef struct
{
    std::shared_ptr<btRigidBody> mBody;
    std::shared_ptr<btDefaultMotionState> mMotionState;
    std::shared_ptr<btCollisionShape> mShape;
    std::shared_ptr<btTriangleMesh> mMesh;
} FullRigidBody;

typedef struct
{
    std::string mName;
    FullRigidBody mRigidBody;
} NamedRigidBody;

}

#endif
