#ifndef SPATIAL_ENTITY_LOADER_H
#define SPATIAL_ENTITY_LOADER_H

#include <iostream>
#include <vector>
#include <string>
#include <spatial_entity/spatial_entity_implementation.hpp>
#include <osg/Node>
#include <bullet_utils.hpp>

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
std::cout << n.getName() << std::endl;
                mEntities.push_back(newEntity);
            }
        }

        // find the corresponding collision:

        if (mEntities.size() > 0)
            for (int i = 0; i < mTreeKlzBodies->size(); ++i)    // FIXME: ugly and slow?
            {

    if ( (*mTreeKlzBodies)[i].mName.compare(mEntities.back().getName()) == 0)
        std::cout << "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAsasas" << std::endl;

            }

        if (n.asGroup())         // FIXME: why does traverse() not work?
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

    return result;
}

};

#endif
