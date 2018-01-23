#ifndef CAMERA_CONTROLLER_H
#define CAMERA_CONTROLLER_H

#include <renderer/base_renderer.hpp>
#include <input/base_input_manager.hpp>

namespace MFGame
{

class CameraController
{
public:
    CameraController(MFRender::Renderer *renderer, MFInput::InputManager *inputManager);
    virtual void update(double dt)=0;

protected:
    MFRender::Renderer *mRenderer;
    MFInput::InputManager *mInputManager;
};

CameraController::CameraController(MFRender::Renderer *renderer, MFInput::InputManager *inputManager)
{
    mRenderer = renderer;
    mInputManager = inputManager;
}

}

#endif
