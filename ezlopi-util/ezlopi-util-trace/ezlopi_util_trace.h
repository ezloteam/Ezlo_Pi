/* ===========================================================================
** Copyright (C) 2025 Ezlo Innovation Inc
**
** Under EZLO AVAILABLE SOURCE LICENSE (EASL) AGREEMENT
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met:
**
** 1. Redistributions of source code must retain the above copyright notice,
**    this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. Neither the name of the copyright holder nor the names of its
**    contributors may be used to endorse or promote products derived from
**    this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
** AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
** ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
** LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
** CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
** SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
** INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
** CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
** ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
** POSSIBILITY OF SUCH DAMAGE.
** ===========================================================================
*/

/**
 * @file    main.c
 * @brief   perform some function on data
 * @author
 * @version 0.1
 * @date    1st January 2024
 */

#ifndef _EZLOPI_UTIL_H_
#define _EZLOPI_UTIL_H_

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include "../../build/config/sdkconfig.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "EZLOPI_USER_CONFIG.h"
#include "ezlopi_core_errors.h"

/*******************************************************************************
 *                          C++ Declaration Wrapper
 *******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

    typedef enum
    {
        ENUM_EZLOPI_TRACE_SEVERITY_NONE = 0,
        ENUM_EZLOPI_TRACE_SEVERITY_ERROR,
        ENUM_EZLOPI_TRACE_SEVERITY_WARNING,
        ENUM_EZLOPI_TRACE_SEVERITY_INFO,
        ENUM_EZLOPI_TRACE_SEVERITY_DEBUG,
        ENUM_EZLOPI_TRACE_SEVERITY_TRACE,
        ENUM_EZLOPI_TRACE_SEVERITY_MAX,
    } e_trace_severity_t;

/*******************************************************************************
 *                          Type & Macro Declarations
 *******************************************************************************/
#if defined(CONFIG_EZPI_UTIL_TRACE_EN) || defined(CONFIG_EZPI_LOG_CLOUD_EN)
#define ENABLE_TRACE 1
#else
#define ENABLE_TRACE 0
#endif // CONFIG_EZPI_UTIL_TRACE_EN

    // void trace_color_print(const char* txt_color, uint8_t severity, const char* format, ...);

#ifndef __FILENAME__
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif

#ifdef CONFIG_EZPI_OPENTELEMETRY_ENABLE_LOGS
    typedef ezlopi_error_t (*f_ezlopi_log_upcall_t)(int severity_level, const char *log_str);
    typedef int (*f_otel_log_upcall_t)(uint8_t severity, const char *file, uint32_t line, char *log);

    void ezlopi_util_set_otel_log_upcall(f_otel_log_upcall_t __log_upcall, uint32_t max_log_len);
    void ezlopi_util_log_otel(uint8_t severity, const char *file, int line, const char *format, ...);

#define TRACE_OTEL(severity, format, reg...) ezlopi_util_log_otel(severity, __FILENAME__, __LINE__, format, ##reg)
#else // CONFIG_EZPI_OPENTELEMETRY_ENABLE_LOGS
#define TRACE_OTEL(severity, format, reg...)
#endif

#if (1 == ENABLE_TRACE)

    void trace_otel(uint8_t severity, const char *file, int line, const char *format, ...);
    void trace_color_print(const char *txt_color, uint8_t severity, const char *file, int line, const char *format, ...);
    void __dump(const char *file_name, uint32_t line, char *buffer_name, void *_buff, uint32_t ofs, uint32_t cnt);

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

#if 0

#define trace_color_print(txt_color, severity, X, reg...)                                                        \
    {                                                                                                            \
        f_ezlopi_log_upcall_t log_upcall_func = ezlopi_util_get_cloud_log_upcall();                              \
        if (NULL != log_upcall_func)                                                                             \
        {                                                                                                        \
            log_upcall_func(severity, "%s[%d]:" X "", __FILE__, __LINE__, ##reg);                                \
        }                                                                                                        \
        log_upcall_func = ezlopi_util_get_serial_log_upcall();                                                   \
        if (NULL != log_upcall_func)                                                                             \
        {                                                                                                        \
            log_upcall_func(severity, "\x1B[%sm %s[%d]:" X "\x1B[0m\r\n", txt_color, __FILE__, __LINE__, ##reg); \
        }                                                                                                        \
    }
#endif

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

    // #define trace_debug(X, ...) ESP_LOGD(__FILE__, "[%d]: " X, __LINE__, ##__VA_ARGS__)
    // #define trace_imp(X, ...) ESP_LOGI(__FILE__, "[%d]: " X, __LINE__, ##__VA_ARGS__)
    // #define trace_error(X, ...) ESP_LOGE(__FILE__, "[%d]: " X, __LINE__, ##__VA_ARGS__)
    // #define trace_warning(X, ...) ESP_LOGW(__FILE__, "[%d]: " X, __LINE__, ##__VA_ARGS__)
    // #define trace_information(X, ...)
    //     {
    //         printf("\x1B[34m %s[%d]:" X "\x1B[0m\r\n", __FILE__, __LINE__, ##__VA_ARGS__);
    //     }

#define TRACE_Dg(bg, txt, X, reg...)                                                         \
    {                                                                                        \
        ets_printf("\x1B[%s;%sm %s[%d]:" X "\x1B[0m\n", txt, bg, __FILE__, __LINE__, ##reg); \
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

    /*******************************************************************************
     *                          Extern Data Declarations
     *******************************************************************************/

    /*******************************************************************************
     *                          Extern Function Prototypes
     *******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif // _EZLOPI_UTIL_H_

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
