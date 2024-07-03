

#include "ezlopi_core_nvs.h"
#include "ezlopi_core_device_group.h"

#include "ezlopi_cloud_constants.h"

static l_ezlopi_device_grp_t* l_device_grp_head = NULL;

////// Device Groups ////////

l_ezlopi_device_grp_t* ezlopi_device_grp_get_head(void)
{
    return l_device_grp_head;
}

void ezlopi_core_device_group_print(l_ezlopi_device_grp_t* device_grp_node)
{
#if (ENABLE_TRACE)
    if (device_grp_node)
    {
        TRACE_D("----------------- DEVICE-GROUP --------------------");
        TRACE_D("Group_name: %s", device_grp_node->grp_name);
        TRACE_D("Group_id: %08x", device_grp_node->grp_id);
        CJSON_TRACE("Categories", device_grp_node->categories);
        // cJSON * categories_arr = NULL;
        // if (NULL != (categories_arr = cJSON_GetObjectItem(__FUNCTION__, device_grp_node->categories, "categories")))
        // {
        //     TRACE_D("-- Categories:");
        //     int idx = 0;
        //     cJSON* categories = NULL;
        //     while (NULL != (categories = cJSON_GetArrayItem(categories_arr, idx)))
        //     {
        //         idx++;
        //         CJSON_TRACE("\t\t -", categories);
        //     }
        // }

        CJSON_TRACE("Devices", device_grp_node->devices);
        // cJSON * devices_arr = NULL;
        // if (NULL != (devices_arr = cJSON_GetObjectItem(__FUNCTION__, device_grp_node->devices, "devices")))
        // {
        //     TRACE_D("-- Devices:");
        //     int idx = 0;
        //     cJSON* device = NULL;
        //     while (NULL != (device = cJSON_GetArrayItem(devices_arr, idx)))
        //     {
        //         idx++;
        //         CJSON_TRACE("\t\t -", device);
        //     }
        // }

        CJSON_TRACE("Exceptions", device_grp_node->exceptions);
        // cJSON * exceptions_arr = NULL;
        // if (NULL != (exceptions_arr = cJSON_GetObjectItem(__FUNCTION__, device_grp_node->exceptions, "exceptions")))
        // {
        //     TRACE_D("-- Exceptions:");
        //     int idx = 0;
        //     cJSON* exception = NULL;
        //     while (NULL != (exception = cJSON_GetArrayItem(exceptions_arr, idx)))
        //     {
        //         idx++;
        //         CJSON_TRACE("\t\t -", exception);
        //     }
        // }

        TRACE_D("Persistent: %s", device_grp_node->persistent ? "True" : "False");
        TRACE_D("Entry_Delay: %s", (EZLOPI_DEVICE_GRP_ENTRYDELAY_NORMAL == device_grp_node->entry_delay) ? "normal"
            : (EZLOPI_DEVICE_GRP_ENTRYDELAY_LONG_EXTENDED == device_grp_node->entry_delay) ? "long_extended"
            : (EZLOPI_DEVICE_GRP_ENTRYDELAY_EXTENDED == device_grp_node->entry_delay) ? "extended"
            : (EZLOPI_DEVICE_GRP_ENTRYDELAY_INSTANT == device_grp_node->entry_delay) ? "instant"
            : "none");
        TRACE_D("Follow_Entry: %s", device_grp_node->follow_entry ? "True" : "False");
        TRACE_D("Role: %s", (EZLOPI_DEVICE_GRP_ROLE_HOUSE_MODES == device_grp_node->role) ? "house_modes" : "user");
        TRACE_D("Package_Id: %s", device_grp_node->package_id);
        TRACE_D("----------------- ---------- --------------------");
    }
#endif
}


