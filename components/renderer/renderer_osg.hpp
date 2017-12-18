#ifndef RENDERER_OSG_H
#define RENDERER_OSG_H

#include <renderer/renderer_base.hpp>
#include <osg/Node>
#include <osgDB/ReadFile>
#include <osgViewer/Viewer>
#include <osg_loader.hpp>
#include <4ds/parser.hpp>
#include <scene2_bin/parser.hpp>
#include <osgDB/ReadFile>
#include <osg/Texture2D>
#include <osg/LightModel>
#include <loggers/console.hpp>
#include <renderer/renderer_osg.hpp>
#include <osgGA/TrackballManipulator>

namespace MFRender

{

class OSGRenderer: public MFRenderer
{
public:
    OSGRenderer();
    virtual bool loadMission(std::string mission) override;
    virtual void frame() override;
    virtual void setCameraParameters(bool perspective, float fov, float orthoSize) override;

protected:
    osg::ref_ptr<osgViewer::Viewer> mViewer;    
    osg::ref_ptr<osg::Node> mRootNode;            ///< root node of the whole scene being rendered
};

OSGRenderer::OSGRenderer(): MFRenderer()
{
    MFLogger::ConsoleLogger::info("initiating OSG renderer");
    mViewer = new osgViewer::Viewer();

    mViewer->setReleaseContextAtEndOfFrameHint(false);

    if (!mViewer->isRealized())
        mViewer->realize();

    if (!mViewer->getCameraManipulator() && mViewer->getCamera()->getAllowEventFocus())
        mViewer->setCameraManipulator(new osgGA::TrackballManipulator());
}

void OSGRenderer::setCameraParameters(bool perspective, float fov, float orthoSize)
{
    osg::Camera *camera = mViewer->getCamera();

    if (perspective)
    {
        double fovy, aspect, znear, zfar;
        camera->getProjectionMatrixAsPerspective(fovy,aspect,znear,zfar);
        camera->setProjectionMatrixAsPerspective(fov,aspect,znear,zfar);
    }
    else
    {
        // TODO: implement this
    }
}

bool OSGRenderer::loadMission(std::string mission)
{
    std::string missionDir = "../mafia/MISSIONS/" + mission;  // temporarily hard-coded, solve this with VFS?
    std::string textureDir = "../mafia/MAPS/";
    std::string scene4dsPath = missionDir + "/scene.4ds";
    std::string scene2BinPath = missionDir + "/scene2.bin";

    MFFormat::Loader loader;

    osg::ref_ptr<osg::Group> g = new osg::Group();

    std::ifstream f, f2;
    f.open(scene4dsPath, std::ios::binary);

    if (!f.is_open())    // TODO: make a special methof somewhere to load by file names
    {
        MFLogger::ConsoleLogger::fatal("Could not open file " + scene4dsPath + ".");
        return false;
    }

    f2.open(scene2BinPath, std::ios::binary);

    if (!f2.is_open())
    {
        MFLogger::ConsoleLogger::fatal("Could not open file " + scene2BinPath + ".");
        f.close();
        return false;
    }

    loader.setTextureDir(textureDir);
    g->addChild( loader.load4ds(f) );
    g->addChild( loader.loadScene2Bin(f2) );

    mRootNode = g;

    f.close();
    f2.close();

    mViewer->setSceneData(mRootNode);

    return true;
}

void OSGRenderer::frame()
{
    if (mViewer->done() || mDone)
    {
        mDone = true;
    }
    else
    {
        mViewer->advance(0.1);
        mViewer->eventTraversal();
        mViewer->updateTraversal();
        mViewer->renderingTraversals();
    }
}

}

#endif
