#include <iostream>
#include <cache_bin/cachebin_parser.hpp>
#include <utils/logger.hpp>
#include <vfs/vfs.hpp>
#include <utils/openmf.hpp>
#include <cxxopts.hpp>
#include <string>

using namespace MFLogger;

void dump(MFFormat::DataFormatCacheBIN cacheBin)
{
    std::cout << "number of objects: " + std::to_string(cacheBin.getNumObjects()) + "."<< std::endl;
    for (auto object : cacheBin.getObjects())
    {
        std::cout << "object name: " + object.mObjectName + ":"<< std::endl;
        std::cout << "\tbounds: " + MFUtil::arrayToString<int8_t>(object.mBounds, 0x4C, " ") + ","<< std::endl;
        std::cout << "\tnumber of instances: " + std::to_string(object.mInstances.size()) + ","<< std::endl;

        for (auto instance : object.mInstances)
        {
            std::cout << "\t\tmodel name: " + instance.mModelName + ","<< std::endl;
            std::cout << "\t\tposition: [" + instance.mPos.str() + "],"<< std::endl;
            std::cout << "\t\trotation: [" + instance.mRot.str() + "],"<< std::endl;
            std::cout << "\t\tscale: [" + instance.mScale.str() + "],"<< std::endl;
            std::cout << "\t\tscale2: [" + instance.mScale2.str() + "],"<< std::endl;
            std::cout << "\t\tunk0: " + std::to_string(instance.mUnk0) + ","<< std::endl;
            std::cout << ""<< std::endl;
        }

        std::cout << "end of object: " + object.mObjectName<< std::endl;
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
        MFLogger::Logger::fatal("Expected file.");
        std::cout << options.help() << std::endl;
        return 1;
    }

    std::string inputFile = arguments["i"].as<std::string>();

    auto fs = MFFile::FileSystem::getInstance();
    std::ifstream f;

    if (!fs->open(f, inputFile))
    {
        MFLogger::Logger::fatal("Could not open file " + inputFile + ".");
        return 1;
    }

    MFFormat::DataFormatCacheBIN cacheBin;

    bool success = cacheBin.load(f);

    if (!success)
    {
        MFLogger::Logger::fatal("Could not parse file " + inputFile + ".");
        return 1;
    }

    dump(cacheBin);

    return 0;
}
