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

class FreeCameraController: public CameraController
{
public:
    FreeCameraController(MFRender::Renderer *renderer, MFInput::InputManager *inputManager);
    virtual void update(double dt) override;

    void setSpeed(double speed)         { mSpeed = speed;         };
    void setRotationSpeed(double speed) { mRotationSpeed = speed; };

protected:
    virtual void handleMovement(MFMath::Vec3 offset, MFMath::Vec3 angOffset);

    double mPreviousMouseButton;

    bool mInitTransform;

    MFMath::Vec3 mPosition;
    MFMath::Vec3 mRotation;    // Euler angles

    unsigned int mKeyForward;
    unsigned int mKeyBackward;
    unsigned int mKeyRight;
    unsigned int mKeyLeft;
    unsigned int mKeyUp;
    unsigned int mKeyDown;
    unsigned int mKeySpeedup;

    double mSpeed;
    double mRotationSpeed;
};

class RigidCameraController: public FreeCameraController
{
public:
    RigidCameraController(MFRender::Renderer *renderer, MFInput::InputManager *inputManager, SpatialEntity *entity);

protected:
    virtual void handleMovement(MFMath::Vec3 offset, MFMath::Vec3 angOffset) override;
    SpatialEntity *mCameraEntity;
};

}

#endif
