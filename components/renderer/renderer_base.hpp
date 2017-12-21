#ifndef RENDERER_BASE_H
#define RENDERER_BASE_H

namespace MFRender
{

class MFRenderer
{
public:
    MFRenderer();
    virtual bool loadMission(std::string mission)=0;
    virtual void frame()=0;
    virtual void setCameraParameters(bool perspective, float fov, float orthoSize, float nearDist, float farDist)=0;

    virtual void getCameraPositionRotation(double &x, double &y, double &z, double &yaw, double &pitch, double &roll)=0;
    virtual void setCameraPositionRotation(double x, double y, double z, double yaw, double pitch, double roll)=0;

    bool done() { return mDone; };

protected:
    bool mDone;
};

MFRenderer::MFRenderer():
    mDone(false)
{
}

}

#endif
