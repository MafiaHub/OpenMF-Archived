#ifndef FREE_CAMERA_CONTROLLER
#define FREE_CAMERA_CONTROLLER

#include <camera_controller.hpp>
#include <math.hpp>

namespace MFGame
{

class FreeCameraController: public CameraController
{
public:
    FreeCameraController(MFRender::Renderer *renderer, MFInput::InputManager *inputManager);
    virtual void update(double dt) override;

protected:
    double mSpeed;
};

FreeCameraController::FreeCameraController(MFRender::Renderer *renderer, MFInput::InputManager *inputManager):
    CameraController(renderer,inputManager)
{
    mSpeed = 1.0;
}

void FreeCameraController::update(double dt)
{
    MFMath::Vec3 pos, rot;
    double distance = dt * mSpeed;
    MFMath::Vec3 direction = MFMath::Vec3(0,0,0);

    if (mInputManager->keyPressed(26))
        direction.y += 1.0;

    if (mInputManager->keyPressed(22))
        direction.y -= 1.0;

    if (mInputManager->keyPressed(4))
        direction.x -= 1.0;

    if (mInputManager->keyPressed(7))
        direction.x += 1.0;

    direction = direction * ((float) distance);

    mRenderer->getCameraPositionRotation(pos,rot);
    mRenderer->setCameraPositionRotation(pos + direction,rot);
}

}

#endif
