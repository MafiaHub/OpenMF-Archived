#include <iostream>
#include <effects/parser_effects_bin.hpp>
#include <utils/logger.hpp>
#include <vfs/vfs.hpp>
#include <utils/openmf.hpp>
#include <cxxopts.hpp>

void dump(MFFormat::DataFormatEffectsBIN effectsBin)
{
    using namespace MFUtil;
    std::cout << "{\n";
    dumpValue("numberOfEffects", std::to_string(effectsBin.getNumEffects()), 1);
    std::cout << "    \"effects\": [\n";
    for (auto effect : effectsBin.getEffects())
    {
        std::cout << "        {\n";
        dumpValue("id", std::to_string(effect.mEffectId), 3, false);
        dumpValue("position", effect.mPos.str(), 3, false);
        dumpValue("sign", std::to_string(effect.mSign), 3, false);
        dumpValue("unk0", "["+arrayToString<uint8_t>(effect.mUnk0, 48, ", ")+"]", 3, false);
        dumpValue("unk1", std::to_string(effect.mUnk1), 3, false);
        dumpValue("size", std::to_string(effect.mSize), 3, false);
        std::cout << "        },\n";
    }
    std::cout << "    ],\n";
    std::cout << "}\n";
}

int main(int argc, char** argv)
{
    cxxopts::Options options("effects_bin", "CLI utility for Mafia effects.bin format.");

    options.add_options()
        ("h,help", "Display help and exit.")
        ("i,input", "Specify input file name.",cxxopts::value<std::string>());

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
        MFLogger::Logger::fatal("Expected file.", "dump");
        std::cout << options.help() << std::endl;
        return 1;
    }

    std::string inputFile = arguments["i"].as<std::string>();

    std::ifstream f;

    auto fs = MFFile::FileSystem::getInstance();

    if (!fs->open(f, inputFile, std::ifstream::binary))
    {
        MFLogger::Logger::fatal("Could not open file " + inputFile + ".", "dump");
        return 1;
    }

    MFFormat::DataFormatEffectsBIN effectsBin;

    bool success = effectsBin.load(f);

    if (!success)
    {
        MFLogger::Logger::fatal("Could not parse file " + inputFile + ".", "dump");
        return 1;
    }

    dump(effectsBin);

    return 0;
}
