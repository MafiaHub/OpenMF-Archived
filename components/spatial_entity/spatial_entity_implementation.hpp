#ifndef SPATIAL_ENTITY_IMPLEMENTATION_H
#define SPATIAL_ENTITY_IMPLEMENTATION_H

#include <spatial_entity/spatial_entity.hpp>
#include <osg/Node>
#include <osg/PolygonMode>
#include <BulletDynamics/Dynamics/btRigidBody.h>
#include <osg_masks.hpp>

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
    virtual void setAngularVelocity(MFMath::Vec3 velocity) override;
    virtual void setDamping(float lin, float ang) override;
    virtual MFMath::Vec3 getVelocity() override;
    virtual MFMath::Vec3 getAngularVelocity() override;
    virtual MFMath::Vec2 getDamping() override;
    virtual bool hasVisual() override;
    virtual bool hasCollision() override;
    virtual bool canBeMoved() override;

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

osg::ref_ptr<osg::StateSet> SpatialEntityImplementation::sDebugStateSet = 0;

bool SpatialEntityImplementation::canBeMoved()
{
    if (!mBulletBody)    // no physical representation => nothing stops the entity from moving
        return true;

    // TODO

    return false;
}

bool SpatialEntityImplementation::hasVisual()
{
    return mOSGNode != 0;
}

bool SpatialEntityImplementation::hasCollision()
{
    return mBulletBody != 0;
}

void SpatialEntityImplementation::setVelocity(MFMath::Vec3 velocity)
{
    if (!mBulletBody)
        return;

    mBulletBody->setLinearVelocity(btVector3(velocity.x,velocity.y,velocity.z));
}

void SpatialEntityImplementation::setAngularVelocity(MFMath::Vec3 velocity)
{
    if (!mBulletBody)
        return;

    mBulletBody->setAngularVelocity(btVector3(velocity.x,velocity.y,velocity.z));
}

void SpatialEntityImplementation::setDamping(float lin, float ang)
{
    if (!mBulletBody)
        return;

    mBulletBody->setDamping(lin, ang);
}

MFMath::Vec3 SpatialEntityImplementation::getVelocity()
{
    if (!mBulletBody)
        return MFMath::Vec3(0,0,0);

    auto vel = mBulletBody->getLinearVelocity();
    return MFMath::Vec3(vel.x(), vel.y(), vel.z());
}

MFMath::Vec3 SpatialEntityImplementation::getAngularVelocity()
{
    if (!mBulletBody)
        return MFMath::Vec3(0, 0, 0);

    auto vel = mBulletBody->getAngularVelocity();
    return MFMath::Vec3(vel.x(), vel.y(), vel.z());
}

MFMath::Vec2 SpatialEntityImplementation::getDamping()
{
    if (!mBulletBody)
        return MFMath::Vec2(0,0);

    return MFMath::Vec2(mBulletBody->getLinearDamping(), mBulletBody->getAngularDamping());
}

void SpatialEntityImplementation::computeCurrentTransform()
{
    if (mBulletBody)
    {
        btTransform t = mBulletBody->getWorldTransform();
        btVector3 bPos = t.getOrigin();
        btQuaternion bRot = t.getRotation();

        mPosition = MFMath::Vec3(bPos.x(),bPos.y(),bPos.z());
        mRotation = MFMath::Quat(bRot.x(),bRot.y(),bRot.z(),bRot.w());
        mScale = MFMath::Vec3(1,1,1);
    }
    else if (mOSGNode)
    {
        osg::Vec3f oPos = mOSGNode->getMatrix().getTrans();
        osg::Quat oRot = mOSGNode->getMatrix().getRotate();
        osg::Vec3f oScale = mOSGNode->getMatrix().getScale();

        mPosition = MFMath::Vec3(oPos.x(),oPos.y(),oPos.z());
        mRotation = MFMath::Quat(oRot.x(),oRot.y(),oRot.z(),oRot.w());
        mScale = MFMath::Vec3(oScale.x(),oScale.y(),oScale.z());
    }
    else
    {
        mPosition = MFMath::Vec3(0,0,0);
        mRotation = MFMath::Quat(0,0,0,1);
        mScale = MFMath::Vec3(1,1,1);
    }
}

