#include <iostream>
#include <mnu/parser.hpp>
#include <utils/logger.hpp>
#include <vfs/vfs.hpp>
#include <utils/openmf.hpp>
#include <cxxopts.hpp>

void dump(MFFormat::DataFormatMenuDEF menuDef)
{
    MFLogger::Logger::raw("Controls: " + std::to_string(menuDef.getNumControls()));
    for (auto control : menuDef.getControls())
    {
        MFLogger::Logger::raw("type: " + MFUtil::strReverse(std::string(control.mType)));
        MFLogger::Logger::raw("\tpos: " + std::to_string(control.mPos.x) + " " + std::to_string(control.mPos.y));
        MFLogger::Logger::raw("\tscaleX: " + std::to_string(control.mScaleX));
        MFLogger::Logger::raw("\tscaleY: " + std::to_string(control.mScaleY));
        MFLogger::Logger::raw("\ttextID: " + std::to_string(control.mTextId));
        MFLogger::Logger::raw("\ttextColor: " + std::to_string(control.mTextColor));
        MFLogger::Logger::raw("\tbgColor: " + std::to_string(control.mBgColor));
    }
}

void dump(MFFormat::DataFormatMNU mnu)
{
    MFLogger::Logger::raw("Controls: " + std::to_string(mnu.getNumControls()));
    for (auto control : mnu.getControls())
    {
        MFLogger::Logger::raw("type: " + MFUtil::strReverse(std::string(control.mType)));
        MFLogger::Logger::raw("\tpos: " + std::to_string(control.mPos.x) + " " + std::to_string(control.mPos.y));
        MFLogger::Logger::raw("\tscaleX: " + std::to_string(control.mScaleX));
        MFLogger::Logger::raw("\tscaleY: " + std::to_string(control.mScaleY));
        MFLogger::Logger::raw("\ttextID: " + std::to_string(control.mTextId));
        MFLogger::Logger::raw("\ttextColor: " + std::to_string(control.mTextColor));
        MFLogger::Logger::raw("\tbgColor: " + std::to_string(control.mBgColor));
    }
}

int main(int argc, char** argv)
{
    MFLogger::Logger::info("text");

    cxxopts::Options options("menu","CLI utility for Mafia menu.def files.");

    options.add_options()
        ("h,help","Display help and exit.")
        ("i,input","Specify input file name.",cxxopts::value<std::string>());

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

    auto fileExtension = inputFile.substr(inputFile.length() - 4, 4);
    if (fileExtension == ".mnu")
    {
        MFFormat::DataFormatMNU mnu;

        bool success = mnu.load(f);

        if (!success)
        {
            MFLogger::Logger::fatal("Could not parse file " + inputFile + ".", "dump");
            return 1;
        }

        dump(mnu);
    }
    else if (fileExtension == ".def")
    {
        MFFormat::DataFormatMenuDEF menuDef;

        bool success = menuDef.load(f);

        if (!success)
        {
            MFLogger::Logger::fatal("Could not parse file " + inputFile + ".", "dump");
            return 1;
        }

        dump(menuDef);
    }
    else
    {
        MFLogger::Logger::fatal("Unsupported file extension");
        return 1;
    }

    return 0;
}
