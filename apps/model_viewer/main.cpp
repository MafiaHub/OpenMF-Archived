#include <iostream>
#include <fstream>
#include <loggers/console.hpp>
#include <cxxopts.hpp>
#include <renderer/osg_renderer.hpp>
#include <physics/bullet_physics_world.hpp>
#include <string.h>
#include <stdlib.h>
#include <spatial_entity/spatial_entity.hpp>
#include <spatial_entity/manager.hpp>
#include <spatial_entity/factory.hpp>
#include <input/input_manager_implementation.hpp>
#include <free_camera_controller.hpp>

#define DEFAULT_CAMERA_SPEED 7.0
#define VIEWER_MODULE_STR "viewer"

// TODO: Introduce Platform class and store it there?
static bool sIsRunning = true;

// TODO: Replace with configurable value
#define MS_PER_UPDATE 1 / 60.0f

// TODO move this to Platform class
#include <chrono>
#include <thread>
double timeGet()
{
    static auto epoch = std::chrono::high_resolution_clock::now();

    return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - epoch).count() / 1000000000.0;
}

class RightButtonCallback: public MFInput::ButtonInputCallback
{
public:
    RightButtonCallback(MFRender::Renderer *renderer, MFInput::InputManager *inputManager): MFInput::ButtonInputCallback()
    {
        mRenderer = renderer;
        mInputManager = inputManager;
    }

    virtual void call(bool down, unsigned int buttonNumber, unsigned int x, unsigned int y) override
    {
        if (buttonNumber == 3 && down)
        {
            unsigned int w,h;
            mInputManager->getWindowSize(w,h);
            mRenderer->debugClick(x,h - y);
        }
    }

protected:
    MFRender::Renderer *mRenderer;
    MFInput::InputManager *mInputManager;
};

class SpaceKeyCallback: public MFInput::KeyInputCallback
{
public:
    SpaceKeyCallback(MFGame::SpatialEntityFactory *entityFactory, MFGame::SpatialEntityManager *entityManager, MFRender::Renderer *renderer): MFInput::KeyInputCallback()
    {
        mEntityFactory = entityFactory;
        mEntityManager = entityManager;
        mRenderer = renderer;
        mCounter = 0;
    }

    virtual void call(bool down, unsigned int keyCode) override
    {
        if (keyCode == 44 && down)
        {
            MFGame::SpatialEntity *e = mEntityManager->getEntityById(
                mCounter % 2 == 0 ?
                    mEntityFactory->createTestBallEntity() :
                    mEntityFactory->createTestBoxEntity());

            MFMath::Vec3 f,r,u;
            mRenderer->getCameraVectors(f,r,u);

            const float speed = 10.0;

            e->setPosition(mRenderer->getCameraPosition() + f * 2.0f);
            e->setVelocity(f * speed);
            mCounter++;
        }
    }

protected:
    MFGame::SpatialEntityFactory *mEntityFactory;
    MFGame::SpatialEntityManager *mEntityManager;
    MFRender::Renderer *mRenderer;
    unsigned int mCounter;
};

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

