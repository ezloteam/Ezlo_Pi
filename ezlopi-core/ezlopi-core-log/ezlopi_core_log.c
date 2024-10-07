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
#include "ezlopi_core_broadcast.h"
#include "ezlopi_core_event_group.h"
#include "ezlopi_core_errors.h"

#include "ezlopi_cloud_constants.h"

#include "ezlopi_core_log.h"

const char *ezlopi_log_severity_enum[ENUM_EZLOPI_LOG_SEVERITY_MAX] = {
    "NONE",
    "ERROR",
    "WARNING",
    "INFO",
    "DEBUG",
    "TRACE",
};

static e_ezlopi_log_severity_t cloud_log_severity = ENUM_EZLOPI_LOG_SEVERITY_WARNING;
static e_ezlopi_log_severity_t serial_log_severity = ENUM_EZLOPI_LOG_SEVERITY_MAX;

static ezlopi_error_t ezlopi_hub_cloud_log_set_severity(const char *severity_str)
{
    ezlopi_error_t ret = EZPI_FAILED;
    if (severity_str)
    {
        for (int i = 0; i < ENUM_EZLOPI_LOG_SEVERITY_MAX; i++)
        {
            if (0 == strncmp(ezlopi_log_severity_enum[i], severity_str, strlen(ezlopi_log_severity_enum[i])))
            {
                if (i <= ENUM_EZLOPI_LOG_SEVERITY_WARNING)
                {
                    cloud_log_severity = i;
                }
                else
                {
                    cloud_log_severity = ENUM_EZLOPI_LOG_SEVERITY_WARNING;
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
        for (int i = 0; i < ENUM_EZLOPI_LOG_SEVERITY_MAX; i++)
        {
            if (0 == strncmp(ezlopi_log_severity_enum[i], severity_str, strlen(ezlopi_log_severity_enum[i])))
            {
                serial_log_severity = i;
                ret = EZPI_SUCCESS;
                break;
            }
        }
    }
    return ret;
}

void ezlopi_core_read_set_log_severities_internal(e_ezlopi_log_severity_t severity)
{
    serial_log_severity = severity;
}

void ezlopi_core_read_set_log_severities()
{
    EZPI_CORE_nvs_read_cloud_log_severity(&cloud_log_severity);
    EZPI_CORE_nvs_read_serial_log_severity(&serial_log_severity);
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
        cloud_log_severity = ENUM_EZLOPI_LOG_SEVERITY_NONE;
        ret = EZPI_SUCCESS;
    }
    EZPI_CORE_nvs_write_cloud_log_severity(cloud_log_severity);
    return ret;
}

ezlopi_error_t ezlopi_core_cloud_log_severity_process_id(const e_ezlopi_log_severity_t severity_level_id)
{
    ezlopi_error_t ret = EZPI_FAILED;

    if ((ENUM_EZLOPI_LOG_SEVERITY_MAX > severity_level_id) && (ENUM_EZLOPI_LOG_SEVERITY_NONE <= severity_level_id))
    {
        if (severity_level_id <= ENUM_EZLOPI_LOG_SEVERITY_WARNING)
        {
            cloud_log_severity = severity_level_id;
        }
        else
        {
            cloud_log_severity = ENUM_EZLOPI_LOG_SEVERITY_WARNING;
        }
        EZPI_CORE_nvs_write_cloud_log_severity(cloud_log_severity);
        ret = EZPI_SUCCESS;
    }

    return ret;
}

const char **ezlopi_core_cloud_log_get_severity_enums()
{
    return ezlopi_log_severity_enum;
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
        serial_log_severity = ENUM_EZLOPI_LOG_SEVERITY_NONE;
        ret = EZPI_SUCCESS;
    }
    EZPI_CORE_nvs_write_serial_log_severity(serial_log_severity);
    return ret;
}

ezlopi_error_t ezlopi_core_serial_log_severity_process_id(const e_ezlopi_log_severity_t severity_level_id)
{
    ezlopi_error_t ret = EZPI_FAILED;

    if ((ENUM_EZLOPI_LOG_SEVERITY_MAX > severity_level_id) && (ENUM_EZLOPI_LOG_SEVERITY_NONE <= severity_level_id))
    {
        serial_log_severity = severity_level_id;
        EZPI_CORE_nvs_write_serial_log_severity(serial_log_severity);
        ret = EZPI_SUCCESS;
    }

    return ret;
}

const char *ezlopi_core_cloud_log_get_current_severity_enum_str()
{
    return ezlopi_log_severity_enum[cloud_log_severity];
}

const char *ezlopi_core_serial_log_get_current_severity_enum_str()
{
    return ezlopi_log_severity_enum[serial_log_severity];
}

e_ezlopi_log_severity_t ezlopi_core_cloud_log_get_current_severity_enum_val()
{
    return cloud_log_severity;
}

e_ezlopi_log_severity_t ezlopi_core_serial_log_get_current_severity_enum_val()
{
    return serial_log_severity;
}

ezlopi_error_t ezlopi_core_send_cloud_log(int severity, const char *log_str)
{
    ezlopi_error_t ret = EZPI_FAILED;

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

                    size_t total_len = sizeof(timestamp_str) + strlen(log_str) + 2;
                    char message[total_len];
                    memset(message, 0, total_len);
                    snprintf(message, total_len, "%s: %s", timestamp_str, log_str);

                    cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_message_str, message);

                    char severity_str[10];
                    memset(severity_str, 0, 10);
                    snprintf(severity_str, 10, "%s", ezlopi_log_severity_enum[severity]);
                    cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_severity_str, severity_str);
                    if (EZPI_SUCCESS != ezlopi_core_broadcast_add_to_queue(cj_log_broadcast))
                    {
                        cJSON_Delete(__FUNCTION__, cj_log_broadcast);
                    }
                    ret = EZPI_SUCCESS;
                }
            }
        }
    }
    return ret;
}

