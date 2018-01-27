#ifndef OMF_LOGGER_H
#define OMF_LOGGER_H

#ifndef OMF_LOGGER_BUFFER_SIZE
#define OMF_LOGGER_BUFFER_SIZE 4096
#endif

#ifdef __cplusplus
extern "C" {
#endif

    enum {
        OMF_LOGGER_INFO  = 1,
        OMF_LOGGER_WARN  = 2,
        OMF_LOGGER_FATAL = 4,
    };

    typedef void (omf_logger_callback)(const char *details, u8, const char *str, usize size);

    ZPL_DEF usize omf_logger_add(omf_logger_callback *callback);
    ZPL_DEF usize omf_logger_remove(usize id);

    ZPL_DEF isize omf_logger_raw(const char *fmt, ...);
    ZPL_DEF isize omf_logger(u8 verbosity_flag, const char *fmt, ...);
    ZPL_DEF isize omf_logger_ext(const char *details, u8 verbosity_flag, const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#if defined(OMF_LOGGER_IMPLEMENTATION) && !defined(OMF_LOGGER_IMPLEMENTATION_DONE)
#define OMF_LOGGER_IMPLEMENTATION_DONE

#ifdef __cplusplus
extern "C" {
#endif

    static zpl_array_t(omf_logger_callback *) omf_logger_callbacks = NULL;

    usize omf_logger_add(omf_logger_callback *callback) {
        if (!omf_logger_callbacks) {
            zpl_array_init(omf_logger_callbacks, zpl_heap_allocator());
        }

        zpl_array_append(omf_logger_callbacks, callback);
        return zpl_array_count(omf_logger_callbacks) - 1;
    }

    usize omf_logger_remove(usize id) {
        ZPL_ASSERT(id < zpl_array_count(omf_logger_callbacks));
        omf_logger_callbacks[id] = NULL;
        return id;
    }

    inline isize omf__logger_va(const char *detail, u8 verb_flag, const char *fmt, va_list va) {
        if (!omf_logger_callbacks) return 0;

        zpl_local_persist char buf[OMF_LOGGER_BUFFER_SIZE];
        usize len = zpl_snprintf_va(buf, zpl_size_of(buf), fmt, va);

        // force new line at the end
        buf[len - 1] = '\n';
        buf[len] = '\0';

        for (usize i = 0; i < zpl_array_count(omf_logger_callbacks); ++i) {
            if (omf_logger_callbacks[i]) omf_logger_callbacks[i](detail, verb_flag, buf, len);
        }

        return len;
    }

    inline isize omf_logger_raw(const char *fmt, ...) {
        if (!omf_logger_callbacks) return 0;

        isize res;
        va_list va;
        va_start(va, fmt);
        res = omf__logger_va(NULL, 0, fmt, va);
        va_end(va);
        return res;
    }

    inline isize omf_logger(u8 verb_flag, const char *fmt, ...) {
        if (!omf_logger_callbacks) return 0;

        isize res;
        va_list va;
        va_start(va, fmt);
        res = omf__logger_va(NULL, verb_flag, fmt, va);
        va_end(va);
        return res;
    }

    inline isize omf_logger_ext(const char *details, u8 verb_flag, const char *fmt, ...) {
        if (!omf_logger_callbacks) return 0;

        isize res;
        va_list va;
        va_start(va, fmt);
        res = omf__logger_va(details, verb_flag, fmt, va);
        va_end(va);
        return res;
    }

#ifdef __cplusplus
}
#endif

#endif // OMF_LOGGER_IMPLEMENTATION
#endif // OMF_LOGGER_H
