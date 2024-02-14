#ifndef _EZLOPI_UTIL_H_
#define _EZLOPI_UTIL_H_

#include "sdkconfig.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "EZLOPI_USER_CONFIG.h"

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef EZPI_UTIL_ENABLE_TRACE
#define ENABLE_TRACE 1
#endif // EZPI_UTIL_ENABLE_TRACE

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

#define trace(X, reg...)                                       \
    {                                                          \
        printf("%s[%d]:" X "\r\n", __FILE__, __LINE__, ##reg); \
    }
#define trace_color(txt_color, X, reg...)                                                 \
    {                                                                                     \
        printf("\x1B[%sm %s[%d]:" X "\x1B[0m\r\n", txt_color, __FILE__, __LINE__, ##reg); \
    }
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

#define trace_bg(bg, txt, X, reg...)                                                       \
    {                                                                                      \
        printf("\x1B[%s;%sm %s[%d]:" X "\x1B[0m\r\n", txt, bg, __FILE__, __LINE__, ##reg); \
    }

#define trace_bw(X, reg...) trace_bg(COLOR_BG_BLACK, COLOR_FONT_WHITE, X, ##reg)
#define trace_yw(X, reg...) trace_bg(COLOR_BG_YELLOW, COLOR_FONT_WHITE, X, ##reg)
#define trace_wb(X, reg...) trace_bg(COLOR_BG_WHITE, COLOR_FONT_BLACK, X, ##reg)

#define F(X) (flash_attr) X

#define TRACE_D trace_debug
#define TRACE_I trace_information
#define TRACE_S trace_success
#define TRACE_W trace_warning
#define TRACE_E trace_error
#define dump(buffer_name, buffer, offset, count) __dump(__FILE__, __LINE__, buffer_name, buffer, offset, count)

#else

#define TRACE_E(X, ...)
#define TRACE_W(X, ...)
#define TRACE_S(X, ...)
#define TRACE_D(X, ...)
#define TRACE_I(x, ...)
#define dump(name, X, Y, Z)

#endif

#ifdef __cplusplus
}
#endif

#endif // _EZLOPI_UTIL_H_