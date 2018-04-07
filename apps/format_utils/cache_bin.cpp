#include <iostream>
#include <cache_bin/parser_cachebin.hpp>
#include <utils/logger.hpp>
#include <vfs/vfs.hpp>
#include <utils/openmf.hpp>
#include <cxxopts.hpp>
#include <string>

using namespace MFLogger;

void dump(MFFormat::DataFormatCacheBIN cacheBin)
{
    using namespace MFUtil;
    std::cout << "{\n";
    dumpValue("numberOfObjects", std::to_string(cacheBin.getNumObjects()), 1);
    std::cout << "    \"objects\": [" << std::endl;
    for (auto object : cacheBin.getObjects())
    {
        std::cout << "        {\n";
        dumpValue("objectName", object.mObjectName, 3);
        //dumpValue("bounds", "[" + MFUtil::arrayToString<int8_t>(object.mBounds, 0x4C, ", ") + "]", 3, false);
        dumpValue("numberOfInstances", std::to_string(object.mInstances.size()), 3, false);
        std::cout << "            \"instances\": [" << std::endl;

        for (auto instance : object.mInstances)
        {
            std::cout << "                {\n";
            dumpValue("modelName", instance.mModelName, 5);
            dumpValue("position", instance.mPos.str(), 5, false);
            dumpValue("rotation", instance.mRot.str(), 5, false);
            dumpValue("scale", instance.mScale.str(), 5, false);
            dumpValue("scale2", instance.mScale2.str(), 5, false);
            dumpValue("unk0", std::to_string(instance.mUnk0), 5, false);
            std::cout << "                },\n";
        }

        std::cout << "            ],\n        },\n";
    }

    std::cout << "    ],\n}\n";
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
