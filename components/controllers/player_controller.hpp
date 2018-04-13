#ifndef PLAYER_CONTROLLER
#define PLAYER_CONTROLLER

#include <controllers/character_entity_controller.hpp>
#include <controllers/camera_controllers.hpp>
#include <renderer/base_renderer.hpp>
#include <input/base_input_manager.hpp>
#include <physics/base_physics_world.hpp>

namespace MFGame
{

class PlayerController: public CharacterEntityController
{
public:
    PlayerController(MFGame::Entity *playerEntity, MFRender::Renderer *renderer, MFInput::InputManager *inputManager, MFPhysics::PhysicsWorld *physicsWorld=0);
    virtual ~PlayerController();
    void update(double dt);

protected:
    OrbitEntityCameraController *mCameraController;
    MFRender::Renderer *mRenderer;
    MFInput::InputManager *mInputManager;
    std::shared_ptr<MFInput::KeyInputCallback> mKeyCallback;
};

}

#endif
