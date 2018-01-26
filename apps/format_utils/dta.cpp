#include <iostream>
#include <iomanip>
#include <parsers/dta/parser.hpp>
#include <parsers/dta/key_extractor.hpp>
#include <utils.hpp>
#include <loggers/console.hpp>
#include <cxxopts.hpp>
#include <algorithm>
#include <osdefines.hpp>

#define ZPL_IMPLEMENTATION
#include <zpl.h>

#define ALIGN 50
#define DTA_MODULE_STR "DTA util"

void dump(MFFormat::DataFormatDTA &dta, bool displaySize, bool verbose)
{
    std::cout << "number of files: " << dta.getNumFiles() << std::endl;
    std::cout << "file list:" << std::endl;

    std::vector<MFFormat::DataFormatDTA::FileTableRecord> ftr = dta.getFileTableRecords();
    std::vector<MFFormat::DataFormatDTA::DataFileHeader> dfh = dta.getDataFileHeaders();

    for (int i = 0; i < (int) dta.getNumFiles(); ++i)
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

bool extract(MFFormat::DataFormatDTA &dta, std::ifstream &DTAStream, std::string DTAFile, std::string outFile, std::string pathPrefix)
{
    if (pathPrefix.length() > 0 && pathPrefix.end()[0] != ZPL_PATH_SEPARATOR)
        pathPrefix += ZPL_PATH_SEPARATOR;

    outFile = pathPrefix + outFile;

    // TODO: Improve this
    #ifdef OMF_SYSTEM_WINDOWS
        char const *baseName = zpl_path_base_name(outFile.c_str());
        system(std::string("mkdir \"" + std::string(baseName) + "\" && copy /y nul \"" + outFile + "\"").c_str());
    #else
        outFile = "./" + outFile;
        std::replace(outFile.begin(), outFile.end(), '\\', '/');
        system(std::string("mkdir -p \"$(dirname \"" + outFile + "\")\" && touch \"" + outFile + "\"").c_str());
    #endif

    MFLogger::ConsoleLogger::info("Extracting " + DTAFile + " to " + outFile + ".", DTA_MODULE_STR);

    int fileIndex = dta.getFileIndex(DTAFile);

    if (fileIndex < 0)
    {
        MFLogger::ConsoleLogger::fatal("File " + DTAFile + " not found.", DTA_MODULE_STR);
        return false;
    }

    std::ofstream f;
    f.open(outFile, std::ios::binary);

    if (!f.is_open())
    {
        MFLogger::ConsoleLogger::fatal("Could not open file " + outFile + ".", DTA_MODULE_STR);
        return false;
    }


    auto buffer = dta.getFile(DTAStream,fileIndex);

    f.write(buffer,buffer.size());
    f.close();
    return true;
}

int main(int argc, char** argv)
{
    MFLogger::ConsoleLogger::getInstance()->setVerbosityFlags(1);

    cxxopts::Options options(DTA_MODULE_STR,"CLI utility for Mafia DTA format.");

    options.add_options()
        ("g,game-exe", "Specify game executable file name")
        ("s,size","Display file sizes.")
        ("v,verbose","Display extensive info about each file.")
        ("h,help","Display help and exit.")
        ("d,decrypt","Decrypt whole input file with corresponding keys, mostly for debugging. See also -S.")
        ("S,shift-keys","Shift decrypting keys by given number of bytes. Has only effect with -d.",cxxopts::value<int>())
        ("e,extract","Extract given file.",cxxopts::value<std::string>())
        ("E,extract-all","Extract all files from specified DTA file.")
        ("o,output","Specify output file (for -e) or directory (for -E).",cxxopts::value<std::string>())
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
    bool extractAllMode = arguments.count("E") > 0;
    bool verbose = arguments.count("v") > 0;

    std::string output;

    if (arguments.count("o") > 0)
        output = arguments["o"].as<std::string>();

    if (extractMode && extractAllMode)
    {
        MFLogger::ConsoleLogger::fatal("Combination of -e and -E not allowed.", DTA_MODULE_STR);
        return 1;
    }

    if (arguments.count("i") < 1)
    {
        MFLogger::ConsoleLogger::fatal("Expected file.", DTA_MODULE_STR);
        std::cout << options.help() << std::endl;
        return 1;
    }

    std::string executableFile = "game.exe";
    if(arguments.count("g") >= 1) 
    {
        executableFile = arguments["g"].as<std::string>();
    }    

    std::string inputFile = arguments["i"].as<std::string>();

    std::ifstream f, gameExe;

    if (decryptMode) 
    {
        f.open(inputFile, std::ios::ate | std::ios::binary);
        gameExe.open(executableFile, std::ios::binary);
    }
    else
    {
        gameExe.open(executableFile, std::ios::binary);
        f.open(inputFile, std::ios::binary);
    }
        

    if (!f.is_open())
    {
        MFLogger::ConsoleLogger::fatal("Could not open file " + inputFile + ".", DTA_MODULE_STR);
        return 1;
    }

    if(!gameExe.is_open())
    {
        MFLogger::ConsoleLogger::fatal("Could not open file " + executableFile + ".", DTA_MODULE_STR);
        return 1;
    }

    MFFormat::DataFormatDTA dta;
    MFFormat::DataFormatDTAKeyExtrator dtaKeyExtractor;
    std::vector<std::string> filePath = MFUtil::strSplit(inputFile, ZPL_PATH_SEPARATOR);
    std::string fileName = MFUtil::strToLower(filePath.back());

    for(auto dtaFile : dtaKeyExtractor.getFiles())
    {
        if(fileName.compare(dtaFile.mFileName) == 0) 
        {
            uint32_t keysToSet[] = { dtaFile.mFileKey1, dtaFile.mFileKey2 };
            dta.setDecryptKeys(keysToSet);
        }
    }
  
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

        ScopedBuffer buffer(fileSize);
        f.read(buffer,fileSize);
        f.close();

        dta.decrypt(buffer,fileSize,relativeShift);

        f2.write(buffer,fileSize);

        f2.close();
        return 0;
    }

    bool success = dta.load(f);

    if (!success)
    {
        MFLogger::ConsoleLogger::fatal("Could not parse file " + inputFile + ".", DTA_MODULE_STR);
        f.close();
        return 1;
    }

    if (extractMode)
    {
        std::string extractFile = arguments["e"].as<std::string>();
        std::string outputFile = output.length() > 0 ? output : MFUtil::strToLower(extractFile);

        if (!extract(dta,f,extractFile,outputFile,""))
        {
            f.close();
            return 1;
        }
    }
    else if (extractAllMode)
    {
        MFLogger::ConsoleLogger::info("Extracting file " + inputFile + ".", DTA_MODULE_STR);

        for (int i = 0; i < (int) dta.getNumFiles(); ++i)
            extract(dta,f,dta.getFileName(i),MFUtil::strToLower(dta.getFileName(i)),output);
    }
    else
    {
        dump(dta,displaySize,verbose);
    }

    f.close();

    return 0;
}
