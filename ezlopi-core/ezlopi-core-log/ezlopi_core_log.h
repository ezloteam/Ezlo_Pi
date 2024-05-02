
#ifndef _EZLOPI_CORE_LOG_H_
#define _EZLOPI_CORE_LOG_H_


typedef enum {
    ENUM_EZLOPI_LOG_SEVERITY_ERROR = 0,
    ENUM_EZLOPI_LOG_SEVERITY_WARNING,
    ENUM_EZLOPI_LOG_SEVERITY_INFO,
    ENUM_EZLOPI_LOG_SEVERITY_DEBUG,
    ENUM_EZLOPI_LOG_SEVERITY_TRACE,
    ENUM_EZLOPI_LOG_SEVERITY_MAX,
}e_ezlopi_log_severity_t;

int ezlopi_core_log_severity_process(bool severity_enable, const char* severity_str);
const char* ezlopi_core_log_get_current_severity();


#endif // _EZLOPI_CORE_LOG_H_

