#ifndef _EZLOPI_UTIL_H_
#define _EZLOPI_UTIL_H_

#include "../../build/config/sdkconfig.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "EZLOPI_USER_CONFIG.h"

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef CONFIG_EZPI_UTIL_TRACE_EN
#define ENABLE_TRACE 1
#else
#define ENABLE_TRACE 0
#endif // CONFIG_EZPI_UTIL_TRACE_EN

    // void trace_color_print(const char* txt_color, uint8_t severity, const char* format, ...);
    void trace_color_print(const char* txt_color, uint8_t severity, const char* file, int line, const char* format, ...);
    void __dump(const char* file_name, uint32_t line, char* buffer_name, void* _buff, uint32_t ofs, uint32_t cnt);

#ifndef __FILENAME__
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif

#if (1 == ENABLE_TRACE)

#define COLOR_FONT_BLACK "30"
#define COLOR_FONT_RED "31"
#define COLOR_FONT_GREEN "32"
#define COLOR_FONT_YELLOW "33"
#define COLOR_FONT_BLUE "34"
#define COLOR_FONT_MAGENTA "35"
#define COLOR_FONT_CYAN "36"
#define COLOR_FONT_WHITE "37"

#define COLOR_BG_BLACK "40"
#define COLOR_BG_RED "41"
#define COLOR_BG_GREEN "42"
#define COLOR_BG_YELLOW "43"
#define COLOR_BG_BLUE "44"
#define COLOR_BG_MAGENTA "45"
#define COLOR_BG_CYAN "46"
#define COLOR_BG_WHITE "47"

    typedef int (*f_ezlopi_log_upcall_t)(int severity_level, const char* log_str);

#define trace_color(txt_color, X, reg...)                                                 \
    {                                                                                     \
        printf("\x1B[%sm %s[%d]:" X "\x1B[0m\r\n", txt_color, __FILE__, __LINE__, ##reg); \
    }

#if 0
#define trace_color_print(txt_color, severity, X, reg...)                                                                               \
    {                                                                                                                                   \
        f_ezlopi_log_upcall_t log_upcall_func = ezlopi_util_get_cloud_log_upcall();                                                     \
        if (NULL != log_upcall_func)                                                                                                    \
        {                                                                                                                               \
            log_upcall_func(severity, "%s[%d]:" X "", __FILE__, __LINE__, ##reg);                                                       \
        }                                                                                                                               \
        log_upcall_func = ezlopi_util_get_serial_log_upcall();                                                                          \
        if (NULL != log_upcall_func)                                                                                                    \
        {                                                                                                                               \
            log_upcall_func(severity, "\x1B[%sm %s[%d]:" X "\x1B[0m\r\n", txt_color, __FILE__, __LINE__, ##reg);                        \
        }                                                                                                                               \
    }
#endif 
#define trace(X, reg...)                                       \
    {                                                          \
        printf("%s[%d]:" X "\r\n", __FILE__, __LINE__, ##reg); \
    }

#define trace_log_sev_error(X, reg...)  trace_color_print(COLOR_FONT_RED, 0, __FILE__, __LINE__, X, ##reg)
#define trace_log_sev_warning(X, reg...) trace_color_print(COLOR_FONT_YELLOW, 1, __FILE__, __LINE__, X, ##reg)
#define trace_log_sev_info(X, reg...) trace_color_print(COLOR_FONT_BLUE, 2, __FILE__, __LINE__, X, ##reg)
#define trace_log_sev_debug(X, reg...) trace_color_print(COLOR_FONT_WHITE, 3, __FILE__, __LINE__, X, ##reg)
#define trace_log_sev_trace(X, reg...) trace_color_print(COLOR_FONT_GREEN, 4, __FILE__, __LINE__, X, ##reg)

#define trace_debug(X, reg...) trace_color(COLOR_FONT_WHITE, X, ##reg)
#define trace_information(X, reg...) trace_color(COLOR_FONT_BLUE, X, ##reg)
#define trace_success(X, reg...) trace_color(COLOR_FONT_GREEN, X, ##reg)
#define trace_warning(X, reg...) trace_color(COLOR_FONT_YELLOW, X, ##reg)
#define trace_error(X, reg...) trace_color(COLOR_FONT_RED, X, ##reg)


    // #define trace_debug(X, ...) ESP_LOGD(__FILE__, "[%d]: " X, __LINE__, ##__VA_ARGS__)
    // #define trace_imp(X, ...) ESP_LOGI(__FILE__, "[%d]: " X, __LINE__, ##__VA_ARGS__)
    // #define trace_error(X, ...) ESP_LOGE(__FILE__, "[%d]: " X, __LINE__, ##__VA_ARGS__)
    // #define trace_warning(X, ...) ESP_LOGW(__FILE__, "[%d]: " X, __LINE__, ##__VA_ARGS__)
    // #define trace_information(X, ...)
    //     {
    //         printf("\x1B[34m %s[%d]:" X "\x1B[0m\r\n", __FILE__, __LINE__, ##__VA_ARGS__);
    //     }

#define TRACE_Dg(bg, txt, X, reg...)                                                       \
    {                                                                                      \
        printf("\x1B[%s;%sm %s[%d]:" X "\x1B[0m\r\n", txt, bg, __FILE__, __LINE__, ##reg); \
    }

#define TRACE_Dw(X, reg...) TRACE_Dg(COLOR_BG_BLACK, COLOR_FONT_WHITE, X, ##reg)
#define trace_yw(X, reg...) TRACE_Dg(COLOR_BG_YELLOW, COLOR_FONT_WHITE, X, ##reg)
#define trace_wb(X, reg...) TRACE_Dg(COLOR_BG_WHITE, COLOR_FONT_BLACK, X, ##reg)

#define F(X) (flash_attr) X

// #define TRACE_D trace_debug
// #define TRACE_I trace_information
// #define TRACE_S trace_success
// #define TRACE_W trace_warning
// #define TRACE_E trace_error

#define TRACE_E trace_log_sev_error
#define TRACE_W trace_log_sev_warning
#define TRACE_I trace_log_sev_info
#define TRACE_D trace_log_sev_debug
#define TRACE_S trace_log_sev_trace


#define dump(buffer_name, buffer, offset, count) __dump(__FILE__, __LINE__, buffer_name, buffer, offset, count)

    void ezlopi_util_set_log_upcalls(f_ezlopi_log_upcall_t cloud_log_upcall, f_ezlopi_log_upcall_t serial_log_upcall);
    f_ezlopi_log_upcall_t ezlopi_util_get_cloud_log_upcall();
    f_ezlopi_log_upcall_t ezlopi_util_get_serial_log_upcall();

#else // (1 == ENABLE_TRACE)

#define TRACE_E(X, ...)
#define TRACE_W(X, ...)
#define TRACE_S(X, ...)
#define TRACE_D(X, ...)
#define TRACE_I(x, ...)
#define dump(name, X, Y, Z)


#define TRACE_Dw(X, reg...)
#define trace_yw(X, reg...)
#define trace_wb(X, reg...)

#endif // (1 == ENABLE_TRACE)

#ifdef __cplusplus
}
#endif

#endif // _EZLOPI_UTIL_H_