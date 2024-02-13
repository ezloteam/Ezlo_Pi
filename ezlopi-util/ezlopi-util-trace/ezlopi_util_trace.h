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

#define trace(X, reg...)                                         \
    {                                                            \
        printf("%s (%d): " X "\r\n", __FILE__, __LINE__, ##reg); \
    }

#define trace_dbg(X, ...)                               \
    {                                                   \
        printf("\x1B[37m %s[%d]:", __FILE__, __LINE__); \
        printf(X, ##__VA_ARGS__);                       \
        printf("\x1B[0m\r\n");                          \
    }

#define trace_imp(X, ...)                               \
    {                                                   \
        printf("\x1B[32m %s[%d]:", __FILE__, __LINE__); \
        printf(X, ##__VA_ARGS__);                       \
        printf("\x1B[0m\r\n");                          \
    }
#define trace_err(X, ...)                               \
    {                                                   \
        printf("\x1B[31m %s[%d]:", __FILE__, __LINE__); \
        printf(X, ##__VA_ARGS__);                       \
        printf("\x1B[0m\r\n");                          \
    }
#define trace_war(X, ...)                               \
    {                                                   \
        printf("\x1B[33m %s[%d]:", __FILE__, __LINE__); \
        printf(X, ##__VA_ARGS__);                       \
        printf("\x1B[0m\r\n");                          \
    }
#define trace_info(X, ...)                              \
    {                                                   \
        printf("\x1B[34m %s[%d]:", __FILE__, __LINE__); \
        printf(X, ##__VA_ARGS__);                       \
        printf("\x1B[0m\r\n");                          \
    }

#if (1 == ENABLE_TRACE)

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