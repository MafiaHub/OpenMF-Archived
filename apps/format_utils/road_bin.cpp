#include <iostream>
#include <road_bin/parser.hpp>
#include <utils/logger.hpp>
#include <vfs/vfs.hpp>
#include <utils/openmf.hpp>
#include <cxxopts.hpp>

void dump(MFFormat::DataFormatRoadBIN roadBin)
{
    MFLogger::Logger::raw("number of crossroads: " + std::to_string(roadBin.getNumCrossroads()), "dump");
    for (auto crossroad : roadBin.getCrossroads())
    {
        MFLogger::Logger::raw("\tcrossroad position: " + std::to_string(crossroad.mPos.x) + " " + std::to_string(crossroad.mPos.y) + " " + std::to_string(crossroad.mPos.z), "dump");
        MFLogger::Logger::raw("\tcrossroad speed (km/h): " + std::to_string(crossroad.mSpeed * 3), "dump");
    }

    MFLogger::Logger::raw("number of waypoints: " + std::to_string(roadBin.getNumWaypoints()), "dump");
    for (auto waypoint : roadBin.getWaypoints())
    {
        MFLogger::Logger::raw("\twaypoint position: " + std::to_string(waypoint.mPos.x) + " " + std::to_string(waypoint.mPos.y) + " " + std::to_string(waypoint.mPos.z), "dump");
        MFLogger::Logger::raw("\twaypoint speed (km/h): " + std::to_string(waypoint.mSpeed * 3), "dump");
    }
}

int main(int argc, char** argv)
{
    cxxopts::Options options("road_bin", "CLI utility for Mafia road.bin format.");

    options.add_options()
        ("h,help", "Display help and exit.")
        ("i,input", "Specify input file name.",cxxopts::value<std::string>());

    options.parse_positional({"i","f"});
    options.positional_help("file internal_file");
    auto arguments = options.parse(argc,argv);

    if (arguments.count("h") > 0)
    {
        std::cout << options.help() << std::endl;
        return 0;
    }

    if (arguments.count("i") < 1)
    {
        MFLogger::Logger::fatal("Expected file.", "dump");
        std::cout << options.help() << std::endl;
        return 1;
    }

    std::string inputFile = arguments["i"].as<std::string>();

    std::ifstream f;

    auto fs = MFFile::FileSystem::getInstance();

    if (!fs->open(f, inputFile, std::ifstream::binary))
    {
        MFLogger::Logger::fatal("Could not open file " + inputFile + ".", "dump");
        return 1;
    }

    MFFormat::DataFormatRoadBIN roadBin;

    bool success = roadBin.load(f);

    if (!success)
    {
        MFLogger::Logger::fatal("Could not parse file " + inputFile + ".", "dump");
        return 1;
    }

    dump(roadBin);

    return 0;
}
