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

#define OSGRENDERER_MODULE_STR "renderer"

namespace MFRender

{

class OSGRenderer: public MFRenderer
{
public:
    OSGRenderer();
    virtual bool loadMission(std::string mission, bool load4ds=true, bool loadScene2Bin=true, bool loadCacheBin=true) override;
    virtual bool loadSingleModel(std::string model) override;
    virtual void frame() override;
    virtual void setCameraParameters(bool perspective, float fov, float orthoSize, float nearDist, float farDist) override;
    virtual void getCameraPositionRotation(double &x, double &y, double &z, double &yaw, double &pitch, double &roll) override;
    virtual void setCameraPositionRotation(double x, double y, double z, double yaw, double pitch, double roll) override;
    virtual void setFreeCameraSpeed(double newSpeed) override;
    virtual bool exportScene(std::string fileName) override;

protected:
    osg::ref_ptr<osgViewer::Viewer> mViewer;    
    osg::ref_ptr<osg::Group> mRootNode;          ///< root node of the whole scene being rendered
    MFFile::FileSystem *mFileSystem;
    MFUtil::WalkManipulator *mCameraManipulator;
    MFFormat::LoaderCache<MFFormat::OSGLoader::OSGCached> mLoaderCache;

    void optimize();
    void logCacheStats();

    /**
      Given a list of light sources in the scene, the function decides which scene node should be lit by which
      light and applies this decision to the lights and scene, adds a defaults light if there is none etc.
     */

    void setUpLights(std::vector<osg::ref_ptr<osg::LightSource>> *lightNodes);
};

bool OSGRenderer::exportScene(std::string fileName)
{
    const osg::Node *n = mRootNode.get();
    auto result = osgDB::Registry::instance()->writeNode(*n,fileName,NULL);
    return result.success();
}

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
    MFLogger::ConsoleLogger::info("initiating OSG renderer", OSGRENDERER_MODULE_STR);
    mViewer = new osgViewer::Viewer();
                
    mFileSystem = MFFile::FileSystem::getInstance();

mFileSystem->addPath("../mafia/");    // drummy: I need this here, remove later

    mViewer->getCamera()->setComputeNearFarMode(osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);

    mViewer->setReleaseContextAtEndOfFrameHint(false);

    osg::ref_ptr<osgViewer::StatsHandler> statshandler = new osgViewer::StatsHandler;
    statshandler->setKeyEventTogglesOnScreenStats(osgGA::GUIEventAdapter::KEY_F3);
    statshandler->setKeyEventPrintsOutStats(osgGA::GUIEventAdapter::KEY_F4);
    mViewer->addEventHandler(statshandler);

    mRootNode = new osg::Group();
    mRootNode->setName("root");

    mViewer->setSceneData(mRootNode);

    mViewer->setUpViewInWindow(40,40,1024,768); 

    if (!mViewer->isRealized())
        mViewer->realize();

    osg::ref_ptr<MFUtil::WalkManipulator> cameraManipulator = new MFUtil::WalkManipulator();

    mCameraManipulator = cameraManipulator.get();

    if (!mViewer->getCameraManipulator() && mViewer->getCamera()->getAllowEventFocus())
        mViewer->setCameraManipulator(cameraManipulator);

    mViewer->getCamera()->setCullingMode(
        osg::CullSettings::VIEW_FRUSTUM_CULLING |
        osg::CullSettings::NEAR_PLANE_CULLING |
        osg::CullSettings::FAR_PLANE_CULLING |
        osg::CullSettings::VIEW_FRUSTUM_SIDES_CULLING |
        osg::CullSettings::SMALL_FEATURE_CULLING
        );

    mViewer->getCamera()->setSmallFeatureCullingPixelSize(15);
}

void OSGRenderer::setFreeCameraSpeed(double newSpeed)
{
    mCameraManipulator->setMaxVelocity(newSpeed);
}

