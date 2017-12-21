#ifndef RENDERER_OSG_H
#define RENDERER_OSG_H

#include <renderer/base_renderer.hpp>
#include <osg/Node>
#include <osgDB/ReadFile>
#include <osgViewer/Viewer>
#include <4ds/osg.hpp>
#include <scene2_bin/osg.hpp>
#include <cache_bin/osg.hpp>
#include <osg/Texture2D>
#include <osg/LightModel>
#include <loggers/console.hpp>
#include <osgGA/TrackballManipulator>

namespace MFRender

{

class OSGRenderer: public MFRenderer
{
public:
    OSGRenderer();
    virtual bool loadMission(std::string mission) override;
    virtual bool loadSingleModel(std::string model) override;

    virtual void frame() override;
    virtual void setCameraParameters(bool perspective, float fov, float orthoSize, float nearDist, float farDist) override;

    virtual void getCameraPositionRotation(double &x, double &y, double &z, double &yaw, double &pitch, double &roll) override;
    virtual void setCameraPositionRotation(double x, double y, double z, double yaw, double pitch, double roll) override;

    virtual void setFreeCameraSpeed(double newSpeed) override;

protected:
    osg::ref_ptr<osgViewer::Viewer> mViewer;    
    osg::ref_ptr<osg::Group> mRootNode;          ///< root node of the whole scene being rendered
    MFFile::FileSystem *mFileSystem;
    MFUtil::WalkManipulator *mCameraManipulator;
};

void OSGRenderer::getCameraPositionRotation(double &x, double &y, double &z, double &yaw, double &pitch, double &roll)
{
    osg::Matrixd viewMatrix = mViewer->getCamera()->getViewMatrix();

    viewMatrix.invert(viewMatrix);

    osg::Vec3f translation,scale;
    osg::Quat rotation, scaleOrient;

    viewMatrix.decompose(translation,rotation,scale,scaleOrient);

    x = translation.x();
    y = translation.y();
    z = translation.z();

    MFUtil::quatToEuler(rotation,yaw,pitch,roll);
}

void OSGRenderer::setCameraPositionRotation(double x, double y, double z, double yaw, double pitch, double roll)
{
    osg::Matrixd viewMatrix;

    viewMatrix.setTrans(osg::Vec3(x,y,z));

    viewMatrix.setRotate( MFUtil::eulerToQuat(yaw,pitch,roll) );
    mViewer->getCameraManipulator()->setByMatrix(viewMatrix);
}

OSGRenderer::OSGRenderer(): MFRenderer()
{
    MFLogger::ConsoleLogger::info("initiating OSG renderer");
    mViewer = new osgViewer::Viewer();
                
    mFileSystem = MFFile::FileSystem::getInstance();

mFileSystem->addPath("../mafia/");

    //mViewer->getCamera()->setComputeNearFarMode( osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR );  // not working?
    mViewer->getCamera()->setComputeNearFarMode( osg::CullSettings::COMPUTE_NEAR_FAR_USING_PRIMITIVES );
    mViewer->getCamera()->setNearFarRatio(0.0001);

    mViewer->setReleaseContextAtEndOfFrameHint(false);

    mRootNode = new osg::Group();
    mViewer->setSceneData(mRootNode);

    mViewer->setUpViewInWindow(0,0,1024,768); 

    if (!mViewer->isRealized())
        mViewer->realize();

    osg::ref_ptr<MFUtil::WalkManipulator> cameraManipulator = new MFUtil::WalkManipulator();

    mCameraManipulator = cameraManipulator.get();

    if (!mViewer->getCameraManipulator() && mViewer->getCamera()->getAllowEventFocus())
        mViewer->setCameraManipulator(cameraManipulator);
}

void OSGRenderer::setFreeCameraSpeed(double newSpeed)
{
    mCameraManipulator->setMaxVelocity(newSpeed);
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
     std::string missionDir = "missions/" + mission;
    std::string scene4dsPath = missionDir + "/scene.4ds";
    std::string scene2BinPath = missionDir + "/scene2.bin";
    std::string cacheBinPath = missionDir + "/cache.bin";

    MFFormat::OSG4DSLoader l4ds;
    MFFormat::OSGScene2BinLoader lScene2;
    MFFormat::OSGCacheBinLoader lCache;

    std::ifstream file4DS;
    std::ifstream fileScene2Bin;
    std::ifstream fileCacheBin;

    if (!mFileSystem->open(file4DS,scene4dsPath))
        MFLogger::ConsoleLogger::warn("Couldn not open 4ds file: " + scene4dsPath + ".");

    if (!mFileSystem->open(fileScene2Bin,scene2BinPath))
        MFLogger::ConsoleLogger::warn("Couldn not open scene2.bin file: " + scene2BinPath + ".");

    mRootNode->addChild( l4ds.load(file4DS) );
    mRootNode->addChild( lScene2.load(fileScene2Bin) );

    if(mFileSystem->open(fileCacheBin,cacheBinPath)) 
    {
        mRootNode->addChild(lCache.load(fileCacheBin));
        fileCacheBin.close();
    }

    file4DS.close();
    fileScene2Bin.close();

    return true;
}

bool OSGRenderer::loadSingleModel(std::string model)
{
    std::ifstream file4DS;
    MFFormat::OSG4DSLoader l4ds;

    if (!mFileSystem->open(file4DS,"models/" + model))
    {
        MFLogger::ConsoleLogger::warn("Couldn not open 4ds file: " + model + ".");
        return false;
    }
     
    mRootNode->addChild( l4ds.load(file4DS) );

    file4DS.close();

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
