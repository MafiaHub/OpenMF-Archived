#ifndef TREE_KLZ_BULLET_LOADER_H
#define TREE_KLZ_BULLET_LOADER_H

#include <vector>
#include <fstream>
#include <utils/bullet.hpp>
#include <BulletCollision/CollisionShapes/btCollisionShape.h>
#include <BulletCollision/CollisionShapes/btBoxShape.h>
#include <BulletCollision/CollisionShapes/btSphereShape.h>
#include <BulletCollision/CollisionShapes/btCylinderShape.h>
#include <BulletCollision/CollisionShapes/btBvhTriangleMeshShape.h>
#include <BulletDynamics/Dynamics/btRigidBody.h>
#include <formats/klz/parser.hpp>
#include <formats/4ds/parser.hpp>    // needed for face collisions

#define TREE_KLZ_BULLET_LOADER_MODULE_STR "loader tree klz"

namespace MFPhysics
{

class BulletTreeKlzLoader
{
public:
    void load(std::ifstream &srcFile, MFFormat::DataFormat4DS &scene4ds);

    std::vector<MFUtil::NamedRigidBody> mRigidBodies;

protected:
    typedef struct
    {
        unsigned int mI1;
        unsigned int mI2;
        unsigned int mI3;
    } FaceIndices;

    typedef struct
    {
        std::string mMeshName;
        std::vector<FaceIndices> mFaces;
    } MeshFaceCollision;

    std::vector<MeshFaceCollision> mFaceCollisions;
};

}

#endif
