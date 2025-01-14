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
 * @file    ezlopi_cloud_info.c
 * @brief   perform some function on data
 * @author  ezlopi_team_np
 * @version 0.1
 * @date    1st January 2024
 */

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include <string.h>
#include <time.h>

#include "freertos/FreeRTOS.h"

#include "cjext.h"

#include "ezlopi_core_sntp.h"
#include "ezlopi_core_info.h"
#include "ezlopi_core_factory_info.h"

#include "ezlopi_cloud_info.h"
#include "ezlopi_cloud_keywords.h"
#include "ezlopi_cloud_coordinates.h"
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

/*******************************************************************************
 *                          Static Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Definitions
 *******************************************************************************/
void EZPI_info_get(cJSON *cj_request, cJSON *cj_response)
{
    cJSON *cjson_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    if (cjson_result)
    {
        char *device_uuid = EZPI_core_factory_info_v3_get_device_uuid();
        // #include "esp_app_format.h"
        cJSON_AddStringToObject(__FUNCTION__, cjson_result, ezlopi_model_str, EZPI_core_factory_info_v3_get_device_type());
        cJSON_AddStringToObject(__FUNCTION__, cjson_result, "architecture", CONFIG_SDK_TOOLPREFIX);
        cJSON_AddStringToObject(__FUNCTION__, cjson_result, ezlopi_firmware_str, VERSION_STR);
        cJSON_AddStringToObject(__FUNCTION__, cjson_result, "kernel", "FreeRTOS");
        cJSON_AddStringToObject(__FUNCTION__, cjson_result, "hardware", CONFIG_IDF_TARGET);
        cJSON_AddNumberToObject(__FUNCTION__, cjson_result, ezlopi_serial_str, EZPI_core_factory_info_v3_get_id());

        cJSON_AddStringToObject(__FUNCTION__, cjson_result, ezlopi_uuid_str, device_uuid ? device_uuid : ezlopi__str);
        cJSON_AddBoolToObject(__FUNCTION__, cjson_result, "offlineAnonymousAccess", true);
        cJSON_AddBoolToObject(__FUNCTION__, cjson_result, "offlineInsecureAccess", true);

        cJSON *cjson_location = cJSON_AddObjectToObject(__FUNCTION__, cjson_result, ezlopi_location_str);
        if (cjson_location)
        {
            cJSON_AddNumberToObject(__FUNCTION__, cjson_location, ezlopi_latitude_str, EZPI_cloud_get_latitude());
            cJSON_AddNumberToObject(__FUNCTION__, cjson_location, ezlopi_longitude_str, EZPI_cloud_get_longitude());
            char *location = EZPI_core_sntp_get_location();
            if (location)
            {
                cJSON_AddStringToObject(__FUNCTION__, cjson_location, ezlopi_timezone_str, location);
                cJSON_AddStringToObject(__FUNCTION__, cjson_location, ezlopi_state_str, ezlopi_custom_timezone_str);
                ezlopi_free(__FUNCTION__, location);
            }
            else
            {
                cJSON_AddStringToObject(__FUNCTION__, cjson_location, ezlopi_timezone_str, ezlopi_gmt0_str);
                cJSON_AddStringToObject(__FUNCTION__, cjson_location, ezlopi_state_str, ezlopi_default_str);
            }
        }

        cJSON *cjson_build = cJSON_AddObjectToObject(__FUNCTION__, cjson_result, ezlopi_build_str);
        if (cjson_build)
        {
            {
                char build_time[64];
                EZPI_core_sntp_epoch_to_iso8601(build_time, sizeof(build_time), (time_t)BUILD_DATE);
                cJSON_AddStringToObject(__FUNCTION__, cjson_build, ezlopi_time_str, build_time);
            }

            cJSON_AddStringToObject(__FUNCTION__, cjson_build, "builder", DEVELOPER);
            cJSON_AddStringToObject(__FUNCTION__, cjson_build, "branch", CURRENT_BRANCH);
            cJSON_AddStringToObject(__FUNCTION__, cjson_build, "commit", COMMIT_HASH);
        }

        cJSON *cjson_battery = cJSON_AddObjectToObject(__FUNCTION__, cjson_result, "battery");
        if (cjson_battery)
        {
            cJSON_AddNumberToObject(__FUNCTION__, cjson_battery, "stateOfCharge", 0);
            cJSON_AddNumberToObject(__FUNCTION__, cjson_battery, "remainingTime", 0);
            cJSON_AddNumberToObject(__FUNCTION__, cjson_battery, "health", 0);
            cJSON_AddStringToObject(__FUNCTION__, cjson_battery, ezlopi_status_str, ezlopi__str);
        }

        {
            char local_time[100];
            EZPI_core_sntp_get_local_time(local_time, sizeof(local_time));
            cJSON_AddStringToObject(__FUNCTION__, cjson_result, "localtime", local_time);
        }

#if 0
        {
            char local_time[100];
            EZPI_core_sntp_get_up_time(local_time, sizeof(local_time));
            cJSON_AddStringToObject(__FUNCTION__, cjson_result, ezlopi_uptime_str, local_time);
        }
#else
        {
            char time_string[50];
            uint32_t tick_count_ms = xTaskGetTickCount() / portTICK_PERIOD_MS;
            EZPI_core_info_get_tick_to_time_name(time_string, sizeof(time_string), tick_count_ms);
            cJSON_AddStringToObject(__FUNCTION__, cjson_result, ezlopi_uptime_str, time_string);
        }

#endif
        EZPI_core_factory_info_v3_free(device_uuid);
    }
}

/*******************************************************************************
 *                          Static Function Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
