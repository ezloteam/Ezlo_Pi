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

/**
 * @file    ezlopi_cloud_items.c
 * @brief
 * @author  Krishna Kumar Sah (work.krishnasah@gmail.com)
 *          Lomas Subedi
 *          Riken Maharjan
 *          Nabin Dangi
 * @version 1.0
 * @date    October 29th, 2022 12:26 AM
 */

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include "../../build/config/sdkconfig.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "ezlopi_util_trace.h"

#include "ezlopi_core_room.h"
#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_core_devices_list.h"

#include "ezlopi_cloud_items.h"
#include "ezlopi_cloud_constants.h"
#include "ezlopi_cloud_methods_str.h"

#include "ezlopi_service_webprov.h"

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
/**
 * @brief Function that create item table from item properties
 *
 * @param device_prop Pointer to the device to make table of
 * @param item_properties Pointer to the item
 * @return cJSON*
 */

static cJSON *ezpi_device_create_item_table_from_prop(l_ezlopi_device_t *device_prop, l_ezlopi_item_t *item_properties);

/*******************************************************************************
 *                          Static Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Definitions
 *******************************************************************************/
void EZPI_items_list_v3(cJSON *cj_request, cJSON *cj_response)
{
    cJSON *cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    if (cj_result)
    {
        cJSON *cj_items_array = cJSON_AddArrayToObject(__FUNCTION__, cj_result, ezlopi_items_str);

        if (cj_items_array)
        {

            cJSON *params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
            if (params != NULL)
            {
                cJSON *device_ids_array = cJSON_GetObjectItem(__FUNCTION__, params, ezlopi_device_ids_str);

                if (device_ids_array != NULL)
                {
                    if (cJSON_IsArray(device_ids_array))
                    {
                        if (cJSON_GetArraySize(device_ids_array) > 0)
                        {
                            cJSON *device_id;
                            cJSON_ArrayForEach(device_id, device_ids_array)
                            {
                                if (cJSON_IsString(device_id))
                                {
                                    char *device_id_str = device_id->valuestring;
                                    uint32_t device_id = strtol(device_id_str, NULL, 16);

                                    l_ezlopi_device_t *curr_device = EZPI_core_device_get_head();
                                    while (curr_device)
                                    {
                                        if (curr_device->cloud_properties.device_id == device_id)
                                        {
                                            l_ezlopi_item_t *curr_item = curr_device->items;
                                            while (curr_item)
                                            {
                                                cJSON *cj_item_properties = ezpi_device_create_item_table_from_prop(curr_device, curr_item);
                                                if (cj_item_properties)
                                                {
                                                    if (!cJSON_AddItemToArray(cj_items_array, cj_item_properties))
                                                    {
                                                        cJSON_Delete(__FUNCTION__, cj_item_properties);
                                                    }
                                                }

                                                curr_item = curr_item->next;
                                            }
                                        }

                                        curr_device = curr_device->next;
                                    }
                                }
                            }
                        }
                    }
                }
                else
                {
                    l_ezlopi_device_t *curr_device = EZPI_core_device_get_head();
                    while (curr_device)
                    {
                        l_ezlopi_item_t *curr_item = curr_device->items;
                        while (curr_item)
                        {
                            cJSON *cj_item_properties = ezpi_device_create_item_table_from_prop(curr_device, curr_item);
                            if (cj_item_properties)
                            {

                                if (!cJSON_AddItemToArray(cj_items_array, cj_item_properties))
                                {
                                    cJSON_Delete(__FUNCTION__, cj_item_properties);
                                }
                            }

                            curr_item = curr_item->next;
                        }

                        curr_device = curr_device->next;
                    }
                }
            }
        }
    }
}

void EZPI_items_set_value_v3(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);

    cJSON *cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
    if (cj_params)
    {
        uint32_t item_id = EZPI_core_cjson_get_id(cj_params, ezlopi__id_str);
        // CJSON_GET_ID(item_id, cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi__id_str));

        TRACE_S("item_id: %X", item_id);

        l_ezlopi_device_t *curr_device = EZPI_core_device_get_head();
        uint32_t found_item = 0;
        while (curr_device)
        {
            l_ezlopi_item_t *curr_item = curr_device->items;
            while (curr_item)
            {
                if (item_id == curr_item->cloud_properties.item_id)
                {
                    curr_item->func(EZLOPI_ACTION_SET_VALUE, curr_item, cj_params, curr_item->user_arg);
                    found_item = 1;
                    break;
                }
                curr_item = curr_item->next;
            }
            if (found_item)
            {
                break;
            }
            curr_device = curr_device->next;
        }
    }
}

static cJSON *ezpi_device_create_item_table_from_prop(l_ezlopi_device_t *device_prop, l_ezlopi_item_t *item_properties)
{
    cJSON *cj_item_properties = cJSON_CreateObject(__FUNCTION__);
    if (cj_item_properties)
    {
        char tmp_string[64];
        snprintf(tmp_string, sizeof(tmp_string), "%08x", item_properties->cloud_properties.item_id);
        cJSON_AddStringToObject(__FUNCTION__, cj_item_properties, ezlopi__id_str, tmp_string);
        snprintf(tmp_string, sizeof(tmp_string), "%08x", item_properties->cloud_properties.device_id);
        cJSON_AddStringToObject(__FUNCTION__, cj_item_properties, ezlopi_deviceId_str, tmp_string);
        // cJSON_AddStringToObject(__FUNCTION__, cj_item_properties, ezlopi_deviceName_str, curr_device->cloud_properties.device_name);
        // cJSON_AddTrueToObject(__FUNCTION__, cj_item_properties, "deviceArmed");
        cJSON_AddBoolToObject(__FUNCTION__, cj_item_properties, ezlopi_hasGetter_str, item_properties->cloud_properties.has_getter);
        cJSON_AddBoolToObject(__FUNCTION__, cj_item_properties, ezlopi_hasSetter_str, item_properties->cloud_properties.has_setter);
        cJSON_AddStringToObject(__FUNCTION__, cj_item_properties, ezlopi_name_str, item_properties->cloud_properties.item_name);
        cJSON_AddTrueToObject(__FUNCTION__, cj_item_properties, ezlopi_show_str);
        cJSON_AddStringToObject(__FUNCTION__, cj_item_properties, ezlopi_valueType_str, item_properties->cloud_properties.value_type);

        if (item_properties->cloud_properties.scale)
        {
            cJSON_AddStringToObject(__FUNCTION__, cj_item_properties, ezlopi_scale_str, item_properties->cloud_properties.scale);
        }
        item_properties->func(EZLOPI_ACTION_HUB_GET_ITEM, item_properties, cj_item_properties, item_properties->user_arg);
        if (item_properties->cloud_properties.scale)
        {
            cJSON_AddStringToObject(__FUNCTION__, cj_item_properties, ezlopi_scale_str, item_properties->cloud_properties.scale);
        }
        cJSON_AddStringToObject(__FUNCTION__, cj_item_properties, ezlopi_status_str, ezlopi_idle_str);
    }

    return cj_item_properties;
}

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
