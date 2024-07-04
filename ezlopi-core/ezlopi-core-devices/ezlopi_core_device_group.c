

#include "ezlopi_core_nvs.h"
#include "ezlopi_core_device_group.h"

#include "ezlopi_cloud_constants.h"

static l_ezlopi_device_grp_t* l_device_grp_head = NULL;

////// Device Groups ////////

l_ezlopi_device_grp_t* ezlopi_core_device_group_get_head(void)
{
    return l_device_grp_head;
}

//--------------------------------------------------------------------------------
//  Function to Print the 'device_grp_node'
//--------------------------------------------------------------------------------

void ezlopi_core_device_group_print(l_ezlopi_device_grp_t* device_grp_node)
{
#if (ENABLE_TRACE)
    if (device_grp_node)
    {
        TRACE_D("----------------- DEVICE-GROUP --------------------");
        TRACE_D("Group_id: %08x", device_grp_node->_id);
        TRACE_D("Group_name: %s", device_grp_node->name);
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

/**
 * @brief Returns the node associated with required 'device_group_id'
 *
 * @param _id  'device_group_id'
 * @return l_ezlopi_device_grp_t*
 */
l_ezlopi_device_grp_t* ezlopi_core_device_group_get_by_id(uint32_t _id)
{
    l_ezlopi_device_grp_t* ret_devgrp_node = NULL;
    l_ezlopi_device_grp_t* devgrp_list = l_device_grp_head;
    while (devgrp_list)
    {
        if (_id == devgrp_list->_id)
        {
            ret_devgrp_node = devgrp_list;
            break;
        }
        devgrp_list = devgrp_list->next;
    }

    return ret_devgrp_node;
}

static void __edit_devgrp_from_ll(l_ezlopi_device_grp_t* req_devgrp_node, cJSON* cj_devgrp_new)
{
    CJSON_GET_VALUE_STRING_BY_COPY(cj_devgrp_new, ezlopi_name_str, req_devgrp_node->name);
    CJSON_GET_VALUE_BOOL(cj_devgrp_new, "persistent", req_devgrp_node->persistent);
    CJSON_GET_VALUE_BOOL(cj_devgrp_new, "followEntry", req_devgrp_node->follow_entry);
    CJSON_GET_VALUE_STRING_BY_COPY(cj_devgrp_new, "packageId", req_devgrp_node->package_id);

    // ------------- categories -------------
    {
        cJSON_Delete(__FUNCTION__, req_devgrp_node->categories);
        cJSON * cj_categories = cJSON_GetObjectItem(__FUNCTION__, cj_devgrp_new, "categories");
        if (cj_categories && cJSON_IsArray(cj_categories))
        {
            req_devgrp_node->categories = cJSON_Duplicate(__FUNCTION__, cj_categories, cJSON_True);
        }
    }

    // ------------- devices -------------
    {
        cJSON_Delete(__FUNCTION__, req_devgrp_node->devices);
        cJSON * cj_devices = cJSON_GetObjectItem(__FUNCTION__, cj_devgrp_new, "devices");
        if (cj_devices && cJSON_IsArray(cj_devices))
        {
            req_devgrp_node->devices = cJSON_Duplicate(__FUNCTION__, cj_devices, cJSON_True);
        }
    }

    // ------------- exceptions -------------
    {
        cJSON_Delete(__FUNCTION__, req_devgrp_node->exceptions);
        cJSON * cj_exceptions = cJSON_GetObjectItem(__FUNCTION__, cj_devgrp_new, "exceptions");
        if (cj_exceptions && cJSON_IsArray(cj_exceptions))
        {
            req_devgrp_node->exceptions = cJSON_Duplicate(__FUNCTION__, cj_exceptions, cJSON_True);
        }
    }

    // ------------- entryDelay -------------
    {
        cJSON *cj_entry_delay = cJSON_GetObjectItem(__FUNCTION__, cj_devgrp_new, "entryDelay");
        if (cj_entry_delay && cj_entry_delay->valuestring && cj_entry_delay->str_value_len)
        {
            req_devgrp_node->entry_delay = ((0 == strncmp(cj_entry_delay->valuestring, "normal", 7)) ? EZLOPI_DEVICE_GRP_ENTRYDELAY_NORMAL
                : (0 == strncmp(cj_entry_delay->valuestring, "long_extended", 14)) ? EZLOPI_DEVICE_GRP_ENTRYDELAY_LONG_EXTENDED
                : (0 == strncmp(cj_entry_delay->valuestring, "extended", 9)) ? EZLOPI_DEVICE_GRP_ENTRYDELAY_EXTENDED
                : (0 == strncmp(cj_entry_delay->valuestring, "instant", 8)) ? EZLOPI_DEVICE_GRP_ENTRYDELAY_INSTANT
                : EZLOPI_DEVICE_GRP_ENTRYDELAY_NONE);
        }
    }

    // ------------- role -------------
    {
        cJSON *cj_role = cJSON_GetObjectItem(__FUNCTION__, cj_devgrp_new, "role");
        if (cj_role && cj_role->valuestring && cj_role->str_value_len)
        {
            req_devgrp_node->role = ((0 == strncmp(cj_role->valuestring, "house_modes", 12)) ? EZLOPI_DEVICE_GRP_ROLE_HOUSE_MODES : EZLOPI_DEVICE_GRP_ROLE_USER);
        }
    }
}

/**
 * @brief This function edits the populated 'devgrp_id' within linkedlist
 *
 * @param devgrp_id targer 'devgrp_id'
 * @param cj_updated_devgrp new cjson to replace old.
 * @return int
 */
static int __ezlopi_core_device_group_edit_update_id(uint32_t devgrp_id, cJSON* cj_devgrp_new)
{
    int ret = 0;
    CJSON_TRACE("cj_updated_devgrp_node :", cj_devgrp_new);

    if (devgrp_id && cj_devgrp_new)
    {
        l_ezlopi_device_grp_t* req_devgrp_node = ezlopi_core_device_group_get_by_id(devgrp_id);
        if (req_devgrp_node)
        {
            __edit_devgrp_from_ll(req_devgrp_node, cj_devgrp_new);
            ret = 1;
        }
    }
    return ret;
}

/**
 * @brief This function replaces 'new_devgrp' with old
 *
 * @param cj_devgrp_new 'new_devgrp' in cjson format
 * @return int
 */
static int __ezlopi_core_device_group_edit_store_updated_to_nvs(cJSON* cj_devgrp_new)
{
    int ret = 0;
    if (cj_devgrp_new)
    {
        char* update_devgrp_str = cJSON_PrintBuffered(__FUNCTION__, cj_devgrp_new, 1024, false);
        TRACE_D("length of 'update_devgrp_str': %d", strlen(update_devgrp_str));

        if (update_devgrp_str)
        {
            cJSON* cj_devgrp_id = cJSON_GetObjectItem(__FUNCTION__, cj_devgrp_new, "id");  // according to docs --> 'id'
            if (cj_devgrp_id && cj_devgrp_id->valuestring)
            {
                ezlopi_nvs_delete_stored_data_by_name(cj_devgrp_id->valuestring); // delete --> '0x0..devgrp ' : '{}'
                ret = ezlopi_nvs_write_str(update_devgrp_str, strlen(update_devgrp_str), cj_devgrp_id->valuestring); // write --> '0x0..devgrp ' : '{}'
            }

            ezlopi_free(__FUNCTION__, update_devgrp_str);
        }
    }

    return ret;
}

/**
 * @brief This function handles editing of req_id in ll & nvs
 *
 * @param devgrp_id required 'devgrp_id'
 * @param cj_devgrp_new cjson of 'new_devgrp'
 * @return int
 */
int ezlopi_core_device_group_edit_by_id(uint32_t devgrp_id, cJSON* cj_devgrp_new)
{
    int ret = 0;

    if (1 == __ezlopi_core_device_group_edit_update_id(devgrp_id, cj_devgrp_new))
    {
        if (1 == __ezlopi_core_device_group_edit_store_updated_to_nvs(cj_devgrp_new))
        {
            ret = 1;
        }
    }

    return ret;
}

/**
 * @brief   This function de-links a specific node having req '_id'
 *
 * @param _id Target 'device_group_id'
 * @return l_ezlopi_device_grp_t*
 */
static l_ezlopi_device_grp_t* __device_group_pop_by_id(uint32_t _id)
{
    l_ezlopi_device_grp_t* ret_devgrp = NULL;

    if (_id == l_device_grp_head->_id)
    {
        ret_devgrp = l_device_grp_head;
        l_device_grp_head = l_device_grp_head->next;
        ret_devgrp->next = NULL;
    }
    else
    {
        l_ezlopi_device_grp_t* curr_devgrp = l_device_grp_head;
        while (curr_devgrp->next)
        {
            if (_id == curr_devgrp->next->_id)
            {
                ret_devgrp = curr_devgrp->next;
                curr_devgrp->next = curr_devgrp->next->next;
                ret_devgrp->next = NULL;
                break;
            }
            curr_devgrp = curr_devgrp->next;
        }
    }

    return ret_devgrp;
}

static void __device_group_delete_node(l_ezlopi_device_grp_t* devgrp_node)
{
    if (devgrp_node)
    {
        if (NULL != devgrp_node->categories)
        {
            cJSON_Delete(__FUNCTION__, devgrp_node->categories);
            devgrp_node->categories = NULL;
        }

        if (NULL != devgrp_node->devices)
        {
            cJSON_Delete(__FUNCTION__, devgrp_node->devices);
            devgrp_node->devices = NULL;
        }

        if (NULL != devgrp_node->exceptions)
        {
            cJSON_Delete(__FUNCTION__, devgrp_node->exceptions);
            devgrp_node->exceptions = NULL;
        }

        __device_group_delete_node(devgrp_node->next);
        devgrp_node->next = NULL;
        ezlopi_free(__FUNCTION__, devgrp_node);
    }
}

/**
 * @brief This function removes perticular 'devgrp_id' from nvs list
 *
 * @param _id
 */
void ezlopi_core_device_group_remove_id_from_list(uint32_t _id)
{
    char* devgrp_id_list = ezlopi_nvs_read_device_groups();
    if (devgrp_id_list)
    {
        cJSON* cj_devgrp_id_list = cJSON_Parse(__FUNCTION__, devgrp_id_list);
        if (cj_devgrp_id_list)
        {
            uint32_t list_len = cJSON_GetArraySize(cj_devgrp_id_list);

            for (int idx = list_len; idx < list_len; idx++)
            {
                cJSON* cj_id = cJSON_GetArrayItem(cj_devgrp_id_list, idx);
                if (cj_id)
                {
                    if (cj_id->valuedouble == _id)
                    {
                        cJSON_DeleteItemFromArray(__FUNCTION__, cj_devgrp_id_list, idx);

                        char* updated_id_list_str = cJSON_PrintBuffered(__FUNCTION__, cj_devgrp_id_list, 1024, false);
                        TRACE_D("length of 'updated_id_list_str': %d", strlen(updated_id_list_str));

                        if (updated_id_list_str)
                        {
                            ezlopi_nvs_write_device_groups(updated_id_list_str);
                            ezlopi_free(__FUNCTION__, updated_id_list_str);
                        }
                    }
                }
            }

            cJSON_Delete(__FUNCTION__, cj_devgrp_id_list);
        }
        ezlopi_free(__FUNCTION__, devgrp_id_list);
    }
}

/**
 * @brief Depopulate perticular node with 'device-group-id'
 *
 * @param _id 'device-group-id'
 */
void ezlopi_core_device_group_depopulate_by_id_v2(uint32_t _id)
{
    __device_group_delete_node(__device_group_pop_by_id(_id));
}

uint32_t ezlopi_core_device_group_get_list(cJSON* cj_devgrp_array)
{
    int ret = 0;
    char* devgrp_id_list = ezlopi_nvs_read_device_groups();
    if (devgrp_id_list)
    {
        cJSON* cj_devgrp_id_list = cJSON_Parse(__FUNCTION__, devgrp_id_list);
        if (cj_devgrp_id_list)
        {
            int array_size = cJSON_GetArraySize(cj_devgrp_id_list);
            for (int i = 0; i < array_size; i++)
            {
                cJSON* cj_devgrp_id = cJSON_GetArrayItem(cj_devgrp_id_list, i);
                if (cj_devgrp_id && cj_devgrp_id->valuedouble)
                {
                    char tmp_buffer[32];
                    uint32_t devgrp_id = (uint32_t)cj_devgrp_id->valuedouble;
                    snprintf(tmp_buffer, sizeof(tmp_buffer), "%08x", devgrp_id);
                    char* devgrp_str = ezlopi_nvs_read_str(tmp_buffer);
                    if (devgrp_str)
                    {
                        cJSON* cj_devgrp_node = cJSON_Parse(__FUNCTION__, devgrp_str);
                        if (cj_devgrp_node)
                        {
                            char devgrp_id_str[32];
                            snprintf(devgrp_id_str, sizeof(devgrp_id_str), "%08x", (uint32_t)cj_devgrp_id->valuedouble);
                            cJSON_AddStringToObject(__FUNCTION__, cj_devgrp_node, ezlopi__id_str, devgrp_id_str);

                            if (!cJSON_AddItemToArray(cj_devgrp_array, cj_devgrp_node))
                            {
                                cJSON_Delete(__FUNCTION__, cj_devgrp_node);
                            }
                            else
                            {
                                ret += 1;   // return total number of scenes
                            }
                        }

                        ezlopi_free(__FUNCTION__, devgrp_str);
                    }
                }
            }

            cJSON_Delete(__FUNCTION__, cj_devgrp_id_list);
        }

        ezlopi_free(__FUNCTION__, devgrp_id_list);
    }

    return ret;
}

//--------------------------------------------------------------------------------
// Functions related to 'device-group' & 'nvs'
//--------------------------------------------------------------------------------

uint32_t ezlopi_core_device_group_store_nvs_devgrp(cJSON* cj_new_device_grp)
{
    uint32_t new_dev_grp_id = 0;
    if (cj_new_device_grp)
    {
        uint32_t new_dev_grp_id = ezlopi_cloud_generate_device_group_id();
        char tmp_buf[32];
        snprintf(tmp_buf, sizeof(tmp_buf), "%08x", new_dev_grp_id);
        cJSON_AddStringToObject(__FUNCTION__, cj_new_device_grp, ezlopi__id_str, tmp_buf); // _id -> group_id

        char* new_devgrp_str = cJSON_PrintBuffered(__FUNCTION__, cj_new_device_grp, 1024, false);
        TRACE_D("length of 'new_devgrp_str': %d", strlen(new_devgrp_str));

        if (new_devgrp_str)
        {
            if (ezlopi_nvs_write_str(new_devgrp_str, strlen(new_devgrp_str) + 1, tmp_buf))
            {
                bool free_exp_id_list_str = 1;
                char* device_grp_id_list_str = ezlopi_nvs_read_device_groups();
                if (NULL == device_grp_id_list_str)
                {
                    device_grp_id_list_str = "[]";
                    free_exp_id_list_str = 0;
                }

                cJSON* cj_device_grp_id_list = cJSON_Parse(__FUNCTION__, device_grp_id_list_str);
                if (cj_device_grp_id_list)
                {
                    cJSON* cj_new_dev_grp_id = cJSON_CreateNumber(__FUNCTION__, new_dev_grp_id);
                    if (!cJSON_AddItemToArray(cj_device_grp_id_list, cj_new_dev_grp_id))
                    {   // if 'new_dev_grp_id' doesnot exist, then erase the falsly store 'new_devgrp_str' in nvs.
                        cJSON_Delete(__FUNCTION__, cj_new_dev_grp_id);
                        ezlopi_nvs_delete_stored_data_by_id(new_dev_grp_id);
                        new_dev_grp_id = 0;
                    }
                    else
                    {
                        char* updated_devgrp_id_list_str = cJSON_PrintBuffered(__FUNCTION__, cj_device_grp_id_list, 1024, false);
                        TRACE_D("length of 'updated_devgrp_id_list_str': %d", strlen(updated_devgrp_id_list_str));

                        if (updated_devgrp_id_list_str)
                        {
                            TRACE_D("updated_devgrp_id_list_str: %s", updated_devgrp_id_list_str);
                            if (ezlopi_nvs_write_device_groups(updated_devgrp_id_list_str))
                            {
                                TRACE_D("Scenes list updated.");
                            }
                            else
                            {
                                TRACE_E("Scenes list update failed!");
                            }

                            ezlopi_free(__FUNCTION__, updated_devgrp_id_list_str);
                        }
                    }

                    cJSON_Delete(__FUNCTION__, cj_device_grp_id_list);
                }

                if (free_exp_id_list_str)
                {
                    ezlopi_free(__FUNCTION__, device_grp_id_list_str);
                }
            }
            else
            {
                new_dev_grp_id = 0;
            }

            ezlopi_free(__FUNCTION__, new_devgrp_str);
        }
        else
        {
            new_dev_grp_id = 0;
        }
    }

    return new_dev_grp_id;
}

//--------------------------------------------------------------------------------
//  Functions related to 'device_groups' and 'new_node_population'
//--------------------------------------------------------------------------------

static l_ezlopi_device_grp_t* ____device_grp_create_node(cJSON* cj_device_grp, uint32_t device_grp_id)
{
    l_ezlopi_device_grp_t* new_device_grp_node = NULL;
    if (cj_device_grp)
    {
        new_device_grp_node = ezlopi_malloc(__FUNCTION__, sizeof(l_ezlopi_device_grp_t));
        if (new_device_grp_node)
        {
            memset(new_device_grp_node, 0, sizeof(l_ezlopi_device_grp_t));

            new_device_grp_node->_id = device_grp_id;

            CJSON_GET_VALUE_STRING_BY_COPY(cj_device_grp, ezlopi_name_str, new_device_grp_node->name);
            CJSON_GET_VALUE_BOOL(cj_device_grp, "persistent", new_device_grp_node->persistent);
            CJSON_GET_VALUE_BOOL(cj_device_grp, "followEntry", new_device_grp_node->follow_entry);
            CJSON_GET_VALUE_STRING_BY_COPY(cj_device_grp, "packageId", new_device_grp_node->package_id);

            {
                cJSON * cj_categories = cJSON_GetObjectItem(__FUNCTION__, cj_device_grp, "categories");
                if (cj_categories && cJSON_IsArray(cj_categories))
                {
                    new_device_grp_node->categories = cJSON_Duplicate(__FUNCTION__, cj_categories, cJSON_True);
                }
            }

            {
                cJSON * cj_devices = cJSON_GetObjectItem(__FUNCTION__, cj_device_grp, "devices");
                if (cj_devices && cJSON_IsArray(cj_devices))
                {
                    new_device_grp_node->devices = cJSON_Duplicate(__FUNCTION__, cj_devices, cJSON_True);
                }
            }

            {
                cJSON * cj_exceptions = cJSON_GetObjectItem(__FUNCTION__, cj_device_grp, "exceptions");
                if (cj_exceptions && cJSON_IsArray(cj_exceptions))
                {
                    new_device_grp_node->exceptions = cJSON_Duplicate(__FUNCTION__, cj_exceptions, cJSON_True);
                }
            }

            {
                cJSON *cj_entry_delay = cJSON_GetObjectItem(__FUNCTION__, cj_device_grp, "entryDelay");
                if (cj_entry_delay && cj_entry_delay->valuestring && cj_entry_delay->str_value_len)
                {
                    new_device_grp_node->entry_delay = ((0 == strncmp(cj_entry_delay->valuestring, "normal", 7)) ? EZLOPI_DEVICE_GRP_ENTRYDELAY_NORMAL
                        : (0 == strncmp(cj_entry_delay->valuestring, "long_extended", 14)) ? EZLOPI_DEVICE_GRP_ENTRYDELAY_LONG_EXTENDED
                        : (0 == strncmp(cj_entry_delay->valuestring, "extended", 9)) ? EZLOPI_DEVICE_GRP_ENTRYDELAY_EXTENDED
                        : (0 == strncmp(cj_entry_delay->valuestring, "instant", 8)) ? EZLOPI_DEVICE_GRP_ENTRYDELAY_INSTANT
                        : EZLOPI_DEVICE_GRP_ENTRYDELAY_NONE);
                }
            }

            {
                cJSON *cj_role = cJSON_GetObjectItem(__FUNCTION__, cj_device_grp, "role");
                if (cj_role && cj_role->valuestring && cj_role->str_value_len)
                {
                    new_device_grp_node->role = ((0 == strncmp(cj_role->valuestring, "house_modes", 12)) ? EZLOPI_DEVICE_GRP_ROLE_HOUSE_MODES : EZLOPI_DEVICE_GRP_ROLE_USER);
                }
            }

            ezlopi_core_device_group_print(new_device_grp_node);
        }
    }
    return new_device_grp_node;
}
static l_ezlopi_device_grp_t* __device_group_populate(cJSON* cj_device_grp, uint32_t device_grp_id)
{
    l_ezlopi_device_grp_t* new_device_grp_node = NULL;
    if (l_device_grp_head)
    {
        l_ezlopi_device_grp_t* current_device_grp = l_device_grp_head;
        while (current_device_grp->next)
        {
            current_device_grp = current_device_grp->next;
        }

        current_device_grp->next = ____device_grp_create_node(cj_device_grp, device_grp_id);
        new_device_grp_node = current_device_grp->next;
    }
    else
    {
        CJSON_TRACE("new-device-group", cj_device_grp);
        l_device_grp_head = ____device_grp_create_node(cj_device_grp, device_grp_id);
        new_device_grp_node = l_device_grp_head;
    }

    return new_device_grp_node;
}

l_ezlopi_device_grp_t * ezlopi_core_device_group_new_devgrp_populate(cJSON *cj_new_dev_grp, uint32_t new_device_grp_id)
{
    return __device_group_populate(cj_new_dev_grp, new_device_grp_id);
}

//--------------------------------------------------------------------------------
//  Functions to convert 'l_ezlopi_device_grp_t*' to cjson object
//--------------------------------------------------------------------------------
cJSON* ezlopi_core_device_group_create_cjson(l_ezlopi_device_grp_t* devgrp_node)
{
    cJSON* cj_devgrp = NULL;
    if (devgrp_node)
    {
        cj_devgrp = cJSON_CreateObject(__FUNCTION__);
        if (cj_devgrp)
        {
            char tmp_str[16] = { 0 };
            snprintf(tmp_str, sizeof(tmp_str), "%08x", devgrp_node->_id);
            cJSON_AddStringToObject(__FUNCTION__, cj_devgrp, ezlopi__id_str, tmp_str);
            cJSON_AddStringToObject(__FUNCTION__, cj_devgrp, ezlopi_name_str, devgrp_node->name);

            cJSON_AddItemToObject(__FUNCTION__, cj_devgrp, "categories", cJSON_Duplicate(__FUNCTION__, devgrp_node->categories, cJSON_True));
            cJSON_AddItemToObject(__FUNCTION__, cj_devgrp, "devices", cJSON_Duplicate(__FUNCTION__, devgrp_node->devices, cJSON_True));
            cJSON_AddItemToObject(__FUNCTION__, cj_devgrp, "exceptions", cJSON_Duplicate(__FUNCTION__, devgrp_node->exceptions, cJSON_True));

            {   // add 'entryDelay'
                switch (devgrp_node->entry_delay)
                {
                case EZLOPI_DEVICE_GRP_ENTRYDELAY_NORMAL:
                {
                    cJSON_AddStringToObject(__FUNCTION__, cj_devgrp, "entryDelay", "normal");
                    break;
                }
                case EZLOPI_DEVICE_GRP_ENTRYDELAY_LONG_EXTENDED:
                {
                    cJSON_AddStringToObject(__FUNCTION__, cj_devgrp, "entryDelay", "long_extended");
                    break;
                }
                case EZLOPI_DEVICE_GRP_ENTRYDELAY_EXTENDED:
                {
                    cJSON_AddStringToObject(__FUNCTION__, cj_devgrp, "entryDelay", "extended");
                    break;
                }
                case EZLOPI_DEVICE_GRP_ENTRYDELAY_INSTANT:
                {
                    cJSON_AddStringToObject(__FUNCTION__, cj_devgrp, "entryDelay", "instant");
                    break;
                }
                case EZLOPI_DEVICE_GRP_ENTRYDELAY_NONE:
                {
                    cJSON_AddStringToObject(__FUNCTION__, cj_devgrp, "entryDelay", "none");
                    break;
                }
                default:
                    break;
                }
            }
            {   // add 'role'
                switch (devgrp_node->role)
                {
                case EZLOPI_DEVICE_GRP_ROLE_USER:
                {
                    cJSON_AddStringToObject(__FUNCTION__, cj_devgrp, "role", "user");
                    break;
                }
                case EZLOPI_DEVICE_GRP_ROLE_HOUSE_MODES:
                {
                    cJSON_AddStringToObject(__FUNCTION__, cj_devgrp, "role", "house_modes");
                    break;
                }
                default:
                    break;
                }
            }

            cJSON_AddBoolToObject(__FUNCTION__, cj_devgrp, "persistent", devgrp_node->persistent);
            cJSON_AddBoolToObject(__FUNCTION__, cj_devgrp, "followEntry", devgrp_node->follow_entry);
            cJSON_AddStringToObject(__FUNCTION__, cj_devgrp, "packageId", devgrp_node->package_id);
        }
    }

    return cj_devgrp;
}



/**
 * @brief main functions to initialize the device-groups
 *
 */
void ezlopi_device_group_init(void)
{
    char* devgrp_id_list_str = ezlopi_nvs_read_device_groups();
    if (devgrp_id_list_str)
    {
        cJSON* cj_devgrp_ids = cJSON_Parse(__FUNCTION__, devgrp_id_list_str);
        if (cj_devgrp_ids)
        {
            int array_size = cJSON_GetArraySize(cj_devgrp_ids);
            for (int i = 0; i < array_size; i++)
            {
                cJSON* cj_devgrp_id = cJSON_GetArrayItem(cj_devgrp_ids, i);
                if (cj_devgrp_id && cj_devgrp_id->valuedouble)
                {
                    uint32_t tmp_devgrp_id = (uint32_t)cj_devgrp_id->valuedouble;

                    char tmp_buffer[32];
                    snprintf(tmp_buffer, sizeof(tmp_buffer), "%08x", tmp_devgrp_id);
                    ezlopi_cloud_update_device_group_id((uint32_t)tmp_devgrp_id);

                    char* devgrp_str = ezlopi_nvs_read_str(tmp_buffer);
                    if (devgrp_str)
                    {
                        cJSON* cj_devgrp = cJSON_Parse(__FUNCTION__, devgrp_str);
                        if (cj_devgrp)
                        {
                            l_ezlopi_device_grp_t* new_devgrp_node = __device_group_populate(cj_devgrp, tmp_devgrp_id);
                            cJSON_Delete(__FUNCTION__, cj_devgrp);

                            if (new_devgrp_node)
                            {
                                cJSON* cj_new_devgrp_node = ezlopi_core_device_group_create_cjson(new_devgrp_node);
                                // CJSON_TRACE("new_devgrp_node", cj_new_scene);
                                cJSON_Delete(__FUNCTION__, cj_new_devgrp_node);
                            }
                        }

                        ezlopi_free(__FUNCTION__, devgrp_str);
                    }
                }

            }

            cJSON_Delete(__FUNCTION__, cj_devgrp_ids);
        }

        ezlopi_free(__FUNCTION__, devgrp_id_list_str);
    }

    ezlopi_core_device_group_print(l_device_grp_head);
}
