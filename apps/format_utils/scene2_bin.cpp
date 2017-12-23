#include <iostream>
#include <scene2_bin/parser.hpp>
#include <loggers/console.hpp>
#include <vfs/vfs.hpp>
#include <utils.hpp>
#include <cxxopts.hpp>

using namespace MFLogger;

typedef struct
{
    uint32_t mType;
    const char *mName;
} TypeName;

TypeName const gObjectTypeNames[] = {
    {0x02, "OBJECT_TYPE_LIGHT"},
    {0x03, "OBJECT_TYPE_CAMERA"},
    {0x04, "OBJECT_TYPE_SOUND"},
    {0x09, "OBJECT_TYPE_MODEL"},
    {0x0C, "OBJECT_TYPE_OCCLUDER"},
    {0x99, "OBJECT_TYPE_SECTOR"},
    {0x9B, "OBJECT_TYPE_SCRIPT"},
};

TypeName const gLightTypeNames[] = {
    {0x01, "LIGHT_TYPE_POINT"},
    {0x03, "LIGHT_TYPE_DIRECTIONAL"},
    {0x04, "LIGHT_TYPE_AMBIENT"},
    {0x05, "LIGHT_TYPE_FOG"},
    {0x06, "LIGHT_TYPE_POINT_AMBIENT"},
    {0x08, "LIGHT_TYPE_LAYERED_FOG"},
};

char const *getTypeName(size_t count, TypeName const *typeNames, uint32_t objType, char const *defaultType = "TYPE_UNKNOWN")
{
    for (size_t i = 0; i < count; i++)
    {
        if (typeNames[i].mType == objType)
            return typeNames[i].mName;
    }

    return defaultType;
}

void dump(MFFormat::DataFormatScene2BIN scene2Bin, uint32_t objType)
{
    ConsoleLogger::raw("dump", "view distance: " + std::to_string(scene2Bin.getViewDistance()) + ",");
    ConsoleLogger::raw("dump", "field of view: " + std::to_string(scene2Bin.getFov()) + ",");
    ConsoleLogger::raw("dump", "clipping planes: [" + scene2Bin.getClippingPlanes().str() + "],");
    ConsoleLogger::raw("dump", "number of objects: " + std::to_string(scene2Bin.getNumObjects()) + ",");
    ConsoleLogger::raw("dump", "");

    for (auto pair : scene2Bin.getObjects())
    {
        auto object = pair.second;

        if (object.mType != objType && objType != 0) continue;

        ConsoleLogger::raw("dump", "\tobject name: " + object.mName + " {");
        ConsoleLogger::raw("dump", "\t\ttype: " + std::string(getTypeName(7, gObjectTypeNames, object.mType)) + "(" + std::to_string(object.mType) + "),");
        ConsoleLogger::raw("dump", "\t\tposition: [" + object.mPos.str() + "],");
        ConsoleLogger::raw("dump", "\t\tposition2: [" + object.mPos2.str() + "],");
        ConsoleLogger::raw("dump", "\t\trotation: [" + object.mRot.str() + "],");
        ConsoleLogger::raw("dump", "\t\tscale: [" + object.mScale.str() + "],");
        ConsoleLogger::raw("dump", "\t\tparent name: " + object.mParentName + ",");

        if (object.mType == MFFormat::DataFormatScene2BIN::OBJECT_TYPE_MODEL)
            ConsoleLogger::raw("dump", "\t\tmodel name: " + object.mModelName + ",");
        
        if (object.mType == MFFormat::DataFormatScene2BIN::OBJECT_TYPE_LIGHT)
        {
            ConsoleLogger::raw("dump", "\t\tlight properties {");
            ConsoleLogger::raw("dump", "\t\t\tlight type: " + std::string(getTypeName(5, gLightTypeNames, object.mLightType)) + "(" + std::to_string(object.mLightType) + "),");
            ConsoleLogger::raw("dump", "\t\t\tlight colour: [" + object.mLightColour.str() + "],");
            ConsoleLogger::raw("dump", "\t\t\tlight power: " + std::to_string(object.mLightPower));
            ConsoleLogger::raw("dump", "\t\t\tlight unk0: " + std::to_string(object.mLightUnk0) + ",");
            ConsoleLogger::raw("dump", "\t\t\tlight unk1: " + std::to_string(object.mLightUnk1) + ",");
            ConsoleLogger::raw("dump", "\t\t\tlight near: " + std::to_string(object.mLightNear) + ",");
            ConsoleLogger::raw("dump", "\t\t\tlight far: " + std::to_string(object.mLightFar) + ",");
            ConsoleLogger::raw("dump", "\t\t}");
        }
        ConsoleLogger::raw("dump", "\t}");
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
        MFLogger::ConsoleLogger::fatal("dump", "Expected file.");
        std::cout << options.help() << std::endl;
        return 1;
    }

    std::string inputFile = arguments["i"].as<std::string>();

    std::ifstream f;
    f.open(inputFile, std::ios::binary);

    if (!f.is_open())
    {
        ConsoleLogger::fatal("dump", "Could not open file " + inputFile + ".");
        return 1;
    }

    MFFormat::DataFormatScene2BIN scene2Bin;

    bool success = scene2Bin.load(f);

    if (!success)
    {
        ConsoleLogger::fatal("dump", "Could not parse file " + inputFile + ".");
        return 1;
    }

    uint32_t objType = 0;

    if (arguments.count("t") >= 1)
        objType = arguments["t"].as<int>();

    dump(scene2Bin, objType);

    return 0;
}
