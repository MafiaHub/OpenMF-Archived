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

void PlayerController::update(double dt)
{
    mCameraController->update(dt);
}

PlayerController::PlayerController(MFGame::SpatialEntity *playerEntity, MFRender::Renderer *renderer, MFInput::InputManager *inputManager):
    CharacterEntityController(playerEntity)
{
    mRenderer = renderer;
    mInputManager = inputManager;

    mCameraController = new OrbitEntityCameraController(renderer,inputManager,playerEntity);

    std::shared_ptr<CharacterKeyCallback> cb = std::make_shared<CharacterKeyCallback>(this);
    mInputManager->addKeyCallback(cb);
}

PlayerController::~PlayerController()
{
    delete mCameraController;
}

}
