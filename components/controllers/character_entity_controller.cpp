#include <controllers/character_entity_controller.hpp>

namespace MFGame
{

void CharacterEntityController::moveLeft()
{
    // TODO check if we can move (if we're not standing at ground, don't move)
    EntityController::moveLeft();
}

void CharacterEntityController::moveRight()
{
    // TODO check if we can move (if we're not standing at ground, don't move)
    EntityController::moveRight();
}

void CharacterEntityController::moveForward()
{
    // TODO check if we can move (if we're not standing at ground, don't move)
    EntityController::moveForward();
}

void CharacterEntityController::moveBackward()
{
    // TODO check if we can move (if we're not standing at ground, don't move)
    EntityController::moveBackward();
}

void CharacterEntityController::move(MFMath::Vec3 offset)
{
    float speed = mSpeed[mMoveBehavior];
    auto fin = offset * speed;

    EntityController::move(fin);
}

void CharacterEntityController::jump()
{
}

void CharacterEntityController::setCrouch(bool state)
{
    // TODO deal with this
}

}
