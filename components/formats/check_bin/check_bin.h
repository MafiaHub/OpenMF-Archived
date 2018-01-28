#ifndef OMF_CHECKBIN_H
#define OMF_CHECKBIN_H

#ifdef __cplusplus
extern "C" {
#endif

    /* forward decl */
    struct omf_checkbin_t;

    OMF_DEF b32 omf_checkbin_parse(struct omf_checkbin_t *resource, zpl_file_t *file);
    OMF_DEF b32 omf_checkbin_free(struct omf_checkbin_t *resource);

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

#endif // OMF_CHECKBIN_H
