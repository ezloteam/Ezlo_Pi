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
#include "ezlopi_cloud_keywords.h"
#include "ezlopi_cloud_methods_str.h"
#include "ezlopi_cloud_constants.h"
//------------------------------------------------------------------------------------------------------------------
static char *__generate_sha1_of_src(const char *src)
{
    char *ret = NULL;
    if (src)
    {
        if (!mbedtls_sha1_self_test(1))
        {
            unsigned char sha1[20];
            mbedtls_sha1_context sha1_ctx;

            mbedtls_sha1_init(&sha1_ctx);
            if (mbedtls_sha1_starts_ret(&sha1_ctx))
            {
                if (mbedtls_sha1_update_ret(&sha1_ctx, (const unsigned char *)src, strlen(src)))
                {
                    if (mbedtls_sha1_finish_ret(&sha1_ctx, sha1))
                    {
                        if (NULL != (ret = ezlopi_malloc(__FUNCTION__, sizeof(sha1) + 1))) // 20 + 1
                        {
                            memset(ret, 0, sizeof(sha1));
                            snprintf(ret, sizeof(sha1), "%s", sha1);
                        }
                    }
                }
            }
            mbedtls_sha1_free(&sha1_ctx);
        }
    }
    return ret;
}
//------------------------------------------------------------------------------------------------------------------
void devices_list_v3(cJSON *cj_request, cJSON *cj_response)
{
    cJSON *cjson_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    if (cjson_result)
    {
        cJSON *cjson_devices_array = cJSON_AddArrayToObject(__FUNCTION__, cjson_result, "devices");
        if (cjson_devices_array)
        {
            l_ezlopi_device_t *curr_device = ezlopi_device_get_head();

            while (NULL != curr_device)
            {
                cJSON *cj_properties = ezlopi_device_create_device_table_from_prop(curr_device);
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

void device_name_set(cJSON *cj_request, cJSON *cj_response)
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
                    ezlopi_device_name_set_by_device_id(device_id, cj_device_name_str->valuestring);
                }
            }
        }
    }
}

void device_armed_set(cJSON *cj_request, cJSON *cj_response)
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
                ezlopi_device_set_reset_device_armed_status(device_id, armed_status);
            }
        }
    }
    return;
}

void device_room_set(cJSON *cj_request, cJSON *cj_response)
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

                ezlopi_device_set_device_room_id(device_id, cj_room_id->valuestring, cJSON_GetObjectItem(__FUNCTION__, cj_params, "separateChildDevices"));
            }
        }
    }
}

