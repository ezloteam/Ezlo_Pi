#include "../../build/config/sdkconfig.h"

#ifdef CONFIG_EZPI_OTEL_EN

// #elif CONFIG_EZPI_UTIL_TRACE_EN

#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#include "cjext.h"

#include "ezlopi_core_nvs.h"
#include "ezlopi_core_heap.h"
#include "ezlopi_core_sntp.h"
#include "ezlopi_core_errors.h"
#include "ezlopi_core_broadcast.h"
#include "ezlopi_core_event_group.h"

#include "ezlopi_cloud_constants.h"

#include "ezlopi_core_log.h"

static e_ezpi_trace_severity_t cloud_log_severity = E_TRACE_SEVERITY_WARNING;
static e_ezpi_trace_severity_t serial_log_severity = E_TRACE_SEVERITY_MAX;

static ezlopi_error_t ezlopi_hub_cloud_log_set_severity(const char *severity_str);
static ezlopi_error_t ezlopi_hub_serial_log_set_severity(const char *severity_str);
static void __console_log(e_ezpi_trace_severity_t severity, const char *file, int line, uint32_t time, char *msg);
static void ezlopi_core_send_cloud_log(e_ezpi_trace_severity_t severity, const char *file, int line, uint32_t time, char *msg);

void ezlopi_core_set_log_upcalls(void)
{
    ezlopi_util_trace_add_upcall(__console_log);
    ezlopi_util_trace_add_upcall(ezlopi_core_send_cloud_log);
}

void ezlopi_core_read_set_log_severities_internal(e_ezpi_trace_severity_t severity)
{
    serial_log_severity = severity;
}

void ezlopi_core_read_set_log_severities(void)
{
    EZPI_CORE_nvs_read_cloud_log_severity(&cloud_log_severity);
    EZPI_CORE_nvs_read_serial_log_severity(&serial_log_severity);
}

const char *ezlopi_core_log_severity_to_str(e_ezpi_trace_severity_t severity)
{
    const char *ret = ezlopi_util_trace_get_severity_name_str(E_TRACE_SEVERITY_NONE);
    if (severity < E_TRACE_SEVERITY_MAX)
    {
        ret = ezlopi_util_trace_get_severity_name_str(severity);
    }

    return ret;
}

ezlopi_error_t ezlopi_core_cloud_log_severity_process_str(bool severity_enable, const char *severity_str)
{
    ezlopi_error_t ret = EZPI_FAILED;
    if (severity_enable)
    {
        ezlopi_hub_cloud_log_set_severity(severity_str);
        ret = EZPI_SUCCESS;
    }
    else
    {
        cloud_log_severity = E_TRACE_SEVERITY_NONE;
        ret = EZPI_SUCCESS;
    }
    EZPI_CORE_nvs_write_cloud_log_severity(cloud_log_severity);
    return ret;
}

ezlopi_error_t ezlopi_core_cloud_log_severity_process_id(const e_ezpi_trace_severity_t severity_level_id)
{
    ezlopi_error_t ret = EZPI_FAILED;

    if ((E_TRACE_SEVERITY_MAX > severity_level_id) && (E_TRACE_SEVERITY_NONE <= severity_level_id))
    {
        if (severity_level_id <= E_TRACE_SEVERITY_WARNING)
        {
            cloud_log_severity = severity_level_id;
        }
        else
        {
            cloud_log_severity = E_TRACE_SEVERITY_WARNING;
        }
        EZPI_CORE_nvs_write_cloud_log_severity(cloud_log_severity);
        ret = EZPI_SUCCESS;
    }

    return ret;
}

ezlopi_error_t ezlopi_core_serial_log_severity_process_str(const char *severity_str)
{
    ezlopi_error_t ret = EZPI_FAILED;
    if (severity_str)
    {
        ezlopi_hub_serial_log_set_severity(severity_str);
        ret = EZPI_SUCCESS;
    }
    else
    {
        serial_log_severity = E_TRACE_SEVERITY_NONE;
        ret = EZPI_SUCCESS;
    }
    EZPI_CORE_nvs_write_serial_log_severity(serial_log_severity);
    return ret;
}

ezlopi_error_t ezlopi_core_serial_log_severity_process_id(const e_ezpi_trace_severity_t severity_level_id)
{
    ezlopi_error_t ret = EZPI_FAILED;

    if ((E_TRACE_SEVERITY_MAX > severity_level_id) && (E_TRACE_SEVERITY_NONE <= severity_level_id))
    {
        serial_log_severity = severity_level_id;
        EZPI_CORE_nvs_write_serial_log_severity(serial_log_severity);
        ret = EZPI_SUCCESS;
    }

    return ret;
}

const char *ezlopi_core_cloud_log_get_current_severity_enum_str()
{
    return ezlopi_util_trace_get_severity_name_str(cloud_log_severity);
}

const char *ezlopi_core_serial_log_get_current_severity_enum_str()
{
    return ezlopi_util_trace_get_severity_name_str(serial_log_severity);
}

