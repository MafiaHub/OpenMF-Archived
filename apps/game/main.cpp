#define ZPL_IMPLEMENTATION
#define OMF_LOGGER_IMPLEMENTATION

#include <zpl.h>
#include <logger.h>

/**
 * example file output (imaginary)
 */
void file_callback(u8 verbosity_flag, const char *str, usize size) {
    if (verbosity_flag > OMF_LOGGER_INFO)
        zpl_printf("got message sizeof [%u]: %s", size, str);
}

/**
 * example console output
 */
void console_callback(u8 verbosity_flag, const char *str, usize size) {
    if (verbosity_flag >= OMF_LOGGER_WARN)
       zpl_printf("got message sizeof [%u]: %s", size, str);
}

// example usage in some file
#define log_warn(fmt, ...) omf_logger(OMF_LOGGER_WARN, fmt, ##__VA_ARGS__)


int main( int argc, char** argv ) {
    omf_logger_add(file_callback);
    omf_logger_add(console_callback);

    omf_logger_raw("hello world: %d, %d", 14, 24);

    omf_logger(OMF_LOGGER_WARN, "hello world: %d, %d", 14, 24);
    omf_logger(OMF_LOGGER_WARN, "hello world: %d, %d", 14, 24);
    omf_logger(OMF_LOGGER_WARN, "hello world: %d, %d", 14, 24);

    log_warn("hello world: %d, %d", 14, 24);
    log_warn("hello world: %d, %d", 14, 24);

    return 0;
}
