/* ===========================================================================
** Copyright (C) 2024 Ezlo Innovation Inc
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
 * @file    main.c
 * @brief   perform some function on data
 * @author  John Doe
 * @version 0.1
 * @date    1st January 2024
 */

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#if 0

#include <string.h>

#include "cjext.h"
#include "ezlopi_cloud_data.h"
#include "ezlopi_util_trace.h"

#include "ezlopi_core_devices_list.h"
#include "ezlopi_cloud_keywords.h"
#include "ezlopi_cloud_methods_str.h"

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

/**
 * @brief Global/extern function template example
 * Convention : Use capital letter for initial word on extern function
 * @param arg
 */
void data_list(cJSON* cj_request, cJSON* cj_response)
{
    cJSON* cjson_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    if (cjson_result)
    {
        cJSON* cj_settings = cJSON_AddObjectToObject(__FUNCTION__, cjson_result, ezlopi_settings_str);
        if (cj_settings)
        {
            cJSON* cj_first_start = cJSON_AddObjectToObject(__FUNCTION__, cj_settings, ezlopi_first_start_str);
            if (cj_first_start)
            {
                cJSON_AddNumberToObject(__FUNCTION__, cj_first_start, ezlopi_value_str, 0);
            }

            #warning "WARNING: work required here!"
#if 0
                l_ezlopi_configured_devices_t* registered_devices = ezlopi_devices_list_get_configured_items();
            while (NULL != registered_devices)
            {
                if (NULL != registered_devices->properties)
                {
                    break;
                }

                registered_devices = registered_devices->next;
            }
#endif
        }
    }
}

/*******************************************************************************
 *                          Static Function Definitions
 *******************************************************************************/
#if 0
static cJSON* ezlopi_cloud_data_create_device_list(void)
{
    cJSON* cjson_device_list = cJSON_CreateObject(__FUNCTION__);

    if (cjson_device_list)
    {
        cJSON_AddNumberToObject(__FUNCTION__, cjson_device_list, "ids", 1234);
    }

    return cjson_device_list;
}

static cJSON* ezlopi_cloud_data_create_settings_list(void)
{
    cJSON* cjson_device_list = cJSON_CreateObject(__FUNCTION__);

    if (cjson_device_list)
    {
        cJSON_AddNumberToObject(__FUNCTION__, cjson_device_list, "ids", 1234);
    }

    return cjson_device_list;
}

static cJSON* ezlopi_cloud_data_list_settings(l_ezlopi_configured_devices_t* ezlopi_device)
{
    cJSON* cjson_settings = NULL;
    if (ezlopi_device)
    {
        cjson_settings = cJSON_CreateObject(__FUNCTION__);
        if (cjson_settings)
        {
            char tmp_string[64];
            snprintf(tmp_string, sizeof(tmp_string), "%08x", ezlopi_device->properties->ezlopi_cloud.device_id);
            cJSON_AddStringToObject(__FUNCTION__, cjson_settings, ezlopi__id_str, tmp_string);
            cJSON_AddStringToObject(__FUNCTION__, cjson_settings, ezlopi_deviceTypeId_str, ezlopi_ezlopi_str);
            cJSON_AddStringToObject(__FUNCTION__, cjson_settings, ezlopi_parentDeviceId_str, ezlopi__str);
            cJSON_AddStringToObject(__FUNCTION__, cjson_settings, ezlopi_category_str, ezlopi_device->properties->ezlopi_cloud.category);
            cJSON_AddStringToObject(__FUNCTION__, cjson_settings, ezlopi_subcategory_str, ezlopi_device->properties->ezlopi_cloud.subcategory);
            cJSON_AddBoolToObject(__FUNCTION__, cjson_settings, ezlopi_batteryPowered_str, ezlopi_device->properties->ezlopi_cloud.battery_powered);
            cJSON_AddStringToObject(__FUNCTION__, cjson_settings, ezlopi_name_str, ezlopi_device->properties->ezlopi_cloud.device_name);
            cJSON_AddStringToObject(__FUNCTION__, cjson_settings, ezlopi_type_str, ezlopi_device->properties->ezlopi_cloud.device_type);
            cJSON_AddBoolToObject(__FUNCTION__, cjson_settings, ezlopi_reachable_str, ezlopi_device->properties->ezlopi_cloud.reachable);
            cJSON_AddBoolToObject(__FUNCTION__, cjson_settings, ezlopi_persistent_str, true);
            cJSON_AddBoolToObject(__FUNCTION__, cjson_settings, ezlopi_serviceNotification_str, false);
            cJSON_AddBoolToObject(__FUNCTION__, cjson_settings, "armed", false);
            snprintf(tmp_string, sizeof(tmp_string), "%08x", ezlopi_device->properties->ezlopi_cloud.room_id);
            cJSON_AddStringToObject(__FUNCTION__, cjson_settings, ezlopi_roomId_str, tmp_string);
            cJSON_AddStringToObject(__FUNCTION__, cjson_settings, ezlopi_security_str, ezlopi__str);
            cJSON_AddBoolToObject(__FUNCTION__, cjson_settings, ezlopi_ready_str, true);
            cJSON_AddStringToObject(__FUNCTION__, cjson_settings, ezlopi_status_str, ezlopi_synced_str);
            cJSON_AddStringToObject(__FUNCTION__, cjson_settings, ezlopi_info_str, "{}");
        }
    }

    return cjson_settings;
}
#endif

#endif

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
