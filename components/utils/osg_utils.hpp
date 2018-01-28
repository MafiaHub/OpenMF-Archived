#ifndef OMF_OSG_UTILS_H
#define OMF_OSG_UTILS_H

/* forward declarations of osg stuffs */
#ifndef OSG_VEC3F
    namespace osg { class Vec3f; }
#endif

#ifndef OSG_QUAT
    namespace osg { class Quat; }
#endif


    OMF_DEF osg::Vec3f omf_osg_utils_v3(zplm_vec3_t v);
    OMF_DEF osg::Quat omf_osg_utils_quat(zplm_quat_t v);


#if defined(OMF_OSG_UTILS_IMPLEMENTATION) && !defined(OMF_OSG_UTILS_IMPLEMENTATION_DONE)
#define OMF_OSG_UTILS_IMPLEMENTATION_DONE

    osg::Matrixd omf_osg_utils__matrix() {
        zpl_local_persist osg::Matrixd matrix;
        zpl_local_persist b32 initialized = false;
        if (initialized) return matrix;

        initialized = true;

        matrix.makeScale(osg::Vec3f(1,1,-1));
        matrix.postMult( osg::Matrixd::rotate(osg::PI / 2.0,osg::Vec3f(1,0,0)) );

        return matrix;
    }

    osg::Matrixd omf_osg_utils__matrix_inverted() {
        zpl_local_persist osg::Matrixd matrix;
        zpl_local_persist b32 initialized = false;
        if (initialized) return matrix;

        initialized = true;

        matrix = omf_osg_utils__matrix();
        matrix.invert(matrix);

        return matrix;
    }

    osg::Vec3f omf_osg_utils_v3(zplm_vec3_t v) {
        return omf_osg_utils__matrix().preMult(osg::Vec3(v.x, v.y, v.z));
    }

    osg::Quat omf_osg_utils_quat(zplm_quat_t q) {
        osg::Matrixd transform;
        transform.preMult(omf_osg_utils__matrix());
        transform.preMult(osg::Matrixd::rotate(osg::Quat(q.x,q.y,q.z,q.w)));
        transform.preMult(omf_osg_utils__matrix_inverted());
        return transform.getRotate();
    }


#endif // OMF_OSG_UTILS_IMPLEMENTATION
#endif // OMF_OSG_UTILS_H
