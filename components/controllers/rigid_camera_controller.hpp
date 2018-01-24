#ifndef RIGID_CAMERA_CONTROLLER
#define RIGID_CAMERA_CONTROLLER

#include <controllers/free_camera_controller.hpp>
#include <physics/bullet_physics_world.hpp>
#include <math.hpp>

namespace MFGame
{

class RigidCameraController: public FreeCameraController
{
public:
    RigidCameraController(MFRender::Renderer *renderer, MFInput::InputManager *inputManager, SpatialEntity *entity);
protected:
  virtual void handleMovement(MFMath::Vec3 offset, MFMath::Vec3 angOffset) override;
private: 
    MFPhysics::BulletPhysicsWorld *mPhysicsWorld;
    SpatialEntity *mCameraEntity;
};

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

#endif
