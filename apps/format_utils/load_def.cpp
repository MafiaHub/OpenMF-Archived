#include <iostream>
#include <formats/load_def/parser.hpp>
#include <loggers/console.hpp>
#include <vfs/vfs.hpp>
#include <utils/openmf.hpp>
#include <cxxopts.hpp>

void dump(MFFormat::DataFormatLoadDEF loadDef)
{
    MFLogger::ConsoleLogger::raw("number of loading screens: " + std::to_string(loadDef.getNumLoadingScreens()), "dump");
    for (auto loadingScreen : loadDef.getLoadingScreens())
    {
        MFLogger::ConsoleLogger::raw("\t" + std::string(loadingScreen.mMissionName) + "\t" + std::string(loadingScreen.mFileName) + "\t" + std::to_string(loadingScreen.mTextId), "dump");
    }
}

int main(int argc, char** argv)
{
    cxxopts::Options options("load_def", "CLI utility for Mafia load.def format.");

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
        MFLogger::ConsoleLogger::fatal("Expected file.", "dump");
        std::cout << options.help() << std::endl;
        return 1;
    }

    std::string inputFile = arguments["i"].as<std::string>();

    std::ifstream f;

    auto fs = MFFile::FileSystem::getInstance();

    if (!fs->open(f, inputFile, std::ifstream::binary))
    {
        MFLogger::ConsoleLogger::fatal("Could not open file " + inputFile + ".", "dump");
        return 1;
    }

    MFFormat::DataFormatLoadDEF loadDef;

    bool success = loadDef.load(f);

    if (!success)
    {
        MFLogger::ConsoleLogger::fatal("Could not parse file " + inputFile + ".", "dump");
        return 1;
    }

    dump(loadDef);

    return 0;
}
