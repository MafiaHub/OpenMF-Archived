#include <iostream>
#include <scene2_bin/parser.hpp>
#include <loggers/console.hpp>
#include <utils.hpp>

using namespace MFLogger;

void printHelp()
{
    std::cout << "Scene2.bin format tool" << std::endl << std::endl;
    std::cout << "usage: scene2_bin file" << std::endl;
}

void dump(MFFormat::DataFormatScene2BIN scene2_bin)
{
    ConsoleLogger::raw("view distance: " + std::to_string(scene2_bin.getViewDistance()) + ",");
    ConsoleLogger::raw("field of view: " + std::to_string(scene2_bin.getFov()) + ",");
    ConsoleLogger::raw("clipping planes: [" + scene2_bin.getClippingPlanes().str() + "],");
    ConsoleLogger::raw("number of objects: " + std::to_string(scene2_bin.getNumObjects()) + ",");
    ConsoleLogger::raw("");

    for (auto pair : scene2_bin.getObjects())
    {
        auto object = pair.second;
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

    MFFormat::DataFormatScene2BIN scene2_bin;

    bool success = scene2_bin.load(f);

    if (!success)
    {
        ConsoleLogger::fatal("Could not parse file " + file_name + ".");
        return 1;
    }

    dump(scene2_bin);

    return 0;
}
