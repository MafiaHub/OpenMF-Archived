#define ZPL_IMPLEMENTATION
#define OMF_LOGGER_IMPLEMENTATION

#include <zpl.h>
#include <logger.h>
#include <cxxopts.hpp>

#define omf_logger_fatal(fmt, ...) \
        omf_logger_ext(NULL, OMF_LOGGER_FATAL, fmt, #__VA_ARGS__)

#include <formats/check_bin/parser.hpp>
#include <utils/openmf.hpp>
#include <vfs/vfs.hpp>

const char *getStringPointType(uint32_t pointType) {
    switch(pointType) {
        case MFFormat::DataFormatCheckBIN::POINTTYPE_PEDESTRIAN:
            return "PED";

        case MFFormat::DataFormatCheckBIN::POINTTYPE_AI:
            return "AI";

        case MFFormat::DataFormatCheckBIN::POINTTYPE_VEHICLE:
            return "SALINA|TRAIN";

        case MFFormat::DataFormatCheckBIN::POINTTYPE_TRAM_STATION:
            return "SALINA STOP";

        case MFFormat::DataFormatCheckBIN::POINTTYPE_SPECIAL:
            return "AI SPECIAL";
    }

    return "Unknown";
}

inline void debug_dump(MFFormat::DataFormatCheckBIN checkBin) {
    u32 linkIter = 0;

    for (usize i = 0; i != checkBin.getNumPoints(); ++i) {
        auto point = checkBin.getPoints()[i];

        omf_logger_raw("[P%d][%s] %f %f %f", i, getStringPointType(point.mType),
            point.mPos.x, point.mPos.y, point.mPos.z);

        for (u32 j = 0; j < point.mEnterLinks; j++) {
            auto link = checkBin.getLinks()[linkIter+j];
            auto targetPoint = checkBin.getPoints()[link.mTargetPoint];

            omf_logger_raw("[P%d] Link to [P%d][%s] %f %f %f %f",
                i, link.mTargetPoint, getStringPointType(targetPoint.mType),
                targetPoint.mPos.x, targetPoint.mPos.y, targetPoint.mPos.z, point.mPos.z);
        }

        linkIter += point.mEnterLinks;
    }

    omf_logger_raw("number of points: %d", checkBin.getNumPoints());
    omf_logger_raw("number of links: %d", checkBin.getNumLinks());
}

void console_logger(const char *src, u8 vb, const char *str, usize size) {
    if (vb == OMF_LOGGER_FATAL) {
        zpl_printf("[FATAL ERROR]: %s", str);
    } else {
        zpl_printf("%s", str);
    }
}

int main(int argc, char** argv)
{
    omf_logger_add(console_logger);

    cxxopts::Options options("check_bin", "CLI utility for Mafia check.bin format.");
    options.add_options()
        ("h,help", "Display help and exit.")
        ("i,input", "Specify input file name.", cxxopts::value<std::string>());

    options.parse_positional({"i", "f"});
    options.positional_help("file internal_file");

    auto arguments = options.parse(argc,argv);

    if (arguments.count("h") > 0) {
        omf_logger_raw(options.help().c_str());
        return 0;
    }

    if (arguments.count("i") < 1) {
        omf_logger_raw(options.help().c_str());
        omf_logger_fatal("expected file");
        return 1;
    }

    std::string inputFile = arguments["i"].as<std::string>();
    std::ifstream f;
    auto fs = MFFile::FileSystem::getInstance();

    if (!fs->open(f, inputFile, std::ifstream::binary)) {
        omf_logger_fatal("could not open file %f", inputFile.c_str());
        return 1;
    }

    MFFormat::DataFormatCheckBIN checkBin;
    bool success = checkBin.load(f);

    if (!success) {
        omf_logger_fatal("could not parse file %f", inputFile.c_str());
        return 1;
    }

    debug_dump(checkBin);
    return 0;
}
