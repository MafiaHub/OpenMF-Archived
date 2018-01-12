#ifndef BULLET_UTILS_H
#define BULLET_UTILS_H

#include <string>
#include <BulletDynamics/Dynamics/btRigidBody.h>
#include <BulletCollision/CollisionShapes/btTriangleMesh.h>

namespace MFUtil
{

inline btVector3 mafiaVec3ToBullet(double x, double y, double z)
{
    return btVector3(x,z,y); 
}

/**
  Holds together a bullet rigid body and its collision shape.
 */

typedef struct
{
    std::shared_ptr<btRigidBody> mBody;
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
