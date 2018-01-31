#include <iostream>
#include <engine/engine.hpp>

class OpenMFEngine: public MFGame::Engine
{
public:
    OpenMFEngine(MFGame::Engine::EngineSettings settings): MFGame::Engine(settings)
    {
    };
};

int main( int argc, char** argv )
{
    MFGame::Engine::EngineSettings settings;
    OpenMFEngine engine(settings);

    engine.loadMission("tutorial");
    engine.setCameraFromString("126.019127,320.425720,3.670851,2.178184,1.295000,-0.000000");

    engine.getRenderer()->setCameraParameters(true,110,0,0.25,2000);

    engine.run();

    return 0;
}
