#ifndef RENDERER_BASE_H
#define RENDERER_BASE_H

#include <fstream>
#include <utils/math.hpp>

namespace MFRender
{

class Renderer
{
public:
    Renderer() { mDone = false; };
    virtual ~Renderer() {};
    virtual void frame(double dt)=0;
    virtual void setCameraParameters(bool perspective,  float fov,  float orthoSize,  float nearDist,  float farDist)=0;
    virtual void getCameraParameters(bool &perspective, float &fov, float &orthoSize, float &nearDist, float &farDist)=0;
    virtual void setFog(float distFrom, float distTo, MFMath::Vec3 color)=0;
    virtual void setViewDistance(float dist)=0;       ///< Sets the view distance including fog.
    virtual void getCameraPositionRotation(MFMath::Vec3 &position, MFMath::Vec3 &rotYawPitchRoll)=0;
    virtual void setCameraPositionRotation(MFMath::Vec3 position, MFMath::Vec3 rotYawPitchRoll)=0;
    virtual void getCameraVectors(MFMath::Vec3 &forw, MFMath::Vec3 &right, MFMath::Vec3 &up)=0;
    virtual void setWindowSize(unsigned int width, unsigned int height)=0;
    bool done() { return mDone; };
    virtual bool exportScene(std::string fileName)=0;

    /**
      Call this to indicate the user has debug-clicked at given screen coordinates.
    */
    virtual void debugClick(unsigned int x, unsigned int y)=0;
    virtual int  getSelectedEntityId()=0;
    virtual void showProfiler()=0;

    MFMath::Vec3 getCameraPosition() { MFMath::Vec3 p,r; getCameraPositionRotation(p,r); return p; };
    virtual void cameraFace(MFMath::Vec3 position)=0;

protected:
    bool mDone;
};

}

#endif
