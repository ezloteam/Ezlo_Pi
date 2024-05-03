
#ifndef _EZLOPI_CORE_LOG_H_
#define _EZLOPI_CORE_LOG_H_

#include <stdbool.h>

typedef enum {
    ENUM_EZLOPI_LOG_SEVERITY_ERROR = 0,
    ENUM_EZLOPI_LOG_SEVERITY_WARNING,
    ENUM_EZLOPI_LOG_SEVERITY_INFO,
    ENUM_EZLOPI_LOG_SEVERITY_DEBUG,
    ENUM_EZLOPI_LOG_SEVERITY_TRACE,
    ENUM_EZLOPI_LOG_SEVERITY_MAX,
}e_ezlopi_log_severity_t;

void ezlopi_core_read_set_log_severity();
int ezlopi_core_log_severity_process(bool severity_enable, const char* severity_str);
const char* ezlopi_core_log_get_current_severity_enum_str();
e_ezlopi_log_severity_t ezlopi_core_log_get_current_severity_enum_val();
int ezlopi_core_send_log(int severity, const char* format, ...);
void ezlopi_core_log_set_broadcaster();


#endif // _EZLOPI_CORE_LOG_H_

