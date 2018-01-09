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

class SpatialEntityLoaderImplementation
{
public:
    typedef std::vector<MFGame::SpatialEntityImplementation> SpatialEntityList;
    /* FIXME: SpatialEntityList should be a list of SpatialEntity, but can't because
       it's abstract => find a way to do this */

    SpatialEntityList loadFromScene(osg::Group *sceneRoot, std::vector<MFUtil::NamedRigidBody> treeKlzBodies);
    // void loadModel();
};

class CreateEntitiesFromSceneVisitor: public osg::NodeVisitor
{
public:
    CreateEntitiesFromSceneVisitor(std::vector<MFUtil::NamedRigidBody> *treeKlzBodies): osg::NodeVisitor()
    {
        mTreeKlzBodies = treeKlzBodies;
    }

    virtual void apply(osg::Node &n) override
    {
        if (n.getUserDataContainer())
        {
            std::vector<std::string> descriptions = n.getUserDataContainer()->getDescriptions();

            if (descriptions.size() > 0 && descriptions[0].compare("4ds mesh") == 0)
            {
                MFGame::SpatialEntityImplementation newEntity;
                newEntity.setName(n.getName());

                newEntity.setOSGNode(&n);

                // find the corresponding collision:
               
                for (int i = 0; i < mTreeKlzBodies->size(); ++i)    // FIXME: ugly and slow?
                { 
                    if ((*mTreeKlzBodies)[i].mName.compare(newEntity.getName()) == 0)
                    {
                        MFLogger::ConsoleLogger::info("Pairing OSG node to Bullet body: " + newEntity.getName() + ".",SPATIAL_ENTITY_LOADER_MODULE_STR);
                        newEntity.setBulletBody((*mTreeKlzBodies)[i].mRigidBody.mBody.get());
                        break;    // TODO: can a node has multiple collisions/the other way around?
                    }
                }

                mEntities.push_back(newEntity);
            }
        }

        if (n.asGroup())          // FIXME: why does traverse() not work?
            for (int i = 0; i < n.asGroup()->getNumChildren(); ++i)
                n.asGroup()->getChild(i)->accept(*this);
    }

    SpatialEntityLoaderImplementation::SpatialEntityList mEntities;

protected:
    std::vector<MFUtil::NamedRigidBody> *mTreeKlzBodies;
};

SpatialEntityLoaderImplementation::SpatialEntityList SpatialEntityLoaderImplementation::loadFromScene(osg::Group *sceneRoot, std::vector<MFUtil::NamedRigidBody> treeKlzBodies)
{
    SpatialEntityLoaderImplementation::SpatialEntityList result;
    CreateEntitiesFromSceneVisitor v(&treeKlzBodies);
    sceneRoot->accept(v);

    // TODO: make entities for remaining collisions for which graphical node hasn't been found

    return result;
}

};

#endif
