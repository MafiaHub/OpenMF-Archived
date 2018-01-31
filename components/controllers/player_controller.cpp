#include <controllers/player_controller.hpp>

namespace MFGame
{

PlayerController::PlayerController(MFGame::SpatialEntity *playerEntity, MFRender::Renderer *renderer, MFInput::InputManager *inputManager):
    CharacterEntityController(playerEntity)
{
    mRenderer = renderer;
    mInputManager = inputManager;
    mRotationSpeed = 1.0;
    mYaw = 0.0;
    mPitch = 0.0;
}

void PlayerController::update(double dt)
{ 
    unsigned int windowW, windowH;
    mInputManager->getWindowSize(windowW,windowH);

    int dx, dy;
    unsigned int x, y;
    mInputManager->getMousePosition(x,y);

    dx = x - windowW / 2;
    dy = y - windowH / 2;

    MFMath::Vec3 rotOffset = MFMath::Vec3(-1 * mRotationSpeed * dx * dt, -1 * mRotationSpeed * dy * dt, 0);

    mYaw   += rotOffset.x;
    mPitch += rotOffset.y;

    mInputManager->setMousePosition(windowW / 2, windowH / 2);   // center the mouse

    MFMath::Vec3 camOffset = MFMath::Vec3(cos(mYaw) * cos(mPitch),sin(mYaw) * cos(mPitch),sin(mPitch)) * 10.0f;

    mRenderer->setCameraPositionRotation(mEntity->getPosition() + camOffset,MFMath::Vec3(0,0,0));
    mRenderer->cameraFace(mEntity->getPosition());
}

}
