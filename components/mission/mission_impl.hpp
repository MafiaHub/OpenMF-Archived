#pragma once

#include "mission/mission.hpp"

namespace MFGame
{

class MissionImpl: public Mission
{
public:
    MissionImpl();
    virtual ~MissionImpl() override;

    virtual bool load() override;
    virtual bool unload() override;
    virtual bool importFile() override;
    virtual bool exportFile() override;
};

}