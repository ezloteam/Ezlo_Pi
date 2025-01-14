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
 * @file    ezlopi_cloud_location.c
 * @brief   Definitions for cloud location functions
 * @author  ezlopi_team_np
 * @version 1.0
 * @date    January 20, 2024
 */
/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include <string.h>
#include <time.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "cjext.h"
#include "zones.h"

#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_core_sntp.h"

#include "ezlopi_cloud_constants.h"
#include "ezlopi_util_trace.h"

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
void EZPI_CLOUD_location_list(cJSON *cj_request, cJSON *cj_response)
{
    cJSON *cjson_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    if (cjson_result)
    {
        cJSON *json_array_tzs = cJSON_CreateArray(__FUNCTION__);

        if (NULL != json_array_tzs)
        {
            const micro_tz_db_pair *tz_list = get_tz_db();
            uint32_t i = 0;
            // while (tz_list[i].name != NULL)
            while (i <= 50)
            {
                cJSON *json_location_name = cJSON_CreateString(__FUNCTION__, tz_list[i].name);

                if (NULL != json_location_name)
                {
                    cJSON_AddItemToArray(json_array_tzs, json_location_name);
                }
#ifdef CONFIG_EZPI_UTIL_TRACE_EN
                TRACE_E("Here !!! Looping %d : %s", i + 1, tz_list[i].name);
#endif
                vTaskDelay(1);
                i++;
            }
            cJSON_AddItemToObject(__FUNCTION__, cjson_result, ezlopi_locations_str, json_array_tzs);
        }
    }
}

void EZPI_CLOUD_location_get(cJSON *cj_request, cJSON *cj_response)
{
    cJSON *cjson_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    if (cjson_result)
    {
        char *location = EZPI_core_sntp_get_location();
        if (location)
        {
            cJSON_AddStringToObject(__FUNCTION__, cjson_result, ezlopi_location_str, location);
            ezlopi_free(__FUNCTION__, location);
        }
        else
        {
            cJSON_AddStringToObject(__FUNCTION__, cjson_result, ezlopi_location_str, ezlopi__str);
        }
    }
}

void EZPI_CLOUD_location_set(cJSON *cj_request, cJSON *cj_response)
{
    cJSON *cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
    if (cj_params)
    {
        char location_str[128];
        memset(location_str, 0, sizeof(location_str));
        CJSON_GET_VALUE_STRING_BY_COPY(cj_params, ezlopi_location_str, location_str);

        if ('\0' != location_str[0])
        {
            TRACE_I("Location: %s", location_str);
            EZPI_core_sntp_set_location(location_str);
            cJSON *cjson_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
            if (cjson_result)
            {
                cJSON_AddStringToObject(__FUNCTION__, cjson_result, ezlopi_location_str, location_str);
            }
        }
    }
}

/*******************************************************************************
 *                          Static Function Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
