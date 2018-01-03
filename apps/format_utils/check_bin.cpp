#include <iostream>
#include <check_bin/parser.hpp>
#include <loggers/console.hpp>
#include <vfs/vfs.hpp>
#include <utils.hpp>
#include <cxxopts.hpp>

using namespace MFLogger;

std::string getStringPointType(uint32_t pointType)
{
    std::string returnString = "Unknown";
    switch(pointType) 
    {
        case MFFormat::DataFormatCheckBIN::POINTTYPE_PEDESTRIAN:
        {
            returnString = "PED";
        } break;

        case MFFormat::DataFormatCheckBIN::POINTTYPE_AI:
        {
            returnString = "AI";
        } break;

        case MFFormat::DataFormatCheckBIN::POINTTYPE_TRAINSANDSALINAS:
        {
            returnString = "SALINA|TRAIN";
        } break;

        case MFFormat::DataFormatCheckBIN::POINTTYPE_SALINASTOP:
        {
            returnString = "SALINA STOP";
        } break;

        case MFFormat::DataFormatCheckBIN::POINTTYPE_SPECIALAI:
        {
            returnString = "AI SPECIAL";
        } break;
    }
    return returnString;
}

void dump(MFFormat::DataFormatCheckBIN checkBin)
{
    for (auto point : checkBin.getPoints())
    {
        ConsoleLogger::raw("[" + getStringPointType(point.mType) + "] " + std::to_string(point.mPos.x) + " " + std::to_string(point.mPos.y) + " " + std::to_string(point.mPos.z), "dump");
    }
    ConsoleLogger::raw("number of points: " + std::to_string(checkBin.getPointsCount()) + ".", "dump");
}

int main(int argc, char** argv)
{
    cxxopts::Options options("check_bin","CLI utility for Mafia check.bin format.");

    options.add_options()
        ("h,help","Display help and exit.")
        ("i,input","Specify input file name.",cxxopts::value<std::string>());

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
        MFLogger::ConsoleLogger::fatal("Expected file.", "dump");
        std::cout << options.help() << std::endl;
        return 1;
    }

    std::string inputFile = arguments["i"].as<std::string>();

    std::ifstream f;

    f.open(inputFile, std::ifstream::binary);

    if (!f.is_open())
    {
        ConsoleLogger::fatal("Could not open file " + inputFile + ".", "dump");
        return 1;
    }

    MFFormat::DataFormatCheckBIN checkBin;

    bool success = checkBin.load(f);

    if (!success)
    {
        ConsoleLogger::fatal("Could not parse file " + inputFile + ".", "dump");
        return 1;
    }

    dump(checkBin);

    return 0;
}