static ezlopi_error_t ezlopi_core_serial_log_upcall(int severity, const char *log_str)
{
    ezlopi_error_t ret = EZPI_FAILED;
    if ((severity > ENUM_EZLOPI_LOG_SEVERITY_NONE) && (severity <= ezlopi_core_serial_log_get_current_severity_enum_val()))
    {
        printf("%s\n", log_str);
        ret = EZPI_SUCCESS;
    }
    return ret;
}

void ezlopi_core_set_log_upcalls()
{
    ezlopi_util_set_log_upcalls(ezlopi_core_send_cloud_log, ezlopi_core_serial_log_upcall);
}

/////////////////
#include "cjext.h"

static ezlopi_error_t __otel_logs_capture(int severity, const char *log_str)
{
    ezlopi_error_t ret = EZPI_FAILED;

    if ((severity > ENUM_EZLOPI_LOG_SEVERITY_NONE) && (severity <= ezlopi_core_serial_log_get_current_severity_enum_val()))
    {
        cJSON *cj_log = cJSON_CreateObject(__FUNCTION__);

        if (cj_log)
        {
            cJSON_AddStringToObject(__FUNCTION__, cj_log, "body", log_str);

            char tmp_string[128];
            snprintf(tmp_string, sizeof(tmp_string), "<SeverityNumber.%s>: %d", ezlopi_log_severity_enum[severity], severity);
            cJSON_AddStringToObject(__FUNCTION__, cj_log, "severity_number", tmp_string);
            cJSON_AddStringToObject(__FUNCTION__, cj_log, "severity_text", ezlopi_log_severity_enum[severity]);

            cJSON *cj_attributes = cJSON_AddObjectToObject(__FUNCTION__, cj_log, "attributes");
            if (cj_attributes)
            {
                cJSON_AddStringToObject(__FUNCTION__, cj_attributes, "otelSpanID", "0");
                cJSON_AddStringToObject(__FUNCTION__, cj_attributes, "otelTraceID", "0");
                cJSON_AddFalseToObject(__FUNCTION__, cj_attributes, "otelTraceSampled");
                cJSON_AddStringToObject(__FUNCTION__, cj_attributes, "otelServiceName", "ezlopi");
                cJSON_AddStringToObject(__FUNCTION__, cj_attributes, "code.filepath", "");
                cJSON_AddNumberToObject(__FUNCTION__, cj_attributes, "code.lineno", 0);
            }

            cJSON_AddNumberToObject(__FUNCTION__, cj_log, "dropped_attributes", 0);
            cJSON_AddStringToObject(__FUNCTION__, cj_log, "timestamp", "0");
            cJSON_AddStringToObject(__FUNCTION__, cj_log, "observed_timestamp", "0");
            cJSON_AddStringToObject(__FUNCTION__, cj_log, "trace_id", "0x00000000000000000000000000000000");
            cJSON_AddStringToObject(__FUNCTION__, cj_log, "span_id", "0x0000000000000000");
            cJSON_AddFalseToObject(__FUNCTION__, cj_log, "trace_flags");

            cJSON *cj_resource = cJSON_AddObjectToObject(__FUNCTION__, cj_log, "resource");
            if (cj_resource)
            {
                cJSON *cj_attributes_2 = cJSON_AddObjectToObject(__FUNCTION__, cj_resource, "attributes");
                if (cj_attributes_2)
                {
                    cJSON_AddStringToObject(__FUNCTION__, cj_attributes_2, "telemetry.sdk.langauge", "c");
                    cJSON_AddStringToObject(__FUNCTION__, cj_attributes_2, "telemetry.sdk.name", "opentelemetry");
                    cJSON_AddStringToObject(__FUNCTION__, cj_attributes_2, "telemetry.sdk.version", "0.1.0");
                    cJSON_AddStringToObject(__FUNCTION__, cj_attributes_2, "telemetry.sdk.langauge", "c");
                    cJSON_AddStringToObject(__FUNCTION__, cj_attributes_2, "service.name", "ezlopi");
                    cJSON_AddStringToObject(__FUNCTION__, cj_attributes_2, "telemetry.auto.version", "0.18b0");
                }

                cJSON_AddStringToObject(__FUNCTION__, cj_attributes_2, "schema_url", "");
            }
        }

        ret = EZPI_SUCCESS;
    }

    return ret;
}

void ezlopi_core_set_otel_logs_upcalls(void)
{
    ezlopi_util_set_log_upcalls(NULL, ezlopi_core_serial_log_upcall);
}

#endif // CONFIG_EZPI_UTIL_TRACE_EN