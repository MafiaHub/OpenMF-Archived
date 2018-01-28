#define OMF_DEF ZPL_DEF
#define omf_inline zpl_inline
#define omf_assert ZPL_ASSERT
#define omf_assert_msg ZPL_ASSERT_MSG

#if defined(ZPL_MATH_INCLUDE_ZPL_MATH_H)
#   define vec3 zplm_vec3_t
#endif

#define omf_logger_warn(fmt, ...) \
        omf_logger_ext(NULL, OMF_LOGGER_WARN, fmt, #__VA_ARGS__)
