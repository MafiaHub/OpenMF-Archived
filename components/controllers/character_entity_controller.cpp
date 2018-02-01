#include <controllers/character_entity_controller.hpp>

namespace MFGame
{

bool CharacterEntityController::isOnGround()
{
    // TODO: implement this
    return true;
}

CharacterEntityController::CharacterEntityController(SpatialEntity *entity): EntityController(entity)
{
    mMovementState = WALK;
    mMovementVector = MFMath::Vec3(0,0,0);
    setSpeeds(0.5,1.0,2.0);
}

void CharacterEntityController::moveLeft(bool start)
{
    mMovementVector.x = start ? -1 : 0;
    setRelativeVelocityVector(mMovementVector * mSpeeds[WALK]);
}

void CharacterEntityController::moveRight(bool start)
{
    mMovementVector.x = start ? 1 : 0;
    setRelativeVelocityVector(mMovementVector * mSpeeds[WALK]);
}

void CharacterEntityController::moveForward(bool start)
{
    mMovementVector.y = start ? 1 : 0;
    setRelativeVelocityVector(mMovementVector * mSpeeds[WALK]);
}

void CharacterEntityController::moveBackward(bool start)
{
    mMovementVector.y = start ? -1 : 0;
    setRelativeVelocityVector(mMovementVector * mSpeeds[WALK]);
}

void CharacterEntityController::jump()
{
}

}
