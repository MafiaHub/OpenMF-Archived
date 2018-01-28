#include <formats/check_bin/parser.hpp>

namespace MFFormat
{

bool DataFormatCheckBIN::load(std::ifstream &srcFile)
{
    Header header = {};
    read(srcFile, &header);

    if (header.mMagic != 0x1ABCEDF)
    {
        return false;
    }

    uint32_t numLinks = 0;

    for (uint32_t i = 0; i < header.mNumPoints; i++)
    {
        Point point = {};
        read(srcFile, &point);

        numLinks += point.mEnterLinks;

        mPoints.push_back(point);
    }

    // Each point references 0 or more links.
    // For example, if point 0 has mEnterLinks = 2, it means that the first 2 links belong to it.
    // Consequent links in a row belong to point 1, 2 and so on.
    for (uint32_t i = 0; i < numLinks; i++)
    {
        Link link = {};
        read(srcFile, &link);

        mLinks.push_back(link);
    }

    return true;
}

}

#include <zpl.h>
#include <zpl_math.h>
#include <common.h>
#include <logger.h>
#include "check_bin.h"

#ifdef __cplusplus
extern "C" {
#endif

    b32 omf_checkbin_parse(omf_checkbin_t *resource, zpl_file_t *file) {
        omf_assert(resource);

        zpl_array_init(resource->points, zpl_heap_allocator());
        zpl_array_init(resource->links, zpl_heap_allocator());

        omf_checkbin_header_t header = {0};
        zpl_file_read(file, &header, sizeof(header));

        if (header.mMagic != 0x1ABCEDF) {
            return false;
        }

        u32 numLinks = 0;

        for (u32 i = 0; i < header.mNumPoints; i++) {
            omf_checkbin_point_t point = {0};
            zpl_file_read(file, &point, sizeof(point));

            numLinks += point.mEnterLinks;
            zpl_array_append(resource->points, point);
        }

        // Each point references 0 or more links.
        // For example, if point 0 has mEnterLinks = 2, it means that the first 2 links belong to it.
        // Consequent links in a row belong to point 1, 2 and so on.
        for (u32 i = 0; i < numLinks; i++) {
            omf_checkbin_link_t link = {0};
            zpl_file_read(file, &link, sizeof(link));
            zpl_array_append(resource->links, link);
        }

        return true;
    }

    b32 omf_checkbin_free(omf_checkbin_t *resource) {
        omf_assert(resource && resource->points && resource->points);
        zpl_array_free(resource->points);
        zpl_array_free(resource->links);

        return true;
    }

#ifdef __cplusplus
}
#endif
