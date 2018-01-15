#ifndef SPATIAL_ENTITY_LOADER_H
#define SPATIAL_ENTITY_LOADER_H

#include <iostream>
#include <vector>
#include <string>
#include <spatial_entity/spatial_entity_implementation.hpp>
#include <osg/Node>
#include <bullet_utils.hpp>
#include <loggers/console.hpp>

#define SPATIAL_ENTITY_LOADER_MODULE_STR "spatial entity loader"

namespace MFFormat
{

/**
  Handles creating spatial entities from different loaded files.
 */

class SpatialEntityLoader
{
public:
    typedef std::vector<std::shared_ptr<MFGame::SpatialEntity>> SpatialEntityList;
    SpatialEntityList loadFromScene(osg::Group *osgRoot, std::vector<MFUtil::NamedRigidBody> treeKlzBodies);
    // void loadModel();
};

class CreateEntitiesFromSceneVisitor: public osg::NodeVisitor
{
public:
    CreateEntitiesFromSceneVisitor(std::vector<MFUtil::NamedRigidBody> *treeKlzBodies, osg::Group *sceneRoot): osg::NodeVisitor()
    {
        mTreeKlzBodies = treeKlzBodies;
        mOSGRoot = sceneRoot;
    }

    virtual void apply(osg::Node &n) override
    {
        MFUtil::traverse(this,n);
    }

    virtual void apply(osg::MatrixTransform &n) override
    {
        if (n.getUserDataContainer())
        {
            std::vector<std::string> descriptions = n.getUserDataContainer()->getDescriptions();

            if (descriptions.size() > 0 && descriptions[0].compare("4ds mesh") == 0)
            {
                std::shared_ptr<MFGame::SpatialEntity> newEntity = std::make_shared<MFGame::SpatialEntityImplementation>();
                newEntity->setName(n.getName());
                ((MFGame::SpatialEntityImplementation *) newEntity.get())->setOSGRootNode(mOSGRoot);
                ((MFGame::SpatialEntityImplementation *) newEntity.get())->setOSGNode(&n);

                // find the corresponding collision:
               
                for (int i = 0; i < (int) mTreeKlzBodies->size(); ++i)      // FIXME: ugly and slow?
                { 
                    if ((*mTreeKlzBodies)[i].mName.compare(newEntity->getName()) == 0)
                    {
                        ((MFGame::SpatialEntityImplementation *) newEntity.get())->setBulletBody((*mTreeKlzBodies)[i].mRigidBody.mBody.get());
                        mTreeKlzBodies->erase(mTreeKlzBodies->begin() + i);
                        break;    // TODO: can a node have multiple collisions/the other way around?
                    }
                }
 
                newEntity->ready();

                std::shared_ptr<MFGame::SpatialEntity> newPtr = newEntity;
                mEntities.push_back(newPtr);
            }
        }

        MFUtil::traverse(this,n);
    }

    SpatialEntityLoader::SpatialEntityList mEntities;

protected:
    std::vector<MFUtil::NamedRigidBody> *mTreeKlzBodies;
    osg::Group *mOSGRoot;
};

SpatialEntityLoader::SpatialEntityList SpatialEntityLoader::loadFromScene(osg::Group *osgRoot, std::vector<MFUtil::NamedRigidBody> treeKlzBodies)
{
    SpatialEntityLoader::SpatialEntityList result;
    CreateEntitiesFromSceneVisitor v(&treeKlzBodies,osgRoot);
    osgRoot->accept(v);

    result = v.mEntities;

    // process the unmatched rigid bodies:

    for (int i = 0; i < (int) treeKlzBodies.size(); ++i)
    {
        std::shared_ptr<MFGame::SpatialEntity> newEntity = std::make_shared<MFGame::SpatialEntityImplementation>();
        newEntity->setName(treeKlzBodies[i].mName);
        ((MFGame::SpatialEntityImplementation *) newEntity.get())->setOSGRootNode(osgRoot);
        ((MFGame::SpatialEntityImplementation *) newEntity.get())->setBulletBody(treeKlzBodies[i].mRigidBody.mBody.get());
        newEntity->ready();
        result.push_back(newEntity);
    }

    return result;
}

}

#endif
