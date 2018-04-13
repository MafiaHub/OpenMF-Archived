#include <iostream>
#include <engine/engine.hpp>
#include <controllers/player_controller.hpp>
#include <cxxopts.hpp>

class MafiaEngine: public MFGame::Engine
{
public:
    MafiaEngine(MFGame::Engine::EngineSettings settings): MFGame::Engine(settings)
    {
        mPlayerEntity = (MFGame::EntityImpl *)mEntityManager->getEntityById(mEntityFactory->createPawnEntity("tommy.4ds"));
        mPlayerNode = mPlayerEntity->getVisualNode();
        mPlayerController = new MFGame::PlayerController(mPlayerEntity,mRenderer,mInputManager,mPhysicsWorld);
        mPlayerController->setMafiaPhysicsEmulation(false);

        mEntityFactory->setDebugMode(false);

        std::shared_ptr<KeyCallback> cb = std::make_shared<KeyCallback>(this);
        mInputManager->addKeyCallback(cb);
    };

    virtual ~MafiaEngine()
    {
        delete mPlayerController;
    };

    virtual void step() override
    {
        mPlayerController->update(mEngineSettings.mUpdatePeriod);

        if (mInputManager->keyPressed(SDL_SCANCODE_ESCAPE)) {
            mIsRunning = false;
        }
    };

    void setPlayerPosition(MFMath::Vec3 pos)
    {
        mPlayerEntity->setPosition(pos);
    };

    void useDefaultPlayer()
    {
        auto spawnEntity = mEntityManager->getEntityByName("player start");

        if (spawnEntity) {
            auto pos = spawnEntity->getPosition();
            auto spat = (MFGame::EntityImpl *)spawnEntity;
            setPlayerPosition(MFMath::Vec3(pos.x, pos.y, pos.z + 2)); // +1 due to player being moved in the middle of the ground

            if (mPlayerEntity->getVisualNode() == mPlayerNode) {
                mRenderer->getRootNode()->removeChild(mPlayerEntity->getVisualNode());
                mPlayerNode = nullptr;
            }

            mPlayerEntity->setVisualNode(spat->getVisualNode());
        }
    };

    class KeyCallback : public MFInput::KeyInputCallback
    {
    public:
        KeyCallback(MafiaEngine *engine) : MFInput::KeyInputCallback()
        {
            mEngine = engine;
            mCounter = 0;
        }

        virtual void call(bool down, unsigned int keyCode) override
        {
            if (!down)
                return;

            if (keyCode == OMF_SCANCODE_F3) // F3
            {
                mEngine->getRenderer()->showProfiler();
            }

            if (keyCode == OMF_SCANCODE_0) {
                mEngine->loadMission("tutorial");
                mEngine->useDefaultPlayer();
            }

            if (keyCode == OMF_SCANCODE_3) {
                mEngine->loadMission("mise14-parnik");
                mEngine->useDefaultPlayer();
            }
        }

    protected:
        MafiaEngine * mEngine;
        unsigned int mCounter;
    };

protected:
    MFGame::EntityImpl *mPlayerEntity;
    osg::ref_ptr<osg::Group> mPlayerNode;
    MFGame::PlayerController *mPlayerController;
};

int main(int argc, char** argv)
{
    cxxopts::Options options("OpenMF","OpenMF game.");

    options.add_options()
        ("h,help","Display help and exit.")
        ("d,default-pos","Use default player position from selected scenario.")
        ("vsync","Enable VSYNC.")
        ("s,scenario","Set test game scenario number.",cxxopts::value<unsigned int>());

    auto arguments = options.parse(argc,argv);

    if (arguments.count("h") > 0)
    {
        std::cout << options.help() << std::endl;
        return 0;
    }

    unsigned int scenario = 0;

    if (arguments.count("s") > 0)
        scenario = arguments["s"].as<unsigned int>();

    MFGame::Engine::EngineSettings settings;
    settings.mInitWindowWidth = 1024;
    settings.mInitWindowHeight = 768;
    settings.mVsync = arguments.count("vsync") > 1;
    MafiaEngine engine(settings);

    switch (scenario)
    {
        case 1:
            engine.loadMission("freeride");
            engine.setPlayerPosition(MFMath::Vec3(-164.49,-74.50,19.91));
            break;   
        
        case 2:
            engine.loadMission("mise11-vila");
            engine.setPlayerPosition(MFMath::Vec3(104.65,172.62,-4.22));
            break;   
 
        case 3:
            engine.loadMission("mise20-galery");
            engine.setPlayerPosition(MFMath::Vec3(21.00,-9.39,7.78));
            break;   

        case 4:
            engine.loadMission("mise02-saliery");
            engine.setPlayerPosition(MFMath::Vec3(-1774.71,31.45,-2.52));
            break;

        case 5:
            engine.loadMission("mise06-autodrom");
            engine.setPlayerPosition(MFMath::Vec3(161.601547, -136.590485, 1.929724));
            break;

        case 6:
            engine.loadMission("mise04-motorest");
            engine.setPlayerPosition(MFMath::Vec3(9.253828, -0.714492, 2.294598));
            break;

        case 7:
            engine.loadMission("mise14-parnik");
            engine.setPlayerPosition(MFMath::Vec3(-22.779055, 2.942175, 4.983528));
            break;
 
        default:
            engine.loadMission("tutorial");
            engine.setPlayerPosition(MFMath::Vec3(126.02,320.43,3.67));
            break;
    }

    if (arguments.count("d") < 1) {
        engine.useDefaultPlayer();
    }
    
    engine.getRenderer()->setCameraParameters(true,90,0,0.25,2000);
    engine.run();

    return 0;
}
