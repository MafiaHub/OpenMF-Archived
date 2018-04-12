#pragma once

#include <string>

namespace MFGame
{

class Mission
{
public:
    Mission(std::string missionName);
    virtual ~Mission() = 0;

    virtual bool load()=0;
    virtual bool unload()=0;
    virtual bool importFile()=0;
    virtual bool exportFile()=0;

protected:
    std::string mMissionName;


};

}