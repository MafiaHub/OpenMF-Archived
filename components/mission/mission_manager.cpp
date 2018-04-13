#include "mission/mission_manager.hpp"
#include "mission/mission_impl.hpp"
#include "engine/engine.hpp"

namespace MFGame
{

MissionManager::MissionManager(MFGame::Engine *engine)
{
    mEngine = engine;
    mCurrentMission = nullptr;
}

void MissionManager::loadMission(std::string missionName)
{
    auto it = mMissions.find(missionName);

    if (it != mMissions.end()) {   
        if (mCurrentMission) {
            mCurrentMission->unload();
        }
        mCurrentMission = it->second;
        mCurrentMission->load();
    }
    else {
        Mission *mission = new MissionImpl(missionName, mEngine);
        if (mission->importFile()) {
            if (mCurrentMission) 
                mCurrentMission->unload();

            mCurrentMission = mission;
            mission->load();
            mMissions.insert(std::make_pair(missionName, mission));
        }
        else {
            MFLogger::Logger::fatal("Could not load mission: " + missionName, MISSION_MANAGER_MODULE_STR);
        }
    }
}

}