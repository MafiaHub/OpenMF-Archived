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
    virtual void setCameraParameters(bool perspective, float fov, float orthoSize, float nearDist, float farDist) override;

protected:
    osg::ref_ptr<osgViewer::Viewer> mViewer;    
    osg::ref_ptr<osg::MatrixTransform> mRootNode;            ///< root node of the whole scene being rendered
};

OSGRenderer::OSGRenderer(): MFRenderer()
{
    MFLogger::ConsoleLogger::info("initiating OSG renderer");
    mViewer = new osgViewer::Viewer();
                
    //mViewer->getCamera()->setComputeNearFarMode( osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR );  // not working?
    mViewer->getCamera()->setComputeNearFarMode( osg::CullSettings::COMPUTE_NEAR_FAR_USING_PRIMITIVES );
    mViewer->getCamera()->setNearFarRatio(0.0001);

    mViewer->setReleaseContextAtEndOfFrameHint(false);

    mRootNode = new osg::MatrixTransform();

    osg::Matrixd m;

    m.makeScale(osg::Vec3f(1,1,-1));
    m.postMult( osg::Matrixd::rotate(osg::PI / 2.0,osg::Vec3f(1,0,0)) );

    mRootNode->setMatrix(m);   // transform the whole scene to OSG space

    mViewer->setSceneData(mRootNode);

    mViewer->setUpViewInWindow(0,0,800,600); 

    if (!mViewer->isRealized())
        mViewer->realize();

    osg::ref_ptr<MFUtil::WalkManipulator> cameraManipulator = new MFUtil::WalkManipulator();

    cameraManipulator->setMaxVelocity(5);

    if (!mViewer->getCameraManipulator() && mViewer->getCamera()->getAllowEventFocus())
        mViewer->setCameraManipulator(cameraManipulator);
}

void OSGRenderer::setCameraParameters(bool perspective, float fov, float orthoSize, float nearDist, float farDist)
{
    osg::Camera *camera = mViewer->getCamera();

    if (perspective)
    {
        double fovY, aspect, zNear, zFar;
        camera->getProjectionMatrixAsPerspective(fovY,aspect,zNear,zFar);
        camera->setProjectionMatrixAsPerspective(fov,aspect,nearDist,farDist);
    }
    else
    {
        // TODO: implement this
    }
}

bool OSGRenderer::loadMission(std::string mission)
{
    std::string missionDir = "MISSIONS/" + mission;  // temporarily hard-coded, solve this with VFS?
    std::string textureDir = "MAPS/";
    std::string scene4dsPath = missionDir + "/scene.4ds";
    std::string scene2BinPath = missionDir + "/scene2.bin";

    MFFormat::OSG4DSLoader l4ds;
    MFFormat::OSGScene2BinLoader lScene2;

l4ds.setBaseDir("../mafia/");  // tmp
lScene2.setBaseDir("../mafia/");

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
