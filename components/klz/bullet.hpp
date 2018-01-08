#ifndef TREE_KLZ_BULLET_LOADER_H
#define TREE_KLZ_BULLET_LOADER_H

#include <vector>
#include <fstream>
#include <BulletCollision/CollisionShapes/btCollisionShape.h>
#include <BulletCollision/CollisionShapes/btBoxShape.h>
#include <klz/parser.hpp>

namespace MFPhysics
{

class BulletTreeKlzLoader
{
public:
    typedef struct
    {
        std::string mName;
        std::shared_ptr<btCollisionShape> mShape;
    } LinkedShape;

    std::vector<LinkedShape> load(std::ifstream &srcFile);
};

std::vector<BulletTreeKlzLoader::LinkedShape> BulletTreeKlzLoader::load(std::ifstream &srcFile)
{
    std::vector<BulletTreeKlzLoader::LinkedShape> result;

    MFFormat::DataFormatTreeKLZ klz;

    klz.load(srcFile);

    auto colsAABB = klz.getAABBCols();

    for (int i = 0; i < colsAABB.size(); ++i)
    {
        BulletTreeKlzLoader::LinkedShape newShape;
        newShape.mShape = std::make_shared<btBoxShape>(btVector3(1,1,1));
        result.push_back(newShape);
    } 

    return result;
}

};

#endif
