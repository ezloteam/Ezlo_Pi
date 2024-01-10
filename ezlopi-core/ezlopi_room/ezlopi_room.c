#include "cJSON.h"
#include "trace.h"

#include "ezlopi_nvs.h"
#include "ezlopi_room.h"
#include "ezlopi_cloud.h"
#include "web_provisioning.h"
#include "ezlopi_cjson_macros.h"
#include "ezlopi_cloud_constants.h"

static s_ezlopi_room_t *l_room_head = NULL;

static void __sort_by_pos(void);
static void __free_nodes(s_ezlopi_room_t *room);
static void __update_cloud_room_deleted(uint32_t room_id);

s_ezlopi_room_t *ezlopi_room_get_room_head(void)
{
    return l_room_head;
}

int ezlopi_room_name_set(cJSON *cj_room)
{
    int ret = 0;
    if (cj_room)
    {
        cJSON *cj_room_id = cJSON_GetObjectItem(cj_room, ezlopi__id_str);
        cJSON *cj_room_name = cJSON_GetObjectItem(cj_room, ezlopi_name_str);

        if (cj_room_id && cj_room_id->valuestring && cj_room_name && cj_room_name->valuestring)
        {
            uint32_t room_id = strtoul(cj_room_id->valuestring, NULL, 16);
            if (room_id)
            {
                char *rooms_str = ezlopi_nvs_read_rooms();
                if (rooms_str)
                {
                    cJSON *cj_rooms = cJSON_Parse(rooms_str);
                    free(rooms_str);

                    if (cj_rooms)
                    {
                        int idx = 0;
                        cJSON *cj_room_tmp = NULL;
                        while (NULL != (cj_room_tmp = cJSON_GetArrayItem(cj_rooms, idx++)))
                        {
                            cJSON *cj_room_tmp_id = cJSON_GetObjectItem(cj_room_tmp, ezlopi__id_str);
                            if (cj_room_tmp_id && cj_room_tmp_id->valuestring)
                            {
                                uint32_t tmp_room_id = strtoul(cj_room_tmp_id->valuestring, NULL, 16);
                                if (room_id == tmp_room_id)
                                {
                                    cJSON_DeleteItemFromObject(cj_room_tmp, ezlopi_name_str);
                                    ret = cJSON_AddItemReferenceToObject(cj_room_tmp, ezlopi_name_str, cj_room_name);

                                    s_ezlopi_room_t *room_node = l_room_head;
                                    while (room_node)
                                    {
                                        if (room_node->_id == room_id)
                                        {
                                            snprintf(room_node->name, sizeof(room_node->name), "%s", cj_room_name->valuestring);
                                            break;
                                        }
                                        room_node = room_node->next;
                                    }
                                    break;
                                }
                            }
                        }

                        if (ret)
                        {
                            char *updated_rooms = cJSON_Print(cj_rooms);
                            if (updated_rooms)
                            {
                                cJSON_Minify(updated_rooms);
                                ezlopi_nvs_write_rooms(updated_rooms);
                                free(updated_rooms);
                            }
                        }

                        cJSON_Delete(cj_rooms);
                    }
                }
            }
        }
    }

    return ret;
}

int ezlopi_room_delete(cJSON *cj_room)
{
    int ret = 0;
    if (cj_room)
    {
        cJSON *cj_room_id = cJSON_GetObjectItem(cj_room, ezlopi__id_str);
        if (cj_room_id && cj_room_id->valuestring)
        {
            uint32_t room_id = strtoul(cj_room_id->valuestring, NULL, 16);
            if (room_id)
            {
                char *rooms_str = ezlopi_nvs_read_rooms();
                if (rooms_str)
                {
                    cJSON *cj_rooms = cJSON_Parse(rooms_str);
                    free(rooms_str);

                    if (cj_rooms)
                    {
                        int idx = 0;
                        cJSON *cj_room_tmp = NULL;
                        while (NULL != (cj_room_tmp = cJSON_GetArrayItem(cj_rooms, idx)))
                        {
                            cJSON *cj_room_tmp_id = cJSON_GetObjectItem(cj_room_tmp, ezlopi__id_str);
                            if (cj_room_tmp_id && cj_room_tmp_id->valuestring)
                            {
                                uint32_t tmp_room_id = strtoul(cj_room_tmp_id->valuestring, NULL, 16);
                                if (room_id == tmp_room_id)
                                {
                                    cJSON_DeleteItemFromArray(cj_rooms, idx);
                                    if (l_room_head)
                                    {
                                        if (l_room_head->_id == room_id)
                                        {
                                            s_ezlopi_room_t *free_node = l_room_head;
                                            l_room_head = l_room_head->next;
                                            free(free_node);
                                        }
                                        else
                                        {
                                            s_ezlopi_room_t *room_node = l_room_head;
                                            while (room_node->next)
                                            {
                                                s_ezlopi_room_t *free_node = room_node->next;
                                                if (free_node->_id == room_id)
                                                {
                                                    room_node->next = free_node->next;
                                                    free(free_node);
                                                    break;
                                                }
                                                room_node = room_node->next;
                                            }
                                        }
                                    }

                                    break;
                                }
                            }

                            idx++;
                        }

                        if (ret)
                        {
                            char *updated_rooms = cJSON_Print(cj_rooms);
                            if (updated_rooms)
                            {
                                cJSON_Minify(updated_rooms);
                                ezlopi_nvs_write_rooms(updated_rooms);
                                free(updated_rooms);
                            }
                        }

                        cJSON_Delete(cj_rooms);
                    }
                }
            }
        }
    }

    return ret;
}

