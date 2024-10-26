
#ifndef _EZLOPI_CORE_LOG_H_
#define _EZLOPI_CORE_LOG_H_

#include "../../build/config/sdkconfig.h"

#ifdef CONFIG_EZPI_UTIL_TRACE_EN
#include "ezlopi_util_trace.h"
#include "ezlopi_core_errors.h"
#include <stdbool.h>

void ezlopi_core_read_set_log_severities(void);
void ezlopi_core_read_set_log_severities_internal(e_ezpi_trace_severity_t severity);
ezlopi_error_t ezlopi_core_cloud_log_severity_process_str(bool severity_enable, const char *severity_str);
ezlopi_error_t ezlopi_core_cloud_log_severity_process_id(const e_ezpi_trace_severity_t severity_level_id);

const char *ezlopi_core_log_severity_to_str(e_ezpi_trace_severity_t severity);

const char *ezlopi_core_cloud_log_get_current_severity_enum_str(void);
e_ezpi_trace_severity_t ezlopi_core_cloud_log_get_current_severity_enum_val(void);
void ezlopi_core_set_log_upcalls(void);

ezlopi_error_t ezlopi_core_serial_log_severity_process_str(const char *severity_str);
ezlopi_error_t ezlopi_core_serial_log_severity_process_id(const e_ezpi_trace_severity_t severity_level_id);
const char *ezlopi_core_serial_log_get_current_severity_enum_str(void);
e_ezpi_trace_severity_t ezlopi_core_serial_log_get_current_severity_enum_val(void);

#endif // CONFIG_EZPI_UTIL_TRACE_EN
#endif // _EZLOPI_CORE_LOG_H_
