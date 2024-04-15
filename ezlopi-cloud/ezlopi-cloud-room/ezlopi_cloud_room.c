#include <string.h>
#include "ezlopi_util_trace.h"

#include "cjext.h"
#include "ezlopi_core_nvs.h"
#include "ezlopi_core_room.h"
#include "ezlopi_cloud_room.h"
#include "ezlopi_core_devices_list.h"
#include "ezlopi_cloud_keywords.h"
#include "ezlopi_cloud_methods_str.h"

void room_list(cJSON* cj_request, cJSON* cj_response)
{
    char* rooms_str = ezlopi_nvs_read_rooms();
    if (rooms_str)
    {
        cJSON_AddRawToObject(cj_response, ezlopi_result_str, rooms_str);
        free(rooms_str);
    }
    else
    {
        cJSON_AddObjectToObject(cj_response, ezlopi_result_str);
    }
}

void room_get(cJSON* cj_request, cJSON* cj_response)
{
    cJSON* cj_params = cJSON_GetObjectItem(cj_request, ezlopi_params_str);
    if (cj_params)
    {
        cJSON* cj_result = cJSON_AddArrayToObject(cj_response, ezlopi_result_str);
        if (cj_result)
        {
            int idx = 0;
            cJSON* cj_room_id = NULL;

            while (NULL != (cj_room_id = cJSON_GetArrayItem(cj_params, idx++)))
            {
                uint32_t room_id = strtoul(cj_room_id->valuestring, NULL, 16);
                s_ezlopi_room_t* l_room_node = ezlopi_room_get_room_head();
                while (l_room_node)
                {
                    if (l_room_node->_id == room_id)
                    {
                        cJSON* cj_room_obj = cJSON_CreateObject();
                        if (cj_room_obj)
                        {
                            cJSON_AddItemToObject(cj_room_obj, ezlopi__id_str, cj_room_id);
                            cJSON_AddStringToObject(cj_room_obj, ezlopi_name_str, l_room_node->name);

                            if (!cJSON_AddItemToArray(cj_result, cj_room_obj))
                            {
                                cJSON_Delete(cj_room_obj);
                            }
                        }

                        break;
                    }

                    l_room_node = l_room_node->next;
                }
            }
        }
    }
}

void room_create(cJSON* cj_request, cJSON* cj_response)
{
    cJSON* cj_params = cJSON_GetObjectItem(cj_request, ezlopi_params_str);
    if (cj_params)
    {
        ezlopi_room_add_to_list(cj_params);
        ezlopi_room_add_to_nvs(cj_params);

        cJSON_AddItemToObject(cj_response, ezlopi_result_str, cJSON_Duplicate(cj_params, cJSON_True));
    }
}

void room_name_set(cJSON* cj_request, cJSON* cj_response)
{
    cJSON_AddObjectToObject(cj_response, ezlopi_result_str);

    cJSON* cj_params = cJSON_GetObjectItem(cj_request, ezlopi_params_str);
    if (cj_params)
    {
        ezlopi_room_name_set(cj_params);
    }
}

void room_delete(cJSON* cj_request, cJSON* cj_response)
{
    cJSON_AddObjectToObject(cj_response, ezlopi_result_str);

    cJSON* cj_params = cJSON_GetObjectItem(cj_request, ezlopi_params_str);
    if (cj_params)
    {
        ezlopi_room_delete(cj_params);
    }
}

void room_all_delete(cJSON* cj_request, cJSON* cj_response)
{
    cJSON_AddObjectToObject(cj_response, ezlopi_result_str);

    ezlopi_room_delete_all();
}

void room_order_set(cJSON* cj_request, cJSON* cj_response)
{
    cJSON* cj_params = cJSON_GetObjectItem(cj_request, ezlopi_params_str);
    if (cj_params)
    {
        cJSON* cj_rooms_ids = cJSON_GetObjectItem(cj_params, ezlopi_roomsId_str);
        if (cj_rooms_ids)
        {
            ezlopi_room_reorder(cj_rooms_ids);
        }
    }
}

//////////////////
void room_created(cJSON* cj_request, cJSON* cj_response)
{
    cJSON_DeleteItemFromObject(cj_response, ezlopi_sender_str);
    cJSON_DeleteItemFromObject(cj_response, ezlopi_error_str);

    cJSON_AddStringToObject(cj_response, ezlopi_id_str, ezlopi_ui_broadcast_str);
    cJSON_AddStringToObject(cj_response, ezlopi_msg_subclass_str, ezlopi_hub_room_created_str);

    cJSON* cj_params = cJSON_GetObjectItem(cj_request, ezlopi_params_str);
    cJSON_AddItemToObject(cj_response, ezlopi_result_str, cJSON_Duplicate(cj_params, cJSON_True));
}

void room_edited(cJSON* cj_request, cJSON* cj_response)
{
    cJSON_DeleteItemFromObject(cj_response, ezlopi_id_str);

    cJSON_AddStringToObject(cj_response, ezlopi_id_str, ezlopi_ui_broadcast_str);
    cJSON_AddStringToObject(cj_response, ezlopi_msg_subclass_str, ezlopi_hub_room_edited_str);
}

void room_deleted(cJSON* cj_request, cJSON* cj_response)
{
    cJSON_DeleteItemFromObject(cj_response, ezlopi_id_str);

    cJSON_AddStringToObject(cj_response, ezlopi_id_str, ezlopi_ui_broadcast_str);
    cJSON_AddStringToObject(cj_response, ezlopi_msg_subclass_str, ezlopi_hub_room_deleted_str);

    cJSON* cj_method = cJSON_GetObjectItem(cj_request, ezlopi_method_str);
    cJSON_AddItemToObject(cj_response, ezlopi_method_str, cJSON_Duplicate(cj_method, cJSON_True));

    cJSON* cj_result = cJSON_AddObjectToObject(cj_response, ezlopi_result_str);
    if (cj_result)
    {
        cJSON* cj_params = cJSON_GetObjectItem(cj_request, ezlopi_params_str);
        if (cj_params)
        {
            cJSON* cj__id = cJSON_GetObjectItem(cj_params, ezlopi__id_str);
            cJSON_AddItemToObject(cj_result, ezlopi__id_str, cJSON_Duplicate(cj__id, cJSON_True));
        }
    }
}

void room_reordered(cJSON* cj_request, cJSON* cj_response)
{
    cJSON_DeleteItemFromObject(cj_response, ezlopi_id_str);

    cJSON_AddStringToObject(cj_response, ezlopi_id_str, ezlopi_ui_broadcast_str);
    cJSON_AddStringToObject(cj_response, ezlopi_msg_subclass_str, ezlopi_hub_room_reordered_str);

    cJSON* cj_method = cJSON_GetObjectItem(cj_request, ezlopi_method_str);
    cJSON_AddItemToObject(cj_response, ezlopi_method_str, cJSON_Duplicate(cj_method, cJSON_True));
}