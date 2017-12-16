#include <iostream>
#include <format_parser_cache_bin.hpp>
#include <logger_console.hpp>
#include <utils.hpp>

using namespace MFLogger;

void printHelp()
{
    std::cout << "Cache.bin format tool" << std::endl << std::endl;
    std::cout << "usage: cache_bin file" << std::endl;
}

void dump(MFFormat::DataFormatCacheBIN cache_bin)
{
    ConsoleLogger::raw("number of objects: " + std::to_string(cache_bin.getNumObjects()) + ".");
    for (auto object : cache_bin.getObjects())
    {
        ConsoleLogger::raw("object name: " + std::string(object.mObjectName) + ":");
        ConsoleLogger::raw("\tbounds: " + MFUtil::arrayToString<int8_t>(object.mBounds, 0x4C, " ") + ",");
        ConsoleLogger::raw("\tnumber of instances: " + std::to_string(object.mInstances.size()) + ",");

        for (auto instance : object.mInstances)
        {
            ConsoleLogger::raw("\t\tmodel name: " + std::string(instance.mModelName) + ",");
            ConsoleLogger::raw("\t\tposition: [" + instance.mPos.str() + "],");
            ConsoleLogger::raw("\t\trotation: [" + instance.mRot.str() + "],");
            ConsoleLogger::raw("\t\tscale: [" + instance.mScale.str() + "],");
            ConsoleLogger::raw("\t\tscale2: [" + instance.mScale2.str() + "],");
            ConsoleLogger::raw("\t\tunk0: " + std::to_string(instance.mUnk0) + ",");
            ConsoleLogger::raw("");
        }

        ConsoleLogger::raw("end of object: " + std::string(object.mObjectName));
    }
}

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        ConsoleLogger::fatal("Expecting file name.");
        printHelp();
        return 1;
    }

    std::string file_name = std::string(argv[1]);

    std::ifstream f;
    f.open(file_name);

    if (!f.is_open())
    {
        ConsoleLogger::fatal("Could not open file " + file_name + ".");
        return 1;
    }

    MFFormat::DataFormatCacheBIN cache_bin;

    bool success = cache_bin.load(f);

    if (!success)
    {
        ConsoleLogger::fatal("Could not parse file " + file_name + ".");
        return 1;
    }

    dump(cache_bin);

    return 0;
}