e_ezpi_trace_severity_t ezlopi_core_cloud_log_get_current_severity_enum_val()
{
    return cloud_log_severity;
}

e_ezpi_trace_severity_t ezlopi_core_serial_log_get_current_severity_enum_val()
{
    return serial_log_severity;
}

static void ezlopi_core_send_cloud_log(e_ezpi_trace_severity_t severity, const char *file, int line, uint32_t time, char *msg)
{
    if (severity <= ezlopi_core_cloud_log_get_current_severity_enum_val())
    {
        e_ezlopi_event_t event = ezlopi_get_event_bit_status();
        if ((event & EZLOPI_EVENT_NMA_REG) == EZLOPI_EVENT_NMA_REG)
        {
            cJSON *cj_log_broadcast = cJSON_CreateObject(__FUNCTION__);
            if (cj_log_broadcast)
            {
                cJSON_AddStringToObject(__FUNCTION__, cj_log_broadcast, ezlopi_id_str, ezlopi_ui_broadcast_str);
                cJSON_AddStringToObject(__FUNCTION__, cj_log_broadcast, ezlopi_msg_subclass_str, "hub.log");
                cJSON *cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_log_broadcast, ezlopi_result_str);
                if (cj_result)
                {
                    uint64_t timestamp = EZPI_CORE_sntp_get_current_time_sec();

                    char timestamp_str[64];

                    EZPI_CORE_sntp_epoch_to_iso8601(timestamp_str, sizeof(timestamp_str), (time_t)timestamp);

                    size_t total_len = sizeof(timestamp_str) + strlen(msg) + 2;
                    char message[total_len];
                    memset(message, 0, total_len);
                    snprintf(message, total_len, "%s: %s", timestamp_str, msg);

                    cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_message_str, message);

                    char severity_str[10];
                    memset(severity_str, 0, 10);
                    snprintf(severity_str, 10, "%s", ezlopi_util_trace_get_severity_name_str(severity));
                    cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_severity_str, severity_str);
                    if (EZPI_SUCCESS != ezlopi_core_broadcast_add_to_queue(cj_log_broadcast))
                    {
                        cJSON_Delete(__FUNCTION__, cj_log_broadcast);
                    }
                }
            }
        }
    }
}

/* Static Functions */
static void __console_log(e_ezpi_trace_severity_t severity, const char *file, int line, uint32_t time, char *msg)
{
    if ((severity > E_TRACE_SEVERITY_MAX) && (severity <= ezlopi_core_serial_log_get_current_severity_enum_val()))
    {

        switch (severity)
        {
        case E_TRACE_SEVERITY_ERROR:
        {
            printf("\r\n\x1B[%sm %s[%d]-%u: ", COLOR_FONT_RED, file, line, time);
            break;
        }
        case E_TRACE_SEVERITY_WARNING:
        {
            printf("\r\n\x1B[%sm %s[%d]-%u: ", COLOR_FONT_YELLOW, file, line, time);
            break;
        }
        case E_TRACE_SEVERITY_INFO:
        {
            printf("\r\n\x1B[%sm %s[%d]-%u: ", COLOR_FONT_BLUE, file, line, time);
            break;
        }
        case E_TRACE_SEVERITY_TRACE:
        {
            printf("\r\n\x1B[%sm %s[%d]-%u: ", COLOR_FONT_GREEN, file, line, time);
            break;
        }
        case E_TRACE_SEVERITY_DEBUG:
        default:
        {
            printf("\r\n\x1B[%sm %s[%d]-%u: ", COLOR_FONT_BLACK, file, line, time);
            break;
        }
        }

        printf(msg);
        printf("\x1B[0m\r\n");
    }
}

static ezlopi_error_t ezlopi_hub_cloud_log_set_severity(const char *severity_str)
{
    ezlopi_error_t ret = EZPI_FAILED;
    if (severity_str)
    {
        for (int i = 0; i < E_TRACE_SEVERITY_MAX; i++)
        {
            if (0 == strncmp(ezlopi_util_trace_get_severity_name_str(i), severity_str, strlen(ezlopi_util_trace_get_severity_name_str(i))))
            {
                if (i <= E_TRACE_SEVERITY_WARNING)
                {
                    cloud_log_severity = i;
                }
                else
                {
                    cloud_log_severity = E_TRACE_SEVERITY_WARNING;
                }
                ret = EZPI_SUCCESS;
                break;
            }
        }
    }
    return ret;
}

static ezlopi_error_t ezlopi_hub_serial_log_set_severity(const char *severity_str)
{
    ezlopi_error_t ret = EZPI_FAILED;
    if (severity_str)
    {
        for (int i = 0; i < E_TRACE_SEVERITY_MAX; i++)
        {
            if (0 == strncmp(ezlopi_util_trace_get_severity_name_str(i), severity_str, strlen(ezlopi_util_trace_get_severity_name_str(i))))
            {
                serial_log_severity = i;
                ret = EZPI_SUCCESS;
                break;
            }
        }
    }
    return ret;
}

#endif // CONFIG_EZPI_UTIL_TRACE_EN