#include <iostream>
#include <cache_bin/parser.hpp>
#include <utils/logger.hpp>
#include <vfs/vfs.hpp>
#include <utils/openmf.hpp>
#include <cxxopts.hpp>
#include <string>

using namespace MFLogger;

void dump(MFFormat::DataFormatCacheBIN cacheBin)
{
    MFLogger::Logger::raw("number of objects: " + std::to_string(cacheBin.getNumObjects()) + ".", "dump");
    for (auto object : cacheBin.getObjects())
    {
        MFLogger::Logger::raw("object name: " + object.mObjectName + ":", "dump");
        MFLogger::Logger::raw("\tbounds: " + MFUtil::arrayToString<int8_t>(object.mBounds, 0x4C, " ") + ",", "dump");
        MFLogger::Logger::raw("\tnumber of instances: " + std::to_string(object.mInstances.size()) + ",", "dump");

        for (auto instance : object.mInstances)
        {
            MFLogger::Logger::raw("\t\tmodel name: " + instance.mModelName + ",", "dump");
            MFLogger::Logger::raw("\t\tposition: [" + instance.mPos.str() + "],", "dump");
            MFLogger::Logger::raw("\t\trotation: [" + instance.mRot.str() + "],", "dump");
            MFLogger::Logger::raw("\t\tscale: [" + instance.mScale.str() + "],", "dump");
            MFLogger::Logger::raw("\t\tscale2: [" + instance.mScale2.str() + "],", "dump");
            MFLogger::Logger::raw("\t\tunk0: " + std::to_string(instance.mUnk0) + ",", "dump");
            MFLogger::Logger::raw("", "dump");
        }

        MFLogger::Logger::raw("end of object: " + object.mObjectName, "dump");
    }
}

int main(int argc, char** argv)
{
    cxxopts::Options options("cache_bin","CLI utility for Mafia cache.bin format.");

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
        MFLogger::Logger::fatal("Expected file.", "dump");
        std::cout << options.help() << std::endl;
        return 1;
    }

    std::string inputFile = arguments["i"].as<std::string>();

    std::ifstream f;

    f.open(inputFile);

    if (!f.is_open())
    {
        MFLogger::Logger::fatal("Could not open file " + inputFile + ".", "dump");
        return 1;
    }

    MFFormat::DataFormatCacheBIN cacheBin;

    bool success = cacheBin.load(f);

    if (!success)
    {
        MFLogger::Logger::fatal("Could not parse file " + inputFile + ".", "dump");
        return 1;
    }

    dump(cacheBin);

    return 0;
}
