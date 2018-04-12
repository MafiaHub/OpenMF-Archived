#include "mission/mission_manager.hpp"
#include "mission/mission_impl.hpp"
#include "engine/engine.hpp"

namespace MFGame
{

MissionManager::MissionManager(MFGame::Engine *engine)
{
    mEngine = engine;
}

void MissionManager::loadMission(std::string missionName)
{
    auto it = mMissions.find(missionName);

    if (it != mMissions.end()) {
        if (mCurrentMission && mCurrentMission != it->second) {
            mCurrentMission->unload();
        }
        mCurrentMission = it->second;
    }
    else {
        Mission *mission = new MissionImpl(missionName, mEngine);
        if (mission->load()) {
            mCurrentMission = mission;
            mMissions.insert(std::make_pair(missionName, mission));
        }
        else {
            MFLogger::Logger::fatal("Could not load mission: " + missionName, MISSION_MANAGER_MODULE_STR);
        }
    }
}

}