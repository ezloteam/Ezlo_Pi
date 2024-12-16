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
* @file    ezlopi_core_device_value_updated.c
* @brief   Function for sensor device operations
* @author  xx
* @version 0.1
* @date    12th DEC 2024
*/

/*******************************************************************************
*                          Include Files
*******************************************************************************/

// #include "ezlopi_core_errors.h"
// #include "ezlopi_core_devices_list.h"
#include "ezlopi_core_broadcast.h"

#include "ezlopi_cloud_items.h"
#include "ezlopi_cloud_settings.h"
#include "ezlopi_cloud_constants.h"

#include "ezlopi_service_webprov.h"
#include "ezlopi_core_device_value_updated.h"

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
static cJSON *__broadcast_message_items_updated_from_device(l_ezlopi_device_t *device, l_ezlopi_item_t *item);
static cJSON *__broadcast_message_settings_updated_from_devices_v3(l_ezlopi_device_t *device, l_ezlopi_device_settings_v3_t *setting);

/*******************************************************************************
*                          Static Data Definitions
*******************************************************************************/

/*******************************************************************************
*                          Extern Data Definitions
*******************************************************************************/

/*******************************************************************************
*                          Extern Function Definitions
*******************************************************************************/
ezlopi_error_t EZPI_core_device_value_updated_from_device_broadcast(l_ezlopi_item_t *item)
{
    ezlopi_error_t ret = EZPI_SUCCESS;

    // TRACE_D("%d -> here", xTaskGetTickCount());

    if (item)
    {
        l_ezlopi_device_t *curr_device = EZPI_core_device_get_head();
        while (curr_device)
        {
            l_ezlopi_item_t *curr_item = curr_device->items;

            while (curr_item)
            {
                if (item == curr_item)
                {
                    // TRACE_D("%d -> here", xTaskGetTickCount());

                    cJSON *cj_response = __broadcast_message_items_updated_from_device(curr_device, item);

                    // TRACE_D("%d -> here", xTaskGetTickCount());

                    // CJSON_TRACE("----------------- broadcasting - cj_response", cj_response);
                    if (cj_response)
                    {
                        if (EZPI_SUCCESS != ezlopi_core_broadcast_add_to_queue(cj_response))
                        {
                            ret = EZPI_FAILED;
                            cJSON_Delete(__FUNCTION__, cj_response);
                        }

                        // TRACE_D("%d -> here", xTaskGetTickCount());
                    }

                    break;
                }

                curr_item = curr_item->next;
            }

            curr_device = curr_device->next;
        }
    }

    return ret;
}

ezlopi_error_t EZPI_core_device_value_updated_from_device_broadcast_by_item_id(uint32_t item_id)
{
    ezlopi_error_t ret = EZPI_SUCCESS;

    l_ezlopi_device_t *curr_device = EZPI_core_device_get_head();
    while (curr_device)
    {
        l_ezlopi_item_t *curr_item = curr_device->items;

        while (curr_item)
        {
            if (item_id == curr_item->cloud_properties.item_id)
            {
                // cJSON* cj_response = NULL;
                cJSON *cj_response = __broadcast_message_items_updated_from_device(curr_device, curr_item);
                CJSON_TRACE("----------------- broadcasting - cj_response", cj_response);

                ret = ezlopi_core_broadcast_add_to_queue(cj_response);
                if (EZPI_SUCCESS != ret)
                {
                    ret = EZPI_FAILED;
                    cJSON_Delete(__FUNCTION__, cj_response);
                }

                break;
            }

            curr_item = curr_item->next;
        }

        curr_device = curr_device->next;
    }

    return ret;
}

ezlopi_error_t EZPI_core_device_value_updated_settings_broadcast(l_ezlopi_device_settings_v3_t *setting)
{
    ezlopi_error_t ret = EZPI_SUCCESS;

    if (setting)
    {
        l_ezlopi_device_t *curr_device = EZPI_core_device_get_head();
        while (curr_device)
        {
            l_ezlopi_device_settings_v3_t *curr_setting = curr_device->settings;
            while (curr_setting)
            {
                if (setting == curr_setting)
                {
                    cJSON *cj_response = __broadcast_message_settings_updated_from_devices_v3(curr_device, setting);
                    CJSON_TRACE("----------------- broadcasting - cj_response", cj_response);
                    ret = ezlopi_core_broadcast_add_to_queue(cj_response);

                    if (EZPI_SUCCESS != ret)
                    {
                        ret = EZPI_FAILED;
                        cJSON_Delete(__FUNCTION__, cj_response);
                    }
                    break;
                }
                curr_setting = curr_setting->next;
            }
            curr_device = curr_device->next;
        }
    }

    return ret;
}

#if 0
int ezlopi_setting_value_updated_from_device_settings_id_v3(uint32_t setting_id)
{
    int ret = 0;

    // if (setting)
    {
        l_ezlopi_device_t *curr_device = EZPI_core_device_get_head();
        while (curr_device)
        {
            l_ezlopi_device_settings_v3_t *curr_setting = curr_device->settings;
            while (curr_setting)
            {
                if (setting_id == curr_setting->cloud_properties.setting_id)
                {
                    cJSON *cj_response = __broadcast_message_settings_updated_from_devices_v3(curr_device, curr_setting);
                    CJSON_TRACE("----------------- broadcasting - cj_response", cj_response);
                    ret = ezlopi_core_broadcast_add_to_queue(cj_response);

                    if (0 == ret)
                    {
                        cJSON_Delete(__FUNCTION__, cj_response);
                    }
                    break;
                }
                curr_setting = curr_setting->next;
            }
            curr_device = curr_device->next;
        }
    }

    return ret;
}
#endif

