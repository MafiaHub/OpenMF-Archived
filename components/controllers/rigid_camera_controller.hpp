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
    SpatialEntity *mCameraEntity;
};

}

#endif
