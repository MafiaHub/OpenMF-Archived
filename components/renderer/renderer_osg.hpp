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

bool OSGRenderer::loadMission(std::string mission)
{
    std::string missionDir = "../mafia/MISSIONS/" + mission;  // temporarily hard-coded, solve this with VFS?
    std::string textureDir = "../mafia/MAPS/";
    std::string scene4dsPath = missionDir + "/scene.4ds";

    MFFormat::Loader loader;

    osg::ref_ptr<osg::Group> g = new osg::Group();

    std::ifstream f;
    f.open(scene4dsPath, std::ios::binary);

    if (!f.is_open())
    {
        MFLogger::ConsoleLogger::fatal("Could not open file " + scene4dsPath + ".");
        return false;
    }

    loader.setTextureDir(textureDir);
    osg::ref_ptr<osg::Node> n = loader.load4ds(f);
    g->addChild(n);

    mRootNode = g;

    f.close();

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
