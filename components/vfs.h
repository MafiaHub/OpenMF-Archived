#ifndef OMF_VFS_H
#define OMF_VFS_H

#define MAFIA_INSTALL_DIR "MAFIA_INSTALL_DIR"

#if __cplusplus
extern "C" {
#endif

    zpl_global zpl_array_t(zpl_string_t) omf_vfs_paths;

    ZPL_DEF void omf_vfs_init        ();
    ZPL_DEF b32  omf_vfs_open        (zpl_file_t *file, char const *file_name, zpl_file_mode_t mode);
    ZPL_DEF void omf_vfs_add_path_at (char const *path, usize index);
    ZPL_DEF void omf_vfs_add_path    (char const *path);
    ZPL_DEF void omf_vfs_prepend_path(char const *path);

#if __cplusplus
}
#endif

#if defined(OMF_VFS_IMPLEMENTATION) && !defined(OMF_VFS_IMPLEMENTATION_DONE)
#define OMF_VFS_IMPLEMENTATION_DONE

///////////////////////
//
// Implementation part
//
//

#if __cplusplus
extern "C" {
#endif

    #define omf_add_path(x) zpl_array_append(omf_vfs_paths, zpl_string_make(zpl_heap_allocator(), x))

    zpl_inline void omf_vfs_init() {
        zpl_array_init(omf_vfs_paths, zpl_heap_allocator());

        omf_add_path("resources");
        omf_add_path("mafia");

        char const *env_mafia_dir = zpl_get_env(MAFIA_INSTALL_DIR);
        if (env_mafia_dir) {
            omf_add_path(env_mafia_dir);
        }

        // TODO(zaklaus): Add "~/.openmf/" path for Linux/macOS
    }

    #undef omf_add_path

    zpl_inline zpl_string_t omf_vfs__make_path(char const *path) {
        zpl_string_t str = zpl_string_make(zpl_heap_allocator(), path);
        zpl_str_to_lower(str);

        if (str[zpl_string_length(str)-1] == '/') {
            str[zpl_string_length(str)-1] = '\0';
        }

        return str;
    }

    zpl_inline void omf_vfs_prepend_path(char const *path) {
        zpl_array_append_at(omf_vfs_paths, omf_vfs__make_path(path), 0);
    }

    zpl_inline void omf_vfs_add_path_at(char const *path, usize index) {
        zpl_array_append_at(omf_vfs_paths, omf_vfs__make_path(path), index);
    }

    zpl_inline void omf_vfs_add_path(char const *path) {
        zpl_array_append(omf_vfs_paths, omf_vfs__make_path(path));
    }

    zpl_inline b32 omf_vfs_open(zpl_file_t *file, char const *file_name, zpl_file_mode_t mode) {
        for (usize i=0; i<zpl_array_count(omf_vfs_paths); i++) {
            zpl_string_t real_path = zpl_string_duplicate(zpl_heap_allocator(), omf_vfs_paths[i]);
            real_path = zpl_string_append_fmt(real_path, "%c%s", ZPL_PATH_SEPARATOR, file_name);
            zpl_str_to_lower(real_path);

            b32 ok = false;
            if (zpl_file_open_mode(file, mode, real_path) == ZPL_FILE_ERROR_NONE) {
                ok = true;
            }

            zpl_string_free(real_path);

            if (ok) {
                return ok;
            }
        }

        return false;
    }

#if __cplusplus
}
#endif

///////////////////////////////////////////
//
// Example usage
//
//

// int main(void) {
//     omf_vfs_init();
//     omf_vfs_prepend_path("testdir");

//     zpl_file_t f = {0};
//     if (omf_vfs_open(&f, "test.txt", ZPL_FILE_MODE_READ)) {
//        char buf[5] = {0};
//        zpl_file_read(&f, buf, 4);
//        zpl_printf("Output: %s\n", buf);
//     }
//     return 0;
// }

#endif // OMF_VFS_IMPLEMENTATION
#endif // OMF_VFS_H
