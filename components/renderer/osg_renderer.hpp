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
    osg::ref_ptr<osg::Group> mRootNode;          ///< root node of the whole scene being rendered
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

void OSGRenderer::showProfiler()
{
    // hack: simulate the key press
    osg::ref_ptr<osgGA::GUIEventAdapter> event = new osgGA::GUIEventAdapter;
    event->setEventType(osgGA::GUIEventAdapter::KEYDOWN);
    event->setKey(osgGA::GUIEventAdapter::KEY_F3);
    mStatsHandler->handle(*(event.get()),*(mViewer.get()));
}

void OSGRenderer::setRenderMask(osg::Node::NodeMask mask)
{
    mViewer->getCamera()->setCullMask(mask);
}

bool OSGRenderer::exportScene(std::string fileName)
{
    const osg::Node *n = mRootNode.get();
    auto result = osgDB::Registry::instance()->writeNode(*n,fileName,NULL);
    return result.success();
}

void OSGRenderer::setWindowSize(unsigned int width, unsigned int height)
{
    mViewer->getCamera()->setViewport(0,0,width,height);

    double fovy, aspect, zNear, zFar;

    mViewer->getCamera()->getProjectionMatrixAsPerspective(fovy, aspect, zNear, zFar);
    mViewer->getCamera()->setProjectionMatrixAsPerspective(fovy, width / ((double) height), zNear, zFar);
}

void OSGRenderer::debugClick(unsigned int x, unsigned int y)
{
    osg::ref_ptr<osgUtil::LineSegmentIntersector> intersector =
        new osgUtil::LineSegmentIntersector(osgUtil::Intersector::WINDOW, x, y);

    MFUtil::RobustIntersectionVisitor iv(intersector.get());
    iv.setTraversalMask(mViewer->getCamera()->getCullMask());
    mViewer->getCamera()->accept(iv);

    if (intersector->containsIntersections())
    {
        const osgUtil::LineSegmentIntersector::Intersection result = intersector->getFirstIntersection();

        if (mSelected)
        {
            if (mMaterialBackup)
                mSelected->getOrCreateStateSet()->setAttributeAndModes(mMaterialBackup);
            else
                mSelected->getOrCreateStateSet()->removeAttribute(osg::StateAttribute::MATERIAL);
        }

        if (mSelected == result.drawable)  // clicking the same node twice will deselect it
        {
            mSelected = 0;
        }
        else
        {
            mSelected = result.drawable;
            mMaterialBackup = static_cast<osg::Material *>(mSelected->getOrCreateStateSet()->getAttribute(osg::StateAttribute::MATERIAL));
            mSelected->getOrCreateStateSet()->setAttributeAndModes(mHighlightMaterial);

            MFLogger::ConsoleLogger::info(MFUtil::makeInfoString(result.drawable.get()),OSGRENDERER_MODULE_STR);

            for (int i = 0; i < (int) result.nodePath.size(); ++i)
                MFLogger::ConsoleLogger::info("  " + MFUtil::makeInfoString(result.nodePath[result.nodePath.size() - 1 - i]),OSGRENDERER_MODULE_STR);

            MFLogger::ConsoleLogger::info("------",OSGRENDERER_MODULE_STR);
        }
    }
}

void OSGRenderer::getCameraPositionRotation(MFMath::Vec3 &position, MFMath::Vec3 &rotYawPitchRoll)
{
    osg::Matrixd viewMatrix = mViewer->getCamera()->getViewMatrix();

    viewMatrix.invert(viewMatrix);

    osg::Vec3f translation,scale;
    osg::Quat rotation, scaleOrient;

    viewMatrix.decompose(translation,rotation,scale,scaleOrient);

    position.x = translation.x();
    position.y = translation.y();
    position.z = translation.z();

    double y,p,r;

    MFUtil::quatToEuler(rotation,y,p,r);
    rotYawPitchRoll.x = y;
    rotYawPitchRoll.y = p;
    rotYawPitchRoll.z = r;
}

void OSGRenderer::getCameraVectors(MFMath::Vec3 &forw, MFMath::Vec3 &right, MFMath::Vec3 &up)
{
    osg::Vec3f f,r,u;

    osg::Matrixd cameraMat = mViewer->getCamera()->getInverseViewMatrix();
    cameraMat.preMultRotate(osg::Quat(-osg::PI / 2.0,osg::Vec3f(1,0,0)));   // camera is looking down by default, fix that
    MFUtil::rotationToVectors(cameraMat.getRotate(),f,r,u);

    forw  = MFMath::Vec3(f.x(),f.y(),f.z());
    right = MFMath::Vec3(r.x(),r.y(),r.z());
    up    = MFMath::Vec3(u.x(),u.y(),u.z());
}

void OSGRenderer::setUpInCustomWindow(unsigned int width, unsigned int height)
{
    mViewer->setUpViewInWindow(40,40,width,height); 

    if (!mViewer->isRealized())
        mViewer->realize();
}

