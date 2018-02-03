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
    // Cast a ray downwards and measure the distance.
    double distance = mPhysicsWorld->castRay(mEntity->getPosition(),MFMath::Vec3(0,0,-1));
    return distance < mEntity->getSize().z / 2.0 + 0.1;
}

void CharacterEntityController::applyCurrentMotion()
{
    MFMath::Vec3 movementVec = mMovementVector * mSpeeds[mMovementState];
    movementVec.z = mEntity->getVelocity().z;
    setRelativeVelocityVector(movementVec);
}

CharacterEntityController::CharacterEntityController(SpatialEntity *entity, MFPhysics::PhysicsWorld *physicsWorld): EntityController(entity)
{
    mPhysicsWorld = physicsWorld;
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
