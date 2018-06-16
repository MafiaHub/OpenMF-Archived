#pragma once

#include <entity/entity_impl.hpp>
#include <controllers/player_controller.hpp>

namespace MFGame
{
    class Player : public EntityImpl
    {
    public:
        Player(): mPlayerController(nullptr)
        {
        }

        void think() override;

        void setPlayerController(PlayerController *controller) { mPlayerController = controller; }

    protected:
        PlayerController *mPlayerController = nullptr;
    };
}