void SpatialEntityImplementation::applyCurrentTransform()
{
    MFMath::Vec3 relPos = mPosition - mInitialPosition;

    /* TODO: if a visual node has parents, its world transform will
       be affected by it and will possibly be desynced with the collision
       node as a result - we should resolve this somehow (disallow movement,
       forcefully set the same transform,  don't care?) */

    if (mBulletBody)
    {
        btTransform t = mBulletBody->getWorldTransform();

        t.setOrigin(mBulletInitialTransform.getOrigin() + btVector3(relPos.x,relPos.y,relPos.z));
//        t.setRotation(mBulletInitialTransform.getRotation() * btQuaternion(mRotation.x,mRotation.y,mRotation.z,mRotation.w));

        mBulletBody->setWorldTransform(t);
        syncDebugPhysicsNode();
    } 

    if (mOSGNode)
    {
        osg::Matrixd m = mOSGInitialTransform;

        m.preMultRotate(osg::Quat(mRotation.x,mRotation.y,mRotation.z,mRotation.w));
        m.setTrans(mOSGInitialTransform.getTrans() + osg::Vec3f(relPos.x,relPos.y,relPos.z));

        // TODO: scale

        mOSGNode->setMatrix(m);
    }
}

void SpatialEntityImplementation::setPosition(MFMath::Vec3 position)
{
    SpatialEntity::setPosition(position);
    applyCurrentTransform();
}

SpatialEntityImplementation::SpatialEntityImplementation(): SpatialEntity()
{
    if (!SpatialEntityImplementation::sDebugStateSet)
    {
        // let the first entity create the shared debug material
        SpatialEntityImplementation::sDebugStateSet = new osg::StateSet;

        osg::ref_ptr<osg::Material> mat = new osg::Material();
        osg::ref_ptr<osg::PolygonMode> mode = new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK,osg::PolygonMode::LINE);
        osg::Vec4f debugColor = osg::Vec4f(0,1,0,1);

        mat->setAmbient(osg::Material::FRONT_AND_BACK,debugColor);
        mat->setDiffuse(osg::Material::FRONT_AND_BACK,debugColor);
        mat->setSpecular(osg::Material::FRONT_AND_BACK,debugColor);
        mat->setEmission(osg::Material::FRONT_AND_BACK,debugColor);

        SpatialEntityImplementation::sDebugStateSet->setAttributeAndModes(mat);
        SpatialEntityImplementation::sDebugStateSet->setAttributeAndModes(mode);
    }

    mOSGNode = 0;
    mBulletBody = 0;
    mBulletMotionState = 0;
    mCreateDebugGeometry = false;
}

SpatialEntityImplementation::~SpatialEntityImplementation()
{ 
}

void SpatialEntityImplementation::update(double dt)
{
    // TODO: make use of MotionState to see if the transform has to be updated

    if (mBulletBody && mBulletMotionState)
    {
        computeCurrentTransform();
        applyCurrentTransform();
    }
}

std::string SpatialEntityImplementation::toString()
{
    int hasOSG = mOSGNode != 0;
    int hasBullet = mBulletBody != 0;

    return "\"" + mName + "\", ID: " + std::to_string(mId) + ", representations: " + std::to_string(hasOSG) + std::to_string(hasBullet) + ", pos: " + mPosition.str();
}

void SpatialEntityImplementation::ready()
{
    MFLogger::ConsoleLogger::info("readying entity: " + toString(),SPATIAL_ENTITY_IMPLEMENTATION_MODULE_STR);

    if (mOSGNode)
    {
        mOSGInitialTransform = mOSGNode->getMatrix();

        osg::ref_ptr<MFUtil::UserIntData> intData = new MFUtil::UserIntData(mId);
        MFUtil::AssignUserDataVisitor v(intData.get());
        mOSGNode->accept(v);
    }

    if (mBulletBody)
    {
        mBulletBody->setUserIndex(mId);

        mBulletInitialTransform = mBulletBody->getWorldTransform();
        makePhysicsDebugOSGNode();
    }

    computeCurrentTransform();

    mInitialPosition = mPosition;
    mInitialRotation = mRotation;
    mInitialScale = mScale;

    mReady = true;
}

