#include "entity_impl.hpp"
#include <entity/entity_impl.hpp>
#include <utils/logger.hpp>

#include "renderer/osg_renderer.hpp"
#include "physics/bullet_physics_world.hpp"

#include "engine/engine.hpp"

namespace MFGame
{

osg::ref_ptr<osg::StateSet> EntityImpl::sDebugStateSet = nullptr;

void EntityImpl::setPhysicsBehavior(Entity::PhysicsBehavior behavior)
{
    if (!hasPhysics())
        return;

    mPhysicsBehavior = behavior;

    // TODO: implement the cases

    switch (behavior)
    {
        case Entity::STATIC:
            break;

        case Entity::KINEMATIC:
            break;

        case Entity::RIGID:
            break;

        case Entity::RIGID_PAWN:
            mBulletBody->setAngularFactor(0);
            break;

        default:
            break;
    }
}

MFMath::Vec3 EntityImpl::getSize()
{
    if (mBulletBody) {
        btVector3 p1, p2;
        mBulletBody->getAabb(p1,p2);
        return MFMath::Vec3(p2.x() - p1.x(), p2.y() - p1.y(), p2.z() - p1.z());
    }

    // TODO: Get the AABBox from OSG here?

    return MFMath::Vec3(0,0,0);
}

void EntityImpl::setFriction(double factor)
{
    if (mBulletBody)
        mBulletBody->setFriction(factor);
}

bool EntityImpl::canBeMoved()
{
    if (!mBulletBody)    // no physical representation => nothing stops the entity from moving
        return true;

    // TODO

    return false;
}

bool EntityImpl::hasVisual()
{
    return mOSGNode != nullptr;
}

bool EntityImpl::hasPhysics()
{
    return mBulletBody != nullptr;
}

bool EntityImpl::hasCollision()
{
    return mBulletBody != nullptr;
}

void EntityImpl::setVelocity(MFMath::Vec3 velocity)
{
    if (!mBulletBody)
        return;

    mBulletBody->setLinearVelocity(btVector3(velocity.x,velocity.y,velocity.z));
}

MFMath::Vec3 EntityImpl::getVelocity()
{
    if (mBulletBody)
    {
        btVector3 v = mBulletBody->getLinearVelocity();
        return MFMath::Vec3(v.x(),v.y(),v.z());
    }

    return MFMath::Vec3(0,0,0);
}

void EntityImpl::setAngularVelocity(MFMath::Vec3 velocity)
{
    if (!mBulletBody)
        return;

    mBulletBody->setAngularVelocity(btVector3(velocity.x,velocity.y,velocity.z));
}

void EntityImpl::setDamping(float lin, float ang)
{
    if (!mBulletBody)
        return;

    mBulletBody->setDamping(lin, ang);
}

MFMath::Vec3 EntityImpl::getAngularVelocity()
{
    if (!mBulletBody)
        return MFMath::Vec3(0, 0, 0);

    auto vel = mBulletBody->getAngularVelocity();
    return MFMath::Vec3(vel.x(), vel.y(), vel.z());
}

MFMath::Vec2 EntityImpl::getDamping()
{
    if (!mBulletBody)
        return MFMath::Vec2(0,0);

    return MFMath::Vec2(mBulletBody->getLinearDamping(), mBulletBody->getAngularDamping());
}

void EntityImpl::computeCurrentTransform()
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
        // An entity with no physical nor visual representation shall have identity transformation in case it's being used for special purposes.
        mPosition = MFMath::Vec3(0,0,0);
        mRotation = MFMath::Quat(0,0,0,1);
        mScale = MFMath::Vec3(1,1,1);
    }
}

void EntityImpl::applyCurrentTransform()
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

void EntityImpl::setRotation(MFMath::Quat rotation)
{
    mRotation = rotation;

    if (mOSGNode)
    {
        osg::Matrixd m = mOSGNode->getMatrix();
        m.setRotate(osg::Quat(rotation.x,rotation.y,rotation.z,rotation.w));
        mOSGNode->setMatrix(m);
    }

    if (mBulletBody)
    {
        // TODO: test this
        btTransform t = mBulletBody->getWorldTransform();
        t.setRotation(btQuaternion(rotation.x,rotation.y,rotation.z,rotation.w));
        mBulletBody->setWorldTransform(t);
    }
}

    void EntityImpl::think()
    {

    }

    void EntityImpl::setPosition(MFMath::Vec3 position)
{
    Entity::setPosition(position);
    applyCurrentTransform();
}

EntityImpl::EntityImpl(): Entity()
{
    if (!EntityImpl::sDebugStateSet)
    {
        // let the first entity create the shared debug material
        EntityImpl::sDebugStateSet = new osg::StateSet;

        osg::ref_ptr<osg::Material> mat = new osg::Material();
        osg::ref_ptr<osg::PolygonMode> mode = new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK,osg::PolygonMode::LINE);
        osg::Vec4f debugColor = osg::Vec4f(0,1,0,1);

        mat->setAmbient(osg::Material::FRONT_AND_BACK,debugColor);
        mat->setDiffuse(osg::Material::FRONT_AND_BACK,debugColor);
        mat->setSpecular(osg::Material::FRONT_AND_BACK,debugColor);
        mat->setEmission(osg::Material::FRONT_AND_BACK,debugColor);

        EntityImpl::sDebugStateSet->setAttributeAndModes(mat);
        EntityImpl::sDebugStateSet->setAttributeAndModes(mode);
    }

    mOSGNode = nullptr;
    mBulletBody = nullptr;
    mBulletMotionState = nullptr;
    mCreateDebugGeometry = false;
}

