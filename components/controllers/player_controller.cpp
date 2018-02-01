#include <controllers/player_controller.hpp>

namespace MFGame
{

class CharacterKeyCallback: public MFInput::KeyInputCallback
{
public:
    CharacterKeyCallback(PlayerController *controller)
    {
        mController = controller;
    }

    virtual void call(bool down, unsigned int keyCode) override
    {
        switch (keyCode)
        {
            case 26: mController->moveForward(down); break;
            case 22: mController->moveBackward(down); break;
            case 4:  mController->moveLeft(down); break;
            case 7:  mController->moveRight(down); break;
            case 44: mController->jump(); break;
            default: break;
        }
    }

protected:
    CharacterEntityController *mController;
};

PlayerController::PlayerController(MFGame::SpatialEntity *playerEntity, MFRender::Renderer *renderer, MFInput::InputManager *inputManager):
    CharacterEntityController(playerEntity)
{
    mRenderer = renderer;
    mInputManager = inputManager;
    mRotationSpeed = 1.0;
    mYaw = 0.0;
    mPitch = 0.0;

    std::shared_ptr<CharacterKeyCallback> cb = std::make_shared<CharacterKeyCallback>(this);
    mInputManager->addKeyCallback(cb);
}

void PlayerController::update(double dt)
{
    // TODO: optimize this function, as it's being called each frame
 
    unsigned int windowW, windowH;
    mInputManager->getWindowSize(windowW,windowH);

    int dx, dy;
    unsigned int x, y;
    mInputManager->getMousePosition(x,y);

    dx = x - windowW / 2;
    dy = y - windowH / 2;

    MFMath::Vec3 rotOffset = MFMath::Vec3(-1 * mRotationSpeed * dx * dt, mRotationSpeed * dy * dt, 0);

    mYaw   += rotOffset.x;
    mPitch = std::min((double) MFMath::PI / 2.0 - 0.001,std::max((double) -MFMath::PI / 2.0 + 0.001,mPitch + rotOffset.y));
    mInputManager->setMousePosition(windowW / 2, windowH / 2);   // center the mouse

    MFMath::Vec3 camOffset = MFMath::Vec3(cos(mYaw) * cos(mPitch),sin(mYaw) * cos(mPitch),sin(mPitch)) * 10.0f;

    mRenderer->setCameraPositionRotation(mEntity->getPosition() + camOffset,MFMath::Vec3(0,0,0));
    mRenderer->cameraFace(mEntity->getPosition());
}

}
