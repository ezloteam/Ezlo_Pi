

/**
 * @file    ezlopi_cloud_log.h
 * @brief
 * @author
 * @version
 * @date
 */
/* ===========================================================================
** Copyright (C) 2022 Ezlo Innovation Inc
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

#include "../../build/config/sdkconfig.h"

#ifdef CONFIG_EZPI_UTIL_TRACE_EN

#include <stdbool.h>
#include <string.h>

#include "ezlopi_core_log.h"
#include "ezlopi_core_sntp.h"

#include "ezlopi_cloud_constants.h"
#include "ezlopi_cloud_log.h"

void EZPI_hub_cloud_log_set(cJSON *cj_request, cJSON *cj_response)
{
    cJSON *cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    if (cj_result)
    {
        cJSON *cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
        if (cj_params)
        {
            cJSON *cj_log_enable = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_enable_str);
            cJSON *cj_log_severity = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_severity_str);
            if (cj_log_enable && cj_log_severity)
            {
                bool severity_enable = cJSON_IsTrue(cj_log_enable);
                const char *log_severity_type = cj_log_severity->type == cJSON_String ? cj_log_severity->valuestring : NULL;
                ezlopi_core_cloud_log_severity_process_str(severity_enable, log_severity_type);
            }
        }
    }
}

void EZPI_hub_serial_log_set(cJSON *cj_request, cJSON *cj_response)
{
    cJSON *cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    if (cj_result)
    {
        cJSON *cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
        if (cj_params)
        {
            cJSON *cj_log_severity = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_severity_str);
            if (cj_log_severity)
            {
                const char *log_severity_type = cj_log_severity->type == cJSON_String ? cj_log_severity->valuestring : NULL;
                ezlopi_core_serial_log_severity_process_str(log_severity_type);
            }
        }
    }
}

void ezlopi_hub_serial_log_set_updater(cJSON *cj_request, cJSON *cj_response)
{
    cJSON *cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    if (cj_result)
    {
        cJSON *cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
        if (cj_params)
        {
            cJSON *cj_log_severity = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_severity_str);
            if (cj_log_severity)
            {
                cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_name_str, "log.level");
                const char *severity_str = ezlopi_core_serial_log_get_current_severity_enum_str();
                cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_value_str, severity_str);
            }
        }
    }
}

#endif // CONFIG_EZPI_UTIL_TRACE_EN

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