std::string getCameraString(MFRender::Renderer *renderer)
{
    double cam[6];
    MFMath::Vec3 pos,rot;
    renderer->getCameraPositionRotation(pos,rot);

    cam[0] = pos.x;
    cam[1] = pos.y;
    cam[2] = pos.z;
    cam[3] = rot.x;
    cam[4] = rot.y;
    cam[5] = rot.z;

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

std::string getCollisionString(MFRender::Renderer *renderer, MFPhysics::MFPhysicsWorld *world,
    MFGame::SpatialEntityManager *entityManager)
{
    std::string result = "collision: ";

    MFMath::Vec3 pos = renderer->getCameraPosition();

    int entityID = world->pointCollision(pos);

    if (entityID >= 0)
        result += entityManager->getEntityById(entityID)->toString();
    else
        result += "none";

    return result;
}

void parseCameraString(std::string str, double params[6])
{
    char *token;
    char cStr[256];
    char delims[] = ",; ";

    strncpy(cStr,str.c_str(),256);

    token = strtok(cStr,delims);

    for (int i = 0; i < 6; ++i)
    {
        params[i] = atof(token);
        token = strtok(NULL,delims);
    } 
}

int main(int argc, char** argv)
{
    cxxopts::Options options(VIEWER_MODULE_STR,"3D viewer for Mafia 4DS files.");

    options.add_options()
        ("h,help","Display help and exit.")
        ("i,input","Specify input, mission name by default.",cxxopts::value<std::string>())
        ("4,4ds","Load single 4ds model instead of mission.")
        ("f,fov","Specify camera field of view in degrees.",cxxopts::value<int>())
        ("s,camera-speed","Set camera speed (default is " + std::to_string(DEFAULT_CAMERA_SPEED) +  ").",cxxopts::value<double>())
        ("c,camera-info","Write camera position and rotation in console.")
        ("C,collision-info","Write camera collisions in console.")
        ("v,verbosity","Print verbose output.")
        ("P,physics","Simulate physics and allow space-key controlled spawning of test entities.")
        ("e,export","Export scene to file and exit.",cxxopts::value<std::string>())
        ("b,base-dir","Specify base game directory.",cxxopts::value<std::string>())
        ("p,place-camera","Place camera at position X,Y,Z,YAW,PITCH,ROLL.",cxxopts::value<std::string>())
        ("l,log-id","Specify a module to print logs of, with a string ID. Combine with -v.",cxxopts::value<std::string>())
        ("no-4ds","Do not load scene.4ds for the mission.")
        ("no-scene2bin","Do not load scene2.bin for the mission.")
        ("no-cachebin","Do not load cache.bin for the mission.")
        ("no-treeklz","Do not load tree.klz (collisions) for the mission.")
        ("m,mask","Set rendering mask.",cxxopts::value<unsigned int>());

    options.parse_positional({"i"});
    auto arguments = options.parse(argc,argv);

    bool cameraInfo = arguments.count("c") > 0;
    bool collisionInfo = arguments.count("C") > 0;
    bool cameraPlace = arguments.count("p") > 0;
    bool model = arguments.count("4") > 0;
    bool simulatePhysics = arguments.count("P") > 0;
    std::string exportFileName;

    if (arguments.count("e") > 0)
        exportFileName = arguments["e"].as<std::string>();

    bool load4ds = arguments.count("no-4ds") < 1;
    bool loadScene2Bin = arguments.count("no-scene2bin") < 1;
    bool loadCacheBin = arguments.count("no-cachebin") < 1;
    bool loadTreeKlz = arguments.count("no-treeklz") < 1;

    double cameraSpeed = DEFAULT_CAMERA_SPEED;

    if (arguments.count("s") > 0)
        cameraSpeed = arguments["s"].as<double>();

    if (arguments.count("h") > 0)
    {
        std::cout << options.help() << std::endl;
        return 0;
    }

    if (arguments.count("v") > 0)
    {
        MFLogger::ConsoleLogger::getInstance()->setVerbosityFlags(0xffff);
    }
    else
    {
        MFLogger::ConsoleLogger::getInstance()->setVerbosityFlags(0xffff);
        MFLogger::ConsoleLogger::getInstance()->addFilter(VIEWER_MODULE_STR);
        MFLogger::ConsoleLogger::getInstance()->addFilter(OSGRENDERER_MODULE_STR);
        MFLogger::ConsoleLogger::getInstance()->setFilterMode(false);
    }

    if (arguments.count("l") > 0)
    {
        MFLogger::ConsoleLogger::getInstance()->addFilter(arguments["l"].as<std::string>());
        MFLogger::ConsoleLogger::getInstance()->setFilterMode(false);
    }

    if (arguments.count("i") < 1)
    {
        MFLogger::ConsoleLogger::fatal("Expected file.", VIEWER_MODULE_STR);
        std::cout << options.help() << std::endl;
        return 1;
    }

    int fov = 75;

    if (arguments.count("f") > 0)
        fov = arguments["f"].as<int>();

    if (arguments.count("b") > 0)
        MFFile::FileSystem::getInstance()->prependPath(arguments["b"].as<std::string>());

    std::string inputFile = arguments["i"].as<std::string>();

    MFRender::OSGRenderer renderer;

    MFPhysics::BulletPhysicsWorld physicsWorld;
    MFGame::SpatialEntityManager entityManager;
    MFGame::SpatialEntityFactory entityFactory(&renderer,&physicsWorld,&entityManager);

    MFInput::InputManagerImplementation inputManager;
    inputManager.initWindow(800,600,100,100);
    renderer.setUpInWindow(inputManager.getWindow());

    std::shared_ptr<RightButtonCallback> rbcb = std::make_shared<RightButtonCallback>(&renderer,&inputManager);
    inputManager.addButtonCallback(rbcb);

    std::shared_ptr<WindowResizeCallback> wrcb = std::make_shared<WindowResizeCallback>(&renderer);
    inputManager.addWindowResizeCallback(wrcb);

    if (simulatePhysics)
    {
        std::shared_ptr<SpaceKeyCallback> skcb = std::make_shared<SpaceKeyCallback>(&entityFactory,&entityManager,&renderer);
        inputManager.addKeyCallback(skcb);
    }

    MFGame::FreeCameraController cameraController(&renderer,&inputManager);
    cameraController.setSpeed(cameraSpeed);

    if (model)
    {
        renderer.loadSingleModel(inputFile);
    }
    else
    {
        renderer.loadMission(inputFile,load4ds,loadScene2Bin,loadCacheBin);

        if (loadTreeKlz)
            physicsWorld.loadMission(inputFile);

        entityFactory.createMissionEntities();
    }

    renderer.setCameraParameters(true,fov,0,0.25,2000);

    if (arguments.count("m") > 0)
    {
        renderer.setRenderMask(arguments["m"].as<unsigned int>());
    }

    if (cameraPlace)
    {
        double cam[6];
        parseCameraString(arguments["p"].as<std::string>(),cam);
        renderer.setCameraPositionRotation(MFMath::Vec3(cam[0],cam[1],cam[2]),MFMath::Vec3(cam[3],cam[4],cam[5]));
    }

    int lastSelectedEntity = -1;
    int infoCounter = 0;

    if (exportFileName.length() > 0)
    {
        if (!renderer.exportScene(exportFileName))
            return 1;
    }

    // TODO Simplify this code
    else
    {
        double lastTime = timeGet();
        double extraTime = 0.0f;

        while (sIsRunning)
        {
            if (inputManager.windowClosed())
                sIsRunning = false;
          
            bool render = false;
            double startTime = timeGet();
            double passedTime = startTime - lastTime;
            lastTime = startTime;
            extraTime += passedTime;

            while (extraTime > MS_PER_UPDATE)
            {
                int selectedId = renderer.getSelectedEntityId();

                if (selectedId >= 0 && selectedId != lastSelectedEntity)
                {
                    std::cout << "selected entity: " << entityManager.getEntityById(selectedId)->toString() << std::endl;
                    lastSelectedEntity = selectedId;
                }

                if (cameraInfo || collisionInfo)
                {
                    if (infoCounter <= 0)
                    {
                        if (cameraInfo)
                            std::cout << "camera: " + getCameraString(&renderer) << std::endl;

                        if (collisionInfo)
                            std::cout << getCollisionString(&renderer, &physicsWorld, &entityManager) << std::endl;

                        infoCounter = 30;
                    }

                    infoCounter--;
                }

                // TODO use UPDATE_TIME to make fixed-time-delta loop

                entityManager.update(MS_PER_UPDATE);

                if (simulatePhysics)
                    physicsWorld.frame(MS_PER_UPDATE);
              
                render = true;
                extraTime -= MS_PER_UPDATE;
            }

            inputManager.processEvents();
              
            if (render)
            {
                renderer.frame(MS_PER_UPDATE); // TODO: Use actual delta time
                cameraController.update(MS_PER_UPDATE);
              
                if (renderer.done())
                    sIsRunning = false;
            }
            else
            {
                // Let OS do background work while we wait.
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        }
    }

    inputManager.destroyWindow();
    return 0;
}