static uint32_t __device_grp_store_to_nvs(uint32_t device_grp_id, cJSON* cj_device_grp)
{
    if (0 == device_grp_id)
    {
        char* device_grp_str = cJSON_PrintBuffered(__FUNCTION__, cj_device_grp, 1024, false);
        TRACE_D("length of 'device_grp_str': %d", strlen(device_grp_str));

        if (device_grp_str)
        {
            device_grp_id = ezlopi_cloud_generate_device_group_id();
            if (device_grp_id)
            {
                char device_grp_id_str[32];
                snprintf(device_grp_id_str, sizeof(device_grp_id_str), "%08x", device_grp_id);

                if (ezlopi_nvs_write_str(device_grp_str, strlen(device_grp_str), device_grp_id_str))
                {
                    bool free_exp_id_list_str = 1;
                    char* device_grp_id_list_str = ezlopi_nvs_read_device_groups();
                    if (NULL == device_grp_id_list_str)
                    {
                        device_grp_id_list_str = "[]";
                        free_exp_id_list_str = 0;
                        TRACE_W("Device-group ids-list not found in NVS");
                    }

                    TRACE_D("Device-group-IDs: %s", device_grp_id_list_str);
                    cJSON* cj_device_grp_id_list = cJSON_Parse(__FUNCTION__, device_grp_id_list_str);

                    if (free_exp_id_list_str)
                    {
                        ezlopi_free(__FUNCTION__, device_grp_id_list_str);
                        device_grp_id_list_str = NULL;
                    }

                    CJSON_TRACE("cj_esp-ids", cj_device_grp_id_list);

                    if (cj_device_grp_id_list)
                    {
                        // TRACE_D("Here");
                        cJSON* cj_exp_id = cJSON_CreateString(__FUNCTION__, device_grp_id_str);
                        if (cj_exp_id)
                        {
                            // TRACE_D("Here");
                            if (cJSON_AddItemToArray(cj_device_grp_id_list, cj_exp_id))
                            {
                                device_grp_id_list_str = cJSON_PrintBuffered(__FUNCTION__, cj_device_grp_id_list, 1024, false);
                                TRACE_D("length of 'device_grp_id_list_str': %d", strlen(device_grp_id_list_str));

                                if (device_grp_id_list_str)
                                {
                                    ezlopi_nvs_write_device_groups(device_grp_id_list_str);
                                    ezlopi_free(__FUNCTION__, device_grp_id_list_str);
                                }
                            }
                            else
                            {
                                cJSON_Delete(__FUNCTION__, cj_exp_id);
                            }
                        }

                        cJSON_Delete(__FUNCTION__, cj_device_grp_id_list);
                    }
                }
                else
                {
                    TRACE_E("Failed to store 'new-device-grp' in nvs");
                }
            }
            ezlopi_free(__FUNCTION__, device_grp_str);
        }
    }
    else
    {
        ezlopi_cloud_update_device_group_id(device_grp_id);
    }

    return device_grp_id;
}

