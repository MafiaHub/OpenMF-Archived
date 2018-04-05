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
			mVsync              = true;

			mTicksPerSecond     = 60.0;
            mUpdatePeriod       = 1.0 / mTicksPerSecond;
			mFrameSkip          = 5;
            mSleepPeriod        = 1;
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

		double        mTicksPerSecond;
		double        mUpdatePeriod;
		int          mFrameSkip;
        unsigned int mSleepPeriod;

    } EngineSettings;

    Engine(EngineSettings settings);
    virtual ~Engine();

    virtual void frame() {};           ///< This can be overriden to perform something each frame.
    virtual void step(double dt=-1);   ///< Performs one iteration of the game loop, positive dt can be used to step out of real-time.
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

protected:
    double getTime();
	int getTickCount();


	int mNumberOfLoops;
	int mNextGameTick;
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
