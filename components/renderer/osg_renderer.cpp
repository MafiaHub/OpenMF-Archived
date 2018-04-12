#include <renderer/osg_renderer.hpp>

namespace MFRender
{

void OSGRenderer::cameraFace(MFMath::Vec3 position)
{
    osg::Vec3f pos = mViewer->getCamera()->getInverseViewMatrix().getTrans();
    mViewer->getCamera()->setViewMatrixAsLookAt(pos,osg::Vec3f(position.x,position.y,position.z),osg::Vec3f(0,0,1));
}

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
    auto result = osgDB::Registry::instance()->writeNode(*n,fileName,nullptr);
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
            mSelected = nullptr;
        }
        else
        {
            mSelected = result.drawable;
            mMaterialBackup = static_cast<osg::Material *>(mSelected->getOrCreateStateSet()->getAttribute(osg::StateAttribute::MATERIAL));
            mSelected->getOrCreateStateSet()->setAttributeAndModes(mHighlightMaterial);

            MFLogger::Logger::info(MFUtil::makeInfoString(result.drawable.get()),OSGRENDERER_MODULE_STR);

            for (int i = 0; i < (int) result.nodePath.size(); ++i)
                MFLogger::Logger::info("  " + MFUtil::makeInfoString(result.nodePath[result.nodePath.size() - 1 - i]),OSGRENDERER_MODULE_STR);

            MFLogger::Logger::info("------",OSGRENDERER_MODULE_STR);
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
    MFLogger::Logger::info("initiating OSG renderer", OSGRENDERER_MODULE_STR);
    mViewer = new osgViewer::Viewer();
                
    mFileSystem = MFFile::FileSystem::getInstance();

    mViewer->getCamera()->setComputeNearFarMode(osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);

    mViewer->setReleaseContextAtEndOfFrameHint(false);

    mStatsHandler = new osgViewer::StatsHandler;
    mStatsHandler->setKeyEventTogglesOnScreenStats(osgGA::GUIEventAdapter::KEY_F3);
    mViewer->addEventHandler(mStatsHandler);

    mStatsHandler->addUserStatsLine("Physics", osg::Vec4(1.0f, 0.0f, 1.0f, 1.0f),
                                    osg::Vec4(1.0f, 0.0f, 1.0f, 1.0f), "physics_time_taken", 1000.0f, true, false, "physics_time_begin", "physics_time_end", 10000);

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
    mMaterialBackup = nullptr;
    mSelected = nullptr;

    // TODO make this work
    /*mImGuiHandler = new ImGuiHandler(new ImGuiHandler::GuiCallback);
    mImGuiHandler->setCameraCallbacks(mViewer->getCamera());
    mViewer->addEventHandler(mImGuiHandler);*/
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

void OSGRenderer::getCameraParameters(bool &perspective, float &fov, float &orthoSize, float &nearDist, float &farDist)
{
    perspective = true;    // TODO: fix this when ortho is implemented
    double aspect, fovY, nd, fd;
    mViewer->getCamera()->getProjectionMatrixAsPerspective(fovY,aspect,nd,fd);
    
    fov = fovY;
    nearDist = nd;
    farDist = fd;
}

void OSGRenderer::setViewDistance(float dist)
{
    bool p;
    float fov, os, nd, fd;

    getCameraParameters(p,fov,os,nd,fd);

    fd = dist;

    setCameraParameters(p,fov,os,nd,fd); 
    setFog(fd / 3.0,fd,MFMath::Vec3(0.5,0.5,0.5));
}

void OSGRenderer::setFog(float distFrom, float distTo, MFMath::Vec3 color)
{
    osg::ref_ptr<osg::Fog> fog = new osg::Fog;

    fog->setMode(osg::Fog::LINEAR);
    fog->setStart(distFrom);
    fog->setEnd(distTo);
    fog->setColor(osg::Vec4f(color.x,color.y,color.z,1));

    mRootNode->getOrCreateStateSet()->setAttributeAndModes(fog,osg::StateAttribute::ON);
}

void OSGRenderer::optimize()
{
    // TODO(drummy): I went crazy with optimization, but this will probably
    // need to be changed once we want to have dynamic objects etc.

    MFLogger::Logger::info("optimizing", OSGRENDERER_MODULE_STR);

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

void OSGRenderer::frame(double dt)
{
    if (mViewer->done() || mDone)
    {
        mDone = true;
    }
    else
    {
        mViewer->advance(dt);
        mViewer->eventTraversal();
        mViewer->updateTraversal();
        mViewer->renderingTraversals();
    }
}

void OSGRenderer::setUpLights(std::vector<osg::ref_ptr<osg::LightSource>> *lightNodes)
{
    if (lightNodes == nullptr || lightNodes->empty())
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

        for (auto & lightNode : *lightNodes)
        {
            if (lightNum > 7)     // fixed pipeline only supports 8 lights
                break;

            std::string lightTypeStr = lightNode->getName();

            // for now only add global lights, i.e. directional and ambient
            if (lightTypeStr.compare("directional") == 0 ||
                lightTypeStr.compare("ambient") == 0)
            {
                MFLogger::Logger::info("Adding " + lightTypeStr + " light.",OSGRENDERER_MODULE_STR);
                lightNode->getLight()->setLightNum(lightNum);
                mRootNode->getOrCreateStateSet()->setAttributeAndModes(lightNode->getLight());
                lightNum++;
            }
            else
                lightNode->getLight()->setLightNum(0);
        }
    }
}

}