static l_ezlopi_device_grp_t* __device_grp_create_node(uint32_t device_grp_id, cJSON* cj_device_grp)
{
    l_ezlopi_device_grp_t* new_device_grp_node = NULL;
    if (cj_device_grp)
    {
        new_device_grp_node = ezlopi_malloc(__FUNCTION__, sizeof(l_ezlopi_device_grp_t));
        if (new_device_grp_node)
        {
            memset(new_device_grp_node, 0, sizeof(l_ezlopi_device_grp_t));

            CJSON_GET_VALUE_STRING_BY_COPY(cj_device_grp, ezlopi_name_str, new_device_grp_node->grp_name);

            cJSON * cj_categories = cJSON_GetObjectItem(__FUNCTION__, cj_device_grp, "categories");
            if (cj_categories)
            {
                new_device_grp_node->categories = cJSON_Duplicate(__FUNCTION__, cj_categories, cJSON_True);
            }

            cJSON * cj_devices = cJSON_GetObjectItem(__FUNCTION__, cj_device_grp, "devices");
            if (cj_devices)
            {
                new_device_grp_node->devices = cJSON_Duplicate(__FUNCTION__, cj_devices, cJSON_True);
            }

            cJSON * cj_exceptions = cJSON_GetObjectItem(__FUNCTION__, cj_device_grp, "exceptions");
            if (cj_exceptions)
            {
                new_device_grp_node->exceptions = cJSON_Duplicate(__FUNCTION__, cj_exceptions, cJSON_True);
            }

            CJSON_GET_VALUE_BOOL(cj_device_grp, "persistent", new_device_grp_node->persistent);

            cJSON *cj_entry_delay = cJSON_GetObjectItem(__FUNCTION__, cj_device_grp, "entryDelay");
            if (cj_entry_delay && cj_entry_delay->valuestring && cj_entry_delay->str_value_len)
            {
                new_device_grp_node->entry_delay = ((0 == strncmp(cj_entry_delay->valuestring, "normal", 7)) ? EZLOPI_DEVICE_GRP_ENTRYDELAY_NORMAL
                    : (0 == strncmp(cj_entry_delay->valuestring, "long_extended", 14)) ? EZLOPI_DEVICE_GRP_ENTRYDELAY_LONG_EXTENDED
                    : (0 == strncmp(cj_entry_delay->valuestring, "extended", 9)) ? EZLOPI_DEVICE_GRP_ENTRYDELAY_EXTENDED
                    : (0 == strncmp(cj_entry_delay->valuestring, "instant", 8)) ? EZLOPI_DEVICE_GRP_ENTRYDELAY_INSTANT
                    : EZLOPI_DEVICE_GRP_ENTRYDELAY_NONE);
            }

            CJSON_GET_VALUE_BOOL(cj_device_grp, "followEntry", new_device_grp_node->follow_entry);

            cJSON *cj_role = cJSON_GetObjectItem(__FUNCTION__, cj_device_grp, "role");
            if (cj_role && cj_role->valuestring && cj_role->str_value_len)
            {
                new_device_grp_node->role = ((0 == strncmp(cj_role->valuestring, "house_modes", 12)) ? EZLOPI_DEVICE_GRP_ROLE_HOUSE_MODES : EZLOPI_DEVICE_GRP_ROLE_USER);
            }

            CJSON_GET_VALUE_STRING_BY_COPY(cj_device_grp, "packageId", new_device_grp_node->package_id);

            new_device_grp_node->grp_id = __device_grp_store_to_nvs(device_grp_id, cj_device_grp);
            ezlopi_core_device_group_print(new_device_grp_node);
        }
    }
    return new_device_grp_node;
}


uint32_t ezlopi_core_device_grp_add_to_head(uint32_t device_grp_id, cJSON* cj_device_grp)
{
    uint32_t new_device_grp_id = 0;
    if (l_device_grp_head)
    {
        l_ezlopi_device_grp_t* current_device_grp = l_device_grp_head;
        while (current_device_grp->next)
        {
            current_device_grp = current_device_grp->next;
        }

        current_device_grp->next = __device_grp_create_node(device_grp_id, cj_device_grp);
        if (current_device_grp->next)
        {
            new_device_grp_id = current_device_grp->next->grp_id;
        }
    }
    else
    {
        l_device_grp_head = __device_grp_create_node(device_grp_id, cj_device_grp);
        if (l_device_grp_head)
        {
            new_device_grp_id = l_device_grp_head->grp_id;
        }
    }
    return new_device_grp_id;
}

// void scenes_create(cJSON* cj_request, cJSON* cj_response)
// {
//     cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);

//     cJSON* cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
//     if (cj_params)
//     {
//         uint32_t new_scene_id = ezlopi_store_new_scene_v2(cj_params);
//         TRACE_D("new-scene-id: %08x", new_scene_id);

//         if (new_scene_id)
//         {
//             char tmp_buff[32];
//             snprintf(tmp_buff, sizeof(tmp_buff), "%08x", new_scene_id);
//             cJSON_AddStringToObject(__FUNCTION__, cj_request, ezlopi__id_str, tmp_buff);
//             ezlopi_scenes_new_scene_populate(cj_params, new_scene_id);
//         }
//     }
// }

