

/**
 * @file    ezlopi_cloud_devices.c
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

#include <string.h>

#include "mbedtls/sha1.h"

#include "ezlopi_util_trace.h"

#include "ezlopi_core_factory_info.h"
#include "ezlopi_core_devices_list.h"
#include "ezlopi_core_api_methods.h"
#include "ezlopi_core_sntp.h"
#include "ezlopi_core_nvs.h"
#include "ezlopi_core_device_group.h"

#include "ezlopi_cloud_devices.h"
#include "ezlopi_cloud_methods_str.h"
#include "ezlopi_cloud_constants.h"

 //------------------------------------------------------------------------------------------------------------------
 /**
  * @brief Function to generate sha1 for src
  *
  * @param src Pointer to the source of which SHA1 has to be generated
  * @return char*
  * @retval Generated SHA1 else NULL
  */
static char *ezpi_generate_sha1_of_src(const char *src);

//------------------------------------------------------------------------------------------------------------------
void EZPI_devices_list_v3(cJSON *cj_request, cJSON *cj_response)
{
    cJSON *cjson_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    if (cjson_result)
    {
        cJSON *cjson_devices_array = cJSON_AddArrayToObject(__FUNCTION__, cjson_result, ezlopi_devices_str);
        if (cjson_devices_array)
        {
            l_ezlopi_device_t *curr_device = EZPI_core_device_get_head();

            while (NULL != curr_device)
            {
                cJSON *cj_properties = EZPI_core_device_create_device_table_from_prop(curr_device);
                if (cj_properties)
                {
                    if (!cJSON_AddItemToArray(cjson_devices_array, cj_properties))
                    {
                        cJSON_Delete(__FUNCTION__, cj_properties);
                    }
                }

                curr_device = curr_device->next;
            }
        }
#if (1 == ENABLE_TRACE)
        else
        {
            TRACE_E("Failed to create devices-array");
        }
#endif
    }

#if (1 == ENABLE_TRACE)
    else
    {
        TRACE_E("Failed to create 'result'");
    }
#endif
}

void EZPI_device_name_set(cJSON *cj_request, cJSON *cj_response)
{
    cJSON *cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    if (cj_result)
    {
        cJSON *cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
        if (cj_params)
        {
            cJSON *cj_device_id = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi__id_str);
            if (cj_device_id)
            {
                uint32_t device_id = strtoul(cj_device_id->valuestring, NULL, 16);
                cJSON *cj_device_name_str = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_name_str);
                if (device_id && cj_device_name_str->valuestring)
                {
                    EZPI_core_device_name_set_by_device_id(device_id, cj_device_name_str->valuestring);
                }
            }
        }
    }
}

void EZPI_device_armed_set(cJSON *cj_request, cJSON *cj_response)
{
    cJSON *cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    if (cj_result)
    {
        cJSON *cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
        if (cj_params)
        {
            cJSON *cj_device_id = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi__id_str);
            cJSON *cj_armed_status = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_armed_str);
            if (cj_device_id && cj_armed_status)
            {
                uint32_t device_id = strtoul(cj_device_id->valuestring, NULL, 16);
                bool armed_status = cj_armed_status->type == cJSON_True ? true : false;
                EZPI_core_device_set_reset_device_armed_status(device_id, armed_status);
            }
        }
    }
    return;
}

void EZPI_device_room_set(cJSON *cj_request, cJSON *cj_response)
{
    cJSON *cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    if (cj_result)
    {
        cJSON *cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
        if (cj_params)
        {
            cJSON *cj_device_id = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi__id_str);
            cJSON *cj_room_id = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_roomId_str);

            if (cj_device_id && cj_room_id && cj_room_id->valuestring)
            {
                uint32_t device_id = strtoul(cj_device_id->valuestring, NULL, 16);

                EZPI_core_device_set_device_room_id(device_id, cj_room_id->valuestring, cJSON_GetObjectItem(__FUNCTION__, cj_params, "separateChildDevices"));
            }
        }
    }
}

