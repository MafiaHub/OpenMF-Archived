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

    if (mEmulatesMafiaPhysics) {
        if (movementVec.z > 0.0) movementVec.z *= -1;

        if (!isOnGround() && mIsGrounded) {
            mIsGrounded = false;
            mLastJumpMovementVector = mEntity->getVelocity();
        }
        else if (isOnGround() && !mIsGrounded) {
            mIsGrounded = true;
        }

        if (!mIsGrounded) {
            movementVec.z = -9.81;
            movementVec.x = mLastJumpMovementVector.x;
            movementVec.y = mLastJumpMovementVector.y;
        }
    }

    setRelativeVelocityVector(movementVec);
}

void CharacterEntityController::setMafiaPhysicsEmulation(bool state)
{
    mEmulatesMafiaPhysics = state;
}

CharacterEntityController::CharacterEntityController(Entity *entity, MFPhysics::PhysicsWorld *physicsWorld): EntityController(entity)
{
    mPhysicsWorld = physicsWorld;
    mMovementState = WALK;
    mIsGrounded = false;
    mMovementVector = MFMath::Vec3(0,0,0);
    mLastJumpMovementVector = MFMath::Vec3(0, 0, -9.81);
    setSpeeds(2,5,10);
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
    if (mEmulatesMafiaPhysics) {
        // TODO: Jumping in Mafia is more advanced than it seems
        // Cast a ray to see if we can step up, otherwise perform forward dash		
    }
    else {
        if (isOnGround())
            mEntity->setVelocity(mEntity->getVelocity() + MFMath::Vec3(0, 0, 4));
    }
}
    
}
