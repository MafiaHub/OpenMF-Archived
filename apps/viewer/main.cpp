#include <engine/engine.hpp>
#include <cxxopts.hpp>
#include <controllers/camera_controllers.hpp>
#include <entity/entity.hpp>

#define VIEWER_MODULE_STR "viewer"

#define DEFAULT_CAMERA_SPEED 7.0
#define DEFAULT_CAMERA_FOV 75.0

class RightButtonCallback: public MFInput::ButtonInputCallback
{
public:
    RightButtonCallback(MFGame::Engine *engine)
    {
        mEngine = engine;
    }

    void call(bool down, unsigned int buttonNumber, unsigned int x, unsigned int y) override
    {
        if (buttonNumber == 3 && down)
        {
            unsigned int w,h;
            mEngine->getInputManager()->getWindowSize(w,h);
            mEngine->getRenderer()->debugClick(x,h - y);
        }
    }

protected:
    MFGame::Engine *mEngine;
};

class KeyCallback: public MFInput::KeyInputCallback
{
public:
    KeyCallback(MFGame::Engine *engine)
    {
        mEngine = engine;
        mCounter = 0;
    }

    void call(bool down, unsigned int keyCode) override
    {
        if (!down)
            return;

        if (keyCode == OMF_SCANCODE_SPACE)   // SPACE
        {
            const auto mission = mEngine->getMissionManager()->getCurrentMission();
            if (!mission) return;

            MFGame::Entity *e = mEngine->getEntityManager()->getEntityById(
                mEngine->getEntityFactory()->createPropEntity("bedna02.4ds"));

            MFMath::Vec3 f,r,u;
            mEngine->getRenderer()->getCameraVectors(f,r,u);

            const float speed = 10.0;

            e->setPosition(mEngine->getRenderer()->getCameraPosition() + f * 2.0f);
            e->setVelocity(f * speed);
            mCounter++;
        }
        else if (keyCode == OMF_SCANCODE_F3) // F3
        {
            mEngine->getRenderer()->showProfiler();
        }
        else if (keyCode == OMF_SCANCODE_ESCAPE) {
            mEngine->RequestExit();
        }
    }

protected:
    MFGame::Engine *mEngine;
    unsigned int mCounter;
};

class ViewerEngine: public MFGame::Engine
{
public:
    ViewerEngine(MFGame::Engine::EngineSettings settings, bool printCameraInfo, bool printCollisionInfo, bool rigidCamera, std::string cameraInitString): MFGame::Engine(settings)
    {
        if (cameraInitString.length() > 0)
            setCameraFromString(cameraInitString);

        const std::shared_ptr<RightButtonCallback> rbcb = std::make_shared<RightButtonCallback>(this);
        mInputManager->addButtonCallback(rbcb);

        const std::shared_ptr<KeyCallback> kcb = std::make_shared<KeyCallback>(this);
        mInputManager->addKeyCallback(kcb);

        mPrintCameraInfo = printCameraInfo;
        mPrintCollisionInfo = printCollisionInfo;

        if (!rigidCamera)
        {
            mCameraController = new MFGame::FreeCameraController(mRenderer,mInputManager);
        }
        else
        {
            const auto cameraEntity = mEntityFactory->createCameraEntity();
            auto cameraPtr = mEntityManager->getEntityById(cameraEntity);
            const auto pos = mRenderer->getCameraPosition();

            cameraPtr->setPosition(pos);
            mCameraController = new MFGame::RigidCameraController(mRenderer,mInputManager,cameraPtr);
        }
    }

    virtual ~ViewerEngine()
    {
        delete mCameraController;
    }

    virtual void step() override
    {
        mCameraController->update(mEngineSettings.mUpdatePeriod);

        if (mFrameNumber % 128)
        {
            if (mPrintCameraInfo)
                std::cout << "cam: " << getCameraInfoString() << std::endl;

            if (mPrintCollisionInfo)
            {
                std::cout << "col: ";

                const MFMath::Vec3 pos = mRenderer->getCameraPosition();
                const MFGame::Entity::Id entityID = mPhysicsWorld->pointCollision(pos);
                MFGame::Entity *e = mEntityManager->getEntityById(entityID);

                std::cout << (e ? e->toString() : "none") << std::endl;
            }
        }
    }

    void setCameraSpeed(double speed) const
    {
        mCameraController->setSpeed(speed);
    }

protected:
    MFGame::FreeCameraController *mCameraController;
    bool mPrintCameraInfo;
    bool mPrintCollisionInfo;
};

