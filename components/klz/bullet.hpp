#ifndef TREE_KLZ_BULLET_LOADER_H
#define TREE_KLZ_BULLET_LOADER_H

#include <vector>
#include <fstream>
#include <bullet_utils.hpp>
#include <BulletCollision/CollisionShapes/btCollisionShape.h>
#include <BulletCollision/CollisionShapes/btBoxShape.h>
#include <BulletDynamics/Dynamics/btRigidBody.h>
#include <klz/parser.hpp>

namespace MFPhysics
{

class BulletTreeKlzLoader
{
public:
    typedef struct
    {
        std::string mName;
        MFUtil::FullRigidBody mRigidBody;
    } LinkedCollisionBody;

    std::vector<LinkedCollisionBody> load(std::ifstream &srcFile);
};

std::vector<BulletTreeKlzLoader::LinkedCollisionBody> BulletTreeKlzLoader::load(std::ifstream &srcFile)
{
    std::vector<BulletTreeKlzLoader::LinkedCollisionBody> result;
    MFFormat::DataFormatTreeKLZ klz;
    klz.load(srcFile);
    auto colsAABB = klz.getAABBCols();

    for (int i = 0; i < colsAABB.size(); ++i)
    {
        btVector3 p1 = MFUtil::mafiaVec3ToBullet(colsAABB[i].mMin.x,colsAABB[i].mMin.y,colsAABB[i].mMin.z);
        btVector3 p2 = MFUtil::mafiaVec3ToBullet(colsAABB[i].mMax.x,colsAABB[i].mMax.y,colsAABB[i].mMax.z);

        btVector3 center = (p1 + p2) / 2.0f;
        btVector3 bboxCorner = p2 - center;

        BulletTreeKlzLoader::LinkedCollisionBody newLinkedBody;
        newLinkedBody.mRigidBody.mShape = std::make_shared<btBoxShape>(bboxCorner);
        btRigidBody::btRigidBodyConstructionInfo ci(0,0,newLinkedBody.mRigidBody.mShape.get());
        newLinkedBody.mRigidBody.mBody = std::make_shared<btRigidBody>(ci);
        newLinkedBody.mRigidBody.mBody->translate(center);

        result.push_back(newLinkedBody);
    } 

    return result;
}

};

#endif
