#include <iostream>
#include <textdb/parser_textdbdef.hpp>
#include <utils/logger.hpp>
#include <vfs/vfs.hpp>
#include <utils/openmf.hpp>
#include <cxxopts.hpp>

void dump(MFFormat::DataFormatTextdbDEF textDb)
{
    MFLogger::Logger::raw("number of text entries: " + std::to_string(textDb.getNumTextEntries()), "dump");
    for (auto textEntry : textDb.getTextEntries())
    {
        MFLogger::Logger::raw("[" + std::to_string(textEntry.first) + "] " + textEntry.second, "dump");
    }
}

int main(int argc, char** argv)
{
    cxxopts::Options options("textdb", "CLI utility for Mafia textdb_xx.def format.");

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

    MFFormat::DataFormatTextdbDEF textDb;

    bool success = textDb.load(f);

    if (!success)
    {
        MFLogger::Logger::fatal("Could not parse file " + inputFile + ".", "dump");
        return 1;
    }

    dump(textDb);

    return 0;
}