void device_updated(cJSON *cj_request, cJSON *cj_response)
{
    if (cj_request)
    {
        cJSON_AddStringToObject(__FUNCTION__, cj_response, ezlopi_id_str, ezlopi_ui_broadcast_str);
        cJSON_AddStringToObject(__FUNCTION__, cj_response, ezlopi_msg_subclass_str, "hub.device.updated");

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
                    l_ezlopi_device_t *device_node = ezlopi_device_get_head();
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

                            s_ezlopi_cloud_controller_t *controller_info = ezlopi_device_get_controller_information();

                            if (controller_info)
                            {
                                cJSON_AddBoolToObject(__FUNCTION__, cj_result, ezlopi_armed_str, controller_info->armed ? cJSON_True : cJSON_False);
                                cJSON_AddBoolToObject(__FUNCTION__, cj_result, ezlopi_serviceNotification_str, controller_info->service_notification ? cJSON_True : cJSON_False);
                            }

                            uint64_t time = EZPI_CORE_sntp_get_current_time_ms();
                            cJSON_AddNumberToObject(__FUNCTION__, cj_result, "fwTimestampMs", time);

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

void device_group_create(cJSON *cj_request, cJSON *cj_response)
{
    if (cj_request && cj_response)
    {
        cJSON *cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
        if (cj_result)
        {
            cJSON *cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
            if (cj_params)
            {
                uint32_t new_device_grp_id = ezlopi_core_device_group_store_nvs_devgrp(cj_params);
                TRACE_D("new-device-grp-id: %08x", new_device_grp_id);

                if (new_device_grp_id)
                {
                    char tmp_buff[32];
                    snprintf(tmp_buff, sizeof(tmp_buff), "%08x", new_device_grp_id);
                    cJSON_AddStringToObject(__FUNCTION__, cj_request, ezlopi__id_str, tmp_buff); // this is for (reply)
                    ezlopi_core_device_group_new_devgrp_populate(cj_params, new_device_grp_id);
                }
            }
        }
    }
}

void device_group_get(cJSON *cj_request, cJSON *cj_response)
{
    if (cj_request && cj_response)
    {
        cJSON *cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
        if (cj_params)
        {
            cJSON *cj_devgrp_id = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_id_str);
            if (cj_devgrp_id && cj_devgrp_id->valuestring)
            {
                char *devgrp_str = ezlopi_nvs_read_str(cj_devgrp_id->valuestring);
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

void device_groups_list(cJSON *cj_request, cJSON *cj_response)
{
    if (cj_request && cj_response)
    {
        cJSON *cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
        if (cj_result)
        {
            cJSON *cj_device_groups = cJSON_AddArrayToObject(__FUNCTION__, cj_result, "deviceGroups");
            if (cj_device_groups)
            {
                ezlopi_core_device_group_get_list(cj_device_groups);
            }

            // check if the version is identical to the ones from request.
            cJSON *cj_ver_str = cJSON_GetObjectItem(__FUNCTION__, cj_request, "version");
            if (cj_ver_str && cj_ver_str->valuestring && cj_ver_str->str_value_len)
            {
                char *res_str = cJSON_PrintBuffered(__FUNCTION__, cj_result, 1024, false);
                if (res_str)
                {

                    char *hash_str = NULL;
                    if (NULL != (hash_str = __generate_sha1_of_src(res_str))) // returns malloc ; need to free
                    {
                        TRACE_D("'version': %s [%d]", cj_ver_str->valuestring, cj_ver_str->str_value_len);
                        TRACE_S("'hash': %s [%d]", res_str, strlen(res_str));

                        // remove the 'deviceGroups' array , if the generated 'version_hash' is identical
                        if (EZPI_STRNCMP_IF_EQUAL(hash_str, cj_ver_str->valuestring, strlen(hash_str), cj_ver_str->str_value_len))
                        {
                            cJSON_DeleteItemFromObject(__FUNCTION__, cj_result, "deviceGroups");
                        }

                        // now add the 'version_hash' into result.
                        cJSON_AddStringToObject(__FUNCTION__, cj_result, "version", cj_ver_str->valuestring);

                        ezlopi_free(__FUNCTION__, hash_str);
                    }
                    ezlopi_free(__FUNCTION__, res_str);
                }
            }
        }
    }
}

void device_group_delete(cJSON *cj_request, cJSON *cj_response)
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
                    l_ezlopi_device_grp_t *curr_devgrp = ezlopi_core_device_group_get_by_id(dev_grp_id);
                    if (curr_devgrp)
                    {
                        if (curr_devgrp->persistent)
                        { // if 'true'
                            cJSON *cj_force = cJSON_GetObjectItem(__FUNCTION__, cj_params, "force");
                            if (cj_force && cJSON_IsTrue(cj_force))
                            {
                                ezlopi_nvs_delete_stored_data_by_id(dev_grp_id);          // from nvs
                                ezlopi_core_device_group_depopulate_by_id_v2(dev_grp_id); // from ll
                                ezlopi_core_device_group_remove_id_from_list(dev_grp_id); // from nvs-list

                                cJSON_AddStringToObject(__FUNCTION__, cj_request, ezlopi__id_str, cj_id->valuestring); // for (reply_broadcast); if delete is successful
                            }
                            else
                            {
                                TRACE_E("Error!! , Unable to delete 'device_group_id' : [%d]", dev_grp_id);
                            }
                        }
                        else
                        {
                            ezlopi_nvs_delete_stored_data_by_id(dev_grp_id);
                            ezlopi_core_device_group_depopulate_by_id_v2(dev_grp_id);
                            ezlopi_core_device_group_remove_id_from_list(dev_grp_id);
                        }
                    }
                }
            }
        }
    }
}

void device_group_update(cJSON *cj_request, cJSON *cj_response)
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
                    ezlopi_core_device_group_edit_by_id(req_devgrp_id, cj_params);
                }
            }
        }
    }
}

void device_group_find(cJSON *cj_request, cJSON *cj_response)
{
    if (cj_request && cj_response)
    {
        cJSON *cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
        if (cj_result)
        {
            cJSON *cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
            if (cj_params)
            {
                ezlopi_core_device_group_find(cJSON_AddArrayToObject(__FUNCTION__, cj_result, "deviceGroups"), cj_params);
            }
        }
    }
}