ezlopi_error_t EZPI_core_device_value_update_wifi_scan_broadcast(cJSON *network_array)
{
    ezlopi_error_t ret = EZPI_SUCCESS;
    if (network_array)
    {
        cJSON *cj_response = cJSON_CreateObject(__FUNCTION__);
        if (cj_response)
        {
            cJSON_AddStringToObject(__FUNCTION__, cj_response, ezlopi_id_str, ezlopi_ui_broadcast_str);
            cJSON_AddStringToObject(__FUNCTION__, cj_response, ezlopi_msg_subclass_str, method_hub_network_wifi_scan_progress);
            // cJSON_AddNumberToObject(__FUNCTION__, cj_response, ezlopi_msg_id_str, EZPI_service_web_provisioning_get_message_count());

            cJSON *result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
            if (result)
            {
                cJSON_AddStringToObject(__FUNCTION__, result, ezlopi_interfaceId_str, ezlopi_wlan0_str);
                cJSON_AddStringToObject(__FUNCTION__, result, ezlopi_status_str, "process");
                cJSON_AddItemToObject(__FUNCTION__, result, "networks", network_array);
            }

            CJSON_TRACE("----------------- broadcasting - cj_response", cj_response);
            ret = ezlopi_core_broadcast_add_to_queue(cj_response);

            if (EZPI_SUCCESS != ret)
            {
                ret = EZPI_SUCCESS;
                cJSON_Delete(__FUNCTION__, cj_response);
            }
        }
    }
    return ret;
}


/*******************************************************************************
*                         Static Function Definitions
*******************************************************************************/
static cJSON *__broadcast_message_items_updated_from_device(l_ezlopi_device_t *device, l_ezlopi_item_t *item)
{
    cJSON *cjson_response = NULL;

    if (NULL != item && NULL != device)
    {
        cjson_response = cJSON_CreateObject(__FUNCTION__);
        if (cjson_response)
        {
            cJSON_AddStringToObject(__FUNCTION__, cjson_response, ezlopi_msg_subclass_str, method_hub_item_updated);
            // cJSON_AddNumberToObject(__FUNCTION__, cjson_response, ezlopi_msg_id_str, EZPI_service_web_provisioning_get_message_count());
            cJSON_AddStringToObject(__FUNCTION__, cjson_response, ezlopi_id_str, ezlopi_ui_broadcast_str);

            cJSON *cj_result = cJSON_AddObjectToObject(__FUNCTION__, cjson_response, ezlopi_result_str);
            if (cj_result)
            {
                char tmp_string[64];
                snprintf(tmp_string, sizeof(tmp_string), "%08x", item->cloud_properties.item_id);
                cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi__id_str, tmp_string);
                snprintf(tmp_string, sizeof(tmp_string), "%08x", device->cloud_properties.device_id);
                cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_deviceId_str, tmp_string);
                cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_deviceName_str, device->cloud_properties.device_name);
                cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_deviceCategory_str, device->cloud_properties.category);
                cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_deviceSubcategory_str, device->cloud_properties.subcategory);
                cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_roomName_str, ezlopi__str); // roomName -> logic needs to be understood first
                cJSON_AddFalseToObject(__FUNCTION__, cj_result, ezlopi_serviceNotification_str);
                cJSON_AddFalseToObject(__FUNCTION__, cj_result, ezlopi_userNotification_str);
                cJSON_AddNullToObject(__FUNCTION__, cj_result, ezlopi_notifications_str);
                cJSON_AddFalseToObject(__FUNCTION__, cj_result, ezlopi_syncNotification_str);
                cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_name_str, item->cloud_properties.item_name);

                if (item->cloud_properties.scale)
                {
                    cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_scale_str, item->cloud_properties.scale);
                }

                item->func(EZLOPI_ACTION_GET_EZLOPI_VALUE, item, cj_result, item->user_arg);
                // registered_device->device->func(EZLOPI_ACTION_GET_EZLOPI_VALUE, registered_device->properties, cj_result, registered_device->user_arg);
                cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_valueType_str, item->cloud_properties.value_type);
            }
        }
        else
        {
            cJSON_Delete(__FUNCTION__, cjson_response);
            cjson_response = NULL;
        }
    }

    return cjson_response;
}

static cJSON *__broadcast_message_settings_updated_from_devices_v3(l_ezlopi_device_t *device, l_ezlopi_device_settings_v3_t *setting)
{
    cJSON *cjson_response = cJSON_CreateObject(__FUNCTION__);
    if (cjson_response)
    {
        if (NULL != setting)
        {
            cJSON_AddStringToObject(__FUNCTION__, cjson_response, ezlopi_msg_subclass_str, method_hub_device_setting_updated);
            // cJSON_AddNumberToObject(__FUNCTION__, cjson_response, ezlopi_msg_id_str, EZPI_service_web_provisioning_get_message_count());
            cJSON_AddStringToObject(__FUNCTION__, cjson_response, ezlopi_id_str, ezlopi_ui_broadcast_str);

            cJSON *cj_result = cJSON_AddObjectToObject(__FUNCTION__, cjson_response, ezlopi_result_str);
            if (cj_result)
            {
                char tmp_string[64];
                snprintf(tmp_string, sizeof(tmp_string), "%08x", setting->cloud_properties.setting_id);
                cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi__id_str, tmp_string);
                setting->func(EZLOPI_SETTINGS_ACTION_UPDATE_SETTING, setting, cj_result, setting->user_arg);
            }
        }
        else
        {
            cJSON_Delete(__FUNCTION__, cjson_response);
            cjson_response = NULL;
        }
    }

    return cjson_response;
}

/*******************************************************************************
*                          End of File
*******************************************************************************/