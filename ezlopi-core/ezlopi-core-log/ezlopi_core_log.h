
#ifndef _EZLOPI_CORE_LOG_H_
#define _EZLOPI_CORE_LOG_H_

#include "../../build/config/sdkconfig.h"

#ifdef CONFIG_EZPI_UTIL_TRACE_EN
#include "ezlopi_util_trace.h"
#include "ezlopi_core_errors.h"

#include <stdbool.h>

void ezlopi_core_read_set_log_severities();
void ezlopi_core_read_set_log_severities_internal(e_trace_severity_t severity);
ezlopi_error_t ezlopi_core_cloud_log_severity_process_str(bool severity_enable, const char *severity_str);
ezlopi_error_t ezlopi_core_cloud_log_severity_process_id(const e_trace_severity_t severity_level_id);
const char **ezlopi_core_cloud_log_get_severity_enums();

const char *ezlopi_core_cloud_log_get_current_severity_enum_str();
e_trace_severity_t ezlopi_core_cloud_log_get_current_severity_enum_val();
ezlopi_error_t ezlopi_core_send_cloud_log(int severity, const char *log_str);
void ezlopi_core_set_log_upcalls();

ezlopi_error_t ezlopi_core_serial_log_severity_process_str(const char *severity_str);
ezlopi_error_t ezlopi_core_serial_log_severity_process_id(const e_trace_severity_t severity_level_id);
const char *ezlopi_core_serial_log_get_current_severity_enum_str();
e_trace_severity_t ezlopi_core_serial_log_get_current_severity_enum_val();

#endif // CONFIG_EZPI_UTIL_TRACE_EN

#endif // _EZLOPI_CORE_LOG_H_
