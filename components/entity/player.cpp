#include "player.hpp"
#include <engine/engine.hpp>

void MFGame::Player::think()
{
    const auto period = getEngine()->getSettings().mUpdatePeriod;

    if (mPlayerController)
    {
        mPlayerController->update(period);
    }

    mNextthink = getEngine()->getTime() + period;
}
