#ifndef CAMERA_CONTROLLERS_H
#define CAMERA_CONTROLLERS_H

#include <renderer/base_renderer.hpp>
#include <input/base_input_manager.hpp>
#include <utils/math.hpp>
#include <spatial_entity/spatial_entity.hpp>

namespace MFGame
{

class CameraController
{
public:
    virtual ~CameraController() {};
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
    virtual ~MouseRotateCameraController() {};
    void setRotationSpeed(double speed) { mRotationSpeed = speed; };
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
    OrbitEntityCameraController(MFRender::Renderer *renderer, MFInput::InputManager *inputManager, SpatialEntity *entity);
    virtual ~OrbitEntityCameraController() {};

protected:
    virtual void applyRotation() override;

    SpatialEntity *mEntity;
    float mMaxCameraDistance;
};

class FreeCameraController: public MouseRotateCameraController
{
public:
    FreeCameraController(MFRender::Renderer *renderer, MFInput::InputManager *inputManager);
    virtual ~FreeCameraController() {};
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
    RigidCameraController(MFRender::Renderer *renderer, MFInput::InputManager *inputManager, SpatialEntity *entity);
    virtual ~RigidCameraController() {};

protected:
    virtual void handleMovement(MFMath::Vec3 offset) override;
    SpatialEntity *mCameraEntity;
};

}

#endif
