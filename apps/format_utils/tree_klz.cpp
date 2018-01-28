#include <iostream>
#include <formats/klz/parser.hpp>
#include <loggers/console.hpp>
#include <cxxopts.hpp>
#include <vector>

#define KLZ_MODULE_STR "KLZ util"

void dump(MFFormat::DataFormatTreeKLZ &klz)
{
    std::vector<MFFormat::DataFormatTreeKLZ::Link> links = klz.getLinks();

    std::cout << "LINKS (" << links.size() << "):" << std::endl;

    std::vector<std::string> linkStrings = klz.getLinkStrings();

    for (int i = 0; i < (int) linkStrings.size(); ++i)
        std::cout << "  link " << i << ": " << linkStrings[i] << std::endl;

    std::cout << std::endl;
    std::cout << "GRID (" << klz.getGridWidth() << " x " << klz.getGridHeight() << "):" << std::endl;

    for (int y = 0; y < (int) klz.getGridHeight(); ++y)
        for (int x = 0; x < (int) klz.getGridWidth(); ++x)
        {
            MFFormat::DataFormatTreeKLZ::Cell cell = klz.getGridCell(x,y);

            if (cell.mNumObjects > 0)
                std::cout << "  [" << x << "," << y << "]\tobjects: " << cell.mNumObjects << std::endl;
        }

    std::cout << std::endl;

    #define linkPrint "\t\tlink: " << item.mLink << " (" << linkStrings[item.mLink] << ")"

    #define dumpItems(getFunc,printCmd) \
    {\
        auto items = getFunc;\
        for (int i = 0; i < (int) items.size(); ++i)\
        {\
            auto item = items[i];\
            printCmd;\
        }\
    }

    std::cout << "COLLISIONS:" << std::endl;
    dumpItems(klz.getSphereCols(),std::cout << "  sphere\tr: " << item.mRadius << "\t\tp: " << item.mPosition.str() << linkPrint << std::endl);
    dumpItems(klz.getCylinderCols(),std::cout << "  cylinder\tr: " << item.mRadius << "\t\tp: " << item.mPosition.str() << linkPrint << std::endl);
    dumpItems(klz.getAABBCols(),std::cout << "  AABB\t\tp1: " << item.mMin.str() << "\t\tp2: " << item.mMax.str() << linkPrint << std::endl);
    dumpItems(klz.getOBBCols(),std::cout << "  OBB\t\textends: [" << item.mExtends[0].str() << "] [" << item.mExtends[2].str() << "] " << linkPrint << std::endl);
    dumpItems(klz.getXTOBBCols(),std::cout << "  XTOBB\t\tp1: " << item.mMin.str() << "\t\tp2: " << item.mMax.str() << "\t\textends: [" << item.mExtends[0].str() << "] [" << item.mExtends[2].str() << "] " << linkPrint << std::endl);
    dumpItems(klz.getFaceCols(),std::cout << "  face\t (" << item.mIndices[0].mIndex << ", " << item.mIndices[1].mIndex << ", " << item.mIndices[2].mIndex << "), normal: " << item.mNormal.str() << ", link: " << item.mIndices[0].mLink << " (" << linkStrings[item.mIndices[0].mLink] << ")" << std::endl);
}

int main(int argc, char** argv)
{
    cxxopts::Options options(KLZ_MODULE_STR,"CLI utility for Mafia TREE_KLZ format.");

    options.add_options()
        ("h,help","Display help and exit.")
        ("i,input","Specify input file name.",cxxopts::value<std::string>());

    options.parse_positional({"i"});
    options.positional_help("file");
    auto arguments = options.parse(argc,argv);

    if (arguments.count("h") > 0)
    {
        std::cout << options.help() << std::endl;
        return 0;
    }

    if (arguments.count("i") < 1)
    {
        MFLogger::ConsoleLogger::fatal("Expected file.", KLZ_MODULE_STR);
        std::cout << options.help() << std::endl;
        return 1;
    }

    std::string inputFile = arguments["i"].as<std::string>();

    std::ifstream f;

    f.open(inputFile, std::ios::binary);

    if (!f.is_open())
    {
        MFLogger::ConsoleLogger::fatal("Could not open file " + inputFile + ".", KLZ_MODULE_STR);
        return 1;
    }

    MFFormat::DataFormatTreeKLZ klz;

    klz.load(f);

    f.close();

    dump(klz);

    return 0;
}
