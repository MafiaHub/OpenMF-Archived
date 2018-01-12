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
    typedef std::vector<MFGame::SpatialEntityImplementation> SpatialEntityList;
    /* FIXME: SpatialEntityList should be a list of SpatialEntity, but can't because
       it's abstract => find a way to do this */

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
        if (n.asGroup())          // FIXME: why does traverse() not work?
            for (int i = 0; i < (int) n.asGroup()->getNumChildren(); ++i)
                n.asGroup()->getChild(i)->accept(*this);
    }

    virtual void apply(osg::MatrixTransform &n) override
    {
        if (n.getUserDataContainer())
        {
            std::vector<std::string> descriptions = n.getUserDataContainer()->getDescriptions();

            if (descriptions.size() > 0 && descriptions[0].compare("4ds mesh") == 0)
            {
                MFGame::SpatialEntityImplementation newEntity;
                newEntity.setName(n.getName());
                newEntity.setOSGRootNode(mOSGRoot);

                newEntity.setOSGNode(&n);

                // find the corresponding collision:
               
                for (int i = 0; i < (int) mTreeKlzBodies->size(); ++i)    // FIXME: ugly and slow?
                { 
                    if ((*mTreeKlzBodies)[i].mName.compare(newEntity.getName()) == 0)
                    {
                        newEntity.setBulletBody((*mTreeKlzBodies)[i].mRigidBody.mBody.get());
                        mTreeKlzBodies->erase(mTreeKlzBodies->begin() + i);
                        break;    // TODO: can a node has multiple collisions/the other way around?
                    }
                }
 
                newEntity.ready();
                mEntities.push_back(newEntity);
            }
        }

        if (n.asGroup())          // FIXME: why does traverse() not work?
            for (int i = 0; i < (int) n.asGroup()->getNumChildren(); ++i)
                n.asGroup()->getChild(i)->accept(*this);
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
        MFGame::SpatialEntityImplementation newEntity;
        newEntity.setName(treeKlzBodies[i].mName);
        newEntity.setOSGRootNode(osgRoot);
        newEntity.setBulletBody(treeKlzBodies[i].mRigidBody.mBody.get());
        newEntity.ready();
        result.push_back(newEntity);
    }

    // TODO: make entities for remaining collisions for which graphical node hasn't been found

    return result;
}

}

#endif
