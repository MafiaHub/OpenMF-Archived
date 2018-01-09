#ifndef SPATIAL_ENTITY_IMPLEMENTATION_H
#define SPATIAL_ENTITY_IMPLEMENTATION_H

#include <spatial_entity/spatial_entity.hpp>
#include <osg/Node>
#include <BulletDynamics/Dynamics/btRigidBody.h>
#include <loggers/console.hpp>

#define SPATIAL_ENTITY_IMPLEMENTATION_STR "spatial entity"

namespace MFGame
{

class SpatialEntityImplementation: public SpatialEntity
{
public:
    SpatialEntityImplementation();
    virtual void update(double dt) override;
    virtual void ready() override;
    virtual void move(Vec3 destPosition) override;
    virtual std::string toString() override;

    void setOSGNode(osg::Node *node)       { mOSGNode = node;       };
    void setBulletBody(btRigidBody *body)  { mBulletBody = body;    };
    osg::Node *getOSGNode()                { return mOSGNode.get(); };
    btRigidBody *getBulletBody()           { return mBulletBody;    };

protected:
    osg::ref_ptr<osg::Node> mOSGNode;
    btRigidBody *mBulletBody;
};

SpatialEntityImplementation::SpatialEntityImplementation()
{
    mOSGNode = 0;
    mBulletBody = 0;
}

void SpatialEntityImplementation::update(double dt)
{
}

std::string SpatialEntityImplementation::toString()
{
    return "\"" + mName + "\"";
}

void SpatialEntityImplementation::ready()
{
    MFLogger::ConsoleLogger::info("readying entity: " + toString(),SPATIAL_ENTITY_IMPLEMENTATION_STR);
    mReady = true;
}

void SpatialEntityImplementation::move(Vec3 destPosition)
{
}

};

#endif
