#pragma once

#define MISSION_MANAGER_MODULE_STR "MissionManager"

#include "mission/mission.hpp"

#include <unordered_map>

namespace MFGame
{
    typedef std::unordered_map<std::string, MFGame::Mission*> MissionPair;
    class Engine;
    
class MissionManager 
{
public:
    MissionManager(MFGame::Engine *engine);
    ~MissionManager() = default;

    void loadMission(std::string missionName);
    Mission *getCurrentMission() { return mCurrentMission; }

private:
    MFGame::Engine *mEngine;
    MissionPair mMissions;
    Mission *mCurrentMission;
};

}