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

    void setOSGNode(osg::MatrixTransform *t)  { mOSGNode = t;          };
    void setBulletBody(btRigidBody *body)     { mBulletBody = body;    };
    osg::MatrixTransform *getOSGNode()        { return mOSGNode.get(); };
    btRigidBody *getBulletBody()              { return mBulletBody;    };

protected:
    osg::ref_ptr<osg::MatrixTransform> mOSGNode;
    btRigidBody *mBulletBody;

    osg::Matrixd mOSGInitialTransform;     ///< captures the OSG node transform when ready() is called
    btTransform mBulletInitialTransform;   ///< captures the Bullet body transform when ready() is called
};

SpatialEntityImplementation::SpatialEntityImplementation(): SpatialEntity()
{
    mOSGNode = 0;
    mBulletBody = 0;
}

void SpatialEntityImplementation::update(double dt)
{
}

std::string SpatialEntityImplementation::toString()
{
    int hasOSG = mOSGNode != 0;
    int hasBullet = mBulletBody != 0;

    return "\"" + mName + "\", representations: " + std::to_string(hasOSG) + std::to_string(hasBullet) + " pos: " + mPosition.str();
}

void SpatialEntityImplementation::ready()
{
    if (mOSGNode)
    {
        mOSGInitialTransform = mOSGNode->getMatrix();

        if (!mBulletBody)
        {
            osg::Vec3d pos = mOSGInitialTransform.getTrans();
            mPosition.x = pos.x();
            mPosition.y = pos.y();
            mPosition.z = pos.z();
        }
    }

    if (mBulletBody)
    {
        mBulletInitialTransform = mBulletBody->getWorldTransform();

        btVector3 pos = mBulletInitialTransform.getOrigin();
        mPosition.x = pos.x();
        mPosition.y = pos.y();
        mPosition.z = pos.z();
    }

    MFLogger::ConsoleLogger::info("readying entity: " + toString(),SPATIAL_ENTITY_IMPLEMENTATION_STR);
    mReady = true;
}

void SpatialEntityImplementation::move(Vec3 destPosition)
{
}

};

#endif
