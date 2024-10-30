#ifndef _EZLOPI_UTIL_H_
#define _EZLOPI_UTIL_H_

#include "../../build/config/sdkconfig.h"

#include <time.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "EZLOPI_USER_CONFIG.h"
#include "ezlopi_core_errors.h"

#ifdef __cplusplus
extern "C"
{
#endif

#if defined(CONFIG_EZPI_UTIL_TRACE_EN) || defined(CONFIG_EZPI_LOG_CLOUD_EN)
#define ENABLE_TRACE 1
#else
#define ENABLE_TRACE 0
#endif // CONFIG_EZPI_UTIL_TRACE_EN

    typedef enum
    {
        E_TRACE_SEVERITY_NONE = 0,
        E_TRACE_SEVERITY_ERROR,
        E_TRACE_SEVERITY_WARNING,
        E_TRACE_SEVERITY_INFO,
        E_TRACE_SEVERITY_DEBUG,
        E_TRACE_SEVERITY_TRACE,
        E_TRACE_SEVERITY_MAX
    } e_ezpi_trace_severity_t;

    typedef void (*f_trace_upcall_t)(e_ezpi_trace_severity_t severity, const char *file, uint32_t line, uint64_t time, char *msg);

    void ezlopi_util_trace_init(f_trace_upcall_t upcall);
    const char *ezlopi_util_trace_get_severity_name_str(e_ezpi_trace_severity_t severity);

    void trace_color_print(const char *txt_color, uint8_t severity, const char *file, int line, const char *format, ...);
    void __dump(const char *file_name, uint32_t line, char *buffer_name, void *_buff, uint32_t ofs, uint32_t cnt);

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

    typedef ezlopi_error_t (*f_ezlopi_log_upcall_t)(int severity_level, const char *log_str);

#define trace_color(txt_color, X, reg...)                                                   \
    {                                                                                       \
        ets_printf("\x1B[%sm %s[%d]:" X "\x1B[0m\n", txt_color, __FILE__, __LINE__, ##reg); \
    }

#define trace(X, reg...)                                         \
    {                                                            \
        ets_printf("%s[%d]:" X "\n", __FILE__, __LINE__, ##reg); \
    }

#define trace_log_sev_error(X, reg...) trace_color_print(COLOR_FONT_RED, 1, __FILE__, __LINE__, X, ##reg)
#define trace_log_sev_warning(X, reg...) trace_color_print(COLOR_FONT_YELLOW, 2, __FILE__, __LINE__, X, ##reg)
#define trace_log_sev_info(X, reg...) trace_color_print(COLOR_FONT_BLUE, 3, __FILE__, __LINE__, X, ##reg)
#define trace_log_sev_debug(X, reg...) trace_color_print(COLOR_FONT_WHITE, 4, __FILE__, __LINE__, X, ##reg)
#define trace_log_sev_trace(X, reg...) trace_color_print(COLOR_FONT_GREEN, 5, __FILE__, __LINE__, X, ##reg)

#define trace_debug(X, reg...) trace_color(COLOR_FONT_WHITE, X, ##reg)
#define trace_information(X, reg...) trace_color(COLOR_FONT_BLUE, X, ##reg)
#define trace_success(X, reg...) trace_color(COLOR_FONT_GREEN, X, ##reg)
#define trace_warning(X, reg...) trace_color(COLOR_FONT_YELLOW, X, ##reg)
#define trace_error(X, reg...) trace_color(COLOR_FONT_RED, X, ##reg)

#define TRACE_Dg(bg, txt, X, reg...)                                                         \
    {                                                                                        \
        ets_printf("\x1B[%s;%sm %s[%d]:" X "\x1B[0m\n", txt, bg, __FILE__, __LINE__, ##reg); \
    }

#define TRACE_Dw(X, reg...) TRACE_Dg(COLOR_BG_BLACK, COLOR_FONT_WHITE, X, ##reg)
#define trace_yw(X, reg...) TRACE_Dg(COLOR_BG_YELLOW, COLOR_FONT_WHITE, X, ##reg)
#define trace_wb(X, reg...) TRACE_Dg(COLOR_BG_WHITE, COLOR_FONT_BLACK, X, ##reg)

#define F(X) (flash_attr) X

#define TRACE_E trace_log_sev_error
#define TRACE_W trace_log_sev_warning
#define TRACE_I trace_log_sev_info
#define TRACE_D trace_log_sev_debug
#define TRACE_S trace_log_sev_trace

#define dump(buffer_name, buffer, offset, count) __dump(__FILE__, __LINE__, buffer_name, buffer, offset, count)

    void ezlopi_util_set_log_upcalls(f_ezlopi_log_upcall_t cloud_log_upcall, f_ezlopi_log_upcall_t serial_log_upcall);

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