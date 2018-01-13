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
    virtual void update(double dt) override;
    virtual void ready() override;
    virtual void move(Vec3 destPosition) override;
    virtual std::string toString() override;

    void setOSGNode(osg::MatrixTransform *t)  { mOSGNode = t;          };
    void setBulletBody(btRigidBody *body)     { mBulletBody = body;    };
    osg::MatrixTransform *getOSGNode()        { return mOSGNode.get(); };
    btRigidBody *getBulletBody()              { return mBulletBody;    };
    void setOSGRootNode(osg::Group *root)     { mOSGRoot = root;       };

    static osg::ref_ptr<osg::StateSet> sDebugStateSet;

protected:
    void makePhysicsDebugOSGNode();        ///< Creates a visual representation of the physical representation.

    osg::ref_ptr<osg::MatrixTransform> mOSGNode;
    btRigidBody *mBulletBody;

    osg::Group *mOSGRoot;

    osg::ref_ptr<osg::MatrixTransform> mOSGPgysicsDebugNode;

    osg::Matrixd mOSGInitialTransform;     ///< Captures the OSG node transform when ready() is called.
    btTransform mBulletInitialTransform;   ///< Captures the Bullet body transform when ready() is called.
};

osg::ref_ptr<osg::StateSet> SpatialEntityImplementation::sDebugStateSet = 0;

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

    MFLogger::ConsoleLogger::info("readying entity: " + toString(),SPATIAL_ENTITY_IMPLEMENTATION_MODULE_STR);
    mReady = true;
}

void SpatialEntityImplementation::makePhysicsDebugOSGNode()        ///< Creates a visual representation of the physical representation.
{
    if (!mBulletBody)
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
            shapeNode = new osg::ShapeDrawable(shape.get()); // toBulletNode;
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
                MFGame::Vec3 v;
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

        mOSGPgysicsDebugNode = new osg::MatrixTransform();
        mOSGPgysicsDebugNode->addChild(shapeNode);

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

        mOSGPgysicsDebugNode->setMatrix(transformMat);
        mOSGRoot->addChild(mOSGPgysicsDebugNode);
    }
}

void SpatialEntityImplementation::move(Vec3 destPosition)
{
}

}

#endif