void device_group_devitem_expand(cJSON *cj_request, cJSON *cj_response)
{
    if (cj_request && cj_response)
    {
        cJSON *cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
        if (cj_result)
        {
            cJSON *cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
            if (cj_params)
            {
                cJSON *cj_devices = cJSON_AddArrayToObject(__FUNCTION__, cj_result, "devices");
                if (cj_devices)
                {
                    ezlopi_core_device_group_devitem_expand(cj_devices, cj_params);
                }

                // check if the version is identical to the ones from request.
                cJSON *cj_ver_str = cJSON_GetObjectItem(__FUNCTION__, cj_request, "version");
                if (cj_ver_str && cj_ver_str->valuestring && cj_ver_str->str_value_len)
                {
                    char *res_str = cJSON_PrintBuffered(__FUNCTION__, cj_result, 1024, false);
                    if (res_str)
                    {
                        char *hash_str = NULL;
                        if (NULL != (hash_str = __generate_sha1_of_src(res_str))) // returns malloc ; need to free
                        {
                            TRACE_D("'version': %s [%d]", cj_ver_str->valuestring, cj_ver_str->str_value_len);
                            TRACE_S("'hash': %s [%d]", res_str, strlen(res_str));

                            // remove the 'deviceGroups' array , if the generated 'version_hash' is identical
                            if (EZPI_STRNCMP_IF_EQUAL(hash_str, cj_ver_str->valuestring, strlen(hash_str), cj_ver_str->str_value_len))
                            {
                                cJSON_DeleteItemFromObject(__FUNCTION__, cj_result, "devices");
                            }

                            // now add the 'version_hash' into result.
                            cJSON_AddStringToObject(__FUNCTION__, cj_result, "version", cj_ver_str->valuestring);

                            ezlopi_free(__FUNCTION__, hash_str);
                        }
                        ezlopi_free(__FUNCTION__, res_str);
                    }
                }
            }
        }
    }
}

////// updater for device-grps
////// for 'hub.device.group.__'

void device_group_created(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_DeleteItemFromObject(__FUNCTION__, cj_response, ezlopi_sender_str);
    cJSON_DeleteItemFromObject(__FUNCTION__, cj_response, ezlopi_error_str);

    cJSON_AddStringToObject(__FUNCTION__, cj_response, ezlopi_id_str, ezlopi_ui_broadcast_str);
    cJSON_AddStringToObject(__FUNCTION__, cj_response, ezlopi_msg_subclass_str, ezlopi_hub_device_group_created);

    cJSON *new_devgrp_id = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi__id_str);
    if (new_devgrp_id && new_devgrp_id->valuestring)
    {
        char *new_devgrp = ezlopi_nvs_read_str(new_devgrp_id->valuestring);
        if (new_devgrp)
        {
            cJSON_AddRawToObject(__FUNCTION__, cj_response, ezlopi_result_str, new_devgrp);
            ezlopi_free(__FUNCTION__, new_devgrp);
        }
    }
}

void device_group_deleted(cJSON *cj_request, cJSON *cj_response)
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

void device_group_updated(cJSON *cj_request, cJSON *cj_response)
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
            char *devgrp_str = ezlopi_nvs_read_str(cj_devgrp_id->valuestring);
            if (devgrp_str)
            {
                cJSON_AddRawToObject(__FUNCTION__, cj_response, ezlopi_result_str, devgrp_str);
                ezlopi_free(__FUNCTION__, devgrp_str);
            }
        }
    }
}

//---- item_group_api ------

void item_group_create(cJSON *cj_request, cJSON *cj_response)
{
    if (cj_request && cj_response)
    {
        cJSON *cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
        if (cj_result)
        {
            cJSON *cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
            if (cj_params)
            {
                uint32_t new_item_grp_id = ezlopi_core_item_group_store_nvs_itemgrp(cj_params);
                TRACE_D("new-item-grp-id: %08x", new_item_grp_id);

                if (new_item_grp_id)
                {
                    char tmp_buff[32];
                    snprintf(tmp_buff, sizeof(tmp_buff), "%08x", new_item_grp_id);
                    cJSON_AddStringToObject(__FUNCTION__, cj_request, ezlopi__id_str, tmp_buff); // this is for (reply)
                    ezlopi_core_item_group_new_itemgrp_populate(cj_params, new_item_grp_id);
                }
            }
        }
    }
}

