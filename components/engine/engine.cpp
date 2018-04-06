#include <engine/engine.hpp>
#include <chrono>
#include <thread>
#include <utils/logger.hpp>
#include <renderer/osg_renderer.hpp>
#include <utils/math.hpp>

#ifdef _WIN32
#include <Windows.h>
#endif

#define ENGINE_MODULE_STR "engine"

namespace MFGame

{

class WindowResizeCallback: public MFInput::WindowResizeCallback
{
public:
    WindowResizeCallback(MFRender::Renderer *renderer)
    {
        mRenderer = renderer;
    }

    virtual void call(unsigned int width, unsigned int height) override
    {
        mRenderer->setWindowSize(width,height);
    }

protected:
    MFRender::Renderer *mRenderer;
};

Engine::Engine(EngineSettings settings)
{
    MFLogger::Logger::info("Initiating engine.",ENGINE_MODULE_STR);

    mFrameNumber = 0;

    mEngineSettings = settings;
    mIsRunning = false;

    mRenderer = new MFRender::OSGRenderer();
    mPhysicsWorld = new MFPhysics::BulletPhysicsWorld();
    mInputManager = new MFInput::InputManagerImpl();
    mSpatialEntityManager = new SpatialEntityManager();
    mSpatialEntityFactory = new SpatialEntityFactory(mRenderer,mPhysicsWorld,mSpatialEntityManager);

    mInputManager->initWindow(
        mEngineSettings.mInitWindowWidth,
        mEngineSettings.mInitWindowHeight,
        mEngineSettings.mInitWindowX,
        mEngineSettings.mInitWindowY,
        mEngineSettings.mVsync);

    std::shared_ptr<WindowResizeCallback> wrcb = std::make_shared<WindowResizeCallback>(mRenderer);
    mInputManager->addWindowResizeCallback(wrcb);

    mRenderer->setUpInWindow(mInputManager->getWindow());
}

std::string Engine::getCameraInfoString()
{
    double cam[6];
    MFMath::Vec3 pos,rot;
    mRenderer->getCameraPositionRotation(pos,rot);

    cam[0] = pos.x; cam[1] = pos.y; cam[2] = pos.z;
    cam[3] = rot.x; cam[4] = rot.y; cam[5] = rot.z;

    std::string camStr;

    bool first = true;

    for (int i = 0; i < 6; ++i)
    {
        if (first)
            first = false;
        else
            camStr += ",";

        camStr += std::to_string(cam[i]);
    }

    return camStr;
}

void Engine::setCameraFromString(std::string cameraString)
{
    double cam[6];

    char *token;
    char cStr[256];
    char delims[] = ",; ";

    strncpy(cStr,cameraString.c_str(),256);

    token = strtok(cStr,delims);

    for (int i = 0; i < 6; ++i)
    {
        cam[i] = atof(token);
        token = strtok(NULL,delims);
    }

    mRenderer->setCameraPositionRotation(MFMath::Vec3(cam[0],cam[1],cam[2]),MFMath::Vec3(cam[3],cam[4],cam[5]));
}

bool Engine::exportScene(std::string outputFileName)
{    
    return mRenderer->exportScene(outputFileName);
}

bool Engine::loadMission(std::string missionName)
{ 
    mRenderer->loadMission(missionName,mEngineSettings.mLoad4ds,mEngineSettings.mLoadScene2Bin,mEngineSettings.mLoadCacheBin);

    if (mEngineSettings.mLoadTreeKlz)
        mPhysicsWorld->loadMission(missionName);

    mSpatialEntityFactory->createMissionEntities();

    return true;   // TODO: perform some actual checks
}
 
bool Engine::loadSingleModel(std::string modelName)
{
    mRenderer->loadSingleModel(modelName);
    return true;   // TODO: perform some actual checks
} 

double Engine::getTime()
{
    static auto epoch = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - epoch).count() / 1000000000.0;
}

Engine::~Engine()
{
    MFLogger::Logger::info("Shutting down the engine.",ENGINE_MODULE_STR);

    mInputManager->destroyWindow();

    delete mRenderer;
    delete mInputManager;
    delete mSpatialEntityManager;
    delete mSpatialEntityFactory;
    delete mPhysicsWorld;
}

void Engine::update(double dt)
{
    if (mInputManager->windowClosed())
    {
        mIsRunning = false;
        return;
    }

    double startTime = getTime();
    double passedTime = startTime - mLastTime;
    mLastTime = startTime;
    mUnprocessedTime += passedTime;

    if (dt > 0)
        mUnprocessedTime = dt;

    double physicsTime = 0.0;
    double physicsTimeBegin = 0.0;
    double physicsTimeEnd = physicsTimeBegin;

    while (mUnprocessedTime >= mEngineSettings.mUpdatePeriod)
    {
        mInputManager->processEvents();
        mSpatialEntityManager->update(mEngineSettings.mUpdatePeriod);

        if (mEngineSettings.mSimulatePhysics) {
            if (physicsTimeBegin == 0.0f)
                physicsTimeBegin = getTime();

            mPhysicsWorld->frame(mEngineSettings.mUpdatePeriod);

            physicsTimeEnd = getTime();
            physicsTime += (physicsTimeEnd - physicsTimeBegin);
        }

        mUnprocessedTime -= mEngineSettings.mUpdatePeriod;

        step();
    }
    
    mRenderTime = mEngineSettings.mUpdatePeriod; // Use actual delta time
    frame(mRenderTime);
    mRenderer->frame(mRenderTime);
    
    if (mRenderer->done())
        mIsRunning = false;

    auto frameNumber = mRenderer->getViewer()->getFrameStamp()->getFrameNumber();
    auto stats = mRenderer->getViewer()->getViewerStats();
    if (stats) 
    {
        stats->setAttribute(frameNumber, "physics_time_begin", physicsTimeBegin);
        stats->setAttribute(frameNumber, "physics_time_taken", physicsTime);
        stats->setAttribute(frameNumber, "physics_time_end", physicsTimeEnd);
    }

    mFrameNumber++;
}

void Engine::RequestExit()
{
    mIsRunning = false;
}

void Engine::run()
{
    MFLogger::Logger::info("Starting the engine.",ENGINE_MODULE_STR);

    mIsRunning = true;

    mLastTime = getTime();
    mUnprocessedTime = 0.0f;
    mRenderTime = 0.0f;

    while (mIsRunning)       // main loop
        update();
}

}
