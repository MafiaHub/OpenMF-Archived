#include <iostream>
#include <iomanip>
#include <dta/parser.hpp>
#include <utils.hpp>
#include <loggers/console.hpp>
#include <cxxopts.hpp>

#define ALIGN 50

void dump(MFFormat::DataFormatDTA &dta, bool displaySize)
{
    std::cout << "number of files: " << dta.getNumFiles() << std::endl;
    std::cout << "file list:" << std::endl;

    for (int i = 0; i < dta.getNumFiles(); ++i)
    {
        if (displaySize)
            std::cout << std::setw(ALIGN) << std::left << dta.getFileName(i) << dta.getFileSize(i) << " B" << std::endl;
        else
            std::cout << dta.getFileName(i) << std::endl;
    }
}

int main(int argc, char** argv)
{
    cxxopts::Options options("dta","CLI utility for Mafia DTA format.");

    options.add_options()
        ("s,size","Display file sizes.")
        ("h,help","Display help and exit.")
        ("e,extract","Extract given file. NOT IMPLEMENTED YET",cxxopts::value<std::string>())
        ("i,input","Specify input file name.",cxxopts::value<std::string>());

    options.parse_positional({"i"});
    options.positional_help("file");
    auto arguments = options.parse(argc,argv);

    if (arguments.count("h") > 0)
    {
        std::cout << options.help() << std::endl;
        return 1;
    }

    bool displaySize = arguments.count("s") > 0;

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
        MFLogger::ConsoleLogger::fatal("Could not open file " + inputFile + ".");
        return 1;
    }

    MFFormat::DataFormatDTA dta;

    std::vector<std::string> filePath = MFUtil::strSplit(inputFile,'/');  // FIXME: platform-independece needed 
    std::string fileName = MFUtil::strToLower(filePath.back());

    if (fileName.compare("a0.dta") == 0)
        dta.setDecryptKeys(dta.A0_KEYS);
    else if (fileName.compare("a1.dta") == 0)
        dta.setDecryptKeys(dta.A1_KEYS);
    else if (fileName.compare("a2.dta") == 0)
        dta.setDecryptKeys(dta.A2_KEYS);
    else if (fileName.compare("a3.dta") == 0)
        dta.setDecryptKeys(dta.A3_KEYS);
    else if (fileName.compare("a4.dta") == 0)
        dta.setDecryptKeys(dta.A4_KEYS);
    else if (fileName.compare("a5.dta") == 0)
        dta.setDecryptKeys(dta.A5_KEYS);
    else if (fileName.compare("a6.dta") == 0)
        dta.setDecryptKeys(dta.A6_KEYS);
    else if (fileName.compare("a7.dta") == 0)
        dta.setDecryptKeys(dta.A7_KEYS);
    else if (fileName.compare("a9.dta") == 0)
        dta.setDecryptKeys(dta.A9_KEYS);
    else if (fileName.compare("aa.dta") == 0)
        dta.setDecryptKeys(dta.AA_KEYS);
    else if (fileName.compare("ab.dta") == 0)
        dta.setDecryptKeys(dta.AB_KEYS);
    else if (fileName.compare("ac.dta") == 0)
        dta.setDecryptKeys(dta.AC_KEYS);

    bool success = dta.load(f);

    f.close();

    if (!success)
    {
        MFLogger::ConsoleLogger::fatal("Could not parse file " + inputFile + ".");
        return 1;
    }

    dump(dta,displaySize);

    return 0;
}
