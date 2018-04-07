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

// draft

void dump(MFFormat::DataFormatScene2BIN scene2Bin, uint32_t objType)
{
    using namespace MFUtil;
    std::cout << "{" << std::endl;
    dumpValue("viewDistance", std::to_string(scene2Bin.getViewDistance()), 1, false);
    dumpValue("fieldOfView", std::to_string(scene2Bin.getFov()), 1, false);
    dumpValue("clippingPlanes", scene2Bin.getClippingPlanes().str(), 1, false);
    dumpValue("numberOfObjects", std::to_string(scene2Bin.getNumObjects()), 1, false);
    std::cout << "    \"objects\": ["<< std::endl;

    for (auto pair : scene2Bin.getObjects())
    {
        auto object = pair.second;
        if (object.mType != objType && objType != 0) continue;

        std::cout << "        {\n";

        dumpValue("objectName", object.mName, 3);
        dumpValue("type", std::string(getTypeName(sizeof(gObjectTypeNames) / sizeof(gObjectTypeNames[0]), gObjectTypeNames, object.mType)), 3);
        dumpValue("typeRaw", std::to_string(object.mType), 3, false);
        dumpValue("position", object.mPos.str(), 3, false);
        dumpValue("position2", object.mPos2.str(), 3, false);
        dumpValue("rotation", object.mRot.str(), 3, false);
        dumpValue("scale", object.mScale.str(), 3, false);
        dumpValue("parentName", object.mParentName, 3, true);

        if (object.mType == MFFormat::DataFormatScene2BIN::OBJECT_TYPE_MODEL || object.mType == MFFormat::DataFormatScene2BIN::SPECIAL_OBJECT_TYPE_PHYSICAL)
            dumpValue("modelName", object.mModelName, 3, true);

        if (object.mType == MFFormat::DataFormatScene2BIN::SPECIAL_OBJECT_TYPE_PHYSICAL) {
            auto props = object.mSpecialProps;
            
            dumpValue("movVal1", std::to_string(props.mMovVal1), 3, false);
            dumpValue("movVal2", std::to_string(props.mMovVal2), 3, false);
            dumpValue("friction", std::to_string(props.mFriction), 3, false);
            dumpValue("movVal4", std::to_string(props.mMovVal4), 3, false);
            dumpValue("movVal5", std::to_string(props.mMovVal5), 3, false);
            dumpValue("weight", std::to_string(props.mWeight), 3, false);
            dumpValue("sound", std::to_string(props.mSound), 3, false);
        }
        
        if (object.mType == MFFormat::DataFormatScene2BIN::OBJECT_TYPE_LIGHT)
        {
            dumpValue("lightType", std::string(getTypeName(5, gLightTypeNames, object.mLightType)), 3);
            dumpValue("lightTypeRaw", std::to_string(object.mLightType), 3, false);
            dumpValue("lightColour", object.mLightColour.str(), 3, false);
            dumpValue("lightPower", std::to_string(object.mLightPower), 3, false);
            dumpValue("lightUnk0", std::to_string(object.mLightUnk0), 3, false);
            dumpValue("lightUnk1", std::to_string(object.mLightUnk1), 3, false);
            dumpValue("lightNear", std::to_string(object.mLightNear), 3, false);
            dumpValue("lightFar", std::to_string(object.mLightFar), 3, false);
        }
        std::cout << "        },\n";
    }
    std::cout << "    ],\n}" << std::endl;
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
