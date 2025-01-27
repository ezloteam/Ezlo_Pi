/* ===========================================================================
** Copyright (C) 2025 Ezlo Innovation Inc
**
** Under EZLO AVAILABLE SOURCE LICENSE (EASL) AGREEMENT
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met:
**
** 1. Redistributions of source code must retain the above copyright notice,
**    this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. Neither the name of the copyright holder nor the names of its
**    contributors may be used to endorse or promote products derived from
**    this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
** AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
** ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
** LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
** CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
** SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
** INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
** CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
** ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
** POSSIBILITY OF SUCH DAMAGE.
** ===========================================================================
*/
/**
 * @file    ezlopi_core_log.c
 * @brief   Function operaters on system-logs
 * @author
 * @date    12th DEC 2024
 */

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include "../../build/config/sdkconfig.h"

#ifdef CONFIG_EZPI_UTIL_TRACE_EN

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
// #include "ezlopi_core_errors.h"

#include "ezlopi_cloud_constants.h"

#include "ezlopi_core_log.h"

/*******************************************************************************
 *                          Extern Data Declarations
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Declarations
 *******************************************************************************/

/*******************************************************************************
 *                          Type & Macro Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Static Function Prototypes
 *******************************************************************************/
static ezlopi_error_t ezlopi_hub_cloud_log_set_severity(const char *severity_str);
static ezlopi_error_t ezlopi_hub_serial_log_set_severity(const char *severity_str);
static ezlopi_error_t EZPI_core_serial_log_upcall(int severity, const char *log_str);

/*******************************************************************************
 *                          Static Data Definitions
 *******************************************************************************/
static e_trace_severity_t cloud_log_severity = ENUM_EZLOPI_TRACE_SEVERITY_WARNING;
static e_trace_severity_t serial_log_severity = ENUM_EZLOPI_TRACE_SEVERITY_MAX;

/*******************************************************************************
 *                          Extern Data Definitions
 *******************************************************************************/
const char *ezlopi_log_severity_enum[ENUM_EZLOPI_TRACE_SEVERITY_MAX] = {
    "NONE",
    "ERROR",
    "WARNING",
    "INFO",
    "DEBUG",
    "TRACE"};

/*******************************************************************************
 *                          Extern Function Definitions
 *******************************************************************************/
void EZPI_core_read_set_log_severities_internal(e_trace_severity_t severity)
{
    serial_log_severity = severity;
}

void EZPI_core_read_set_log_severities(void)
{
    EZPI_core_nvs_read_cloud_log_severity(&cloud_log_severity);
    EZPI_core_nvs_read_serial_log_severity(&serial_log_severity);
}

ezlopi_error_t EZPI_core_cloud_log_severity_process_str(bool severity_enable, const char *severity_str)
{
    ezlopi_error_t ret = EZPI_FAILED;
    if (severity_enable)
    {
        ezlopi_hub_cloud_log_set_severity(severity_str);
        ret = EZPI_SUCCESS;
    }
    else
    {
        cloud_log_severity = ENUM_EZLOPI_TRACE_SEVERITY_NONE;
        ret = EZPI_SUCCESS;
    }
    EZPI_core_nvs_write_cloud_log_severity(cloud_log_severity);
    return ret;
}

ezlopi_error_t EZPI_core_cloud_log_severity_process_id(const e_trace_severity_t severity_level_id)
{
    ezlopi_error_t ret = EZPI_FAILED;

    if ((ENUM_EZLOPI_TRACE_SEVERITY_MAX > severity_level_id) && (ENUM_EZLOPI_TRACE_SEVERITY_NONE <= severity_level_id))
    {
        if (severity_level_id <= ENUM_EZLOPI_TRACE_SEVERITY_WARNING)
        {
            cloud_log_severity = severity_level_id;
        }
        else
        {
            cloud_log_severity = ENUM_EZLOPI_TRACE_SEVERITY_WARNING;
        }
        EZPI_core_nvs_write_cloud_log_severity(cloud_log_severity);
        ret = EZPI_SUCCESS;
    }

    return ret;
}

const char **EZPI_core_cloud_log_get_severity_enums()
{
    return ezlopi_log_severity_enum;
}

