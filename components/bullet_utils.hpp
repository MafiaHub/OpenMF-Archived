#ifndef BULLET_UTILS_H
#define BULLET_UTILS_H

#include <BulletDynamics/Dynamics/btRigidBody.h>

namespace MFUtil
{

inline btVector3 mafiaVec3ToBullet(double x, double y, double z)
{
    return btVector3(x,y,z);   //TODO: make the actual transform
}

/**
  Holds together a bullet rigid body and its collision shape.
 */

typedef struct
{
    std::shared_ptr<btRigidBody> mBody;
    std::shared_ptr<btCollisionShape> mShape;
} FullRigidBody;

};

#endif
