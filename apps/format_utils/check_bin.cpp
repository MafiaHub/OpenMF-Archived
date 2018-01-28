#include <zpl.h>
#include <general/vfs.h>
#include <general/logger.h>
#include <formats/check_bin/check_bin.h>

#include <cxxopts.hpp>

#define omf_logger_fatal(fmt, ...) \
        omf_logger_ext(NULL, OMF_LOGGER_FATAL, fmt, #__VA_ARGS__)

const char *dump_type(u32 pointType) {
    switch(pointType) {
        case OMF_CHECKBIN_POINTTYPE_PEDESTRIAN:
            return "PED";

        case OMF_CHECKBIN_POINTTYPE_AI:
            return "AI";

        case OMF_CHECKBIN_POINTTYPE_VEHICLE:
            return "SALINA|TRAIN";

        case OMF_CHECKBIN_POINTTYPE_TRAM_STATION:
            return "SALINA STOP";

        case OMF_CHECKBIN_POINTTYPE_SPECIAL:
            return "AI SPECIAL";
    }

    return "Unknown";
}

inline void debug_dump(omf_checkbin_t *checkBin) {
    u32 linkIter = 0;

    for (usize i = 0; i != zpl_array_count(checkBin->points); ++i) {
        auto point = checkBin->points[i];

        omf_logger_raw("[P%d][%s] %f %f %f", i, dump_type(point.mType),
            point.mPos.x, point.mPos.y, point.mPos.z);

        for (u32 j = 0; j < point.mEnterLinks; j++) {
            auto link = checkBin->links[linkIter+j];
            auto targetPoint = checkBin->points[link.mTargetPoint];

            omf_logger_raw("[P%d] Link to [P%d][%s] %f %f %f %f",
                i, link.mTargetPoint, dump_type(targetPoint.mType),
                targetPoint.mPos.x, targetPoint.mPos.y, targetPoint.mPos.z, point.mPos.z);
        }

        linkIter += point.mEnterLinks;
    }

    omf_logger_raw("number of points: %d", zpl_array_count(checkBin->points));
    omf_logger_raw("number of links: %d", zpl_array_count(checkBin->links));
}

void console_logger(const char *src, u8 vb, const char *str, usize size) {
    if (vb == OMF_LOGGER_FATAL) {
        zpl_printf("[FATAL ERROR]: %s", str);
        return;
    }

    zpl_printf("%s", str);
}

int main(int argc, char** argv)
{
    omf_vfs_init();
    omf_logger_add(console_logger);

    cxxopts::Options options("check_bin", "CLI utility for Mafia check.bin format.");
    options.add_options()
        ("h,help", "Display help and exit.")
        ("i,input", "Specify input file name.", cxxopts::value<std::string>());

    options.parse_positional({"i", "f"});
    options.positional_help("file internal_file");

    auto arguments = options.parse(argc, argv);

    char foo[55];
    char bar[55];
    zpl_memcopy(foo, bar, 55);

    if (arguments.count("h") > 0) {
        omf_logger_raw(options.help().c_str());
        return 0;
    }

    if (arguments.count("i") < 1) {
        omf_logger_raw(options.help().c_str());
        omf_logger_fatal("expected file");
        return 1;
    }

    std::string input = arguments["i"].as<std::string>();
    zpl_file_t file = {0};

    if (!omf_vfs_open(&file, input.c_str())) {
        omf_logger_fatal("could not open file %s", input.c_str());
        return 1;
    }

    omf_checkbin_t format = {0};
    if (!omf_checkbin_parse(&format, &file)) {
        omf_logger_fatal("could not parse file %f", inputFile.c_str());
        return 1;
    }

    debug_dump(&format);
    omf_vfs_close(&file);
    omf_checkbin_free(&format);

    return 0;
}
