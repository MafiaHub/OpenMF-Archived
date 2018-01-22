#ifndef RENDERER_BASE_H
#define RENDERER_BASE_H

#include <fstream>
#include <math.hpp>

namespace MFRender
{

class MFRenderer
{
public:
    MFRenderer();
    virtual bool loadMission(std::string mission, bool load4ds=true, bool loadScene2Bin=true, bool loadCacheBin=true)=0;
    virtual bool loadSingleModel(std::string model)=0;
    virtual void frame(double dt)=0;
    virtual void setCameraParameters(bool perspective, float fov, float orthoSize, float nearDist, float farDist)=0;
    virtual void getCameraPositionRotation(double &x, double &y, double &z, double &yaw, double &pitch, double &roll)=0;
    virtual void setCameraPositionRotation(double x, double y, double z, double yaw, double pitch, double roll)=0;
    virtual void setFreeCameraSpeed(double newSpeed)=0;
    bool done() { return mDone; };
    virtual bool exportScene(std::string fileName)=0;
    virtual int getSelectedEntityId()=0;

    MFMath::Vec3 getCameraPosition() 
    { 
        double x,y,z,ya,pi,ro; getCameraPositionRotation(x,y,z,ya,pi,ro);
        return MFMath::Vec3((float)x, (float)y, (float)z);
    };

  protected:
    bool mDone;
};

MFRenderer::MFRenderer():
    mDone(false)
{
}

}

#endif
