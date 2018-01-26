#ifndef RENDERER_OSG_H
#define RENDERER_OSG_H

#include <renderer/base_renderer.hpp>
#include <osg/Node>
#include <osgDB/ReadFile>
#include <osgViewer/Viewer>
#include <4ds/osg.hpp>
#include <scene2_bin/osg.hpp>
#include <cache_bin/osg.hpp>
#include <check_bin/osg.hpp>
#include <osg/Texture2D>
#include <osg/LightModel>
#include <loggers/console.hpp>
#include <osgGA/TrackballManipulator>
#include <loader_cache.hpp>
#include <osgViewer/ViewerEventHandlers>
#include <osgUtil/Optimizer>
#include <osg/Fog>
#include <osgUtil/PrintVisitor>
#include <vfs/vfs.hpp>

#define OSGRENDERER_MODULE_STR "renderer"

namespace MFRender
{

class OSGRenderer: public Renderer
{
public:
    OSGRenderer();
    virtual bool loadMission(std::string mission, bool load4ds=true, bool loadScene2Bin=true, bool loadCacheBin=true) override;
    virtual bool loadSingleModel(std::string model) override;
    virtual void frame(double dt) override;
    virtual void setCameraParameters(bool perspective, float fov, float orthoSize, float nearDist, float farDist) override;
    virtual void getCameraPositionRotation(MFMath::Vec3 &position, MFMath::Vec3 &rotYawPitchRoll) override;
    virtual void setCameraPositionRotation(MFMath::Vec3 position, MFMath::Vec3 rotYawPitchRoll) override;
    virtual void getCameraVectors(MFMath::Vec3 &forw, MFMath::Vec3 &right, MFMath::Vec3 &up) override;
    virtual void setWindowSize(unsigned int width, unsigned int height) override;
    virtual bool exportScene(std::string fileName) override;
    virtual void debugClick(unsigned int x, unsigned int y) override;
    virtual int  getSelectedEntityId() override;
    virtual void showProfiler() override;

    /**
      Sets a custom viewer window to display the rendering. Do not use this if setUpInWindow is called.
    */
    void setUpInCustomWindow(unsigned int width, unsigned int height);
    void setUpInWindow(osgViewer::GraphicsWindow *window);

    void setRenderMask(osg::Node::NodeMask mask);
    osg::Group *getRootNode()                      { return mRootNode.get(); };
    osgViewer::Viewer *getViewer()                 { return mViewer.get();   };

protected:
    osg::ref_ptr<osgViewer::Viewer> mViewer;    
    osg::ref_ptr<osg::Group> mRootNode;            ///< root node of the whole scene being rendered
    MFFile::FileSystem *mFileSystem;
    MFFormat::LoaderCache<MFFormat::OSGLoader::OSGCached> mLoaderCache;

    void optimize();

    /**
      Given a list of light sources in the scene, the function decides which scene node should be lit by which
      light and applies this decision to the lights and scene, adds a defaults light if there is none etc.
     */

    void setUpLights(std::vector<osg::ref_ptr<osg::LightSource>> *lightNodes);

    osg::ref_ptr<osg::Drawable> mSelected;           ///< debug selection
    osg::ref_ptr<osg::Material> mHighlightMaterial;  ///< for highlighting debug selection
    osg::ref_ptr<osg::Material> mMaterialBackup;

    osg::ref_ptr<osgViewer::StatsHandler> mStatsHandler;
};

}

#endif
