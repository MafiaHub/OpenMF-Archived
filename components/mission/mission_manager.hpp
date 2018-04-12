#pragma once

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

private:
    MFGame::Engine *mEngine;
    MissionPair mMissions;
    Mission *mCurrentMission;
};

}