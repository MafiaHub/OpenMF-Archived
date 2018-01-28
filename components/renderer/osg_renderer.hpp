#ifndef RENDERER_OSG_H
#define RENDERER_OSG_H

#include <memory>
#include <renderer/base_renderer.hpp>

#define OSGRENDERER_MODULE_STR "renderer"

// forwards
namespace osg {
class Group;
} // namespace osg

// forwards
namespace osgViewer {
class Viewer;
class GraphicsWindow;
} // namespace osg

namespace MFRender
{

class OSGRenderer : public Renderer
{
public:
    OSGRenderer();
    ~OSGRenderer();

    bool loadMission(std::string mission, bool load4ds=true, bool loadScene2Bin=true, bool loadCacheBin=true) override;
    bool loadSingleModel(std::string model) override;
    void frame(double dt) override;
    void setCameraParameters(bool perspective, float fov, float orthoSize, float nearDist, float farDist) override;
    void getCameraPositionRotation(MFMath::Vec3 &position, MFMath::Vec3 &rotYawPitchRoll) override;
    void setCameraPositionRotation(MFMath::Vec3 position, MFMath::Vec3 rotYawPitchRoll) override;
    void getCameraVectors(MFMath::Vec3 &forw, MFMath::Vec3 &right, MFMath::Vec3 &up) override;
    void setWindowSize(unsigned int width, unsigned int height) override;
    bool exportScene(std::string fileName) override;
    void debugClick(unsigned int x, unsigned int y) override;
    int  getSelectedEntityId() override;
    void showProfiler() override;

    // specific osg stuff
    osg::Group *getRootNode();
    osgViewer::Viewer *getViewer();
    void setUpInWindow(osgViewer::GraphicsWindow *window);
    void setRenderMask(unsigned mask);

protected:
    class Impl;
    std::unique_ptr<Impl> impl;
};

}

#endif
