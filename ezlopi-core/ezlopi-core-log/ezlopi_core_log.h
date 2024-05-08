
#ifndef _EZLOPI_CORE_LOG_H_
#define _EZLOPI_CORE_LOG_H_

#include "../../build/config/sdkconfig.h"

#ifdef CONFIG_EZPI_UTIL_TRACE_EN


#include <stdbool.h>

typedef enum {
    ENUM_EZLOPI_CLOUD_LOG_SEVERITY_ERROR = 0,
    ENUM_EZLOPI_CLOUD_LOG_SEVERITY_WARNING,
    ENUM_EZLOPI_CLOUD_LOG_SEVERITY_INFO,
    ENUM_EZLOPI_CLOUD_LOG_SEVERITY_DEBUG,
    ENUM_EZLOPI_CLOUD_LOG_SEVERITY_TRACE,
    ENUM_EZLOPI_CLOUD_LOG_SEVERITY_MAX,
}e_ezlopi_cloud_log_severity_t;

typedef enum {
    ENUM_EZLOPI_SERIAL_LOG_SEVERITY_NONE = 0,
    ENUM_EZLOPI_SERIAL_LOG_SEVERITY_ERROR,
    ENUM_EZLOPI_SERIAL_LOG_SEVERITY_WARNING,
    ENUM_EZLOPI_SERIAL_LOG_SEVERITY_INFO,
    ENUM_EZLOPI_SERIAL_LOG_SEVERITY_DEBUG,
    ENUM_EZLOPI_SERIAL_LOG_SEVERITY_TRACE,
    ENUM_EZLOPI_SERIAL_LOG_SEVERITY_MAX,
}e_ezlopi_serial_log_severity_t;

void ezlopi_core_read_set_log_severities();
int ezlopi_core_cloud_log_severity_process(bool severity_enable, const char* severity_str);
const char* ezlopi_core_cloud_log_get_current_severity_enum_str();
e_ezlopi_cloud_log_severity_t ezlopi_core_cloud_log_get_current_severity_enum_val();
int ezlopi_core_send_cloud_log(int severity, const char* format, ...);
void ezlopi_core_set_log_upcalls();

int ezlopi_core_serial_log_severity_process(const char* severity_str);
const char* ezlopi_core_serial_log_get_current_severity_enum_str();
e_ezlopi_serial_log_severity_t ezlopi_core_serial_log_get_current_severity_enum_val();

#endif // CONFIG_EZPI_UTIL_TRACE_EN

#endif // _EZLOPI_CORE_LOG_H_