int main(int argc, char** argv)
{
    cxxopts::Options options(VIEWER_MODULE_STR,"3D viewer for Mafia models and missions.");

    options.add_options()
        ("h,help","Display help and exit.")
        ("i,input","Specify input, mission name by default.",cxxopts::value<std::string>())
        ("f,fov","Specify camera field of view in degrees.",cxxopts::value<int>())
        ("s,camera-speed","Set camera speed (default is " + std::to_string(DEFAULT_CAMERA_SPEED) +  ").",cxxopts::value<double>())
        ("c,camera-info","Write camera position and rotation in console.")
        ("r,camera-rigid", "Camera will collide with the world.")
        ("C,collision-info","Write camera collisions in console.")
        ("V,view-distance","Override the view distance with custom value.",cxxopts::value<int>())
        ("v,verbosity","Print verbose output.")
        ("P,physics","Simulate physics and allow space-key controlled spawning of test entities.")
        ("e,export","Export scene to file and exit.",cxxopts::value<std::string>())
        ("b,base-dir","Specify base game directory.",cxxopts::value<std::string>())
        ("p,place-camera","Place camera at position X,Y,Z,YAW,PITCH,ROLL.",cxxopts::value<std::string>())
        ("l,log-id","Specify a module to print logs of, with a string ID. Combine with -v.",cxxopts::value<std::string>())
        ("vsync","Enable VSYNC.")
        ("no-4ds","Do not load scene.4ds for the mission.")
        ("no-scene2bin","Do not load scene2.bin for the mission.")
        ("no-cachebin","Do not load cache.bin for the mission.")
        ("no-treeklz","Do not load tree.klz (collisions) for the mission.")
        ("m,mask","Set rendering mask.",cxxopts::value<unsigned int>());

    options.parse_positional({"i"});
    auto arguments = options.parse(argc,argv);

    if (arguments.count("h") > 0 || arguments.count("i") < 1)
    {
        std::cout << options.help() << std::endl;
        return 0;
    }

    if (arguments.count("b") > 0)
        MFFile::FileSystem::getInstance()->prependPath(arguments["b"].as<std::string>());

    MFGame::Engine::EngineSettings settings;

    settings.mSimulatePhysics = arguments.count("P") > 0;
    settings.mLoad4ds         = arguments.count("no-4ds") < 1;
    settings.mLoadScene2Bin   = arguments.count("no-scene2bin") < 1;
    settings.mLoadCacheBin    = arguments.count("no-cachebin") < 1;
    settings.mLoadTreeKlz     = arguments.count("no-treeklz") < 1;
    settings.mVsync           = arguments.count("vsync") > 0;

    std::string cameraString = "";

    if (arguments.count("p") > 0)
        cameraString =  arguments["p"].as<std::string>();

    ViewerEngine engine(
        settings,
        arguments.count("c") > 0,
        arguments.count("C") > 0,
        arguments.count("r") > 0,
        cameraString);

    const std::string inputFile = arguments["i"].as<std::string>();

    const double fov = arguments.count("f") > 0 ? arguments["f"].as<int>() : DEFAULT_CAMERA_FOV;
    engine.getRenderer()->setCameraParameters(true,fov,0,0.25,2000);

    const double camSpeed = arguments.count("s") > 0 ? arguments["s"].as<double>() : DEFAULT_CAMERA_SPEED;
    engine.setCameraSpeed(camSpeed);

    if (arguments.count("v") > 0)
    {
        MFLogger::Logger::setVerbosityFlags(0xffff);
    }
    else
    {
        MFLogger::Logger::setVerbosityFlags(0xffff);
        MFLogger::Logger::addFilter(VIEWER_MODULE_STR);
        MFLogger::Logger::addFilter(OSGRENDERER_MODULE_STR);
        MFLogger::Logger::addFilter(MISSION_MANAGER_MODULE_STR);
        MFLogger::Logger::setFilterMode(false);
    }

    if (arguments.count("l") > 0)
    {
        MFLogger::Logger::addFilter(arguments["l"].as<std::string>());
        MFLogger::Logger::setFilterMode(false);
    }

    if (arguments.count("m") > 0)
    {
        dynamic_cast<MFRender::OSGRenderer *>(engine.getRenderer())->setRenderMask(arguments["m"].as<unsigned int>());
        engine.getEntityFactory()->setDebugMode(true);
    }

    engine.loadMission(inputFile);

    if (arguments.count("V") > 0)
        engine.getRenderer()->setViewDistance(arguments["V"].as<int>());

    if (arguments.count("e") < 1)
        engine.run();
    else
        engine.exportScene(arguments["e"].as<std::string>());

    return 0;
}