ezlopi_error_t EZPI_core_serial_log_severity_process_str(const char *severity_str)
{
    ezlopi_error_t ret = EZPI_FAILED;
    if (severity_str)
    {
        ezlopi_hub_serial_log_set_severity(severity_str);
        ret = EZPI_SUCCESS;
    }
    else
    {
        serial_log_severity = ENUM_EZLOPI_TRACE_SEVERITY_NONE;
        ret = EZPI_SUCCESS;
    }
    EZPI_core_nvs_write_serial_log_severity(serial_log_severity);
    return ret;
}

ezlopi_error_t EZPI_core_serial_log_severity_process_id(const e_trace_severity_t severity_level_id)
{
    ezlopi_error_t ret = EZPI_FAILED;

    if ((ENUM_EZLOPI_TRACE_SEVERITY_MAX > severity_level_id) && (ENUM_EZLOPI_TRACE_SEVERITY_NONE <= severity_level_id))
    {
        serial_log_severity = severity_level_id;
        EZPI_core_nvs_write_serial_log_severity(serial_log_severity);
        ret = EZPI_SUCCESS;
    }

    return ret;
}

const char *EZPI_core_cloud_log_get_current_severity_enum_str()
{
    return ezlopi_log_severity_enum[cloud_log_severity];
}

const char *EZPI_core_serial_log_get_current_severity_enum_str()
{
    return ezlopi_log_severity_enum[serial_log_severity];
}

e_trace_severity_t EZPI_core_cloud_log_get_current_severity_enum_val()
{
    return cloud_log_severity;
}

e_trace_severity_t EZPI_core_serial_log_get_current_severity_enum_val()
{
    return serial_log_severity;
}

ezlopi_error_t EZPI_core_send_cloud_log(int severity, const char *log_str)
{
    ezlopi_error_t ret = EZPI_FAILED;

    if (severity <= EZPI_core_cloud_log_get_current_severity_enum_val())
    {
        e_ezlopi_event_t event = EZPI_core_event_group_get_eventbit_status();
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
                    time_t timestamp = EZPI_core_sntp_get_current_time_sec();

                    char timestamp_str[64];

                    EZPI_core_sntp_epoch_to_iso8601(timestamp_str, sizeof(timestamp_str), (time_t)timestamp);

                    size_t total_len = sizeof(timestamp_str) + strlen(log_str) + 2;
                    char message[total_len];
                    memset(message, 0, total_len);
                    snprintf(message, total_len, "%s: %s", timestamp_str, log_str);

                    cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_message_str, message);

                    char severity_str[10];
                    memset(severity_str, 0, 10);
                    snprintf(severity_str, 10, "%s", ezlopi_log_severity_enum[severity]);
                    cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_severity_str, severity_str);
                    if (EZPI_SUCCESS != EZPI_core_broadcast_add_to_queue(cj_log_broadcast, timestamp))
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

void EZPI_core_set_log_upcalls()
{
    ezlopi_util_set_log_upcalls(EZPI_core_send_cloud_log, EZPI_core_serial_log_upcall);
}

/*******************************************************************************
 *                         Static Function Definitions
 *******************************************************************************/
static ezlopi_error_t ezlopi_hub_cloud_log_set_severity(const char *severity_str)
{
    ezlopi_error_t ret = EZPI_FAILED;
    if (severity_str)
    {
        for (int i = 0; i < ENUM_EZLOPI_TRACE_SEVERITY_MAX; i++)
        {
            if (0 == strncmp(ezlopi_log_severity_enum[i], severity_str, strlen(ezlopi_log_severity_enum[i])))
            {
                if (i <= ENUM_EZLOPI_TRACE_SEVERITY_WARNING)
                {
                    cloud_log_severity = i;
                }
                else
                {
                    cloud_log_severity = ENUM_EZLOPI_TRACE_SEVERITY_WARNING;
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
        for (int i = 0; i < ENUM_EZLOPI_TRACE_SEVERITY_MAX; i++)
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

static ezlopi_error_t EZPI_core_serial_log_upcall(int severity, const char *log_str)
{
    ezlopi_error_t ret = EZPI_FAILED;
    if ((severity > ENUM_EZLOPI_TRACE_SEVERITY_NONE) && (severity <= EZPI_core_serial_log_get_current_severity_enum_val()))
    {
        printf("%s\n", log_str);
        ret = EZPI_SUCCESS;
    }
    return ret;
}

#endif // CONFIG_EZPI_UTIL_TRACE_EN

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
