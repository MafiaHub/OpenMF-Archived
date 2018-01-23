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
  RigidCameraController(MFRender::Renderer *renderer, MFInput::InputManager *inputManager, SpatialEntity *entity);
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
    SpatialEntity *mCameraEntity;
};

RigidCameraController::RigidCameraController(MFRender::Renderer *renderer, MFInput::InputManager *inputManager, SpatialEntity *entity): CameraController(renderer, inputManager)
{
    mSpeed = 100.0;
    mRotationSpeed = 0.005;
    mPreviousMouseButton = false;

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
    mCameraEntity = entity;
}

void RigidCameraController::update(double dt)
{
    if (mInitTransform)
    {
        MFMath::Vec3 pos,rot;
        mRenderer->getCameraPositionRotation(pos,rot);
        mRotation = rot;
        mInitTransform = false;
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

    if (mCameraEntity)
    {
        mCameraEntity->setDamping(0.15f, 0);

        MFMath::Vec3 prevVel = mCameraEntity->getVelocity();
        MFMath::Vec3 vel = (0.4f * prevVel + 0.6f * 65.f * offset);
        mCameraEntity->setVelocity(vel);
    }

    mRotation += rotOffset;

    mRotation.y = std::max(0.0f,std::min(MFMath::PI - 0.001f,mRotation.y));

    mRenderer->setCameraPositionRotation(mCameraEntity->getPosition(),mRotation);
}

}

#endif
