#include <iostream>
#include <cache_bin/parser.hpp>
#include <loggers/console.hpp>
#include <vfs/vfs.hpp>
#include <utils.hpp>
#include <cxxopts.hpp>

using namespace MFLogger;

void dump(MFFormat::DataFormatCacheBIN cacheBin)
{
    ConsoleLogger::raw("number of objects: " + std::to_string(cacheBin.getNumObjects()) + ".");
    for (auto object : cacheBin.getObjects())
    {
        ConsoleLogger::raw("object name: " + object.mObjectName + ":");
        ConsoleLogger::raw("\tbounds: " + MFUtil::arrayToString<int8_t>(object.mBounds, 0x4C, " ") + ",");
        ConsoleLogger::raw("\tnumber of instances: " + std::to_string(object.mInstances.size()) + ",");

        for (auto instance : object.mInstances)
        {
            ConsoleLogger::raw("\t\tmodel name: " + instance.mModelName + ",");
            ConsoleLogger::raw("\t\tposition: [" + instance.mPos.str() + "],");
            ConsoleLogger::raw("\t\trotation: [" + instance.mRot.str() + "],");
            ConsoleLogger::raw("\t\tscale: [" + instance.mScale.str() + "],");
            ConsoleLogger::raw("\t\tscale2: [" + instance.mScale2.str() + "],");
            ConsoleLogger::raw("\t\tunk0: " + std::to_string(instance.mUnk0) + ",");
            ConsoleLogger::raw("");
        }

        ConsoleLogger::raw("end of object: " + object.mObjectName);
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
        MFLogger::ConsoleLogger::fatal("Expected file.");
        std::cout << options.help() << std::endl;
        return 1;
    }

    std::string inputFile = arguments["i"].as<std::string>();

    MFFiles::Filesystem fs;

    std::ifstream f;
    fs.open(f, inputFile);

    if (!f.is_open())
    {
        ConsoleLogger::fatal("Could not open file " + inputFile + ".");
        return 1;
    }

    MFFormat::DataFormatCacheBIN cacheBin;

    bool success = cacheBin.load(f);

    if (!success)
    {
        ConsoleLogger::fatal("Could not parse file " + inputFile + ".");
        return 1;
    }

    dump(cacheBin);

    return 0;
}
