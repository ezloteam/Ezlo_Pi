#ifndef _EZLOPI_UTIL_H_
#define _EZLOPI_UTIL_H_

#include "sdkconfig.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define ENABLE_TRACE 1

    void __dump(const char *file_name, uint32_t line, char *buffer_name, void *_buff, uint32_t ofs, uint32_t cnt);

#ifndef __FILENAME__
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif

#if (1 == ENABLE_TRACE)

#define DBG_FONT_COLOR_BLACK "30"
#define DBG_FONT_COLOR_RED "31"
#define DBG_FONT_COLOR_GREEN "32"
#define DBG_FONT_COLOR_YELLOW "33"
#define DBG_FONT_COLOR_BLUE "34"
#define DBG_FONT_COLOR_MAGENTA "35"
#define DBG_FONT_COLOR_CYAN "36"
#define DBG_FONT_COLOR_WHITE "37"

#define DBG_BG_COLOR_BLACK "40"
#define DBG_BG_COLOR_RED "41"
#define DBG_BG_COLOR_GREEN "42"
#define DBG_BG_COLOR_YELLOW "43"
#define DBG_BG_COLOR_BLUE "44"
#define DBG_BG_COLOR_MAGENTA "45"
#define DBG_BG_COLOR_CYAN "46"
#define DBG_BG_COLOR_WHITE "47"

#define trace(X, reg...)                                         \
    {                                                            \
        printf("%s (%d): " X "\r\n", __FILE__, __LINE__, ##reg); \
    }
#define trace_dbg(X, ...) ESP_LOGD(__FILE__, "[%d]: " X, __LINE__, ##__VA_ARGS__)
#define trace_imp(X, ...) ESP_LOGI(__FILE__, "[%d]: " X, __LINE__, ##__VA_ARGS__)
#define trace_err(X, ...) ESP_LOGE(__FILE__, "[%d]: " X, __LINE__, ##__VA_ARGS__)
#define trace_war(X, ...) ESP_LOGW(__FILE__, "[%d]: " X, __LINE__, ##__VA_ARGS__)
#define trace_info(X, ...)                              \
    {                                                   \
        printf("\x1B[34m %s[%d]:", __FILE__, __LINE__); \
        printf(X, ##__VA_ARGS__);                       \
        printf("\x1B[0m\r\n");                          \
    }

#define trace_bg(bg, txt, X, ...)

#define F(X) (flash_attr) X

#define TRACE_E trace_err
#define TRACE_W trace_war
#define TRACE_I trace_imp
#define TRACE_D trace
#define TRACE_B trace_info
#define dump(buffer_name, buffer, offset, count) __dump(__FILE__, __LINE__, buffer_name, buffer, offset, count)

#else

#define TRACE_E(X, ...)
#define TRACE_W(X, ...)
#define TRACE_I(X, ...)
#define TRACE_D(X, ...)
#define TRACE_B(x, ...)
#define dump(name, X, Y, Z)

#endif

#ifdef __cplusplus
}
#endif

#endif // _EZLOPI_UTIL_H_