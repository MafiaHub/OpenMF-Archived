#include <iostream>
#include <cache_bin/parser.hpp>
#include <loggers/console.hpp>
#include <vfs/vfs.hpp>
#include <utils.hpp>
#include <cxxopts.hpp>

using namespace MFLogger;

void dump(MFFormat::DataFormatCacheBIN cacheBin)
{
    ConsoleLogger::raw("dump", "number of objects: " + std::to_string(cacheBin.getNumObjects()) + ".");
    for (auto object : cacheBin.getObjects())
    {
        ConsoleLogger::raw("dump", "object name: " + object.mObjectName + ":");
        ConsoleLogger::raw("dump", "\tbounds: " + MFUtil::arrayToString<int8_t>(object.mBounds, 0x4C, " ") + ",");
        ConsoleLogger::raw("dump", "\tnumber of instances: " + std::to_string(object.mInstances.size()) + ",");

        for (auto instance : object.mInstances)
        {
            ConsoleLogger::raw("dump", "\t\tmodel name: " + instance.mModelName + ",");
            ConsoleLogger::raw("dump", "\t\tposition: [" + instance.mPos.str() + "],");
            ConsoleLogger::raw("dump", "\t\trotation: [" + instance.mRot.str() + "],");
            ConsoleLogger::raw("dump", "\t\tscale: [" + instance.mScale.str() + "],");
            ConsoleLogger::raw("dump", "\t\tscale2: [" + instance.mScale2.str() + "],");
            ConsoleLogger::raw("dump", "\t\tunk0: " + std::to_string(instance.mUnk0) + ",");
            ConsoleLogger::raw("dump", "");
        }

        ConsoleLogger::raw("dump", "end of object: " + object.mObjectName);
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
        MFLogger::ConsoleLogger::fatal("dump", "Expected file.");
        std::cout << options.help() << std::endl;
        return 1;
    }

    std::string inputFile = arguments["i"].as<std::string>();

    std::ifstream f;

    f.open(inputFile);

    if (!f.is_open())
    {
        ConsoleLogger::fatal("dump", "Could not open file " + inputFile + ".");
        return 1;
    }

    MFFormat::DataFormatCacheBIN cacheBin;

    bool success = cacheBin.load(f);

    if (!success)
    {
        ConsoleLogger::fatal("dump", "Could not parse file " + inputFile + ".");
        return 1;
    }

    dump(cacheBin);

    return 0;
}
