#include <controllers/player_controller.hpp>

namespace MFGame
{

PlayerController::PlayerController(MFGame::SpatialEntity *playerEntity, MFRender::Renderer *renderer, MFInput::InputManager *inputManager):
    CharacterEntityController(playerEntity)
{
    mRenderer = renderer;
    mInputManager = inputManager;
}

void PlayerController::update()
{
    mRenderer->setCameraPositionRotation(mEntity->getPosition() + MFMath::Vec3(10,0,0),MFMath::Vec3(0,0,0));
}

}
