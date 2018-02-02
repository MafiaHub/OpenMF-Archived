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
            case 26: mController->moveForward(down); break;
            case 22: mController->moveBackward(down); break;
            case 4:  mController->moveLeft(down); break;
            case 7:  mController->moveRight(down); break;
            case 44: if (down) mController->jump(); break;
            case 225:
                mController->setMovementState(down
                    ? CharacterEntityController::RUN :
                      CharacterEntityController::WALK);
                break;
            case 224:
                mController->setMovementState(down
                    ? CharacterEntityController::CROUCH :
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

PlayerController::PlayerController(MFGame::SpatialEntity *playerEntity, MFRender::Renderer *renderer, MFInput::InputManager *inputManager, MFPhysics::PhysicsWorld *physicsWorld):
    CharacterEntityController(playerEntity)
{
    mRenderer = renderer;
    mInputManager = inputManager;

    mCameraController = new OrbitEntityCameraController(renderer,inputManager,playerEntity,physicsWorld);
    mCameraController->setRelativeOffset(MFMath::Vec3(0,0,1.5));

    std::shared_ptr<CharacterKeyCallback> cb = std::make_shared<CharacterKeyCallback>(this);
    mInputManager->addKeyCallback(cb);
}

PlayerController::~PlayerController()
{
    delete mCameraController;
}

}
