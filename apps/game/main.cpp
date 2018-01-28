#include <zpl.h>
#include <logger.h>

/**
 * example file output (imaginary)
 */
void file_callback(const char *det, u8 verb, const char *str, usize size) {
    if (verb < OMF_LOGGER_INFO) {
        zpl_printf("got message sizeof [%zu]: %s", size, str);
    }
}

/**
 * example console output
 */
void console_callback(const char *det, u8 verb, const char *str, usize size) {
    if (verb <= OMF_LOGGER_WARN) {
        char buffer[26];
        time_t timer; time(&timer);
        struct tm* tm_info = localtime(&timer);
        strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", tm_info);
        zpl_printf("[%s] got message sizeof from(%s) [%zu]: %s", buffer, det, size, str);
    }
}

// example usage in some file
#define log_warn(fmt, ...) omf_logger_ext(__FILE__, OMF_LOGGER_WARN, fmt, ##__VA_ARGS__)

int main( int argc, char** argv ) {
    omf_logger_add(file_callback);
    omf_logger_add(console_callback);

    omf_logger_raw("hello world: %d, %d", 14, 24);

    omf_logger(OMF_LOGGER_WARN, "hello world: %d, %d", 14, 24);
    omf_logger(OMF_LOGGER_WARN, "hello world: %d, %d", 14, 24);
    omf_logger(OMF_LOGGER_WARN, "hello world: %d, %d", 14, 24);

    log_warn("hello world: %d, %d", 14, 24);
    log_warn("hello world: %s, %d", "hwllo world", __LINE__);

    return 0;
}
