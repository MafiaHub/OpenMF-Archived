#ifndef SPATIAL_ENTITY_IMPLEMENTATION_H
#define SPATIAL_ENTITY_IMPLEMENTATION_H

#include <spatial_entity/spatial_entity.hpp>
#include <osg/Node>
#include <osg/PolygonMode>
#include <BulletDynamics/Dynamics/btRigidBody.h>
#include <renderer/osg_masks.hpp>
#include <osg/MatrixTransform>
#include <btBulletDynamicsCommon.h>
#include <osg/Material>
#include <utils/osg.hpp>
#include <osg/ShapeDrawable>
#include <osg/Geometry>
#include <osg/Geode>

#define SPATIAL_ENTITY_IMPLEMENTATION_MODULE_STR "spatial entity"

namespace MFGame
{

class SpatialEntityImplementation: public SpatialEntity
{
public:
    SpatialEntityImplementation();
    ~SpatialEntityImplementation();
    virtual void update(double dt) override;
    virtual void ready() override;
    virtual std::string toString() override;
    virtual void setPosition(MFMath::Vec3 position) override;
    virtual void setVelocity(MFMath::Vec3 velocity) override;
    virtual MFMath::Vec3 getVelocity() override;
    virtual void setAngularVelocity(MFMath::Vec3 velocity) override;
    virtual void setDamping(float lin, float ang) override;
    virtual MFMath::Vec3 getAngularVelocity() override;
    virtual MFMath::Vec2 getDamping() override;
    virtual bool hasVisual() override;
    virtual bool hasPhysics() override;
    virtual bool hasCollision() override;
    virtual bool canBeMoved() override;
    virtual void setPhysicsBehavior(SpatialEntity::PhysicsBehavior behavior) override;
    virtual void setFriction(double factor) override;
    virtual MFMath::Vec3 getSize() override;

    void setVisualNode(osg::MatrixTransform *t)                                    { mOSGNode = t;                     };
    void setPhysicsBody(std::shared_ptr<btRigidBody> body)                         { mBulletBody = body;               };
    osg::MatrixTransform *getVisualNode()                                          { return mOSGNode.get();            };
    std::shared_ptr<btRigidBody> getPhysicsBody()                                  { return mBulletBody;               };
    void setOSGRootNode(osg::Group *root)                                          { mOSGRoot = root;                  };
    void setPhysicsMotionState(std::shared_ptr<btDefaultMotionState> motionState)  { mBulletMotionState = motionState; };
    std::shared_ptr<btDefaultMotionState> getPhysicsMotionState()                  { return mBulletMotionState;        };
    void setDebugMode(bool enable)                                                 { mCreateDebugGeometry = enable;    };

    static osg::ref_ptr<osg::StateSet> sDebugStateSet;

protected:
    void makePhysicsDebugOSGNode();        ///< Creates a visual representation of the physical representation.

    void computeCurrentTransform();        ///< Sets the values of position, rotation and scale of the entity from the current state of the managed nodes.
    void applyCurrentTransform();          ///< Sets the states of managed nodes from current values of position, rotation and scale.
    void syncDebugPhysicsNode();

    MFMath::Vec3 mInitialPosition;
    MFMath::Vec3 mInitialScale;
    MFMath::Quat mInitialRotation;

    osg::ref_ptr<osg::MatrixTransform> mOSGNode;
    std::shared_ptr<btRigidBody> mBulletBody;
    std::shared_ptr<btDefaultMotionState> mBulletMotionState;

    osg::Group *mOSGRoot;

    osg::ref_ptr<osg::MatrixTransform> mOSGPhysicsDebugNode;
    osg::Matrixd mOSGInitialTransform;     ///< Captures the OSG node transform when ready() is called.
    btTransform mBulletInitialTransform;   ///< Captures the Bullet body transform when ready() is called.

    bool mCreateDebugGeometry;
};

}

#endif
