#ifndef FREE_CAMERA_CONTROLLER
#define FREE_CAMERA_CONTROLLER

#include <controllers/camera_controller.hpp>
#include <utils/math.hpp>

namespace MFGame
{

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

}

#endif