void OSGRenderer::setCameraParameters(bool perspective, float fov, float orthoSize, float nearDist, float farDist)
{
    // FIXME: looks like near/far setting doesn't work - OSG automatically computes them from viewport - turn it off

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

bool OSGRenderer::loadMission(std::string mission, bool load4ds, bool loadScene2Bin, bool loadCacheBin)
{
    std::string missionDir = "missions/" + mission;
    std::string scene4dsPath = missionDir + "/scene.4ds";
    std::string scene2BinPath = missionDir + "/scene2.bin";
    std::string cacheBinPath = missionDir + "/cache.bin";
    std::string checkBinPath = missionDir + "/check.bin";

    MFFormat::OSG4DSLoader l4ds;
    MFFormat::OSGScene2BinLoader lScene2;
    MFFormat::OSGCacheBinLoader lCache;
    MFFormat::OSGCheckBinLoader lCheck;

    l4ds.setLoaderCache(&mLoaderCache);
    lScene2.setLoaderCache(&mLoaderCache);
    lCache.setLoaderCache(&mLoaderCache);

    std::ifstream file4DS;
    std::ifstream fileScene2Bin;
    std::ifstream fileCacheBin;
    std::ifstream fileCheckBin;

    if (load4ds && mFileSystem->open(file4DS,scene4dsPath))
    {
        osg::ref_ptr<osg::Node> n = l4ds.load(file4DS);

        if (!n)
            MFLogger::ConsoleLogger::warn("Couldn't not parse 4ds file: " + scene4dsPath + ".", OSGRENDERER_MODULE_STR);
        else
            mRootNode->addChild(n);

        file4DS.close();
    }
    else if (load4ds) // each mission must have 4ds file, therefore not opening means warning
        MFLogger::ConsoleLogger::warn("Couldn't not open 4ds file: " + scene4dsPath + ".", OSGRENDERER_MODULE_STR);

    bool lightsAreSet = false;

    if (loadScene2Bin && mFileSystem->open(fileScene2Bin,scene2BinPath))
    {
        osg::ref_ptr<osg::Node> n = lScene2.load(fileScene2Bin);

        if (!n)
            MFLogger::ConsoleLogger::warn("Couldn't not parse scene2.bin file: " + scene2BinPath + ".", OSGRENDERER_MODULE_STR);
        else
        {
            mRootNode->addChild(n);
            std::vector<osg::ref_ptr<osg::LightSource>> lightNodes = lScene2.getLightNodes();
            setUpLights(&lightNodes);
            lightsAreSet = true;
        }

        fileScene2Bin.close();
    }

    if (!lightsAreSet)
        setUpLights(0);

    if (loadCacheBin && mFileSystem->open(fileCacheBin,cacheBinPath)) 
    {
        osg::ref_ptr<osg::Node> n = lCache.load(fileCacheBin);

        if (!n)
            MFLogger::ConsoleLogger::warn("Couldn't not parse cache.bin file: " + cacheBinPath + ".", OSGRENDERER_MODULE_STR);
        else
            mRootNode->addChild(n);

        fileCacheBin.close();
    }

    //NOTE(DavoSK): Only for debug 
    if(mFileSystem->open(fileCheckBin,checkBinPath))
    {
        osg::ref_ptr<osg::Node> n = lCheck.load(fileCheckBin);
        if (!n)
            MFLogger::ConsoleLogger::warn("Couldn't not parse check.bin file: " + checkBinPath + ".", OSGRENDERER_MODULE_STR);
        else
            mRootNode->addChild(n);

        fileCheckBin.close();    
    }

    osg::ref_ptr<osg::Fog> fog = new osg::Fog;

    fog->setMode(osg::Fog::LINEAR);
    fog->setStart(300);
    fog->setEnd(1500);
    fog->setColor(osg::Vec4f(0.4,0.4,0.4,1));

    mRootNode->getOrCreateStateSet()->setAttributeAndModes(fog,osg::StateAttribute::ON);

    optimize();

    logCacheStats();

    return true;
}

void OSGRenderer::optimize()
{
    // TODO(drummy): I went crazy with optimization, but this will probably
    // need to be changed once we want to have dynamic objects etc.

    MFLogger::ConsoleLogger::info("optimizing", OSGRENDERER_MODULE_STR);

    osgUtil::Optimizer::FlattenStaticTransformsVisitor flattener;
    osgUtil::Optimizer::SpatializeGroupsVisitor sceneBalancer;
    osgUtil::Optimizer::CombineStaticTransformsVisitor transformCombiner;
    osgUtil::Optimizer::RemoveRedundantNodesVisitor redundantRemover;
    osgUtil::Optimizer::RemoveEmptyNodesVisitor emptyRemover;
    osgUtil::Optimizer::StateVisitor stateOptimizer(true,true,true);
    osgUtil::Optimizer::StaticObjectDetectionVisitor staticDetector;
    osgUtil::Optimizer::CopySharedSubgraphsVisitor subgraphCopier;
    osgUtil::Optimizer::MergeGeometryVisitor geometryMerger;
    osgUtil::Optimizer::MergeGeodesVisitor geodesMerger;
    osgUtil::Optimizer::TessellateVisitor tesselator;
    osgUtil::Optimizer::MakeFastGeometryVisitor geometryOptimizer;

    mRootNode->accept(staticDetector);
    mRootNode->accept(redundantRemover);
    mRootNode->accept(emptyRemover);
//    mRootNode->accept(subgraphCopier);
    mRootNode->accept(flattener);
//    mRootNode->accept(transformCombiner);
    mRootNode->accept(geometryOptimizer);
//    mRootNode->accept(tesselator);
//    mRootNode->accept(geometryMerger);
    mRootNode->accept(geodesMerger);
//    mRootNode->accept(stateOptimizer);
    mRootNode->accept(sceneBalancer);
}

bool OSGRenderer::loadSingleModel(std::string model)
{
    std::ifstream file4DS;
    MFFormat::OSG4DSLoader l4ds;

    l4ds.setLoaderCache(&mLoaderCache);

    if (!mFileSystem->open(file4DS,"models/" + model))
    {
        MFLogger::ConsoleLogger::warn("Couldn't not open 4ds file: " + model + ".", OSGRENDERER_MODULE_STR);
        return false;
    }
     
    mRootNode->addChild( l4ds.load(file4DS) );

    file4DS.close();

    optimize();
    setUpLights(0);

    logCacheStats();

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

void OSGRenderer::logCacheStats()
{
    MFLogger::ConsoleLogger::info("cache hits: " + std::to_string(mLoaderCache.getCacheHits()),OSGRENDERER_MODULE_STR);
    MFLogger::ConsoleLogger::info("cache objects total: " + std::to_string(mLoaderCache.getNumObjects()),OSGRENDERER_MODULE_STR);
}

void OSGRenderer::setUpLights(std::vector<osg::ref_ptr<osg::LightSource>> *lightNodes)
{
    if (lightNodes == 0 || lightNodes->size() == 0)
    {
        // no lights, add a default one

        mRootNode->getOrCreateStateSet()->setMode(GL_LIGHT0,osg::StateAttribute::OFF);
        mRootNode->getOrCreateStateSet()->setMode(GL_LIGHT1,osg::StateAttribute::OFF);
        mRootNode->getOrCreateStateSet()->setMode(GL_LIGHT2,osg::StateAttribute::OFF);
        mRootNode->getOrCreateStateSet()->setMode(GL_LIGHT3,osg::StateAttribute::OFF);
        mRootNode->getOrCreateStateSet()->setMode(GL_LIGHT4,osg::StateAttribute::OFF);
        mRootNode->getOrCreateStateSet()->setMode(GL_LIGHT5,osg::StateAttribute::OFF);
        mRootNode->getOrCreateStateSet()->setMode(GL_LIGHT6,osg::StateAttribute::OFF);
        mRootNode->getOrCreateStateSet()->setMode(GL_LIGHT7,osg::StateAttribute::OFF);

        osg::ref_ptr<osg::LightSource> defaultLight = new osg::LightSource;

        defaultLight->getLight()->setPosition( osg::Vec4f(1,1,1,0) );  // w = 0 => directional light
        defaultLight->getLight()->setLightNum(0);

        defaultLight->getLight()->setAmbient(osg::Vec4f(0.5,0.5,0.5,1));
        defaultLight->getLight()->setDiffuse(osg::Vec4f(1,1,1,1));

        mRootNode->addChild(defaultLight);
        mRootNode->getOrCreateStateSet()->setAttributeAndModes(defaultLight->getLight(),osg::StateAttribute::ON);
    }
    else
    {
        unsigned int lightNum = 0;

        for (auto i = 0; i < lightNodes->size(); ++i)
        {
            if (lightNum > 7)     // fixed pipeline only supports 8 lights
                break;

            std::string lightTypeStr = (*lightNodes)[i]->getName();

            // for now only add global lights, i.e. directional and ambient
            if (lightTypeStr.compare("directional") == 0 ||
                lightTypeStr.compare("ambient") == 0)
            {
                MFLogger::ConsoleLogger::info("Adding " + lightTypeStr + " light.",OSGRENDERER_MODULE_STR);
                (*lightNodes)[i]->getLight()->setLightNum(lightNum);
                mRootNode->getOrCreateStateSet()->setAttributeAndModes((*lightNodes)[i]->getLight());
                lightNum++;
            }
            else
                (*lightNodes)[i]->getLight()->setLightNum(0);
        }
    }
}

}

#endif
