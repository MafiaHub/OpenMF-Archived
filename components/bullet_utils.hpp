#ifndef BULLET_UTILS_H
#define BULLET_UTILS_H

#include <string>
#include <id_manager.hpp>
#include <BulletDynamics/Dynamics/btRigidBody.h>
#include <BulletCollision/CollisionShapes/btTriangleMesh.h>
#include <btBulletDynamicsCommon.h>

namespace MFUtil
{

inline btVector3 mafiaVec3ToBullet(double x, double y, double z)
{
    return btVector3(x,z,y); 
}

inline void setRigidBodyEntityId(btRigidBody *body, MFGame::Id ident)
{
    if (body == nullptr) 
        return;

    body->setUserIndex(ident.Index);
    body->setUserIndex2(ident.Generation);
}

inline MFGame::Id getCollisionObjectEntityId(const btCollisionObject *body)
{
    if (body == nullptr)
        return MFGame::NullId;

    MFGame::Id ident;
    ident.Index = body->getUserIndex();
    ident.Generation = body->getUserIndex2();
    return ident;
}

btTransform mafiaMat4ToBullet(MFMath::Mat4 m)
{
    MFMath::Vec3 trans = m.getTranslation();
    m.separateRotation();
        
    btMatrix3x3 rotMat;
    rotMat.setValue(m[0][0],m[0][1],m[0][2],m[1][0],m[1][1],m[1][2],m[2][0],m[2][1],m[2][2]);

    btTransform result(rotMat,mafiaVec3ToBullet(trans.x,trans.y,trans.z));
   
    btQuaternion q = result.getRotation();
    result.setRotation(btQuaternion(q.x(),q.z(),q.y(),q.w()));   /* TODO: find out why Y and Z have to be switched here */
    
    // TODO: scale

    return result;
}

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
