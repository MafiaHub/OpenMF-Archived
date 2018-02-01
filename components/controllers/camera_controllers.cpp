#include <controllers/camera_controllers.hpp>

#include <SDL2/SDL.h> // TODO: temporary!!!

namespace MFGame
{

MouseRotateCameraController::MouseRotateCameraController(MFRender::Renderer *renderer, MFInput::InputManager *inputManager):
    CameraController(renderer,inputManager)
{
    mRotationSpeed = 0.2;
    mRotationActive = true;
    mPreviousRotationActive = false;
    mHideCursorOnRotation = true;
    mPreviouslyCentered = false;
    mRotation = MFMath::Vec2(0,0);
}

void MouseRotateCameraController::update(double dt)
{
    if (mRotationActive != mPreviousRotationActive)
    {
        mInputManager->setCursorVisible(!mRotationActive);
    }

    mPreviousRotationActive = mRotationActive;

    if (!mRotationActive)
    {
        mPreviouslyCentered = false;
        return;
    }

    unsigned int x, y;

    mInputManager->getCursorPosition(x,y);

    unsigned int windowW, windowH, halfWindowW, halfWindowH;
    mInputManager->getWindowSize(windowW,windowH);

    halfWindowW = windowW / 2;
    halfWindowH = windowH / 2;

    int dx = x - halfWindowW;
    int dy = y - halfWindowH;
    
    if (mPreviouslyCentered)
    {
        const double pitchBias = 0.0001;
        mRotation.x -= mRotationSpeed * dx * dt;
        mRotation.y = std::max(pitchBias,
            std::min(MFMath::PI - pitchBias,mRotation.y - mRotationSpeed * dy * dt));
        applyRotation();
    }

    mInputManager->setCursorPosition(halfWindowW,halfWindowH);   // center the mouse
    mPreviouslyCentered = true;
}

OrbitEntityCameraController::OrbitEntityCameraController(MFRender::Renderer *renderer, MFInput::InputManager *inputManager, SpatialEntity *entity):
    MouseRotateCameraController(renderer,inputManager)
{
    mEntity = entity;
    mMaxCameraDistance = 5.0;
}

void OrbitEntityCameraController::applyRotation()
{
    float yaw = mRotation.x;
    float pitch = mRotation.y + MFMath::PI / 2.0;

    MFMath::Vec3 camOffset = MFMath::Vec3(cos(yaw) * cos(pitch),sin(yaw) * cos(pitch),sin(pitch)) * ((float) mMaxCameraDistance);
    mRenderer->setCameraPositionRotation(mEntity->getPosition() + camOffset,MFMath::Vec3(0,0,0));
    mRenderer->cameraFace(mEntity->getPosition());
}

FreeCameraController::FreeCameraController(MFRender::Renderer *renderer, MFInput::InputManager *inputManager):
    MouseRotateCameraController(renderer,inputManager)
{
    mSpeed = 1.0;
    mPreviousMouseButton = false;

    mPosition = MFMath::Vec3(0,0,0);

    mInitTransform = true;

    // default key binding:

    mKeyForward  = SDL_SCANCODE_W;
    mKeyBackward = SDL_SCANCODE_S;
    mKeyRight    = SDL_SCANCODE_D; 
    mKeyLeft     = SDL_SCANCODE_A;
    mKeyUp       = SDL_SCANCODE_E;
    mKeyDown     = SDL_SCANCODE_Q;
    mKeySpeedup  = SDL_SCANCODE_LSHIFT;
}

void FreeCameraController::update(double dt)
{
    if (mInitTransform)
    {
        MFMath::Vec3 pos,rot;
        mRenderer->getCameraPositionRotation(pos,rot);
        mPosition = pos;
        mRotation = MFMath::Vec2(rot.x,rot.y);
        mInitTransform = false;
    }

    mRotationActive = mInputManager->mouseButtonPressed(1);

    MouseRotateCameraController::update(dt);

    MFMath::Vec3 pos, rot;
    float distance = dt * mSpeed;
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

    direction = direction * distance;

    MFMath::Vec3 f,r,u;
    mRenderer->getCameraVectors(f,r,u);

    MFMath::Vec3 offset;

    offset = f * direction.y + r * direction.x + u * direction.z;

    handleMovement(offset);
}

void FreeCameraController::applyRotation()
{
    MFMath::Vec3 pos,rot;
    mRenderer->getCameraPositionRotation(pos,rot);
    mRenderer->setCameraPositionRotation(pos,MFMath::Vec3(mRotation.x,mRotation.y,0));
}

void FreeCameraController::handleMovement(MFMath::Vec3 offset)
{
    MFMath::Vec3 pos,rot;
    mPosition += offset;
    mRenderer->getCameraPositionRotation(pos,rot);
    mRenderer->setCameraPositionRotation(mPosition,rot);
}

RigidCameraController::RigidCameraController(MFRender::Renderer *renderer, MFInput::InputManager *inputManager, SpatialEntity *entity): FreeCameraController(renderer, inputManager)
{
    mCameraEntity = entity;
}

void RigidCameraController::handleMovement(MFMath::Vec3 offset)
{
    if (mCameraEntity)
    {
        mCameraEntity->setDamping(0.15f, 0);
        MFMath::Vec3 prevVel = mCameraEntity->getVelocity();
        MFMath::Vec3 vel = (0.4f * prevVel + 0.6f * 65.f * offset);
        mCameraEntity->setVelocity(vel);
    }

    mRenderer->setCameraPositionRotation(mCameraEntity->getPosition(),MFMath::Vec3(mRotation.x,mRotation.y,0));
}

}
