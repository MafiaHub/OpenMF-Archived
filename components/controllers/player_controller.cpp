#include <controllers/player_controller.hpp>
#include <iostream>
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
            case OMF_SCANCODE_W: mController->moveForward(down); break;
            case OMF_SCANCODE_S: mController->moveBackward(down); break;
            case OMF_SCANCODE_A:  mController->moveLeft(down); break;
            case OMF_SCANCODE_D:  mController->moveRight(down); break;
            case OMF_SCANCODE_SPACE: if (down) mController->jump(); break;
            case OMF_SCANCODE_RSHIFT:
            case OMF_SCANCODE_LSHIFT:
                mController->setMovementState(down
                    ? CharacterEntityController::RUN :
                      CharacterEntityController::WALK);
                break;
            default: break;
        }
    }

protected:
    CharacterEntityController *mController;
};

void PlayerController::update(double dt)
{
    mCameraController->update(dt);
    MFMath::Vec2 camRot = mCameraController->getRotation();
    instantRotate(MFMath::Vec3(camRot.x - MFMath::PI / 2.0,0,0).toQuat());
}

PlayerController::PlayerController(MFGame::Entity *playerEntity, MFRender::Renderer *renderer, MFInput::InputManager *inputManager, MFPhysics::PhysicsWorld *physicsWorld):
    CharacterEntityController(playerEntity, physicsWorld)
{
    mRenderer = renderer;
    mInputManager = inputManager;

    mCameraController = new OrbitEntityCameraController(renderer,inputManager,playerEntity,physicsWorld);
    mCameraController->setRelativeOffset(MFMath::Vec3(0,0,1.5));

    mKeyCallback = std::make_shared<CharacterKeyCallback>(this);
    mInputManager->addKeyCallback(mKeyCallback);
}

PlayerController::~PlayerController()
{
    delete mCameraController;
    mInputManager->removeKeyCallback(mKeyCallback);
}

}
