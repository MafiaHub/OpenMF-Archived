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

        case MFFormat::DataFormatCheckBIN::POINTTYPE_VEHICLE:
        {
            returnString = "SALINA|TRAIN";
        } break;

        case MFFormat::DataFormatCheckBIN::POINTTYPE_TRAM_STATION:
        {
            returnString = "SALINA STOP";
        } break;

        case MFFormat::DataFormatCheckBIN::POINTTYPE_SPECIAL:
        {
            returnString = "AI SPECIAL";
        } break;
    }
    return returnString;
}

void dump(MFFormat::DataFormatCheckBIN checkBin)
{
    uint linkIter = 0;
    for (std::size_t i = 0; i != checkBin.getNumPoints(); ++i)
    {
        auto point = checkBin.getPoints()[i];
        ConsoleLogger::raw("[P" + std::to_string(i) + "][" + getStringPointType(point.mType) + "] " + std::to_string(point.mPos.x) + " " + std::to_string(point.mPos.y) + " " + std::to_string(point.mPos.z), "dump");
        for (uint j = 0; j < point.mEnterLinks; j++)
        {
            auto link = checkBin.getLinks()[linkIter+j];
            auto targetPoint = checkBin.getPoints()[link.mTargetPoint];
            ConsoleLogger::raw("[P" + std::to_string(i) + "] Link to [P" + std::to_string(link.mTargetPoint) + "][" + getStringPointType(targetPoint.mType) + "] " + std::to_string(targetPoint.mPos.x) + " " + std::to_string(targetPoint.mPos.y) + " " + std::to_string(point.mPos.z), "dump");
        }
        linkIter += point.mEnterLinks;
    }
    ConsoleLogger::raw("number of points: " + std::to_string(checkBin.getNumPoints()), "dump");
    ConsoleLogger::raw("number of links: " + std::to_string(checkBin.getNumLinks()), "dump");
}

int main(int argc, char** argv)
{
    ConsoleLogger::info("text");

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
