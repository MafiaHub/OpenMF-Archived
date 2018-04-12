#include <spatial_entity/spatial_entity_impl.hpp>
#include <utils/logger.hpp>

namespace MFGame
{

osg::ref_ptr<osg::StateSet> SpatialEntityImpl::sDebugStateSet = nullptr;

void SpatialEntityImpl::setPhysicsBehavior(SpatialEntity::PhysicsBehavior behavior)
{
    if (!hasPhysics())
        return;

    mPhysicsBehavior = behavior;

    // TODO: implement the cases

    switch (behavior)
    {
        case SpatialEntity::STATIC:
            break;

        case SpatialEntity::KINEMATIC:
            break;

        case SpatialEntity::RIGID:
            break;

        case SpatialEntity::RIGID_PAWN:
            mBulletBody->setAngularFactor(0);
            break;

        default:
            break;
    }
}

MFMath::Vec3 SpatialEntityImpl::getSize()
{
    if (mBulletBody) {
        btVector3 p1, p2;
        mBulletBody->getAabb(p1,p2);
        return MFMath::Vec3(p2.x() - p1.x(), p2.y() - p1.y(), p2.z() - p1.z());
    }

    // TODO: Get the AABBox from OSG here?

    return MFMath::Vec3(0,0,0);
}

void SpatialEntityImpl::setFriction(double factor)
{
    if (mBulletBody)
        mBulletBody->setFriction(factor);
}

bool SpatialEntityImpl::canBeMoved()
{
    if (!mBulletBody)    // no physical representation => nothing stops the entity from moving
        return true;

    // TODO

    return false;
}

bool SpatialEntityImpl::hasVisual()
{
    return mOSGNode != nullptr;
}

bool SpatialEntityImpl::hasPhysics()
{
    return mBulletBody != nullptr;
}

bool SpatialEntityImpl::hasCollision()
{
    return mBulletBody != nullptr;
}

void SpatialEntityImpl::setVelocity(MFMath::Vec3 velocity)
{
    if (!mBulletBody)
        return;

    mBulletBody->setLinearVelocity(btVector3(velocity.x,velocity.y,velocity.z));
}

MFMath::Vec3 SpatialEntityImpl::getVelocity()
{
    if (mBulletBody)
    {
        btVector3 v = mBulletBody->getLinearVelocity();
        return MFMath::Vec3(v.x(),v.y(),v.z());
    }

    return MFMath::Vec3(0,0,0);
}

void SpatialEntityImpl::setAngularVelocity(MFMath::Vec3 velocity)
{
    if (!mBulletBody)
        return;

    mBulletBody->setAngularVelocity(btVector3(velocity.x,velocity.y,velocity.z));
}

void SpatialEntityImpl::setDamping(float lin, float ang)
{
    if (!mBulletBody)
        return;

    mBulletBody->setDamping(lin, ang);
}

MFMath::Vec3 SpatialEntityImpl::getAngularVelocity()
{
    if (!mBulletBody)
        return MFMath::Vec3(0, 0, 0);

    auto vel = mBulletBody->getAngularVelocity();
    return MFMath::Vec3(vel.x(), vel.y(), vel.z());
}

MFMath::Vec2 SpatialEntityImpl::getDamping()
{
    if (!mBulletBody)
        return MFMath::Vec2(0,0);

    return MFMath::Vec2(mBulletBody->getLinearDamping(), mBulletBody->getAngularDamping());
}

void SpatialEntityImpl::computeCurrentTransform()
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

void SpatialEntityImpl::applyCurrentTransform()
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

void SpatialEntityImpl::setRotation(MFMath::Quat rotation)
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

void SpatialEntityImpl::setPosition(MFMath::Vec3 position)
{
    SpatialEntity::setPosition(position);
    applyCurrentTransform();
}

SpatialEntityImpl::SpatialEntityImpl(): SpatialEntity()
{
    if (!SpatialEntityImpl::sDebugStateSet)
    {
        // let the first entity create the shared debug material
        SpatialEntityImpl::sDebugStateSet = new osg::StateSet;

        osg::ref_ptr<osg::Material> mat = new osg::Material();
        osg::ref_ptr<osg::PolygonMode> mode = new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK,osg::PolygonMode::LINE);
        osg::Vec4f debugColor = osg::Vec4f(0,1,0,1);

        mat->setAmbient(osg::Material::FRONT_AND_BACK,debugColor);
        mat->setDiffuse(osg::Material::FRONT_AND_BACK,debugColor);
        mat->setSpecular(osg::Material::FRONT_AND_BACK,debugColor);
        mat->setEmission(osg::Material::FRONT_AND_BACK,debugColor);

        SpatialEntityImpl::sDebugStateSet->setAttributeAndModes(mat);
        SpatialEntityImpl::sDebugStateSet->setAttributeAndModes(mode);
    }

    mOSGNode = nullptr;
    mBulletBody = nullptr;
    mBulletMotionState = nullptr;
    mCreateDebugGeometry = false;
}

SpatialEntityImpl::~SpatialEntityImpl()
= default;

void SpatialEntityImpl::update(double dt)
{
    // TODO: make use of MotionState to see if the transform has to be updated

    if (mBulletBody && mBulletMotionState)
    {
        computeCurrentTransform();
        applyCurrentTransform();
    }
}

std::string SpatialEntityImpl::toString()
{
    int hasOSG = mOSGNode != nullptr;
    int hasBullet = mBulletBody != nullptr;

    return "\"" + mName + "\", ID: " + std::to_string(mId) + ", representations: " + std::to_string(hasOSG) + std::to_string(hasBullet) + ", pos: " + mPosition.str();
}

void SpatialEntityImpl::ready()
{
    MFLogger::Logger::info("readying entity: " + toString(),SPATIAL_ENTITY_IMPLEMENTATION_MODULE_STR);

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

void SpatialEntityImpl::makePhysicsDebugOSGNode()        ///< Creates a visual representation of the physical representation.
{
    if (!mBulletBody || !mCreateDebugGeometry)
        return;

    if (!mOSGRoot)
    {
        MFLogger::Logger::warn("Cannot create debug OSG node for \"" + mName + "\": no access to scene root.",SPATIAL_ENTITY_IMPLEMENTATION_MODULE_STR);
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
                MFLogger::Logger::warn("Vertex type not supported: " + std::to_string(vertexType) + ".",SPATIAL_ENTITY_IMPLEMENTATION_MODULE_STR);
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
            MFLogger::Logger::warn("Unknown shape type for \"" + mName + "\": " + std::to_string(shapeType) + ".",SPATIAL_ENTITY_IMPLEMENTATION_MODULE_STR);
            break;
    }

    if (shapeNode)
    {
        shapeNode->setName("collision: " + getName());
        shapeNode->setNodeMask(MFRender::MASK_DEBUG);

        shapeNode->setStateSet(SpatialEntityImpl::sDebugStateSet);

        mOSGPhysicsDebugNode = new osg::MatrixTransform();
        mOSGPhysicsDebugNode->addChild(shapeNode);

        syncDebugPhysicsNode();

        mOSGRoot->addChild(mOSGPhysicsDebugNode);
    }
}

void SpatialEntityImpl::syncDebugPhysicsNode()
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
        MFLogger::Logger::warn("Matrix for the entity \"" + getName() + "\" is NaN, replacing with identity.",SPATIAL_ENTITY_IMPLEMENTATION_MODULE_STR);
        transformMat = osg::Matrixd::identity();
    }

    mOSGPhysicsDebugNode->setMatrix(transformMat);
}

}