int ezlopi_room_delete_all(void)
{
    int ret = 1;

    __free_nodes(l_room_head);
    l_room_head = NULL;

    ezlopi_nvs_write_rooms("[]");

    return ret;
}

int ezlopi_room_add_to_nvs(cJSON *cj_room)
{
    int ret = 0;
    if (cj_room)
    {
        char *rooms_str = ezlopi_nvs_read_rooms();
        if (rooms_str)
        {
            cJSON *cj_rooms = cJSON_Parse(rooms_str);
            free(rooms_str);

            if (cj_rooms)
            {
                if (cJSON_AddItemToArray(cj_rooms, cj_room))
                {
                    char *updated_rooms_str = cJSON_Print(cj_rooms);
                    if (updated_rooms_str)
                    {
                        cJSON_Minify(updated_rooms_str);

                        if (ezlopi_nvs_write_rooms(updated_rooms_str))
                        {
                            ret = 1;
                        }

                        free(updated_rooms_str);
                    }
                }

                cJSON_Delete(cj_rooms);
            }
        }
    }

    return ret;
}

int ezlopi_room_reorder(cJSON *cj_rooms_ids)
{
    int ret = 0;

    if (cj_rooms_ids)
    {
        CJSON_TRACE("new-order ids", cj_rooms_ids);
        uint32_t rooms_id_arr_size = cJSON_GetArraySize(cj_rooms_ids);

        if (rooms_id_arr_size)
        {
            int idx = 0;
            cJSON *cj_reordered_rooms = cJSON_CreateArray();

            if (cj_reordered_rooms)
            {
                cJSON *cj_room_id = NULL;

                while (NULL != (cj_room_id = cJSON_GetArrayItem(cj_rooms_ids, idx)))
                {
                    uint32_t room_id = strtoul(cj_room_id->valuestring, NULL, 16);
                    if (room_id)
                    {
                        s_ezlopi_room_t *room_node = l_room_head;
                        while (room_node)
                        {
                            if (room_id == room_node->_id)
                            {
                                room_node->_pos = idx;

                                cJSON *cj_room = cJSON_CreateObject();
                                if (cj_room)
                                {
                                    cJSON_AddStringToObject(cj_room, ezlopi__id_str, cj_room_id->valuestring);
                                    cJSON_AddStringToObject(cj_room, ezlopi_name_str, room_node->name);

                                    if (!cJSON_AddItemToArray(cj_reordered_rooms, cj_room))
                                    {
                                        cJSON_Delete(cj_room);
                                    }
                                }
                            }

                            room_node = room_node->next;
                        }
                    }

                    idx++;
                }

                __sort_by_pos();

                if (cJSON_GetArraySize(cj_reordered_rooms))
                {
                    char *reordered_rooms_str = cJSON_Print(cj_reordered_rooms);
                    if (reordered_rooms_str)
                    {
                        cJSON_Minify(reordered_rooms_str);
                        ezlopi_nvs_write_rooms(reordered_rooms_str);
                        free(reordered_rooms_str);
                    }
                }

                cJSON_Delete(cj_reordered_rooms);
            }

#if 0 
            int idx = 0;
            cJSON *cj_room_id = NULL;
            uint32_t rooms_id_arr[rooms_id_arr_size];

            while (NULL != (cj_room_id = cJSON_GetArrayItem(cj_rooms_ids, idx)))
            {
                rooms_id_arr[idx] = strtoul(cj_room_id->valuestring, NULL, 16);
                idx++;
            }
            char *room_list_str = ezlopi_nvs_read_rooms();

            if (room_list_str)
            {
                TRACE_D("old-order list: %s", room_list_str);
                cJSON *cj_stored_room_list = cJSON_Parse(room_list_str);
                free(room_list_str);

                if (cj_stored_room_list)
                {
                    cJSON *cj_reordered_rooms = cJSON_CreateArray();

                    if (cj_reordered_rooms)
                    {
                        int idx1 = 0;
                        while (idx1 < rooms_id_arr_size)
                        {
                            int idx2 = 0;
                            cJSON *cj_stored_room = NULL;
                            while (NULL != (cj_stored_room = cJSON_GetArrayItem(cj_stored_room_list, idx2)))
                            {
                                cJSON *cj_stored_room_id = cJSON_GetObjectItem(cj_stored_room, ezlopi__id_str);
                                if (cj_stored_room_id && cj_stored_room_id->valuestring)
                                {
                                    uint32_t stored_room_id = strtoul(cj_stored_room_id->valuestring, NULL, 16);
                                    if (stored_room_id == rooms_id_arr[idx1])
                                    {
                                        cJSON_AddItemReferenceToArray(cj_reordered_rooms, cj_stored_room);
                                    }
                                }

                                idx2++;
                            }

                            __rooms_move_to_pos(rooms_id_arr[idx1], idx1);
                            idx1++;
                        }

                        char *reordered_rooms_str = cJSON_Print(cj_reordered_rooms);
                        cJSON_Delete(cj_reordered_rooms);

                        if (reordered_rooms_str)
                        {
                            TRACE_D("reordered room list: %s", reordered_rooms_str);
                            cJSON_Minify(reordered_rooms_str);
                            // ezlopi_nvs_write_rooms(reordered_rooms_str);
                            free(reordered_rooms_str);
                        }
                    }

                    cJSON_Delete(cj_stored_room_list);
                }
            }
#endif
        }
    }

    return ret;
}

