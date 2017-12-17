#include <iostream>
#include <scene2_bin/parser.hpp>
#include <loggers/console.hpp>
#include <utils.hpp>
#include <cxxopts.hpp>

using namespace MFLogger;

void dump(MFFormat::DataFormatScene2BIN scene2Bin, uint32_t objType)
{
    ConsoleLogger::raw("view distance: " + std::to_string(scene2Bin.getViewDistance()) + ",");
    ConsoleLogger::raw("field of view: " + std::to_string(scene2Bin.getFov()) + ",");
    ConsoleLogger::raw("clipping planes: [" + scene2Bin.getClippingPlanes().str() + "],");
    ConsoleLogger::raw("number of objects: " + std::to_string(scene2Bin.getNumObjects()) + ",");
    ConsoleLogger::raw("");

    for (auto pair : scene2Bin.getObjects())
    {
        auto object = pair.second;

        if (object.mType != objType && objType != 0) continue;

        ConsoleLogger::raw("\tobject name: " + object.mName + ",");
        ConsoleLogger::raw("\t\tmodel name: " + object.mModelName + ",");
        ConsoleLogger::raw("\t\ttype: " + std::to_string(object.mType) + ",");
        ConsoleLogger::raw("\t\tposition: [" + object.mPos.str() + "],");
        ConsoleLogger::raw("\t\tposition2: [" + object.mPos2.str() + "],");
        ConsoleLogger::raw("\t\trotation: [" + object.mRot.str() + "],");
        ConsoleLogger::raw("\t\tscale: [" + object.mScale.str() + "],");
        ConsoleLogger::raw("\t\tparent name: " + object.mParentName + ",");
        ConsoleLogger::raw("\t\tlight properties:");
        {
            ConsoleLogger::raw("\t\t\tlight type: " + std::to_string(object.mLightType) + ",");
            ConsoleLogger::raw("\t\t\tlight colour: [" + object.mLightColour.str() + "],");
            ConsoleLogger::raw("\t\t\tlight power: " + std::to_string(object.mLightPower));
            ConsoleLogger::raw("\t\t\tlight unk0: " + std::to_string(object.mLightUnk0) + ",");
            ConsoleLogger::raw("\t\t\tlight unk1: " + std::to_string(object.mLightUnk1) + ",");
            ConsoleLogger::raw("\t\t\tlight near: " + std::to_string(object.mLightNear) + ",");
            ConsoleLogger::raw("\t\t\tlight far: " + std::to_string(object.mLightFar) + ",");
        }
    }
}

int main(int argc, char** argv)
{
    cxxopts::Options options("scene2_bin","CLI utility for Mafia scene2.bin format.");

    options.add_options()
        ("h,help","Display help and exit.")
        ("i,input","Specify input file name.",cxxopts::value<std::string>())
        ("t,type","Specify object type.",cxxopts::value<int>());

    options.parse_positional({"i","t"});
    options.positional_help("file [type]");
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

    std::ifstream f;
    f.open(inputFile);

    if (!f.is_open())
    {
        ConsoleLogger::fatal("Could not open file " + inputFile + ".");
        return 1;
    }

    MFFormat::DataFormatScene2BIN scene2Bin;

    bool success = scene2Bin.load(f);

    if (!success)
    {
        ConsoleLogger::fatal("Could not parse file " + inputFile + ".");
        return 1;
    }

    uint32_t objType = 0;

    if (arguments.count("t") >= 1)
        objType = arguments["t"].as<int>();

    dump(scene2Bin, objType);

    return 0;
}
