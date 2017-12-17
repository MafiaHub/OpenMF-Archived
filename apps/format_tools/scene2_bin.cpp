#include <iostream>
#include <format_parser_scene2_bin.hpp>
#include <logger_console.hpp>
#include <utils.hpp>

using namespace MFLogger;

void printHelp()
{
    std::cout << "Scene2.bin format tool" << std::endl << std::endl;
    std::cout << "usage: scene2_bin file" << std::endl;
}

void dump(MFFormat::DataFormatScene2BIN scene2_bin)
{
    // TODO(zaklaus): dump data
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
