#ifndef SPATIAL_ENTITY_IMPLEMENTATION_H
#define SPATIAL_ENTITY_IMPLEMENTATION_H

#include <spatial_entity/spatial_entity.hpp>
#include <osg/Node>
#include <BulletDynamics/Dynamics/btRigidBody.h>

namespace MFGame
{

class SpatialEntityImplementation: public SpatialEntity
{
public:
    virtual void update(double dt) override;
    virtual void move(Vec3 destPosition) override;

    void setOSGNode(osg::Node *node)       { mOSGNode = node;       };
    void setBulletBody(btRigidBody *body)  { mBulletBody = body;    };
    osg::Node *getOSGNode()                { return mOSGNode.get(); };
    btRigidBody *getBulletBody()           { return mBulletBody;    };

protected:
    osg::ref_ptr<osg::Node> mOSGNode;
    btRigidBody *mBulletBody;
};

void SpatialEntityImplementation::update(double dt)
{
}

void SpatialEntityImplementation::move(Vec3 destPosition)
{
}

};

#endif
