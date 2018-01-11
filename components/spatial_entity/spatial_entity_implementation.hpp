#ifndef SPATIAL_ENTITY_IMPLEMENTATION_H
#define SPATIAL_ENTITY_IMPLEMENTATION_H

#include <spatial_entity/spatial_entity.hpp>
#include <osg/Node>
#include <osg/PolygonMode>
#include <BulletDynamics/Dynamics/btRigidBody.h>
#include <osg_masks.hpp>

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
    void setOSGRootNode(osg::Group *root)     { mOSGRoot = root;       };

protected:
    void makePhysicsDebugOSGNode();        ///< Creates a visual representation of the physical representation.

    osg::ref_ptr<osg::MatrixTransform> mOSGNode;
    btRigidBody *mBulletBody;

    osg::Group *mOSGRoot;

    osg::ref_ptr<osg::MatrixTransform> mOSGPgysicsDebugNode;

    osg::Matrixd mOSGInitialTransform;     ///< Captures the OSG node transform when ready() is called.
    btTransform mBulletInitialTransform;   ///< Captures the Bullet body transform when ready() is called.
};

SpatialEntityImplementation::SpatialEntityImplementation(): SpatialEntity()
{
    mOSGNode = 0;
    mBulletBody = 0;
    mOSGRoot = 0;
}

void SpatialEntityImplementation::update(double dt)
{
}

std::string SpatialEntityImplementation::toString()
{
    int hasOSG = mOSGNode != 0;
    int hasBullet = mBulletBody != 0;

    return "\"" + mName + "\", ID: " + std::to_string(mID) + ", representations: " + std::to_string(hasOSG) + std::to_string(hasBullet) + ", pos: " + mPosition.str();
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

        mBulletBody->setUserIndex(mID);

        makePhysicsDebugOSGNode();
    }

    MFLogger::ConsoleLogger::info("readying entity: " + toString(),SPATIAL_ENTITY_IMPLEMENTATION_STR);
    mReady = true;
}

void SpatialEntityImplementation::makePhysicsDebugOSGNode()        ///< Creates a visual representation of the physical representation.
{
    if (!mBulletBody)
        return;

    if (!mOSGRoot)
    {
        MFLogger::ConsoleLogger::warn("Cannot create debug OSG node for \"" + mName + "\": no access to scene root.",SPATIAL_ENTITY_IMPLEMENTATION_STR);
        return;
    }

    int shapeType = mBulletBody->getCollisionShape()->getShapeType();

    osg::ref_ptr<osg::Node> shapeNode;

    switch (shapeType)
    {
        case BroadphaseNativeTypes::BOX_SHAPE_PROXYTYPE:
        {
            btVector3 boxExtents = static_cast<btBoxShape *>(mBulletBody->getCollisionShape())->getHalfExtentsWithoutMargin();

            osg::ref_ptr<osg::Shape> shape = new osg::Box(osg::Vec3f(0,0,0),
                2 * std::abs(boxExtents.x()),
                2 * std::abs(boxExtents.y()),
                2 * std::abs(boxExtents.z()));

            shapeNode = new osg::ShapeDrawable(shape.get());
            break;
        }

        case BroadphaseNativeTypes::SPHERE_SHAPE_PROXYTYPE:
        {
            osg::ref_ptr<osg::Shape> shape = new osg::Sphere(osg::Vec3f(0,0,0),
                static_cast<btSphereShape *>(mBulletBody->getCollisionShape())->getRadius());

            shapeNode = new osg::ShapeDrawable(shape.get());
            break;
        }

        case BroadphaseNativeTypes::CYLINDER_SHAPE_PROXYTYPE:
        {
            btCylinderShape *cylinder = static_cast<btCylinderShape *>(mBulletBody->getCollisionShape());
            osg::ref_ptr<osg::Shape> shape = new osg::Cylinder(osg::Vec3f(0,0,0),cylinder->getRadius(),cylinder->getHalfExtentsWithoutMargin().z() * 2);
            shapeNode = new osg::ShapeDrawable(shape.get()); // toBulletNode;
            break;
        }

        default:
            MFLogger::ConsoleLogger::warn("Unknown shape type for \"" + mName + "\": " + std::to_string(shapeType) + ".",SPATIAL_ENTITY_IMPLEMENTATION_STR);
            break;
    }

    if (shapeNode)
    {
        shapeNode->setName("collision: " + getName());
        shapeNode->setNodeMask(MFRender::MASK_DEBUG);

        // FIXME: make the StateSet static and shared
        osg::ref_ptr<osg::Material> mat = new osg::Material();
        osg::ref_ptr<osg::PolygonMode> mode = new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK,osg::PolygonMode::LINE);
        osg::Vec4f debugColor = osg::Vec4f(0,1,0,1);

        mat->setAmbient(osg::Material::FRONT_AND_BACK,debugColor);
        mat->setDiffuse(osg::Material::FRONT_AND_BACK,debugColor);
        mat->setSpecular(osg::Material::FRONT_AND_BACK,debugColor);
        mat->setEmission(osg::Material::FRONT_AND_BACK,debugColor);

        shapeNode->getOrCreateStateSet()->setAttributeAndModes(mat);
        shapeNode->getOrCreateStateSet()->setMode(osg::StateAttribute::FOG,osg::StateAttribute::OFF);   // FIXME: why no effect?
        shapeNode->getOrCreateStateSet()->setAttributeAndModes(mode);

        mOSGPgysicsDebugNode = new osg::MatrixTransform();
        mOSGPgysicsDebugNode->addChild(shapeNode);

        btTransform transform = mBulletBody->getWorldTransform();
        btVector3 position = transform.getOrigin();
        btQuaternion rotation = transform.getRotation();

        osg::Matrixd transformMat;
        
        transformMat.makeTranslate(osg::Vec3f(position.x(),position.y(),position.z()));
        transformMat.preMult(osg::Matrixd::rotate(osg::Quat(rotation.x(),rotation.y(),rotation.z(),rotation.w())));

        mOSGPgysicsDebugNode->setMatrix(transformMat);

        mOSGRoot->addChild(mOSGPgysicsDebugNode);
    }
}

void SpatialEntityImplementation::move(Vec3 destPosition)
{
}

};

#endif
