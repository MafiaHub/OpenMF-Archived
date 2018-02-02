#include <iostream>
#include <engine/engine.hpp>
#include <controllers/player_controller.hpp>

class OpenMFEngine: public MFGame::Engine
{
public:
    OpenMFEngine(MFGame::Engine::EngineSettings settings): MFGame::Engine(settings)
    {
        mPlayerEntity = mSpatialEntityManager->getEntityById(mSpatialEntityFactory->createCapsuleEntity());
        mPlayerController = new MFGame::PlayerController(mPlayerEntity,mRenderer,mInputManager,mPhysicsWorld);

        mSpatialEntityFactory->setDebugMode(false);
    };

    virtual ~OpenMFEngine()
    {
        delete mPlayerController;
    };

    virtual void frame() override
    {
        mPlayerController->update(mEngineSettings.mUpdatePeriod);
    };

    void setPlayerPosition(MFMath::Vec3 pos)
    {
        mPlayerEntity->setPosition(pos);
    };

protected:
    MFGame::SpatialEntity *mPlayerEntity;
    MFGame::PlayerController *mPlayerController;
};

int main( int argc, char** argv )
{
    MFGame::Engine::EngineSettings settings;
    OpenMFEngine engine(settings);

    engine.loadMission("tutorial");
    engine.setPlayerPosition(MFMath::Vec3(126.02,320.43,3.67));
    engine.getRenderer()->setCameraParameters(true,90,0,0.25,2000);
    engine.run();

    return 0;
}