void OSGRenderer::setUpInWindow(osgViewer::GraphicsWindow *window)
{
    int x,y,w,h;
    window->getWindowRectangle(x,y,w,h);
    mViewer->getCamera()->setGraphicsContext(window);
    mViewer->getCamera()->setViewport(0,0,w,h);

    if (!mViewer->isRealized())
        mViewer->realize();
}

void OSGRenderer::setCameraPositionRotation(MFMath::Vec3 position, MFMath::Vec3 rotYawPitchRoll)
{
    osg::Matrixd viewMatrix;

    viewMatrix.setTrans(osg::Vec3(position.x,position.y,position.z));
    viewMatrix.setRotate(MFUtil::eulerToQuat(rotYawPitchRoll.x,rotYawPitchRoll.y,rotYawPitchRoll.z));
    viewMatrix.invert(viewMatrix);

    mViewer->getCamera()->setViewMatrix(viewMatrix);
}

int OSGRenderer::getSelectedEntityId()
{
    if (mSelected)
    {
        osg::UserDataContainer *cont = mSelected->getOrCreateUserDataContainer();

        if (cont->getNumUserObjects() > 0)
        {
            osg::Object *o = cont->getUserObject(0);

            if (std::string("UserIntData").compare(o->className()) == 0)
            {
                return ((MFUtil::UserIntData *) o)->mValue;
            }
        }
    }
    
    return -1; 
}

OSGRenderer::OSGRenderer(): Renderer()
{
    MFLogger::ConsoleLogger::info("initiating OSG renderer", OSGRENDERER_MODULE_STR);
    mViewer = new osgViewer::Viewer();
                
    mFileSystem = MFFile::FileSystem::getInstance();

    mViewer->getCamera()->setComputeNearFarMode(osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);

    mViewer->setReleaseContextAtEndOfFrameHint(false);

    mStatsHandler = new osgViewer::StatsHandler;
    mStatsHandler->setKeyEventTogglesOnScreenStats(osgGA::GUIEventAdapter::KEY_F3);
    mViewer->addEventHandler(mStatsHandler);

    mRootNode = new osg::Group();
    mRootNode->setName("root");

    mViewer->setSceneData(mRootNode);

    mViewer->getCamera()->setCullingMode(
        osg::CullSettings::VIEW_FRUSTUM_CULLING |
        osg::CullSettings::NEAR_PLANE_CULLING |
        osg::CullSettings::FAR_PLANE_CULLING |
        osg::CullSettings::VIEW_FRUSTUM_SIDES_CULLING |
        osg::CullSettings::SMALL_FEATURE_CULLING
        );

    mViewer->getCamera()->setSmallFeatureCullingPixelSize(15);
    setRenderMask(MASK_GAME);

    mHighlightMaterial = new osg::Material;
    mHighlightMaterial->setAmbient(osg::Material::FRONT_AND_BACK,osg::Vec4f(0.5,0,0,1));
    mHighlightMaterial->setDiffuse(osg::Material::FRONT_AND_BACK,osg::Vec4f(0.5,0,0,1));
    mHighlightMaterial->setEmission(osg::Material::FRONT_AND_BACK,osg::Vec4f(0.5,0,0,1));
    mMaterialBackup = 0;
    mSelected = 0;
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

    MFFormat::OSGLoader::NodeMap nodeMap;
    l4ds.setNodeMap(&nodeMap);
    lScene2.setNodeMap(&nodeMap);

    if (load4ds && mFileSystem->open(file4DS,scene4dsPath))
    {
        osg::ref_ptr<osg::Node> n = l4ds.load(file4DS);

        if (!n)
            MFLogger::ConsoleLogger::warn("Could not parse 4ds file: " + scene4dsPath + ".", OSGRENDERER_MODULE_STR);
        else
            mRootNode->addChild(n);

        file4DS.close();
    }
    else if (load4ds) // each mission must have 4ds file, therefore not opening means warning
        MFLogger::ConsoleLogger::warn("Could not open 4ds file: " + scene4dsPath + ".", OSGRENDERER_MODULE_STR);

    bool lightsAreSet = false;

    if (loadScene2Bin && mFileSystem->open(fileScene2Bin,scene2BinPath))
    {
        osg::ref_ptr<osg::Node> n = lScene2.load(fileScene2Bin);

        if (!n)
            MFLogger::ConsoleLogger::warn("Could not parse scene2.bin file: " + scene2BinPath + ".", OSGRENDERER_MODULE_STR);
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
            MFLogger::ConsoleLogger::warn("Could not parse cache.bin file: " + cacheBinPath + ".", OSGRENDERER_MODULE_STR);
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

    mLoaderCache.logStats();

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

    mLoaderCache.logStats();

    return true;
}

void OSGRenderer::frame(double /* dt */)
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

        for (int i = 0; i < (int) lightNodes->size(); ++i)
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