void EZPI_device_updated(cJSON *cj_request, cJSON *cj_response)
{
    if (cj_request)
    {
        cJSON_AddStringToObject(__FUNCTION__, cj_response, ezlopi_id_str, ezlopi_ui_broadcast_str);
        cJSON_AddStringToObject(__FUNCTION__, cj_response, ezlopi_msg_subclass_str, ezlopi_hub_device_updated);

        cJSON *cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
        if (cj_result)
        {
            cJSON *cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
            if (cj_params)
            {
                cJSON *cj_device_id = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi__id_str);
                if (cj_device_id && cj_device_id->valuestring)
                {
                    uint32_t device_id = strtoul(cj_device_id->valuestring, NULL, 16);
                    l_ezlopi_device_t *device_node = EZPI_core_device_get_head();
                    while (device_node)
                    {
                        if (device_id == device_node->cloud_properties.device_id)
                        {
                            char tmp_str[32];
                            snprintf(tmp_str, sizeof(tmp_str), "%08x", device_id);
                            cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi__id_str, tmp_str);
                            cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_name_str, device_node->cloud_properties.device_name);
                            cJSON_AddTrueToObject(__FUNCTION__, cj_result, ezlopi_syncNotification_str);

                            if (device_node->cloud_properties.room_id)
                            {
                                snprintf(tmp_str, sizeof(tmp_str), "%08x", device_node->cloud_properties.room_id);
                                cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_roomId_str, tmp_str);
                            }
                            else
                            {
                                cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_roomId_str, "");
                            }

                            s_ezlopi_cloud_controller_t *controller_info = EZPI_core_device_get_controller_information();

                            if (controller_info)
                            {
                                cJSON_AddBoolToObject(__FUNCTION__, cj_result, ezlopi_armed_str, controller_info->armed ? cJSON_True : cJSON_False);
                                cJSON_AddBoolToObject(__FUNCTION__, cj_result, ezlopi_serviceNotification_str, controller_info->service_notification ? cJSON_True : cJSON_False);
                            }

                            uint64_t time = EZPI_core_sntp_get_current_time_ms();
                            cJSON_AddNumberToObject(__FUNCTION__, cj_result, ezlopi_fwTimestampMs_str, time);

                            cJSON *cj_method = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_method_str);
                            if (cj_method)
                            {
                                cJSON *cj_device_changable = cJSON_AddArrayToObject(__FUNCTION__, cj_result, "__DEVICE_CHANGEABLE_FIELD");
                                if (cj_device_changable)
                                {
                                    cJSON_AddItemToArray(cj_device_changable, cJSON_CreateString(__FUNCTION__, ezlopi_name_str));
                                    cJSON_AddItemToArray(cj_device_changable, cJSON_CreateString(__FUNCTION__, ezlopi_armed_str));
                                    cJSON_AddItemToArray(cj_device_changable, cJSON_CreateString(__FUNCTION__, ezlopi_roomId_str));
                                }
                            }

                            if (device_node->cloud_properties.protect_config && (strlen(device_node->cloud_properties.protect_config) > 0))
                            {
                                cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_protect_config_str, device_node->cloud_properties.protect_config);
                            }
                            break;
                        }

                        device_node = device_node->next;
                    }
                }
            }
        }
    }
}

///////////  device/item group ///////

//---- device_group_api ------

void EZPI_device_group_create(cJSON *cj_request, cJSON *cj_response)
{
    if (cj_request && cj_response)
    {
        cJSON *cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
        if (cj_result)
        {
            cJSON *cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
            if (cj_params)
            {
                uint32_t new_device_grp_id = EZPI_core_device_group_store_nvs_devgrp(cj_params);
                TRACE_D("new-device-grp-id: %08x", new_device_grp_id);

                if (new_device_grp_id)
                {
                    char tmp_buff[32];
                    snprintf(tmp_buff, sizeof(tmp_buff), "%08x", new_device_grp_id);
                    cJSON_AddStringToObject(__FUNCTION__, cj_request, ezlopi__id_str, tmp_buff); // this is for (reply)
                    EZPI_core_device_group_new_devgrp_populate(cj_params, new_device_grp_id);
                }
            }
        }
    }
}

void EZPI_device_group_get(cJSON *cj_request, cJSON *cj_response)
{
    if (cj_request && cj_response)
    {
        cJSON *cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
        if (cj_params)
        {
            cJSON *cj_devgrp_id = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_id_str);
            if (cj_devgrp_id && cj_devgrp_id->valuestring)
            {
                char *devgrp_str = EZPI_core_nvs_read_str(cj_devgrp_id->valuestring);
                if (devgrp_str)
                {
                    cJSON_AddRawToObject(__FUNCTION__, cj_response, ezlopi_result_str, devgrp_str);
                    ezlopi_free(__FUNCTION__, devgrp_str);
                }
                else
                {
                    cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
                }
            }
        }
    }
}

