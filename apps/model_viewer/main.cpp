#include <iostream>
#include <fstream>
#include <4ds/parser.hpp>
#include <scene2_bin/parser.hpp>
#include <loggers/console.hpp>
#include <cxxopts.hpp>
#include <renderer/renderer_osg.hpp>

int main(int argc, char** argv)
{
    cxxopts::Options options("viewer","3D viewer for Mafia 4DS files.");

    options.add_options()
        ("h,help","Display help and exit.")
        ("i,input","Specify input, mission name by default.",cxxopts::value<std::string>())
        ("s,scene","Specify scene2.bin file.",cxxopts::value<std::string>())
        ("f,fov","Specify camera field of view in degrees.",cxxopts::value<int>());

    options.parse_positional({"i"});
    auto arguments = options.parse(argc,argv);

    if (arguments.count("h") > 0)
    {
        std::cout << options.help() << std::endl;
        return 0;
    }

    if (arguments.count("i") < 1)
    {
        MFLogger::ConsoleLogger::fatal("Expected file.");
        std::cout << options.help() << std::endl;
        return 1;
    }

    int fov = 75;

    if (arguments.count("f") > 0)
        fov = arguments["f"].as<int>();

    std::string inputFile = arguments["i"].as<std::string>();

    MFRender::OSGRenderer renderer;
    renderer.loadMission(inputFile);

    renderer.setCameraParameters(true,fov,0,0.01,1000);

    while (!renderer.done())
        renderer.frame();

    return 0;
}
