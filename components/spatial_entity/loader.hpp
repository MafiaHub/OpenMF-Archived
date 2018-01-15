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
        mModelName = "";

        for (int i = 0; i < (int) treeKlzBodies->size(); ++i)
            mPhysicsNodesMatched.push_back(false);
    }

    virtual void apply(osg::Node &n) override
    {
        MFUtil::traverse(this,n);
    }

    virtual void apply(osg::MatrixTransform &n) override
    {
        std::string modelName = "";

        if (n.getUserDataContainer())
        {
            std::vector<std::string> descriptions = n.getUserDataContainer()->getDescriptions();

            if (descriptions.size() > 0)
            {
                if (descriptions[0].compare("scene2.bin model") == 0)
                {
                    modelName = n.getName();
                }
                else if (descriptions[0].compare("4ds mesh") == 0)
                {
                    std::shared_ptr<MFGame::SpatialEntity> newEntity = std::make_shared<MFGame::SpatialEntityImplementation>();
                    newEntity->setName(n.getName());
                    ((MFGame::SpatialEntityImplementation *) newEntity.get())->setOSGRootNode(mOSGRoot);
                    ((MFGame::SpatialEntityImplementation *) newEntity.get())->setOSGNode(&n);

                    // find the corresponding collision:
    
                    // TODO: do this much faster
              
                    bool found = false;

                    for (int i = 0; i < (int) mTreeKlzBodies->size(); ++i)      // FIXME: ugly and slow?
                    {
                        if (namesMatch(n.getName(),(*mTreeKlzBodies)[i].mName,mModelName))
                        {
                            ((MFGame::SpatialEntityImplementation *) newEntity.get())->setBulletBody((*mTreeKlzBodies)[i].mRigidBody.mBody.get());
                            found = true;
                            mPhysicsNodesMatched[i] = true;
                            // TODO: can there be other than 1:1 assoctiations, i.e. can we break/delete here?
                            mTreeKlzBodies->erase(mTreeKlzBodies->begin() + i);
                            break;
                        }
                    }

                    if (!found)
                        MFLogger::ConsoleLogger::warn("Could not find matching collision for visual node \"" + newEntity->getName() + "\".",SPATIAL_ENTITY_LOADER_MODULE_STR);

                    newEntity->ready();

                    std::shared_ptr<MFGame::SpatialEntity> newPtr = newEntity;
                    mEntities.push_back(newPtr);
                }
            }
        }

        if (modelName.size() > 0)
            mModelName = modelName;   // traverse downwards with given name prefix

        MFUtil::traverse(this,n);

        if (modelName.size() > 0)
            mModelName = "";                 // going back up => clear the name prefix
    }

    SpatialEntityLoader::SpatialEntityList mEntities;
    std::vector<bool> mPhysicsNodesMatched;

protected:
    std::vector<MFUtil::NamedRigidBody> *mTreeKlzBodies;
    osg::Group *mOSGRoot;
    std::string mModelName;      // when traversin into a model loaded from scene2.bin, this will contain the model name (needed as the name prefix)

    bool namesMatch(std::string nameVisual, std::string nameCol, std::string nameModel)
    {
        if (nameVisual.compare(nameCol) == 0)
            return true;

        if (nameVisual.compare(nameModel + "." + nameCol) == 0)
            return true;

        if (nameCol.compare(nameModel + "." + nameVisual) == 0)
            return true;

        return false;
    }
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
        if (v.mPhysicsNodesMatched[i])
            continue;

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
