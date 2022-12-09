#ifndef __TRACE_H__
#define __TRACE_H__

#include "sdkconfig.h"
#include "esp_log.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define ENABLE_TRACE 1

    void __dump(const char *file_name, uint32_t line, char *buffer_name, void *_buff, uint32_t ofs, uint32_t cnt);

#ifndef __FILENAME__
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif

#define trace(X, reg...)                                         \
    {                                                            \
        printf("%s (%d): " X "\r\n", __FILE__, __LINE__, ##reg); \
    }
#define trace_dbg(X, ...) ESP_LOGD(__FILE__, "(%d): " X, __LINE__, ##__VA_ARGS__)
#define trace_imp(X, ...) ESP_LOGI(__FILE__, "(%d): " X, __LINE__, ##__VA_ARGS__)
#define trace_err(X, ...) ESP_LOGE(__FILE__, "(%d): " X, __LINE__, ##__VA_ARGS__)
#define trace_war(X, ...) ESP_LOGW(__FILE__, "(%d): " X, __LINE__, ##__VA_ARGS__)
#define trace_info(X, ...)                              \
    {                                                   \
        printf("\x1B[34m %s[%d]:", __FILE__, __LINE__); \
        printf(X, ##__VA_ARGS__);                       \
        printf("\x1B[0m\r\n");                          \
    }

#if (1 == ENABLE_TRACE)

#define TRACE_E trace_err
#define TRACE_W trace_war
#define TRACE_I trace_imp
#define TRACE_D trace
#define TRACE_B trace_info
#define dump(buffer_name, buffer, offset, count) __dump(__FILE__, __LINE__, buffer_name, buffer, offset, count) // ESP_LOG_BUFFER_HEX_LEVEL(__FILENAME__, (char *)((uint32_t)X + Y), Z, ESP_LOG_WARN)
    // #define dump(a, b, c, d)

#else

#define TRACE_E(X, ...)
#define TRACE_W(X, ...)
#define TRACE_I(X, ...)
#define TRACE_D(X, ...)
#define TRACE_B(x, ...)
#define dump(X, Y, Z)

#endif

#if 0
class trace
{
private:
    int trace_level = 0;
    void trace();

public:
    void set_trace(int level);

    void error();
    void warning();
    void info();
    void trace();

    void red();
    void green();
    void blue();
    void orange();
    void white();
};
#endif

#ifdef __cplusplus
}
#endif

#endif // __TRACE_H__