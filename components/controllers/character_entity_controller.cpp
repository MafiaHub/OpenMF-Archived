#include <controllers/character_entity_controller.hpp>
#include <iostream>

namespace MFGame
{

class TestCharacterCallback: public MFInput::KeyInputCallback
{
public:
    TestCharacterCallback(CharacterEntityController *controller)
    {
        mController = controller;
    }

    virtual void call(bool down, unsigned int keyCode) override
    {
        switch (keyCode)
        {
            case 12: mController->moveForward(); break;
            case 14: mController->moveBackward(); break;
            case 13: mController->moveLeft(); break;
            case 15: mController->moveRight(); break;
            case 18: mController->jump(); break;
            default: break;
        }

    }

protected:
    CharacterEntityController *mController;
};

TestCharacterController::TestCharacterController(SpatialEntity *entity, MFInput::InputManager *inputManager):
    CharacterEntityController(entity)
{
    std::shared_ptr<TestCharacterCallback> cb = std::make_shared<TestCharacterCallback>(this);
    inputManager->addKeyCallback(cb);
}

bool CharacterEntityController::isOnGround()
{
    // TODO: implement this
    return true;
}

CharacterEntityController::CharacterEntityController(SpatialEntity *entity): EntityController(entity)
{
    mMovementState = WALK;
    setSpeeds(0.5,1.0,2.0);
}

void CharacterEntityController::moveLeft()
{
    if (isOnGround())
        EntityController::moveLeft();
}

void CharacterEntityController::moveRight()
{
    if (isOnGround())
        EntityController::moveRight();
}

void CharacterEntityController::moveForward()
{
    if (isOnGround())
        EntityController::moveForward();
}

void CharacterEntityController::moveBackward()
{
    if (isOnGround())
        EntityController::moveBackward();
}

void CharacterEntityController::move(MFMath::Vec3 offset)
{
    float speed = mSpeeds[mMovementState];
    auto fin = offset * speed;

    EntityController::move(fin);
}

void CharacterEntityController::jump()
{
    if (isOnGround())
        EntityController::moveUp();
}

}