void EZPI_device_groups_list(cJSON *cj_request, cJSON *cj_response)
{
    if (cj_request && cj_response)
    {
        cJSON *cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
        if (cj_result)
        {
            cJSON *cj_device_groups = cJSON_AddArrayToObject(__FUNCTION__, cj_result, ezlopi_deviceGroups_str);
            if (cj_device_groups)
            {
                EZPI_core_device_group_get_list(cj_device_groups);
            }

            // check if the version is identical to the ones from request.
            char *res_str = cJSON_PrintBuffered(__FUNCTION__, cj_result, 1024, false);
            if (res_str)
            {
                char *hash_str = NULL;
                if (NULL != (hash_str = ezpi_generate_sha1_of_src(res_str))) // returns malloc ; need to free
                {
                    // TRACE_S("'hash': %s [%d]", hash_str, strlen(hash_str));
                    cJSON *cj_ver_str = cJSON_GetObjectItem(__FUNCTION__, (cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str)), ezlopi_version_str);
                    if (cj_ver_str && cj_ver_str->valuestring && cj_ver_str->str_value_len)
                    {
                        // TRACE_D("'req_version': '%s'[%d]", cj_ver_str->valuestring, strlen(cj_ver_str->valuestring));
                        if (EZPI_STRNCMP_IF_EQUAL(hash_str, cj_ver_str->valuestring, strlen(hash_str) + 1, cj_ver_str->str_value_len))
                        {
                            cJSON_DeleteItemFromObject(__FUNCTION__, cj_result, ezlopi_deviceGroups_str);
                        }
                    }
                    // now add the 'version_hash' into result.
                    cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_version_str, hash_str);

                    ezlopi_free(__FUNCTION__, hash_str);
                }
                ezlopi_free(__FUNCTION__, res_str);
            }
        }
    }
}

void EZPI_device_group_delete(cJSON *cj_request, cJSON *cj_response)
{
    if (cj_request && cj_response)
    {
        cJSON *cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
        if (cj_result)
        {
            cJSON *cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
            if (cj_params)
            {
                cJSON *cj_id = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_id_str);
                if (cj_id && cj_id->valuestring)
                {
                    uint32_t dev_grp_id = strtoul(cj_id->valuestring, NULL, 16);

                    // 1. check if 'dev_grp' is persistant
                    l_ezlopi_device_grp_t *curr_devgrp = EZPI_core_device_group_get_by_id(dev_grp_id);
                    if (curr_devgrp)
                    {
                        if (curr_devgrp->persistent)
                        { // if 'true'
                            cJSON *cj_force = cJSON_GetObjectItem(__FUNCTION__, cj_params, value_type_force);
                            if (cj_force && cJSON_IsTrue(cj_force))
                            {
                                EZPI_core_nvs_delete_stored_data_by_id(dev_grp_id);          // from nvs
                                EZPI_core_device_group_depopulate_by_id_v2(dev_grp_id); // from ll
                                EZPI_core_device_group_remove_id_from_list(dev_grp_id); // from nvs-list

                                cJSON_AddStringToObject(__FUNCTION__, cj_request, ezlopi__id_str, cj_id->valuestring); // for (reply_broadcast); if delete is successful
                            }
                            else
                            {
                                TRACE_E("Error!! , Unable to delete 'device_group_id' : [%d]", dev_grp_id);
                            }
                        }
                        else
                        {
                            EZPI_core_nvs_delete_stored_data_by_id(dev_grp_id);
                            EZPI_core_device_group_depopulate_by_id_v2(dev_grp_id);
                            EZPI_core_device_group_remove_id_from_list(dev_grp_id);
                        }
                    }
                }
            }
        }
    }
}