EntityImpl::~EntityImpl()
{
    
}

void EntityImpl::update(double dt)
{
    // TODO: make use of MotionState to see if the transform has to be updated

    if (mBulletBody && mBulletMotionState)
    {
        computeCurrentTransform();
        applyCurrentTransform();
    }
}

std::string EntityImpl::toString()
{
    int hasOSG = mOSGNode != nullptr;
    int hasBullet = mBulletBody != nullptr;

    return "\"" + mName + "\", ID: " + std::to_string(mId) + ", representations: " + std::to_string(hasOSG) + std::to_string(hasBullet) + ", pos: " + mPosition.str();
}

void EntityImpl::ready()
{
    MFLogger::Logger::info("readying entity: " + toString(),ENTITY_IMPLEMENTATION_MODULE_STR);

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

void EntityImpl::destroy()
{
    if (mOSGNode) {
        auto rend = (MFRender::OSGRenderer *)mEngine->getRenderer();
        if (rend->getRootNode())
            rend->getRootNode()->removeChild(mOSGNode);

        mOSGNode = nullptr;
    }

    if (mBulletBody) {
        auto phys = (MFPhysics::BulletPhysicsWorld *)mEngine->getPhysicsWorld();
        phys->getWorld()->removeRigidBody(mBulletBody.get());
        mBulletBody = nullptr;
        mBulletMotionState = nullptr;
    }
}

void EntityImpl::makePhysicsDebugOSGNode()        ///< Creates a visual representation of the physical representation.
{
    if (!mBulletBody || !mCreateDebugGeometry)
        return;

    if (!mOSGRoot)
    {
        MFLogger::Logger::warn("Cannot create debug OSG node for \"" + mName + "\": no access to scene root.",ENTITY_IMPLEMENTATION_MODULE_STR);
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
            auto *cylinder = static_cast<btCylinderShape *>(mBulletBody->getCollisionShape());
            osg::ref_ptr<osg::Shape> shape = new osg::Cylinder(osg::Vec3f(0,0,0),cylinder->getRadius(),cylinder->getHalfExtentsWithoutMargin().z() * 2);
            shapeNode = new osg::ShapeDrawable(shape.get());
            break;
        }

        case BroadphaseNativeTypes::CAPSULE_SHAPE_PROXYTYPE:
        {
            auto *capsule = static_cast<btCapsuleShape *>(mBulletBody->getCollisionShape());
            osg::ref_ptr<osg::Shape> shape = new osg::Capsule(osg::Vec3f(0,0,0),capsule->getRadius(),capsule->getHalfHeight() * 2.0);
            shapeNode = new osg::ShapeDrawable(shape.get());
            break;
        }

        case BroadphaseNativeTypes::CONVEX_SHAPE_PROXYTYPE:
        case BroadphaseNativeTypes::TRIANGLE_MESH_SHAPE_PROXYTYPE:
        {
            auto *mesh =
                static_cast<btTriangleMesh *>(
                    static_cast<btTriangleMeshShape *>(mBulletBody->getCollisionShape())->getMeshInterface());

            unsigned char *vertexData,*indexData;
            int numVertices, numFaces, vertexStride, indexStride;
            PHY_ScalarType vertexType, indexType;

            mesh->getLockedVertexIndexBase(&vertexData,numVertices,vertexType,vertexStride,&indexData,indexStride,numFaces,indexType);

            if (vertexType != PHY_FLOAT)  // TODO: maybe support also double?
            {
                MFLogger::Logger::warn("Vertex type not supported: " + std::to_string(vertexType) + ".",ENTITY_IMPLEMENTATION_MODULE_STR);
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
            MFLogger::Logger::warn("Unknown shape type for \"" + mName + "\": " + std::to_string(shapeType) + ".",ENTITY_IMPLEMENTATION_MODULE_STR);
            break;
    }

    if (shapeNode)
    {
        shapeNode->setName("collision: " + getName());
        shapeNode->setNodeMask(MFRender::MASK_DEBUG);

        shapeNode->setStateSet(EntityImpl::sDebugStateSet);

        mOSGPhysicsDebugNode = new osg::MatrixTransform();
        mOSGPhysicsDebugNode->addChild(shapeNode);

        syncDebugPhysicsNode();

        mOSGRoot->addChild(mOSGPhysicsDebugNode);
    }
}

void EntityImpl::syncDebugPhysicsNode()
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
        MFLogger::Logger::warn("Matrix for the entity \"" + getName() + "\" is NaN, replacing with identity.",ENTITY_IMPLEMENTATION_MODULE_STR);
        transformMat = osg::Matrixd::identity();
    }

    mOSGPhysicsDebugNode->setMatrix(transformMat);
}

}
