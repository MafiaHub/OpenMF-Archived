#include <iostream>
#include <fstream>
#include <loggers/console.hpp>
#include <cxxopts.hpp>
#include <string.h>
#include <stdlib.h>

// TODO: Introduce Platform class and store it there?
static bool sIsRunning = true;

#include "core/debug/debug_game.hpp"

// TODO: Replace with configurable value
#define MS_PER_UPDATE 1 / 60.0f

#define ZPL_IMPLEMENTATION
#include <zpl.h>

// TODO move this to Platform class
#include <chrono>
#include <time.h>
double timeGet()
{
    static auto epoch = std::chrono::high_resolution_clock::now();

    return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - epoch).count() / 1000000000.0;
}

int main( int argc, char** argv )
{
    MFGame::DebugGame game;


    MFLogger::ConsoleLogger::getInstance()->setVerbosityFlags(0xffff);
    MFLogger::ConsoleLogger::getInstance()->addFilter(OSGRENDERER_MODULE_STR);
    MFLogger::ConsoleLogger::getInstance()->setFilterMode(false);

    game.init();

    // TODO put the whole loop into Platform class, then call platform.run()
    double lastTime = timeGet();
    double extraTime = 0.0f;

    while (sIsRunning)
    {
        bool render = false;

        double startTime = timeGet();
        double passedTime = startTime - lastTime;
        lastTime = startTime;
        extraTime += passedTime;

        while (extraTime > MS_PER_UPDATE)
        {
            game.input(MS_PER_UPDATE);
            game.update(MS_PER_UPDATE);

            render = true;
            extraTime -= MS_PER_UPDATE;
        }

        if (render)
        {
            game.frame(MS_PER_UPDATE); // TODO: Use actual delta time
        }
        else
        {
            // Let OS do background work while we wait.
            zpl_sleep_ms(1);
        }
    }

    game.shutdown();

    return 0;
}
