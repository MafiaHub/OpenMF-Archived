#include <controllers/character_entity_controller.hpp>
#include <iostream>

namespace MFGame
{

void TestCharacterController::update(double dt)
{
    std::cout << "uuuuuuu"<< std::endl;
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
