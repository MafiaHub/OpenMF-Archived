#include <iostream>
#include <scene2_bin/parser_scene2bin.hpp>
#include <utils/logger.hpp>
#include <vfs/vfs.hpp>
#include <utils/openmf.hpp>
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
    {0x23, "SPECIAL_OBJECT_TYPE_PHYSICAL"},
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
    std::cout << "view distance: " + std::to_string(scene2Bin.getViewDistance()) + ","<< std::endl;
    std::cout << "field of view: " + std::to_string(scene2Bin.getFov()) + ","<< std::endl;
    std::cout << "clipping planes: [" + scene2Bin.getClippingPlanes().str() + "],"<< std::endl;
    std::cout << "number of objects: " + std::to_string(scene2Bin.getNumObjects()) + ","<< std::endl;
    std::cout << ""<< std::endl;

    for (auto pair : scene2Bin.getObjects())
    {
        auto object = pair.second;

        if (object.mType != objType && objType != 0) continue;

        std::cout << "\tobject name: " + object.mName + " {"<< std::endl;
        std::cout << "\t\ttype: " + std::string(getTypeName(sizeof(gObjectTypeNames)/sizeof(gObjectTypeNames[0]), gObjectTypeNames, object.mType)) + "(" + std::to_string(object.mType) + "),"<< std::endl;
        std::cout << "\t\tposition: [" + object.mPos.str() + "],"<< std::endl;
        std::cout << "\t\tposition2: [" + object.mPos2.str() + "],"<< std::endl;
        std::cout << "\t\trotation: [" + object.mRot.str() + "],"<< std::endl;
        std::cout << "\t\tscale: [" + object.mScale.str() + "],"<< std::endl;
        std::cout << "\t\tparent name: " + object.mParentName + ","<< std::endl;

        if (object.mType == MFFormat::DataFormatScene2BIN::OBJECT_TYPE_MODEL | MFFormat::DataFormatScene2BIN::SPECIAL_OBJECT_TYPE_PHYSICAL)
            std::cout << "\t\tmodel name: " + object.mModelName + ","<< std::endl;

        if (object.mType == MFFormat::DataFormatScene2BIN::SPECIAL_OBJECT_TYPE_PHYSICAL) {
            auto props = object.mSpecialProps;
            std::cout << "\t\tphysical object properties {" << std::endl;
            std::cout << "\t\t\tmovement value: [" + MFUtil::arrayToString<float>(props.mMovVal, 4, ", ") + "]," << std::endl;
            std::cout << "\t\t\tmovement value 5: " + std::to_string(props.mMovVal5) + "," << std::endl;
            std::cout << "\t\t\tweight: " + std::to_string(props.mWeight) + "," << std::endl;
            std::cout << "\t\t\tsound: " + std::to_string(props.mSound) + "," << std::endl;
            std::cout << "\t\t}" << std::endl;
        }
        
        if (object.mType == MFFormat::DataFormatScene2BIN::OBJECT_TYPE_LIGHT)
        {
            std::cout << "\t\tlight properties {"<< std::endl;
            std::cout << "\t\t\tlight type: " + std::string(getTypeName(5, gLightTypeNames, object.mLightType)) + "(" + std::to_string(object.mLightType) + "),"<< std::endl;
            std::cout << "\t\t\tlight colour: [" + object.mLightColour.str() + "],"<< std::endl;
            std::cout << "\t\t\tlight power: " + std::to_string(object.mLightPower)<< std::endl;
            std::cout << "\t\t\tlight unk0: " + std::to_string(object.mLightUnk0) + ","<< std::endl;
            std::cout << "\t\t\tlight unk1: " + std::to_string(object.mLightUnk1) + ","<< std::endl;
            std::cout << "\t\t\tlight near: " + std::to_string(object.mLightNear) + ","<< std::endl;
            std::cout << "\t\t\tlight far: " + std::to_string(object.mLightFar) + ","<< std::endl;
            std::cout << "\t\t}"<< std::endl;
        }
        std::cout << "\t}"<< std::endl;
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

    MFFormat::DataFormatScene2BIN scene2Bin;

    bool success = scene2Bin.load(f);

    if (!success)
    {
        MFLogger::Logger::fatal("Could not parse file " + inputFile + ".");
        return 1;
    }

    uint32_t objType = 0;

    if (arguments.count("t") >= 1)
        objType = arguments["t"].as<int>();

    dump(scene2Bin, objType);

    return 0;
}