void EZPI_device_group_update(cJSON *cj_request, cJSON *cj_response)
{
    if (cj_request && cj_response)
    {
        cJSON *cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
        if (cj_result)
        {
            cJSON *cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
            if (cj_params)
            {
                cJSON *cj_devgrp_id = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_id_str); // "id"
                if (cj_devgrp_id && cj_devgrp_id->valuestring)
                {
                    // CJSON_TRACE("dev-grp [new] : ", cj_devgrp_id);
                    uint32_t req_devgrp_id = strtoul(cj_devgrp_id->valuestring, NULL, 16);

                    // edit in nvs and populate again
                    EZPI_core_device_group_edit_by_id(req_devgrp_id, cj_params);
                }
            }
        }
    }
}

void EZPI_device_group_find(cJSON *cj_request, cJSON *cj_response)
{
    if (cj_request && cj_response)
    {
        cJSON *cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
        if (cj_result)
        {
            cJSON *cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
            if (cj_params)
            {
                EZPI_core_device_group_find(cJSON_AddArrayToObject(__FUNCTION__, cj_result, ezlopi_deviceGroups_str), cj_params);
            }
        }
    }
}

void EZPI_device_group_devitem_expand(cJSON *cj_request, cJSON *cj_response)
{
    if (cj_request && cj_response)
    {
        cJSON *cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
        if (cj_result)
        {
            cJSON *cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
            if (cj_params)
            {
                cJSON *cj_devices = cJSON_AddArrayToObject(__FUNCTION__, cj_result, ezlopi_devices_str);
                if (cj_devices)
                {
                    EZPI_core_device_group_devitem_expand(cj_devices, cj_params);
                }

                // check if the version is identical to the ones from request.
                char *res_str = cJSON_PrintBuffered(__FUNCTION__, cj_result, 1024, false);
                if (res_str)
                {
                    char *hash_str = NULL;
                    if (NULL != (hash_str = ezpi_generate_sha1_of_src(res_str))) // returns malloc ; need to free
                    {
                        // TRACE_S("'hash': %s [%d]", hash_str, strlen(hash_str));
                        cJSON *cj_ver_str = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_version_str);
                        if (cj_ver_str && cj_ver_str->valuestring && cj_ver_str->str_value_len)
                        {
                            // TRACE_D("'version': %s [%d]", cj_ver_str->valuestring, cj_ver_str->str_value_len);
                            if (EZPI_STRNCMP_IF_EQUAL(hash_str, cj_ver_str->valuestring, strlen(hash_str) + 1, cj_ver_str->str_value_len))
                            {
                                cJSON_DeleteItemFromObject(__FUNCTION__, cj_result, ezlopi_devices_str);
                            }
                        }
                        // now add the 'version_hash' into result.
                        cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_version_str, hash_str);

                        ezlopi_free(__FUNCTION__, hash_str);
                    }
                    ezlopi_free(__FUNCTION__, res_str);
                }
            }
        }
    }
}

////// updater for device-grps
////// for 'hub.device.group.__'

void EZPI_device_group_created(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_DeleteItemFromObject(__FUNCTION__, cj_response, ezlopi_sender_str);
    cJSON_DeleteItemFromObject(__FUNCTION__, cj_response, ezlopi_error_str);

    cJSON_AddStringToObject(__FUNCTION__, cj_response, ezlopi_id_str, ezlopi_ui_broadcast_str);
    cJSON_AddStringToObject(__FUNCTION__, cj_response, ezlopi_msg_subclass_str, ezlopi_hub_device_group_created);

    cJSON *new_devgrp_id = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi__id_str);
    if (new_devgrp_id && new_devgrp_id->valuestring)
    {
        char *new_devgrp = EZPI_core_nvs_read_str(new_devgrp_id->valuestring);
        if (new_devgrp)
        {
            cJSON_AddRawToObject(__FUNCTION__, cj_response, ezlopi_result_str, new_devgrp);
            ezlopi_free(__FUNCTION__, new_devgrp);
        }
    }
}

void EZPI_device_group_deleted(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_DeleteItemFromObject(__FUNCTION__, cj_response, ezlopi_sender_str);
    cJSON_DeleteItemFromObject(__FUNCTION__, cj_response, ezlopi_error_str);

    cJSON_AddStringToObject(__FUNCTION__, cj_response, ezlopi_id_str, ezlopi_ui_broadcast_str);
    cJSON_AddStringToObject(__FUNCTION__, cj_response, ezlopi_msg_subclass_str, ezlopi_hub_device_group_deleted);

    cJSON *cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    if (cj_result)
    {
        cJSON *deleted_devgrp_id = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi__id_str); // if delete was successful
        if (deleted_devgrp_id && deleted_devgrp_id->valuestring)
        {
            cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi__id_str, deleted_devgrp_id->valuestring);
        }
    }
}

