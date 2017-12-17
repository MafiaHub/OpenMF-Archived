#include <iostream>
#include <iomanip>
#include <dta/parser.hpp>
#include <utils.hpp>
#include <logger_console.hpp>

#define ALIGN 50

void printHelp()
{
    std::cout << "DTA format tool" << std::endl << std::endl;

    std::cout << "usage: dta file [options]" << std::endl;
}

void dump(MFFormat::DataFormatDTA &dta)
{
    std::cout << "number of files: " << dta.getNumFiles() << std::endl;
    std::cout << "file list:" << std::endl;

    for (int i = 0; i < dta.getNumFiles(); ++i)
    {
        std::cout << std::setw(ALIGN) << std::left << dta.getFileName(i) << dta.getFileSize(i) << " B" << std::endl;
    }
}

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        MFLogger::ConsoleLogger::fatal("Expected file.");
        printHelp();
        return 1;
    }

    std::ifstream f;

    f.open(argv[1]);

    if (!f.is_open())
    {
        MFLogger::ConsoleLogger::fatal("Could not open file " + std::string(argv[1]) + ".");
        return 1;
    }

    MFFormat::DataFormatDTA dta;

    std::vector<std::string> filePath = MFUtil::strSplit(argv[1],'/');  // FIXME: platform-independece needed 
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
        MFLogger::ConsoleLogger::fatal("Could not parse file " + std::string(argv[1]) + ".");
        return 1;
    }

    dump(dta);

    return 0;
}
