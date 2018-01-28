#ifndef OMF_CHECKBIN_H
#define OMF_CHECKBIN_H

#include <zpl.h>
#include <zpl_math.h>
#include <general/defines.h>
#include <general/render.h>

#ifdef __cplusplus
extern "C" {
#endif

    /* forward decl */
    struct omf_checkbin_t;

    ////////////////////////////////////////////////////////////////
    //
    // Public API Methods
    //
    //

    OMF_DEF b32 omf_checkbin_parse(struct omf_checkbin_t *resource, zpl_file_t *file);
    OMF_DEF b32 omf_checkbin_free(struct omf_checkbin_t *resource);
    OMF_DEF omf_render_node *omf_checkbin_get_node(struct omf_checkbin_t *resource);


    enum {
        OMF_CHECKBIN_POINTTYPE_PEDESTRIAN = 0x1,
        OMF_CHECKBIN_POINTTYPE_AI = 0x2,
        OMF_CHECKBIN_POINTTYPE_VEHICLE = 0x4,
        OMF_CHECKBIN_POINTTYPE_TRAM_STATION = 0x8,
        OMF_CHECKBIN_POINTTYPE_SPECIAL = 0x10,
    };

    enum {
        OMF_CHECKBIN_LINKTYPE_PEDESTRIAN = 1,
        OMF_CHECKBIN_LINKTYPE_AI = 2,
        OMF_CHECKBIN_LINKTYPE_TRAINSANDSALINAS_FORWARD = 4,
        OMF_CHECKBIN_LINKTYPE_TRAINSANDSALINAS_REVERSE = 0x8400,
        OMF_CHECKBIN_LINKTYPE_OTHER = 0x1000
    };

    #pragma pack(push,1)
    typedef struct {
        // should be 0x1ABCEDF
        u32 mMagic;
        u32 mNumPoints;
    } omf_checkbin_header_t;

    typedef struct {
        vec3 mPos;
        u16 mType;
        u16 mID;
        u16 mAreaSize;
        u8  mUnk[10];
        u8  mEnterLinks;
        u8  mExitLinks; // equals mEnterLinks
    } omf_checkbin_point_t;

    typedef struct {
        u16 mTargetPoint;
        u16 mLinkType;
        f32 mUnk;
    } omf_checkbin_link_t;
    #pragma pack(pop)

    typedef struct omf_checkbin_t {
        zpl_array_t(omf_checkbin_point_t) points;
        zpl_array_t(omf_checkbin_link_t) links;
    } omf_checkbin_t;

#ifdef __cplusplus
}
#endif

#if defined(OMF_CHECKBIN_IMPLEMENTATION) && !defined(OMF_CHECKBIN_IMPLEMENTATION_DONE)
#define OMF_CHECKBIN_IMPLEMENTATION

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

#endif // OMF_CHECKBIN_IMPLEMENTATION
#endif // OMF_CHECKBIN_H
