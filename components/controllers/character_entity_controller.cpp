#include <controllers/character_entity_controller.hpp>

namespace MFGame
{

void CharacterEntityController::instantRotate(MFMath::Quat newRotation)
{
    mEntity->setRotation(newRotation);
    applyCurrentMotion();
}

bool CharacterEntityController::isOnGround()
{
    // TODO: implement this
    return true;
}

void CharacterEntityController::applyCurrentMotion()
{
    MFMath::Vec3 movementVec = mMovementVector * mSpeeds[mMovementState];
    movementVec.z = mEntity->getVelocity().z;
    setRelativeVelocityVector(movementVec);
}

CharacterEntityController::CharacterEntityController(SpatialEntity *entity): EntityController(entity)
{
    mMovementState = WALK;
    mMovementVector = MFMath::Vec3(0,0,0);
    setSpeeds(5,10.0,20);
}

void CharacterEntityController::moveLeft(bool start)
{
    mMovementVector.x = start ? -1 : 0;
    applyCurrentMotion();
}

void CharacterEntityController::moveRight(bool start)
{
    mMovementVector.x = start ? 1 : 0;
    applyCurrentMotion();
}

void CharacterEntityController::moveForward(bool start)
{
    mMovementVector.y = start ? 1 : 0;
    applyCurrentMotion();
}

void CharacterEntityController::moveBackward(bool start)
{
    mMovementVector.y = start ? -1 : 0;
    applyCurrentMotion();
}

void CharacterEntityController::jump()
{
    if (isOnGround())
        mEntity->setVelocity( mEntity->getVelocity() + MFMath::Vec3(0,0,10) );
}

}