void EZPI_device_group_updated(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_DeleteItemFromObject(__FUNCTION__, cj_response, ezlopi_sender_str);
    cJSON_DeleteItemFromObject(__FUNCTION__, cj_response, ezlopi_error_str);

    cJSON_AddStringToObject(__FUNCTION__, cj_response, ezlopi_id_str, ezlopi_ui_broadcast_str);
    cJSON_AddStringToObject(__FUNCTION__, cj_response, ezlopi_msg_subclass_str, ezlopi_hub_device_group_updated);

    cJSON *cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
    if (cj_params)
    {
        cJSON *cj_devgrp_id = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_id_str);
        if (cj_devgrp_id && cj_devgrp_id->valuestring)
        {
            char *devgrp_str = EZPI_core_nvs_read_str(cj_devgrp_id->valuestring);
            if (devgrp_str)
            {
                cJSON_AddRawToObject(__FUNCTION__, cj_response, ezlopi_result_str, devgrp_str);
                ezlopi_free(__FUNCTION__, devgrp_str);
            }
        }
    }
}

//---- item_group_api ------

void EZPI_item_group_create(cJSON *cj_request, cJSON *cj_response)
{
    if (cj_request && cj_response)
    {
        cJSON *cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
        if (cj_result)
        {
            cJSON *cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
            if (cj_params)
            {
                uint32_t new_item_grp_id = EZPI_core_item_group_store_nvs_itemgrp(cj_params);
                TRACE_D("new-item-grp-id: %08x", new_item_grp_id);

                if (new_item_grp_id)
                {
                    char tmp_buff[32];
                    snprintf(tmp_buff, sizeof(tmp_buff), "%08x", new_item_grp_id);
                    cJSON_AddStringToObject(__FUNCTION__, cj_request, ezlopi__id_str, tmp_buff); // this is for (reply)
                    EZPI_core_item_group_new_itemgrp_populate(cj_params, new_item_grp_id);
                }
            }
        }
    }
}

void EZPI_item_group_get(cJSON *cj_request, cJSON *cj_response)
{
    if (cj_request && cj_response)
    {
        cJSON *cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
        if (cj_params)
        {
            cJSON *cj_itemgrp_id = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_id_str);
            if (cj_itemgrp_id && cj_itemgrp_id->valuestring)
            {
                char *devgrp_str = EZPI_core_nvs_read_str(cj_itemgrp_id->valuestring);
                if (devgrp_str)
                {
                    cJSON_AddRawToObject(__FUNCTION__, cj_response, ezlopi_result_str, devgrp_str);
                    ezlopi_free(__FUNCTION__, devgrp_str);
                }
                else
                {
                    cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
                }
            }
        }
    }
}

void EZPI_item_groups_list(cJSON *cj_request, cJSON *cj_response)
{
    if (cj_request && cj_response)
    {
        cJSON *cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
        if (cj_result)
        {
            cJSON *cj_item_groups = cJSON_AddArrayToObject(__FUNCTION__, cj_result, ezlopi_itemGroup_str);
            if (cj_item_groups)
            {
                EZPI_core_item_group_get_list(cj_item_groups);
            }

            // check if the version is identical to the ones from request.
            char *res_str = cJSON_PrintBuffered(__FUNCTION__, cj_result, 1024, false);
            if (res_str)
            {
                char *hash_str = NULL;
                if (NULL != (hash_str = ezpi_generate_sha1_of_src(res_str))) // returns malloc ; need to free
                {
                    // TRACE_S("'hash': %s [%d]", hash_str, strlen(hash_str));
                    cJSON *cj_ver_str = cJSON_GetObjectItem(__FUNCTION__, (cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str)), ezlopi_version_str);
                    if (cj_ver_str && cj_ver_str->valuestring && cj_ver_str->str_value_len)
                    {
                        // TRACE_D("'version': %s [%d]", cj_ver_str->valuestring, cj_ver_str->str_value_len);
                        if (EZPI_STRNCMP_IF_EQUAL(hash_str, cj_ver_str->valuestring, strlen(hash_str) + 1, cj_ver_str->str_value_len))
                        {
                            cJSON_DeleteItemFromObject(__FUNCTION__, cj_result, ezlopi_itemGroup_str);
                        }
                    }
                    // now add the 'version_hash' into result.
                    cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_version_str, hash_str);

                    ezlopi_free(__FUNCTION__, hash_str);
                }
                ezlopi_free(__FUNCTION__, res_str);
            }
        }
    }
}