s_ezlopi_room_t *ezlopi_room_add_to_list(cJSON *cj_room)
{
    s_ezlopi_room_t *new_room = malloc(sizeof(s_ezlopi_room_t));
    if (new_room)
    {
        memset(new_room, 0, sizeof(s_ezlopi_room_t));

        if (cj_room)
        {
            cJSON *cj_room_id = cJSON_GetObjectItem(cj_room, ezlopi__id_str);
            if (cj_room_id && cj_room_id->valuestring)
            {
                new_room->_id = strtoul(cj_room_id->valuestring, NULL, 16);
                ezlopi_cloud_update_room_id(new_room->_id);
            }
            else
            {
                new_room->_id = ezlopi_cloud_generate_room_id();
                char tmp_str[32];
                snprintf(tmp_str, sizeof(tmp_str), "%08x", new_room->_id);
                cJSON_AddStringToObject(cj_room, ezlopi__id_str, tmp_str);
            }

            cJSON *cj_room_name = cJSON_GetObjectItem(cj_room, ezlopi_name_str);
            if (cj_room_name && cj_room_name->valuestring)
            {
                snprintf(new_room->name, sizeof(new_room->name), "%s", cj_room->valuestring);
            }
        }
    }

    return new_room;
}

void ezlopi_room_init(void)
{
    char *rooms_str = ezlopi_nvs_read_rooms();
    if (rooms_str)
    {
        cJSON *cj_rooms = cJSON_Parse(rooms_str);
        free(rooms_str);

        if (cj_rooms)
        {
            int idx = 0;
            cJSON *cj_room = NULL;
            s_ezlopi_room_t *curr_room_node = NULL;

            while (NULL != (cj_room = cJSON_GetArrayItem(cj_rooms, idx)))
            {
                s_ezlopi_room_t *new_room = ezlopi_room_add_to_list(cj_rooms);
                if (new_room)
                {
                    new_room->_pos = idx;
                    if (curr_room_node)
                    {
                        curr_room_node->next = new_room;
                        curr_room_node = curr_room_node->next;
                    }
                    else
                    {
                        l_room_head = new_room;
                        curr_room_node = l_room_head;
                    }
                }

                idx++;
            }
        }
    }
}

static void __update_cloud_room_deleted(uint32_t room_id)
{
    cJSON *cj_response = cJSON_CreateObject();
    {
        cJSON_AddStringToObject(cj_response, ezlopi_id_str, ezlopi_ui_broadcast_str);
        cJSON_AddStringToObject(cj_response, ezlopi_msg_subclass_str, ezlopi_hub_room_deleted_str);
        cJSON_AddStringToObject(cj_response, ezlopi_key_method_str, "hub.room.all.delete");

        cJSON *cj_result = cJSON_AddObjectToObject(cj_response, ezlopi_result_str);
        if (cj_result)
        {
            char tmp_str[32];
            snprintf(tmp_str, sizeof(tmp_str), "%08x", room_id);
            cJSON_AddItemReferenceToObject(cj_result, ezlopi__id_str, tmp_str);
        }

        char *data_str = cJSON_Print(cj_response);
        cJSON_Delete(cj_response);

        if (data_str)
        {
            cJSON_Minify(data_str);
            web_provisioning_send_str_data_to_nma_websocket(data_str, TRACE_TYPE_B);
            free(data_str);
        }
    }
}

static void __free_nodes(s_ezlopi_room_t *room)
{
    if (room)
    {
        __free_nodes(room->next);
        __update_cloud_room_deleted(room->_id);
        free(room);
    }
}

static void __sort_by_pos(void)
{
    uint32_t size = 0;
    s_ezlopi_room_t *room_ptr_arr[32];
    s_ezlopi_room_t *curr_room = l_room_head;

    curr_room = l_room_head;
    memset(room_ptr_arr, 0, sizeof(room_ptr_arr));

    while (curr_room)
    {
        size++;
        room_ptr_arr[curr_room->_pos] = curr_room;
        curr_room = curr_room->next;
    }

    l_room_head = room_ptr_arr[0];
    l_room_head->next = NULL;
    curr_room = l_room_head;

    int idx = 1;
    
    while (idx < size)
    {
        curr_room->next = room_ptr_arr[idx];
        curr_room->next->next = NULL;
        idx++;
    }
}