void item_group_get(cJSON *cj_request, cJSON *cj_response)
{
    if (cj_request && cj_response)
    {
        cJSON *cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
        if (cj_params)
        {
            cJSON *cj_itemgrp_id = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_id_str);
            if (cj_itemgrp_id && cj_itemgrp_id->valuestring)
            {
                char *devgrp_str = ezlopi_nvs_read_str(cj_itemgrp_id->valuestring);
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

void item_groups_list(cJSON *cj_request, cJSON *cj_response)
{
    if (cj_request && cj_response)
    {
        cJSON *cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
        if (cj_result)
        {
            cJSON *cj_item_groups = cJSON_AddArrayToObject(__FUNCTION__, cj_result, "itemGroups");
            if (cj_item_groups)
            {
                ezlopi_core_item_group_get_list(cj_item_groups);
            }
            // check if the version is identical to the ones from request.
            cJSON *cj_ver_str = cJSON_GetObjectItem(__FUNCTION__, cj_request, "version");
            if (cj_ver_str && cj_ver_str->valuestring && cj_ver_str->str_value_len)
            {
                char *res_str = cJSON_PrintBuffered(__FUNCTION__, cj_result, 1024, false);
                if (res_str)
                {
                    char *hash_str = NULL;
                    if (NULL != (hash_str = __generate_sha1_of_src(res_str))) // returns malloc ; need to free
                    {
                        TRACE_D("'version': %s [%d]", cj_ver_str->valuestring, cj_ver_str->str_value_len);
                        TRACE_S("'hash': %s [%d]", res_str, strlen(res_str));

                        // remove the 'deviceGroups' array , if the generated 'version_hash' is identical
                        if (EZPI_STRNCMP_IF_EQUAL(hash_str, cj_ver_str->valuestring, strlen(hash_str), cj_ver_str->str_value_len))
                        {
                            cJSON_DeleteItemFromObject(__FUNCTION__, cj_result, "itemGroups");
                        }

                        // now add the 'version_hash' into result.
                        cJSON_AddStringToObject(__FUNCTION__, cj_result, "version", cj_ver_str->valuestring);

                        ezlopi_free(__FUNCTION__, hash_str);
                    }
                    ezlopi_free(__FUNCTION__, res_str);
                }
            }
        }
    }
}

void item_group_delete(cJSON *cj_request, cJSON *cj_response)
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
                    l_ezlopi_item_grp_t *curr_itemgrp = ezlopi_core_item_group_get_by_id(item_grp_id);
                    if (curr_itemgrp)
                    {
                        if (curr_itemgrp->persistent)
                        { // if 'true'
                            cJSON *cj_force = cJSON_GetObjectItem(__FUNCTION__, cj_params, "force");
                            if (cj_force && cJSON_IsTrue(cj_force))
                            {
                                ezlopi_nvs_delete_stored_data_by_id(item_grp_id);        // from nvs
                                ezlopi_core_item_group_depopulate_by_id_v2(item_grp_id); // from ll
                                ezlopi_core_item_group_remove_id_from_list(item_grp_id); // from nvs-list

                                cJSON_AddStringToObject(__FUNCTION__, cj_request, ezlopi__id_str, cj_id->valuestring); // for (reply_broadcast); if delete is successful
                            }
                            else
                            {
                                TRACE_E("Error!! , Unable to delete 'item_group_id' : [%d]", item_grp_id);
                            }
                        }
                        else
                        {
                            ezlopi_nvs_delete_stored_data_by_id(item_grp_id);
                            ezlopi_core_item_group_depopulate_by_id_v2(item_grp_id);
                            ezlopi_core_item_group_remove_id_from_list(item_grp_id);
                        }
                    }
                }
            }
        }
    }
}

void item_group_update(cJSON *cj_request, cJSON *cj_response)
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
                    ezlopi_core_item_group_edit_by_id(req_devgrp_id, cj_params);
                }
            }
        }
    }
}

////// updater for item-grps
////// for 'hub.item.group.__'

void item_group_created(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_DeleteItemFromObject(__FUNCTION__, cj_response, ezlopi_sender_str);
    cJSON_DeleteItemFromObject(__FUNCTION__, cj_response, ezlopi_error_str);

    cJSON_AddStringToObject(__FUNCTION__, cj_response, ezlopi_id_str, ezlopi_ui_broadcast_str);
    cJSON_AddStringToObject(__FUNCTION__, cj_response, ezlopi_msg_subclass_str, ezlopi_hub_item_group_created);

    cJSON *new_itemgrp_id = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi__id_str);
    if (new_itemgrp_id && new_itemgrp_id->valuestring)
    {
        char *new_itemgrp = ezlopi_nvs_read_str(new_itemgrp_id->valuestring);
        if (new_itemgrp)
        {
            cJSON_AddRawToObject(__FUNCTION__, cj_response, ezlopi_result_str, new_itemgrp);
            ezlopi_free(__FUNCTION__, new_itemgrp);
        }
    }
}

void item_group_deleted(cJSON *cj_request, cJSON *cj_response)
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

void item_group_updated(cJSON *cj_request, cJSON *cj_response)
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
            char *devgrp_str = ezlopi_nvs_read_str(cj_itemgrp_id->valuestring);
            if (devgrp_str)
            {
                cJSON_AddRawToObject(__FUNCTION__, cj_response, ezlopi_result_str, devgrp_str);
                ezlopi_free(__FUNCTION__, devgrp_str);
            }
        }
    }
}
