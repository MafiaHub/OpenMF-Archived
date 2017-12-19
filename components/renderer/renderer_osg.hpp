#ifndef RENDERER_OSG_H
#define RENDERER_OSG_H

#include <renderer/renderer_base.hpp>
#include <osg/Node>
#include <osgDB/ReadFile>
#include <osgViewer/Viewer>
#include <4ds/osg.hpp>
#include <4ds/parser.hpp>
#include <scene2_bin/parser.hpp>
#include <scene2_bin/osg.hpp>
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
    osg::ref_ptr<osg::MatrixTransform> mRootNode;            ///< root node of the whole scene being rendered
};

OSGRenderer::OSGRenderer(): MFRenderer()
{
    MFLogger::ConsoleLogger::info("initiating OSG renderer");
    mViewer = new osgViewer::Viewer();

    mViewer->setReleaseContextAtEndOfFrameHint(false);

    mRootNode = new osg::MatrixTransform();
    mRootNode->setMatrix( osg::Matrixd::scale(osg::Vec3f(1,1,-1)) );
    mViewer->setSceneData(mRootNode);

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

    MFFormat::OSG4DSLoader l4ds;
    MFFormat::OSGScene2BinLoader lScene2;

    l4ds.setTextureDir(textureDir);
    mRootNode->addChild( l4ds.loadFile(scene4dsPath) );
    mRootNode->addChild( lScene2.loadFile(scene2BinPath) );

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
