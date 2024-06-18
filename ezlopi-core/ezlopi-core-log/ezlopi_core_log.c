#include "../../build/config/sdkconfig.h"

#ifdef CONFIG_EZPI_UTIL_TRACE_EN

#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#include "cjext.h"
#include "ezlopi_core_heap.h"

#include "ezlopi_core_sntp.h"
#include "ezlopi_core_ezlopi_broadcast.h"
#include "ezlopi_core_nvs.h"
#include "ezlopi_core_event_group.h"

#include "ezlopi_cloud_constants.h"

#include "ezlopi_core_log.h"

const char* ezlopi_log_severity_enum[ENUM_EZLOPI_LOG_SEVERITY_MAX] = {
    "NONE",
    "ERROR",
    "WARNING",
    "INFO",
    "DEBUG",
    "TRACE"
};


static e_ezlopi_log_severity_t cloud_log_severity = ENUM_EZLOPI_LOG_SEVERITY_WARNING;
static e_ezlopi_log_severity_t serial_log_severity = ENUM_EZLOPI_LOG_SEVERITY_MAX;

static int ezlopi_hub_cloud_log_set_severity(const char* severity_str)
{
    int ret = 0;
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
                ret = 1;
                break;
            }
        }
    }
    return ret;
}

static int ezlopi_hub_serial_log_set_severity(const char* severity_str)
{
    int ret = 0;
    if (severity_str)
    {
        for (int i = 0; i < ENUM_EZLOPI_LOG_SEVERITY_MAX; i++)
        {
            if (0 == strncmp(ezlopi_log_severity_enum[i], severity_str, strlen(ezlopi_log_severity_enum[i])))
            {
                serial_log_severity = i;
                ret = 1;
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

int ezlopi_core_cloud_log_severity_process_str(bool severity_enable, const char* severity_str)
{
    int ret = 0;
    if (severity_enable)
    {
        ezlopi_hub_cloud_log_set_severity(severity_str);
        ret = 1;
    }
    else
    {
        cloud_log_severity = ENUM_EZLOPI_LOG_SEVERITY_NONE;
        ret = 1;
    }
    EZPI_CORE_nvs_write_cloud_log_severity(cloud_log_severity);
    return ret;
}

int ezlopi_core_cloud_log_severity_process_id(const e_ezlopi_log_severity_t severity_level_id)
{
    int ret = 0;

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
        ret = 1;
    }

    return ret;
}

int ezlopi_core_serial_log_severity_process_str(const char* severity_str)
{
    int ret = 0;
    if (severity_str)
    {
        ezlopi_hub_serial_log_set_severity(severity_str);
        ret = 1;
    }
    else
    {
        serial_log_severity = ENUM_EZLOPI_LOG_SEVERITY_NONE;
        ret = 1;
    }
    EZPI_CORE_nvs_write_serial_log_severity(serial_log_severity);
    return ret;
}

int ezlopi_core_serial_log_severity_process_id(const e_ezlopi_log_severity_t severity_level_id)
{
    int ret = 0;

    if ((ENUM_EZLOPI_LOG_SEVERITY_MAX > severity_level_id) && (ENUM_EZLOPI_LOG_SEVERITY_NONE <= severity_level_id))
    {
        serial_log_severity = severity_level_id;
        EZPI_CORE_nvs_write_serial_log_severity(serial_log_severity);
        ret = 1;
    }

    return ret;
}

const char* ezlopi_core_cloud_log_get_current_severity_enum_str()
{
    return ezlopi_log_severity_enum[cloud_log_severity];
}

const char* ezlopi_core_serial_log_get_current_severity_enum_str()
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

int ezlopi_core_send_cloud_log(int severity, const char* log_str)
{
    int ret = 0;

    if (severity <= ezlopi_core_cloud_log_get_current_severity_enum_val())
    {
        e_ezlopi_event_t event = ezlopi_get_event_bit_status();
        if ((event & EZLOPI_EVENT_NMA_REG) == EZLOPI_EVENT_NMA_REG)
        {
            cJSON* cj_log_broadcast = cJSON_CreateObject(__FUNCTION__);
            if (cj_log_broadcast)
            {
                cJSON_AddStringToObject(__FUNCTION__, cj_log_broadcast, ezlopi_id_str, ezlopi_ui_broadcast_str);
                cJSON_AddStringToObject(__FUNCTION__, cj_log_broadcast, ezlopi_msg_subclass_str, "hub.log");
                cJSON* cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_log_broadcast, ezlopi_result_str);
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
                    if (!ezlopi_core_ezlopi_broadcast_add_to_queue(cj_log_broadcast))
                    {
                        cJSON_Delete(__FUNCTION__, cj_log_broadcast);
                    }
                    ret = 1;
                }
            }
        }
    }
    return ret;
}

static int ezlopi_core_serial_log_upcall(int severity, const char* log_str)
{
    int ret = 0;
    if ((severity > ENUM_EZLOPI_LOG_SEVERITY_NONE) && (severity <= ezlopi_core_serial_log_get_current_severity_enum_val()))
    {
        printf("%s\n", log_str);
        ret = 1;
    }
    return ret;
}

void ezlopi_core_set_log_upcalls()
{
    ezlopi_util_set_log_upcalls(ezlopi_core_send_cloud_log, ezlopi_core_serial_log_upcall);
}

#endif // CONFIG_EZPI_UTIL_TRACE_EN