void EZPI_item_group_delete(cJSON *cj_request, cJSON *cj_response)
{
    if (cj_request && cj_response)
    {
        cJSON *cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
        if (cj_result)
        {
            cJSON *cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
            if (cj_params)
            {
                cJSON *cj_id = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_id_str);
                if (cj_id && cj_id->valuestring)
                {
                    uint32_t item_grp_id = strtoul(cj_id->valuestring, NULL, 16);

                    // 1. check if 'item_grp' is persistant
                    l_ezlopi_item_grp_t *curr_itemgrp = EZPI_core_item_group_get_by_id(item_grp_id);
                    if (curr_itemgrp)
                    {
                        if (curr_itemgrp->persistent)
                        { // if 'true'
                            cJSON *cj_force = cJSON_GetObjectItem(__FUNCTION__, cj_params, value_type_force);
                            if (cj_force && cJSON_IsTrue(cj_force))
                            {
                                EZPI_core_nvs_delete_stored_data_by_id(item_grp_id);        // from nvs
                                EZPI_core_item_group_depopulate_by_id_v2(item_grp_id); // from ll
                                EZPI_core_item_group_remove_id_from_list(item_grp_id); // from nvs-list

                                cJSON_AddStringToObject(__FUNCTION__, cj_request, ezlopi__id_str, cj_id->valuestring); // for (reply_broadcast); if delete is successful
                            }
                            else
                            {
                                TRACE_E("Error!! , Unable to delete 'item_group_id' : [%d]", item_grp_id);
                            }
                        }
                        else
                        {
                            EZPI_core_nvs_delete_stored_data_by_id(item_grp_id);
                            EZPI_core_item_group_depopulate_by_id_v2(item_grp_id);
                            EZPI_core_item_group_remove_id_from_list(item_grp_id);
                        }
                    }
                }
            }
        }
    }
}

void EZPI_item_group_update(cJSON *cj_request, cJSON *cj_response)
{
    if (cj_request && cj_response)
    {
        cJSON *cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
        if (cj_result)
        {
            cJSON *cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
            if (cj_params)
            {
                cJSON *cj_itemgrp_id = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_id_str);
                if (cj_itemgrp_id && cj_itemgrp_id->valuestring)
                {
                    // CJSON_TRACE("dev-grp [new] : ", cj_itemgrp_id);
                    uint32_t req_devgrp_id = strtoul(cj_itemgrp_id->valuestring, NULL, 16);

                    // edit in nvs and populate again
                    EZPI_core_item_group_edit_by_id(req_devgrp_id, cj_params);
                }
            }
        }
    }
}

////// updater for item-grps
////// for 'hub.item.group.__'

void EZPI_item_group_created(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_DeleteItemFromObject(__FUNCTION__, cj_response, ezlopi_sender_str);
    cJSON_DeleteItemFromObject(__FUNCTION__, cj_response, ezlopi_error_str);

    cJSON_AddStringToObject(__FUNCTION__, cj_response, ezlopi_id_str, ezlopi_ui_broadcast_str);
    cJSON_AddStringToObject(__FUNCTION__, cj_response, ezlopi_msg_subclass_str, ezlopi_hub_item_group_created);

    cJSON *new_itemgrp_id = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi__id_str);
    if (new_itemgrp_id && new_itemgrp_id->valuestring)
    {
        char *new_itemgrp = EZPI_core_nvs_read_str(new_itemgrp_id->valuestring);
        if (new_itemgrp)
        {
            cJSON_AddRawToObject(__FUNCTION__, cj_response, ezlopi_result_str, new_itemgrp);
            ezlopi_free(__FUNCTION__, new_itemgrp);
        }
    }
}

