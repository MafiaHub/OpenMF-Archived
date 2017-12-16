#include <iostream>
#include <iomanip>
#include <format_parser_dta.hpp>

#define ALIGN 30

void print_help()
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
        std::cout << std::setw(ALIGN) << dta.getFileName(i) << dta.getFileSize(i) << std::endl;
    }
}

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        std::cerr << "ERROR: Expecting file name." << std::endl;
        print_help();
        return 1;
    }

    std::ifstream f;

    f.open(argv[1]);

    if (!f.is_open())
    {
        std::cerr << "ERROR: Could not open file " << argv[1] << "." << std::endl;
        return 1;
    }

    MFFormat::DataFormatDTA dta;

return 0;   // tmp, TODO: supply key or the application freezes!

    bool success = dta.load(f);

    f.close();

    if (!success)
    {
        std::cerr << "ERROR: Could not parse file " << argv[1] << "." << std::endl;
        return 1;
    }

    dump(dta);

    return 0;
}
