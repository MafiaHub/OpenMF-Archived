#ifndef CAMERA_CONTROLLERS_H
#define CAMERA_CONTROLLERS_H

#include <renderer/base_renderer.hpp>
#include <input/base_input_manager.hpp>
#include <utils/math.hpp>
#include <entity/entity.hpp>
#include <physics/base_physics_world.hpp>

namespace MFGame
{

class CameraController
{
public:
    virtual ~CameraController() = default;
    CameraController(MFRender::Renderer *renderer, MFInput::InputManager *inputManager) { mRenderer = renderer; mInputManager = inputManager; };
    virtual void update(double dt)=0;

protected:
    MFRender::Renderer *mRenderer;
    MFInput::InputManager *mInputManager;
};

class MouseRotateCameraController: public CameraController
{
public:
    MouseRotateCameraController(MFRender::Renderer *renderer, MFInput::InputManager *inputManager);
    virtual ~MouseRotateCameraController() = default;
    void setRotationSpeed(double speed)    { mRotationSpeed = speed; };
    MFMath::Vec2 getRotation()             { return mRotation;       };
    virtual void update(double dt) override;

protected:
    virtual void applyRotation()=0;

    MFMath::Vec2 mRotation;    ///< yaw and pitch
    double mRotationSpeed;

    bool mRotationActive;      ///< Allows to turn the mouse rotation on/off.
    bool mHideCursorOnRotation;

    bool mPreviouslyCentered;  ///< Whether mouse was centered in the previous frame.
    bool mPreviousRotationActive;
};

class OrbitEntityCameraController: public MouseRotateCameraController
{
public:
    /**
      Constructor.

      @param physicsWorld Optional pointer to physics world - if provided, the camera will collide. 
    */

    OrbitEntityCameraController(MFRender::Renderer *renderer, MFInput::InputManager *inputManager, Entity *entity, MFPhysics::PhysicsWorld *physicsWorld=nullptr);
    virtual ~OrbitEntityCameraController() = default;
    Entity *getEntity()                   { return mEntity;           };
    void setRelativeOffset(MFMath::Vec3 offset)  { mRelativeOffset = offset; };

protected:
    MFPhysics::PhysicsWorld *mPhysicsWorld;
    virtual void applyRotation() override;
    MFMath::Vec3 mRelativeOffset;
    Entity *mEntity;
    float mMaxCameraDistance;
};

class FreeCameraController: public MouseRotateCameraController
{
public:
    FreeCameraController(MFRender::Renderer *renderer, MFInput::InputManager *inputManager);
    virtual ~FreeCameraController() = default;
    virtual void update(double dt) override;
    void setSpeed(double speed)         { mSpeed = speed;         };

protected:
    virtual void applyRotation() override;
    virtual void handleMovement(MFMath::Vec3 offset);

    double mPreviousMouseButton;
    bool mInitTransform;

    MFMath::Vec3 mPosition;

    unsigned int mKeyForward;
    unsigned int mKeyBackward;
    unsigned int mKeyRight;
    unsigned int mKeyLeft;
    unsigned int mKeyUp;
    unsigned int mKeyDown;
    unsigned int mKeySpeedup;

    double mSpeed;
};

class RigidCameraController: public FreeCameraController
{
public:
    RigidCameraController(MFRender::Renderer *renderer, MFInput::InputManager *inputManager, Entity *entity);
    virtual ~RigidCameraController() = default;

protected:
    virtual void handleMovement(MFMath::Vec3 offset) override;
    Entity *mCameraEntity;
};

}

#endif
