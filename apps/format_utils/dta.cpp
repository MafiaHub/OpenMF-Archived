#include <iostream>
#include <iomanip>
#include <dta/parser.hpp>
#include <utils.hpp>
#include <loggers/console.hpp>
#include <cxxopts.hpp>

#define ALIGN 50

#define DTA_MODULE_STR "loader DTA"

void dump(MFFormat::DataFormatDTA &dta, bool displaySize, bool verbose)
{
    std::cout << "number of files: " << dta.getNumFiles() << std::endl;
    std::cout << "file list:" << std::endl;

    std::vector<MFFormat::DataFormatDTA::FileTableRecord> ftr = dta.getFileTableRecords();
    std::vector<MFFormat::DataFormatDTA::DataFileHeader> dfh = dta.getDataFileHeaders();

    for (int i = 0; i < dta.getNumFiles(); ++i)
    {
        if (displaySize || verbose)
            std::cout << std::setw(ALIGN) << std::left << dta.getFileName(i) << dta.getFileSize(i) << " B";
        else
            std::cout << dta.getFileName(i);

        std::cout << std::endl;

        if (verbose)
        {
            std::cout << "  " << ftr[i].mFileNameChecksum << " " << ftr[i].mFileNameLength << " " << ftr[i].mHeaderOffset <<
            " " << ftr[i].mDataOffset << " " << dfh[i].mUnknown << " " << dfh[i].mUnknown2 << " " << dfh[i].mTimeStamp << " "
            << " " << dfh[i].mCompressedBlockCount << std::endl;
        } 
    }
}

int main(int argc, char** argv)
{
    cxxopts::Options options(DTA_MODULE_STR,"CLI utility for Mafia DTA format.");

    options.add_options()
        ("s,size","Display file sizes.")
        ("v,verbose","Display extensive info about each file.")
        ("h,help","Display help and exit.")
        ("d,decrypt","Decrypt whole input file with corresponding keys, mostly for debugging. See also -S.")
        ("S,shift-keys","Shift decrypting keys by given number of bytes. Has only effect with -d.",cxxopts::value<int>())
        ("e,extract","Extract given file.",cxxopts::value<std::string>())
        ("i,input","Specify input file name.",cxxopts::value<std::string>());

    options.parse_positional({"i"});
    options.positional_help("file");
    auto arguments = options.parse(argc,argv);

    if (arguments.count("h") > 0)
    {
        std::cout << options.help() << std::endl;
        return 0;
    }

    bool displaySize = arguments.count("s") > 0;
    bool decryptMode = arguments.count("d") > 0;
    bool extractMode = arguments.count("e") > 0;
    bool verbose = arguments.count("v") > 0;

    if (arguments.count("i") < 1)
    {
        MFLogger::ConsoleLogger::fatal("Expected file.", DTA_MODULE_STR);
        std::cout << options.help() << std::endl;
        return 1;
    }

    std::string inputFile = arguments["i"].as<std::string>();

    std::ifstream f;

    if (decryptMode)
        f.open(inputFile, std::ios::ate | std::ios::binary);
    else
        f.open(inputFile, std::ios::binary);

    if (!f.is_open())
    {
        MFLogger::ConsoleLogger::fatal("Could not open file " + inputFile + ".", DTA_MODULE_STR);
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
    else if (fileName.compare("a8.dta") == 0)
        dta.setDecryptKeys(dta.A8_KEYS);
    else if (fileName.compare("a9.dta") == 0)
        dta.setDecryptKeys(dta.A9_KEYS);
    else if (fileName.compare("aa.dta") == 0)
        dta.setDecryptKeys(dta.AA_KEYS);
    else if (fileName.compare("ab.dta") == 0)
        dta.setDecryptKeys(dta.AB_KEYS);
    else if (fileName.compare("ac.dta") == 0)
        dta.setDecryptKeys(dta.AC_KEYS);

    if (decryptMode)   // decrypt whole file
    {
        std::streamsize fileSize = f.tellg();
        f.seekg(0, std::ios::beg);

        unsigned int relativeShift = 0;

        if (arguments.count("S") > 0)
            relativeShift = arguments["S"].as<int>();

        std::string outputFile = inputFile + ".decrypt" + std::to_string(relativeShift);
        MFLogger::ConsoleLogger::info("decrypting into " + outputFile, DTA_MODULE_STR);

        std::ofstream f2;
        f2.open(outputFile, std::ios::binary);

        if (!f2.is_open())
        {
            MFLogger::ConsoleLogger::fatal("Could not open file " + outputFile + ".", DTA_MODULE_STR);
            f.close();
            return 1;
        }
           
        char *buffer = (char *) malloc(fileSize);
        
        f.read(buffer,fileSize);
        f.close();

        dta.decrypt(buffer,fileSize,relativeShift);

        f2.write(buffer,fileSize);

        free(buffer);
        f2.close();
        return 0;    
    }

    bool success = dta.load(f);

    if (extractMode)
    {
        std::string extractFile = arguments["e"].as<std::string>();
        std::string outputFile = "out";

        MFLogger::ConsoleLogger::info("Extracting " + extractFile + " to " + outputFile + ".", DTA_MODULE_STR);

        int fileIndex = dta.getFileIndex(extractFile);

        if (fileIndex < 0)
        {
            MFLogger::ConsoleLogger::fatal("File " + extractFile + " not found.", DTA_MODULE_STR);
            f.close();
            return 1;
        }

        std::ofstream f2;
        f2.open(outputFile);
 
        if (!f2.is_open())
        {
            MFLogger::ConsoleLogger::fatal("Could not open file " + outputFile + ".", DTA_MODULE_STR);
            f.close();
            return 1;
        }

        char *buffer;
        unsigned int fileSize;        
 
        dta.getFile(f,fileIndex,&buffer,fileSize);

        f2.write(buffer,fileSize);

        free(buffer);

        f2.close();
        f.close();
        return 0;
    }

    f.close();

    if (!success)
    {
        MFLogger::ConsoleLogger::fatal("Could not parse file " + inputFile + ".", DTA_MODULE_STR);
        return 1;
    }
    
    dump(dta,displaySize,verbose);

    return 0;
}