void EZPI_item_group_deleted(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_DeleteItemFromObject(__FUNCTION__, cj_response, ezlopi_sender_str);
    cJSON_DeleteItemFromObject(__FUNCTION__, cj_response, ezlopi_error_str);

    cJSON_AddStringToObject(__FUNCTION__, cj_response, ezlopi_id_str, ezlopi_ui_broadcast_str);
    cJSON_AddStringToObject(__FUNCTION__, cj_response, ezlopi_msg_subclass_str, ezlopi_hub_item_group_deleted);

    cJSON *cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    if (cj_result)
    {
        cJSON *deleted_itemgrp_id = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi__id_str); // if delete was successful
        if (deleted_itemgrp_id && deleted_itemgrp_id->valuestring)
        {
            cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi__id_str, deleted_itemgrp_id->valuestring);
        }
    }
}

void EZPI_item_group_updated(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_DeleteItemFromObject(__FUNCTION__, cj_response, ezlopi_sender_str);
    cJSON_DeleteItemFromObject(__FUNCTION__, cj_response, ezlopi_error_str);

    cJSON_AddStringToObject(__FUNCTION__, cj_response, ezlopi_id_str, ezlopi_ui_broadcast_str);
    cJSON_AddStringToObject(__FUNCTION__, cj_response, ezlopi_msg_subclass_str, ezlopi_hub_item_group_updated);

    cJSON *cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
    if (cj_params)
    {
        cJSON *cj_itemgrp_id = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_id_str);
        if (cj_itemgrp_id && cj_itemgrp_id->valuestring)
        {
            char *devgrp_str = EZPI_core_nvs_read_str(cj_itemgrp_id->valuestring);
            if (devgrp_str)
            {
                cJSON_AddRawToObject(__FUNCTION__, cj_response, ezlopi_result_str, devgrp_str);
                ezlopi_free(__FUNCTION__, devgrp_str);
            }
        }
    }
}

static char *ezpi_generate_sha1_of_src(const char *src)
{
    char *ret = NULL;
    if (src)
    {
        if (!mbedtls_sha1_self_test(1))
        {
            unsigned char sha1_hash[20];

            if (0 == mbedtls_sha1_ret((unsigned char *)src, strlen(src), sha1_hash))
            {
                ret = (char *)ezlopi_malloc(__FUNCTION__, sizeof(unsigned char) * 20);
                if (ret)
                {
                    for (size_t idx = 0; idx < sizeof(sha1_hash); idx++)
                    {
                        snprintf(ret + strlen(ret), (sizeof(ret) - (strlen(ret))), "%02x", sha1_hash[idx]);
                    }
                    TRACE_D(" sha1_hash = %s ;  => ret = %s", sha1_hash, ret);
                }
            }

            // mbedtls_sha1_context sha1_ctx;
            // mbedtls_sha1_init(&sha1_ctx);
            // if (0 == mbedtls_sha1_starts_ret(&sha1_ctx))
            // {
            //     if (0 == mbedtls_sha1_update_ret(&sha1_ctx, (const unsigned char *)src, strlen(src)))
            //     {
            //         if (0 == mbedtls_sha1_finish_ret(&sha1_ctx, sha1))
            //         {
            //             size_t len = (4 * sizeof(sha1)) + 1;
            //             ret = (char *)ezlopi_malloc(__FUNCTION__, len);
            //             if (ret)
            //             {
            //                 memset(ret, 0, len);
            //                 for (int i = 0; i < sizeof(sha1); i++)
            //                 {
            //                     size_t l = (len - (strlen(ret) + 1));
            //                     if (l > 0)
            //                     {
            //                         ((int)sha1[i] / 100 > 0)  ? (snprintf(ret + strlen(ret), l, "%u", (uint8_t)sha1[i]))    // tripple digit
            //                         : ((int)sha1[i] / 10 > 0) ? (snprintf(ret + strlen(ret), l, "0%u", (uint8_t)sha1[i]))   // double digit
            //                                                   : (snprintf(ret + strlen(ret), l, "00%u", (uint8_t)sha1[i])); // single digit
            //                     }
            //                     else
            //                     {
            //                         break;
            //                     }
            //                 }
            //             }
            //         }
            //     }
            // }

            // mbedtls_sha1_free(&sha1_ctx);
        }
    }
    return ret;
}

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
