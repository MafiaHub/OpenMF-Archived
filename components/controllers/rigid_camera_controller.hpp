#ifndef RIGID_CAMERA_CONTROLLER
#define RIGID_CAMERA_CONTROLLER

#include <controllers/camera_controller.hpp>
#include <physics/bullet_physics_world.hpp>
#include <math.hpp>

namespace MFGame
{

class RigidCameraController: public CameraController
{
public:
    RigidCameraController(MFRender::Renderer *renderer, MFInput::InputManager *inputManager, MFPhysics::BulletPhysicsWorld *physics);
    virtual void update(double dt) override;
protected:
    double mPreviousMouseButton;

    bool mInitTransform;

    MFMath::Vec3 mPosition;
    MFMath::Vec3 mRotation;    // Euler angles

    unsigned int mKeyForward;
    unsigned int mKeyBackward;
    unsigned int mKeyRight;
    unsigned int mKeyLeft;
    unsigned int mKeyUp;
    unsigned int mKeyDown;
    unsigned int mKeySpeedup;

private:
    MFPhysics::BulletPhysicsWorld *mPhysicsWorld;
    MFUtil::FullRigidBody mBulletBody;
};

RigidCameraController::RigidCameraController(MFRender::Renderer *renderer, MFInput::InputManager *inputManager, MFPhysics::BulletPhysicsWorld *physics) : CameraController(renderer, inputManager)
{
    mSpeed = 100.0;
    mRotationSpeed = 0.005;
    mPreviousMouseButton = false;

    mPosition = MFMath::Vec3(0,0,0);
    mRotation = MFMath::Vec3(0,0,0);

    mInitTransform = true;

    // default key binding:

    mKeyForward  = SDL_SCANCODE_W;
    mKeyBackward = SDL_SCANCODE_S;
    mKeyRight    = SDL_SCANCODE_D; 
    mKeyLeft     = SDL_SCANCODE_A;
    mKeyUp       = SDL_SCANCODE_E;
    mKeyDown     = SDL_SCANCODE_Q;
    mKeySpeedup  = SDL_SCANCODE_LSHIFT;

    mPhysicsWorld = physics;
}

void RigidCameraController::update(double dt)
{
    if (mInitTransform)
    {
        MFMath::Vec3 pos,rot;
        mRenderer->getCameraPositionRotation(pos,rot);
        mPosition = pos;
        mRotation = rot;
        mInitTransform = false;

        mBulletBody.mShape = std::make_shared<btSphereShape>(1.0f);

        btScalar mass = 1.0f;
        mBulletBody.mMotionState = std::make_shared<btDefaultMotionState>(
            btTransform(btQuaternion(0, 0, 0, mass), 
            MFUtil::mafiaVec3ToBullet(pos.x, pos.z, pos.y)));

        btVector3 inertia;
        mBulletBody.mShape->calculateLocalInertia(mass, inertia);
        mBulletBody.mBody = std::make_shared<btRigidBody>(mass, mBulletBody.mMotionState.get(), mBulletBody.mShape.get(), inertia);
        //mBulletBody.mBody->setSleepingThresholds(0, 0);
        mBulletBody.mBody->setActivationState(DISABLE_DEACTIVATION);
        mBulletBody.mBody->setAngularFactor(0);
        mPhysicsWorld->getWorld()->addRigidBody(mBulletBody.mBody.get());
        mBulletBody.mBody->setGravity(btVector3(0, 0, 0));
    }

    MFMath::Vec3 pos, rot;
    double distance = dt * mSpeed;
    MFMath::Vec3 direction = MFMath::Vec3(0,0,0);

    if (mInputManager->keyPressed(mKeySpeedup))
        distance *= 5.0;

    if (mInputManager->keyPressed(mKeyForward))
        direction.y += 1.0;

    if (mInputManager->keyPressed(mKeyBackward))
        direction.y -= 1.0;

    if (mInputManager->keyPressed(mKeyRight))
        direction.x += 1.0;

    if (mInputManager->keyPressed(mKeyLeft))
        direction.x -= 1.0;

    if (mInputManager->keyPressed(mKeyUp))
        direction.z += 1.0;

    if (mInputManager->keyPressed(mKeyDown))
        direction.z -= 1.0;

    direction = direction * ((float) distance);

    MFMath::Vec3 f,r,u;
    mRenderer->getCameraVectors(f,r,u);

    MFMath::Vec3 offset, rotOffset;

    offset = direction.x * r + direction.y * f + direction.z * u;
    rotOffset = MFMath::Vec3(0,0,0);

    if (mInputManager->mouseButtonPressed(1))  // rotation
    {
        unsigned int windowW, windowH;
        mInputManager->getWindowSize(windowW,windowH);

        if (mPreviousMouseButton)      // don't rotate if mouse wasn't centered previously
        {
            int dx, dy;
            unsigned int x, y;
            mInputManager->getMousePosition(x,y);
 
            dx = x - windowW / 2;
            dy = y - windowH / 2;

            rotOffset = MFMath::Vec3(-1 * mRotationSpeed * dx, -1 * mRotationSpeed * dy, 0);
        }
        else
        {
            mInputManager->setMouseVisible(false);
        }

        mPreviousMouseButton = true;
        mInputManager->setMousePosition(windowW / 2, windowH / 2);   // center the mouse
    }
    else
    {
        if (mPreviousMouseButton)
            mInputManager->setMouseVisible(true);

        mPreviousMouseButton = false;
    }

    mBulletBody.mBody->setDamping(0.15f, 0);
    mBulletBody.mBody->setLinearVelocity(0.4f*mBulletBody.mBody->getLinearVelocity()+0.6*65*btVector3(offset.x, offset.y, offset.z));
    btTransform t = mBulletBody.mBody->getWorldTransform();
    btVector3 bPos = t.getOrigin();
    mPosition = MFMath::Vec3(bPos.x(), bPos.y(), bPos.z());

    mRotation += rotOffset;

    mRotation.y = std::max(0.0f,std::min(MFMath::PI - 0.001f,mRotation.y));

    mRenderer->setCameraPositionRotation(mPosition,mRotation);
}

}

#endif