void SpatialEntityImplementation::makePhysicsDebugOSGNode()        ///< Creates a visual representation of the physical representation.
{
    if (!mBulletBody || !mCreateDebugGeometry)
        return;

    if (!mOSGRoot)
    {
        MFLogger::ConsoleLogger::warn("Cannot create debug OSG node for \"" + mName + "\": no access to scene root.",SPATIAL_ENTITY_IMPLEMENTATION_MODULE_STR);
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
            shapeNode = new osg::ShapeDrawable(shape.get());
            break;
        }

        case BroadphaseNativeTypes::TRIANGLE_MESH_SHAPE_PROXYTYPE:
        {
            btTriangleMesh *mesh =
                static_cast<btTriangleMesh *>(
                    static_cast<btTriangleMeshShape *>(mBulletBody->getCollisionShape())->getMeshInterface());

            unsigned char *vertexData,*indexData;
            int numVertices, numFaces, vertexStride, indexStride;
            PHY_ScalarType vertexType, indexType;

            mesh->getLockedVertexIndexBase(&vertexData,numVertices,vertexType,vertexStride,&indexData,indexStride,numFaces,indexType);

            if (vertexType != PHY_FLOAT)  // TODO: maybe support also double?
            {
                MFLogger::ConsoleLogger::warn("Vertex type not supported: " + std::to_string(vertexType) + ".",SPATIAL_ENTITY_IMPLEMENTATION_MODULE_STR);
                break;
            }

            osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
            osg::ref_ptr<osg::DrawElementsUInt> indices = new osg::DrawElementsUInt(GL_TRIANGLES);

            for (int i = 0; i < numVertices; ++i)
            {
                MFMath::Vec3 v;
                memcpy(&v,vertexData + i * vertexStride,sizeof(v));
                vertices->push_back(osg::Vec3f(v.x,v.y,v.z));
                indices->push_back(i);
            }

            osg::ref_ptr<osg::Geometry> geom = new osg::Geometry();
            geom->setVertexArray(vertices);
            geom->addPrimitiveSet(indices.get());
            osg::ref_ptr<osg::Geode> geode = new osg::Geode;
            geode->addDrawable(geom.get());
            shapeNode = geode;

            break;
        }

        default:
            MFLogger::ConsoleLogger::warn("Unknown shape type for \"" + mName + "\": " + std::to_string(shapeType) + ".",SPATIAL_ENTITY_IMPLEMENTATION_MODULE_STR);
            break;
    }

    if (shapeNode)
    {
        shapeNode->setName("collision: " + getName());
        shapeNode->setNodeMask(MFRender::MASK_DEBUG);

        shapeNode->setStateSet(SpatialEntityImplementation::sDebugStateSet);

        mOSGPhysicsDebugNode = new osg::MatrixTransform();
        mOSGPhysicsDebugNode->addChild(shapeNode);

        syncDebugPhysicsNode();

        mOSGRoot->addChild(mOSGPhysicsDebugNode);
    }
}

void SpatialEntityImplementation::syncDebugPhysicsNode()
{
    if (!mOSGPhysicsDebugNode)
        return;

    btTransform transform = mBulletBody->getWorldTransform();
    btVector3 position = transform.getOrigin();
    btQuaternion rotation = transform.getRotation();

    osg::Matrixd transformMat;
    
    transformMat.makeTranslate(osg::Vec3f(position.x(),position.y(),position.z()));
    transformMat.preMult(osg::Matrixd::rotate(osg::Quat(rotation.x(),rotation.y(),rotation.z(),rotation.w())));

    if (transformMat.isNaN())
    {
        MFLogger::ConsoleLogger::warn("Matrix for the entity \"" + getName() + "\" is NaN, replacing with identity.",SPATIAL_ENTITY_IMPLEMENTATION_MODULE_STR);
        transformMat = osg::Matrixd::identity();
    }

    mOSGPhysicsDebugNode->setMatrix(transformMat);
}

}

#endif
