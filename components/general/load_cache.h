#ifndef OMF_LOAD_CACHE_H
#define OMF_LOAD_CACHE_H

#ifdef __cplusplus
extern "C" {
#endif

////////////////////////////////////////////////////////////////
//
// API
//
//

    ZPL_DEF void omf_cache_init (void);
    ZPL_DEF void omf_cache_store(char *name, void *ptr);
    ZPL_DEF b32  omf_cache_load (char *name, void **ptr);

#ifdef __cplusplus
}
#endif


#if defined(OMF_LOAD_CACHE_IMPLEMENTATION) && !defined(OMF_LOAD_CACHE_IMPLEMENTATION_DONE)
#define OMF_LOAD_CACHE_IMPLEMENTATION_DONE


////////////////////////////////////////////////////////////////
//
// Implementation
//
//

#ifdef __cplusplus
extern "C" {
#endif

    ZPL_TABLE(static, omf_cache__table_pool, omf_cache__table_, void *)
    zpl_global omf_cache__table_pool omf_cache__entries;

    zpl_inline void *omf_cache__find_entry(u64 key) {
        void **entry = omf_cache__table_get(&omf_cache__entries, key);
        if (entry) {
            return *entry;
        }
        return NULL;
    }

    zpl_inline void omf_cache_init(void) {
        omf_cache__table_init(&omf_cache__entries, zpl_heap_allocator());
    }

    zpl_inline u64 omf_cache__calc_hash(char *name) {
        return zpl_murmur64((void const *)name, (isize)zpl_strlen(name));
    }

    zpl_inline void omf_cache_store(char *name, void *ptr) {
        u64 key = omf_cache__calc_hash(name);
        ZPL_ASSERT(!omf_cache__find_entry(key));

        omf_cache__table_set(&omf_cache__entries, key, ptr);
    }

    zpl_inline b32 omf_cache_load(char *name, void **ptr) {
        u64 key = omf_cache__calc_hash(name);
        void *entry = omf_cache__find_entry(key);

        if (entry) {
            *ptr = entry;
            return true;
        }

        return false;
    }

#ifdef __cplusplus
}
#endif

///////////////////////////////////////////
//
// Example
//
//

// int main(void) {
//     omf_cache_init();

//     omf_cache_store("foo", (void *)42);
//     omf_cache_store("bar", (void *)63);

//     void *data = NULL;

//     ZPL_ASSERT(!omf_cache_load("baz_notexists", &data));

//     if (omf_cache_load("foo", &data)) {
//         zpl_printf("%d\n", *(int*)&data);
//     }
//     return 0;
}

#endif // OMF_LOAD_CACHE_IMPLEMENTATION
#endif // OMF_LOAD_CACHE_H
