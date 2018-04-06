#ifndef ENGINE_H
#define ENGINE_H

#include <input/input_manager_impl.hpp>
#include <spatial_entity/spatial_entity.hpp>
#include <spatial_entity/manager.hpp>
#include <spatial_entity/factory.hpp>
#include <renderer/base_renderer.hpp>
#include <string>

namespace MFGame
{

/**
  Puts most of the game components together and implements the main loop.
*/

class Engine
{
public:
    typedef struct EngineSettingsStruct
    {
        EngineSettingsStruct()
        {
            mInitWindowWidth    = 800;
            mInitWindowHeight   = 600;
            mInitWindowX        = 100;
            mInitWindowY        = 100;

            mSimulatePhysics    = true;

            mLoad4ds            = true;
            mLoadScene2Bin      = true;
            mLoadCacheBin       = true;
            mLoadTreeKlz        = true;
            mVsync              = false;

            mUpdatePeriod       = 1.0 / 60.0;
            mSleepPeriod        = 1.0;
        };

        unsigned int mInitWindowWidth;
        unsigned int mInitWindowHeight;
        unsigned int mInitWindowX;
        unsigned int mInitWindowY;

        bool         mSimulatePhysics;

        bool         mLoad4ds;
        bool         mLoadScene2Bin;
        bool         mLoadCacheBin;
        bool         mLoadTreeKlz;
        bool         mVsync;

        double       mUpdatePeriod;
        double       mSleepPeriod;
    } EngineSettings;

    Engine(EngineSettings settings);
    virtual ~Engine();

    virtual void frame(double dt=-1) {};           ///< This can be overriden to perform something each frame.
    virtual void step() {};            ///< This can be overriden to perform something each update step.
    virtual void update(double dt=-1);   ///< Performs one iteration of the game loop, positive dt can be used to step out of real-time.
    virtual void run();

    bool loadMission(std::string missionName); 
    bool loadSingleModel(std::string modelName); 
    bool exportScene(std::string outputFileName);

    MFRender::Renderer *getRenderer()                       { return mRenderer;             };
    MFGame::SpatialEntityFactory *getSpatialEntityFactory() { return mSpatialEntityFactory; };
    MFGame::SpatialEntityManager *getSpatialEntityManager() { return mSpatialEntityManager; };
    MFInput::InputManager *getInputManager()                { return mInputManager;         };

    std::string getCameraInfoString();                     ///< Get camera position and rotation encoded in string.
    void setCameraFromString(std::string cameraString);    ///< For debug - set current camera from string returned by getCameraInfoString().
    void RequestExit();

protected:
    double getTime();
    void yield();


    double mLastTime;
    double mUnprocessedTime;
    float mRenderTime;

    unsigned long long mFrameNumber;

    MFInput::InputManagerImpl *mInputManager;
    MFGame::SpatialEntityManager        *mSpatialEntityManager;
    MFGame::SpatialEntityFactory        *mSpatialEntityFactory;
    MFRender::OSGRenderer               *mRenderer;
    MFPhysics::BulletPhysicsWorld       *mPhysicsWorld;

    bool mIsRunning;

    EngineSettings mEngineSettings;
};

}

#endif
