#include <controllers/rigid_camera_controller.hpp>

namespace MFGame
{

RigidCameraController::RigidCameraController(MFRender::Renderer *renderer, MFInput::InputManager *inputManager, SpatialEntity *entity): FreeCameraController(renderer, inputManager)
{
    mCameraEntity = entity;
}

void RigidCameraController::handleMovement(MFMath::Vec3 offset, MFMath::Vec3 angOffset)
{
    if (mCameraEntity)
    {
        mCameraEntity->setDamping(0.15f, 0);

        MFMath::Vec3 prevVel = mCameraEntity->getVelocity();
        MFMath::Vec3 vel = (0.4f * prevVel + 0.6f * 65.f * offset);
        mCameraEntity->setVelocity(vel);
    }

    mRotation += angOffset;
    mRotation.y = std::max(0.0f, std::min(MFMath::PI - 0.001f, mRotation.y));
    mRenderer->setCameraPositionRotation(mCameraEntity->getPosition(), mRotation);
}

}

