#ifndef DEBUG_GAME_H
#define DEBUG_GAME_H

// FIXME a fancier way to include this
#include "../game.hpp"

#include <renderer/osg_renderer.hpp>
#include <physics/bullet_physics_world.hpp>
#include <spatial_entity/loader.hpp>

// TODO replace with FSM of mission selection
#define MISSION_NAME "tutorial"

namespace MFGame
{

class DebugGame: public Game
{
public:
    virtual void init() override;
    virtual void shutdown() override;
    virtual void input(double delta) override;
    virtual void update(double delta) override;
    virtual void frame(double delta) override;

private:
    MFRender::OSGRenderer mRenderer;
    MFPhysics::BulletPhysicsWorld mPhysicsWorld;
    MFFormat::SpatialEntityLoader mSpatialEntityLoader;
    MFFormat::SpatialEntityLoader::SpatialEntityList mEntities;
};

void DebugGame::init()
{
    mRenderer.loadMission(MISSION_NAME);
    mPhysicsWorld.loadMission(MISSION_NAME);
    auto treeKlzBodies = mPhysicsWorld.getTreeKlzBodies();
    mEntities = mSpatialEntityLoader.loadFromScene(mRenderer.getRootNode(), treeKlzBodies);

    mRenderer.setCameraParameters(true, 75.0f, 0.0f, 0.25, 2000);

    // TODO replace this by actual player actor, who will have the camera attached on itself.
    mRenderer.setFreeCameraSpeed(7.0);
}

void DebugGame::shutdown()
{
}

void DebugGame::input(double delta)
{
    // TODO process all input events, pass them to relevant actors (Player in particular)
}

void DebugGame::update(double delta)
{
    // TODO process all actor actions
    mPhysicsWorld.frame(delta);

    // TODO tell our platform layer we want to exit
    if (mRenderer.done()) sIsRunning = false;
}

void DebugGame::frame(double delta)
{
    // TODO our rendering pipeline, or an extension of it, this is where we draw our scene, the UI and debug viz
    mRenderer.frame(delta);
}


}

#endif // DEBUG_GAME_H