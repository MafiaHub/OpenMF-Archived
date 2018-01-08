#ifndef TREE_KLZ_BULLET_LOADER_H
#define TREE_KLZ_BULLET_LOADER_H

#include <vector>
#include <fstream>
#include <BulletCollision/CollisionShapes/btCollisionShape.h>

namespace MFPhysics
{

class BulletTreeKlzLoader
{
public:
    typedef struct
    {
        std::string mName;
        btCollisionShape *mShape;
    } LinkedShape;

    typedef std::vector<std::shared_ptr<BulletTreeKlzLoader::LinkedShape>> BulletShapes;

    BulletShapes load(std::ifstream &srcFile);
};

BulletTreeKlzLoader::BulletShapes BulletTreeKlzLoader::load(std::ifstream &srcFile)
{
    BulletTreeKlzLoader::BulletShapes result;
    return result;
}

};

#endif
