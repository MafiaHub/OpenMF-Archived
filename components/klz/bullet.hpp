#ifndef TREE_KLZ_BULLET_LOADER_H
#define TREE_KLZ_BULLET_LOADER_H

#include <vector>
#include <fstream>
#include <BulletCollision/CollisionShapes/btCollisionShape.h>

namespace MFCollision
{

class BulletTreeKlzLoader
{
public:
    typedef struct
    {
        std::string mName;
        btCollisionShape *mShape;
    } LinkedShape;

    std::vector<BulletTreeKlzLoader::LinkedShape> load(std::ifstream &srcFile);
};

std::vector<BulletTreeKlzLoader::LinkedShape> BulletTreeKlzLoader::load(std::ifstream &srcFile)
{
    std::vector<LinkedShape> result;
    return result;
}

};

#endif
