#include <ctype.h>

#include "ezlopi_core_factory_info.h"
#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_core_errors.h"
#include "ezlopi_core_nvs.h"

#include "ezlopi_core_reset.h"
#include "ezlopi_cloud_items.h"
#include "ezlopi_cloud_constants.h"

#include "EZLOPI_USER_CONFIG.h"
#include "../../build/config/sdkconfig.h"

static l_ezlopi_device_t *l_device_head = NULL;

static volatile uint32_t g_store_dev_config_with_id = 0;
static s_ezlopi_cloud_controller_t s_controller_information;

static ezlopi_error_t ezlopi_device_parse_json_v3(cJSON *cj_config);
static void ezlopi_device_free_single(l_ezlopi_device_t *device);
#if (1 == ENABLE_TRACE)
#if 0 // Defined but not used
static void ezlopi_device_print_controller_cloud_information_v3(void);
#endif
#endif

static void ezlopi_device_free_item(l_ezlopi_item_t *items);
static void ezlopi_device_free_setting(l_ezlopi_device_settings_v3_t *settings);
static void ezlopi_device_free_all_device_setting(l_ezlopi_device_t *curr_device);

//-------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------
static int ____store_bool_in_nvs_dev_mod_info(uint32_t nvs_device_id, const char *string_key, bool bool_value)
{
    int ret = 0;
    char __device_id_str[32];
    snprintf(__device_id_str, sizeof(__device_id_str), "%08x", nvs_device_id); // convert (uint32_t) to ('0x1002e001')

    char *device_mod_str = ezlopi_nvs_read_str(__device_id_str);
    if (device_mod_str)
    {
        cJSON *cj_target_dev_mod = cJSON_Parse(__FUNCTION__, device_mod_str);
        if (cj_target_dev_mod)
        {
            // CJSON_TRACE("Prev_dev_mod:", cj_target_dev_mod);

            cJSON *cj_get_dev_name = cJSON_GetObjectItem(__FUNCTION__, cj_target_dev_mod, string_key);
            if (cj_get_dev_name)
            {
                cJSON_DeleteItemFromObject(__FUNCTION__, cj_target_dev_mod, string_key); // delete the old info
            }

            cJSON_AddBoolToObject(__FUNCTION__, cj_target_dev_mod, string_key, bool_value); // add the new info
            CJSON_TRACE("new_dev_mod:", cj_target_dev_mod);

            // Now update the 'nvs_dev_id'
            char *updated_target_dev_mod_str = cJSON_PrintBuffered(__FUNCTION__, cj_target_dev_mod, 1024, false);
            TRACE_D("length of 'updated_target_dev_mod_str': %d", strlen(updated_target_dev_mod_str));

            if (updated_target_dev_mod_str)
            {
                if (EZPI_SUCCESS == ezlopi_nvs_write_str(updated_target_dev_mod_str, strlen(updated_target_dev_mod_str), (const char *)__device_id_str))
                {
                    TRACE_S("Device_modification info updated.");
                    ret = 1;
                }
                else
                {
                    TRACE_E("Device_modification info update failed!");
                }

                ezlopi_free(__FUNCTION__, updated_target_dev_mod_str);
            }

            cJSON_Delete(__FUNCTION__, cj_target_dev_mod);
        }

        ezlopi_free(__FUNCTION__, device_mod_str);
    }
    else
    {
        // create 'new_dev_mod' cjson and store info into nvs
        cJSON *cj_new_dev_mod = cJSON_CreateObject(__FUNCTION__);
        if (cj_new_dev_mod)
        {
            cJSON_AddBoolToObject(__FUNCTION__, cj_new_dev_mod, string_key, bool_value); // add the new info
            CJSON_TRACE("new_dev_mod:", cj_new_dev_mod);

            char *new_dev_mod_str = cJSON_PrintBuffered(__FUNCTION__, cj_new_dev_mod, 1024, false);
            TRACE_D("length of 'new_dev_mod_str': %d", strlen(new_dev_mod_str));

            if (new_dev_mod_str)
            {
                if (EZPI_SUCCESS == ezlopi_nvs_write_str(new_dev_mod_str, strlen(new_dev_mod_str), (const char *)__device_id_str))
                {
                    TRACE_S("New Device_modification info stored.");
                    ret = 1;
                }
                else
                {
                    TRACE_E("New Device_modification info store failed!");
                }

                ezlopi_free(__FUNCTION__, new_dev_mod_str);
            }

            cJSON_Delete(__FUNCTION__, cj_new_dev_mod);
        }
    }
    return ret;
}
static int ____store_string_in_nvs_dev_mod_info(uint32_t nvs_device_id, const char *string_key, const char *string_value)
{
    int ret = 0;
    char __device_id_str[32];
    snprintf(__device_id_str, sizeof(__device_id_str), "%08x", nvs_device_id); // convert (uint32_t) to ('0x1002e001')

    char *device_mod_str = ezlopi_nvs_read_str(__device_id_str);
    if (device_mod_str)
    {
        cJSON *cj_target_dev_mod = cJSON_Parse(__FUNCTION__, device_mod_str);
        if (cj_target_dev_mod)
        {
            // CJSON_TRACE("Prev_dev_mod:", cj_target_dev_mod);

            cJSON *cj_get_dev_name = cJSON_GetObjectItem(__FUNCTION__, cj_target_dev_mod, string_key);
            if (cj_get_dev_name)
            {
                cJSON_DeleteItemFromObject(__FUNCTION__, cj_target_dev_mod, string_key); // delete the old info
            }

            cJSON_AddStringToObject(__FUNCTION__, cj_target_dev_mod, string_key, string_value); // add the new info

            CJSON_TRACE("new_dev_mod:", cj_target_dev_mod);

            // Now update the 'nvs_dev_id'
            char *updated_target_dev_mod_str = cJSON_PrintBuffered(__FUNCTION__, cj_target_dev_mod, 1024, false);
            TRACE_D("length of 'updated_target_dev_mod_str': %d", strlen(updated_target_dev_mod_str));

            if (updated_target_dev_mod_str)
            {
                if (EZPI_SUCCESS == ezlopi_nvs_write_str(updated_target_dev_mod_str, strlen(updated_target_dev_mod_str), (const char *)__device_id_str))
                {
                    TRACE_S("Device_modification info updated.");
                    ret = 1;
                }
                else
                {
                    TRACE_E("Device_modification info update failed!");
                }

                ezlopi_free(__FUNCTION__, updated_target_dev_mod_str);
            }

            cJSON_Delete(__FUNCTION__, cj_target_dev_mod);
        }

        ezlopi_free(__FUNCTION__, device_mod_str);
    }
    else
    {
        // create 'new_dev_mod' cjson and store info into nvs
        cJSON *cj_new_dev_mod = cJSON_CreateObject(__FUNCTION__);
        if (cj_new_dev_mod)
        {
            cJSON_AddStringToObject(__FUNCTION__, cj_new_dev_mod, string_key, string_value);

            CJSON_TRACE("new_dev_mod:", cj_new_dev_mod);

            char *new_dev_mod_str = cJSON_PrintBuffered(__FUNCTION__, cj_new_dev_mod, 1024, false);
            TRACE_D("length of 'new_dev_mod_str': %d", strlen(new_dev_mod_str));

            if (new_dev_mod_str)
            {
                if (EZPI_SUCCESS == ezlopi_nvs_write_str(new_dev_mod_str, strlen(new_dev_mod_str), (const char *)__device_id_str))
                {
                    TRACE_S("New Device_modification info stored.");
                    ret = 1;
                }
                else
                {
                    TRACE_E("New Device_modification info store failed!");
                }

                ezlopi_free(__FUNCTION__, new_dev_mod_str);
            }

            cJSON_Delete(__FUNCTION__, cj_new_dev_mod);
        }
    }
    return ret;
}
//-------------------------------------------------------------------------------------------------------
static int ____store_dev_mod_room_id_in_nvs(uint32_t device_id, const char *new_room_id_str) // this should contain new modificaton data for '_id'
{
    int ret = 0;
    if ((0 < device_id) && new_room_id_str)
    {
        ret = ____store_string_in_nvs_dev_mod_info(device_id, "dev_mod_room_id", new_room_id_str); //  { "dev_mod_xx" : "...new_room_id..." }
    }
    return ret;
}
static int ____store_dev_mod_parent_room_flag_in_nvs(uint32_t device_id, bool parent_room_flag) // this should contain new modificaton data for '_id'
{
    int ret = 0;
    if (0 < device_id)
    {
        ret = ____store_bool_in_nvs_dev_mod_info(device_id, "dev_mod_parent_room", parent_room_flag); //  { "dev_mod_xx" : "...new_room_id..." }
    }
    return ret;
}

static void __change_room_id_in_device_ll_and_nvs(l_ezlopi_device_t *curr_node, uint32_t compare_parent_id, const char *room_id_str, cJSON *cj_separate_child_devices)
{
    if (curr_node && compare_parent_id && room_id_str)
    {
        if ((NULL != curr_node->next) && (curr_node->next->cloud_properties.parent_device_id == compare_parent_id))
        {
            __change_room_id_in_device_ll_and_nvs(curr_node->next, compare_parent_id, room_id_str, cj_separate_child_devices);
        }

        // assign same 'room_id' to 'child_device_node' as jn 'parent_device_node'
        if (cj_separate_child_devices)
        {
            bool change_to_new_room_id = true;
            // int idx = 0;
            cJSON *cj_separate_child = NULL;
            // while (NULL != (cj_separate_child = cJSON_GetArrayItem(cj_separate_child_devices, idx))) // ["102ea001" , "102ea002" ...]
            cJSON_ArrayForEach(cj_separate_child, cj_separate_child_devices)
            {
                uint32_t _id_to_avoid = strtoul(cj_separate_child->valuestring, NULL, 16);
                if (_id_to_avoid == curr_node->cloud_properties.device_id)
                {
                    change_to_new_room_id = false; // 'child_node_device_id' should not be listed in 'cj_separate_child_devices'
                    break;
                }
                // idx++;
            }

            if (change_to_new_room_id)
            {
                curr_node->cloud_properties.room_id = strtoul(room_id_str, NULL, 16);                 // modify in ll
                ____store_dev_mod_room_id_in_nvs(curr_node->cloud_properties.device_id, room_id_str); // modify in nvs

                curr_node->cloud_properties.parent_room = (curr_node->cloud_properties.parent_device_id == 0) ? false : true;
                ____store_dev_mod_parent_room_flag_in_nvs(curr_node->cloud_properties.device_id, curr_node->cloud_properties.parent_room);
            }
            else // since node is listed in 'separateChildDevices_list'
            {
                if (true == curr_node->cloud_properties.parent_room) // 'child' is put in seperate from 'parent' room
                {
                    curr_node->cloud_properties.parent_room = false; // so, bool = false
                    ____store_dev_mod_parent_room_flag_in_nvs(curr_node->cloud_properties.device_id, curr_node->cloud_properties.parent_room);
                }
            }
        }
        else
        {
            // check if parent or child
            if (curr_node->cloud_properties.parent_device_id == 0) // parent node
            {
                curr_node->cloud_properties.room_id = strtoul(room_id_str, NULL, 16);                 // modify in ll
                ____store_dev_mod_room_id_in_nvs(curr_node->cloud_properties.device_id, room_id_str); // modify in nvs

                if (true == curr_node->cloud_properties.parent_room)
                {
                    curr_node->cloud_properties.parent_room = false;
                    ____store_dev_mod_parent_room_flag_in_nvs(curr_node->cloud_properties.device_id, curr_node->cloud_properties.parent_room);
                }
            }
            else // child node
            {
                if (true == curr_node->cloud_properties.parent_room) // checking if 'bool' was previously set 'false'
                {
                    curr_node->cloud_properties.room_id = strtoul(room_id_str, NULL, 16);                 // modify in ll
                    ____store_dev_mod_room_id_in_nvs(curr_node->cloud_properties.device_id, room_id_str); // modify in nvs

                    // curr_node->cloud_properties.parent_room = true;
                    // ____store_dev_mod_parent_room_flag_in_nvs(curr_node->cloud_properties.device_id, curr_node->cloud_properties.parent_room);
                }
                else
                {
                    TRACE_E("Child_node -> parentroom_flag : 'false' ; must mannually set 'child_node[%08x]' to parent-room-id", curr_node->cloud_properties.device_id);
                }
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------------
static int __modify_dev_mod_name_in_nvs(uint32_t device_id, const char *new_dev_name) // this should contain new modificaton data for '_id'
{
    int ret = 0;
    if ((0 < device_id) && new_dev_name)
    {
        ret = ____store_string_in_nvs_dev_mod_info(device_id, "dev_mod_name", new_dev_name); //  { "dev_mod_name" : "...new_dev_name..." }
    }
    return ret;
}
void ezlopi_device_name_set_by_device_id(uint32_t a_device_id, const char *new_dev_name)
{
    if (a_device_id && new_dev_name)
    {
        l_ezlopi_device_t *device_to_change = ezlopi_device_get_by_id(a_device_id);
        if (device_to_change)
        {
            snprintf(device_to_change->cloud_properties.device_name, sizeof(device_to_change->cloud_properties.device_name), "%s", new_dev_name);
            __modify_dev_mod_name_in_nvs(a_device_id, new_dev_name);
        }
    }
}
//-------------------------------------------------------------------------------------------------------
void ezlopi_device_set_reset_device_armed_status(uint32_t device_id, bool armed)
{
    if (device_id)
    {
        l_ezlopi_device_t *device_to_change = ezlopi_device_get_by_id(device_id);
        if (device_to_change)
        {
            device_to_change->cloud_properties.armed = armed;
            s_controller_information.armed = armed;
        }
    }
}

void ezlopi_device_set_device_room_id(uint32_t device_id, const char *room_id_str, cJSON *cj_separate_child_devices)
{
    if (device_id && room_id_str)
    {
        l_ezlopi_device_t *device_to_change = ezlopi_device_get_by_id(device_id);
        if (device_to_change)
        {
            if ((NULL != device_to_change->next) &&
                (device_to_change->cloud_properties.device_id == device_to_change->next->cloud_properties.parent_device_id) &&
                (0 == device_to_change->cloud_properties.parent_device_id))
            {
                TRACE_W("PARENT_TREE_ID: [%#x]", device_to_change->cloud_properties.device_id);
                __change_room_id_in_device_ll_and_nvs(device_to_change, device_to_change->cloud_properties.device_id, room_id_str, cj_separate_child_devices);
            }
            else // parent-device [without device-tree]     or   single 'child_node' of a device-tree
            {
                device_to_change->cloud_properties.room_id = strtoul(room_id_str, NULL, 16);
                ____store_dev_mod_room_id_in_nvs(device_to_change->cloud_properties.device_id, room_id_str);

                device_to_change->cloud_properties.parent_room = false; // default choice
                // must check if new 'room_id' matches 'parent-device-room_id'
                l_ezlopi_device_t *check_parent = ezlopi_device_get_by_id(device_to_change->cloud_properties.parent_device_id);
                if (NULL != check_parent &&
                    (check_parent->cloud_properties.room_id == device_to_change->cloud_properties.room_id))
                {
                    device_to_change->cloud_properties.parent_room = true;
                }
                ____store_dev_mod_parent_room_flag_in_nvs(device_to_change->cloud_properties.device_id, device_to_change->cloud_properties.parent_room);
            }
        }
    }
}

s_ezlopi_cloud_controller_t *ezlopi_device_get_controller_information(void)
{
    return &s_controller_information;
}

l_ezlopi_device_t *ezlopi_device_get_head(void)
{
    return l_device_head;
}

l_ezlopi_device_t *ezlopi_device_get_by_id(uint32_t device_id)
{
    l_ezlopi_device_t *device_node = ezlopi_device_get_head();

    while (device_node)
    {
        if (device_id == device_node->cloud_properties.device_id)
        {
            break;
        }
        device_node = device_node->next;
    }

    return device_node;
}

l_ezlopi_device_t *ezlopi_device_add_device(cJSON *cj_device, const char *last_name)
{
    l_ezlopi_device_t *new_device = ezlopi_malloc(__FUNCTION__, sizeof(l_ezlopi_device_t));
    if (new_device)
    {
        char tmp_device_name[32];
        memset(tmp_device_name, 0, sizeof(tmp_device_name));
        memset(new_device, 0, sizeof(l_ezlopi_device_t));

        // 1. generate and update device_ID for ll
        {
            CJSON_GET_ID(new_device->cloud_properties.device_id, cJSON_GetObjectItem(__FUNCTION__, cj_device, ezlopi_device_id_str));
            CJSON_GET_ID(new_device->cloud_properties.parent_device_id, cJSON_GetObjectItem(__FUNCTION__, cj_device, "child_linked_parent_id"));

            TRACE_D("Device Id (before): %08x", new_device->cloud_properties.device_id);
            if (new_device->cloud_properties.device_id)
            {
                l_ezlopi_device_t *curr_dev_node = l_device_head;
                while (curr_dev_node)
                {
                    if (curr_dev_node->cloud_properties.device_id == new_device->cloud_properties.device_id)
                    {
                        g_store_dev_config_with_id = 1;
                        new_device->cloud_properties.device_id = ezlopi_cloud_generate_device_id();
                        break;
                    }

                    curr_dev_node = curr_dev_node->next;
                }

                ezlopi_cloud_update_device_id(new_device->cloud_properties.device_id);
            }
            else
            {
                new_device->cloud_properties.device_id = ezlopi_cloud_generate_device_id();
                CJSON_ASSIGN_ID(cj_device, new_device->cloud_properties.device_id, ezlopi_device_id_str);
                CJSON_ASSIGN_ID(cj_device, new_device->cloud_properties.device_id, "child_linked_parent_id");
                g_store_dev_config_with_id = 1;
            }
            TRACE_D("Device Id (after): %08x", new_device->cloud_properties.device_id);
        }

        // 2. Add default Values
        {
            // A. Populate Device_name
            CJSON_GET_VALUE_STRING_BY_COPY(cj_device, ezlopi_dev_name_str, tmp_device_name);
            if (NULL != last_name)
            {
                snprintf(new_device->cloud_properties.device_name, sizeof(new_device->cloud_properties.device_name), "%s_%s", tmp_device_name, last_name);
            }
            else
            {
                snprintf(new_device->cloud_properties.device_name, sizeof(new_device->cloud_properties.device_name), "%s", tmp_device_name);
            }

            // B. Populate 'room_id' & 'parent_room' flag
            new_device->cloud_properties.room_id = 0;
            if (new_device->cloud_properties.parent_device_id >= DEVICE_ID_START)
            {
                // TRACE_S("child [%08x]----- linked to ----> parentId [%08x]", new_device->cloud_properties.device_id, new_device->cloud_properties.parent_device_id);
                new_device->cloud_properties.parent_room = true;
            }

            // C. Populate 'Security config link.'
            snprintf(new_device->cloud_properties.protect_config, sizeof(new_device->cloud_properties.protect_config), "%s", ezlopi_default_str);
        }

        // 3. Check for modified info , stored in nvs. If not ; procced as usual.
        {
            char __device_id_str[32];
            snprintf(__device_id_str, sizeof(__device_id_str), "%08x", new_device->cloud_properties.device_id); // convert (uint32_t) to ('0x1002e001')

            char *device_mod_str = ezlopi_nvs_read_str(__device_id_str); // use 'device_id' generated after parent/child-categorization.
            if (device_mod_str)
            {
                cJSON *cj_target_dev_mod = cJSON_Parse(__FUNCTION__, device_mod_str);
                if (cj_target_dev_mod)
                {
                    // A. check for "dev_mod_name" in ('0x1002e001') ---> use the modified name
                    cJSON *cj_name = cJSON_GetObjectItem(__FUNCTION__, cj_target_dev_mod, "dev_mod_name");
                    if (cj_name && cj_name->valuestring && cj_name->str_value_len)
                    {
                        if (0 < strlen(new_device->cloud_properties.device_name))
                        {
                            memset(new_device->cloud_properties.device_name, 0, sizeof(new_device->cloud_properties.device_name));
                        }
                        snprintf(new_device->cloud_properties.device_name, sizeof(new_device->cloud_properties.device_name), "%s", cj_name->valuestring);
                    }

                    // B. check for "dev_mod_room_id" in ('0x1002e001') ---> use the modified room_id
                    cJSON *cj_room_id = cJSON_GetObjectItem(__FUNCTION__, cj_target_dev_mod, "dev_mod_room_id");
                    if (cj_room_id && cj_room_id->valuestring && cj_room_id->str_value_len)
                    {
                        new_device->cloud_properties.room_id = strtoul(cj_room_id->valuestring, NULL, 16);
                    }

                    // C. Check for 'dev_mod_parent_room_flag'
                    cJSON *cj_parent_room_flag = cJSON_GetObjectItem(__FUNCTION__, cj_target_dev_mod, "dev_mod_parent_room");
                    if (cj_parent_room_flag && cJSON_IsBool(cj_parent_room_flag))
                    {
                        new_device->cloud_properties.parent_room = (cJSON_False == cj_parent_room_flag->type) ? false : true;
                    }

                    cJSON_Delete(__FUNCTION__, cj_target_dev_mod);
                }

                ezlopi_free(__FUNCTION__, device_mod_str);
            }
        }

        TRACE_D("Device name: %s", new_device->cloud_properties.device_name);
        TRACE_D("Device_room_id: %08x", new_device->cloud_properties.room_id);
        TRACE_D("Device parentRoom: %d", new_device->cloud_properties.parent_room);
        // TRACE_D("Device protectConfig: %s", new_device->cloud_properties.protect_config);

        // 4. Add the "NODE" to ll
        if (NULL == l_device_head)
        {
            l_device_head = new_device;
        }
        else
        {
            l_ezlopi_device_t *curr_device = l_device_head;
            while (curr_device->next)
            {
                curr_device = curr_device->next;
            }

            curr_device->next = new_device;
        }
    }

    return new_device;
}

static void ____ezlopi_device_clear_bottom_children(l_ezlopi_device_t *curr_node, uint32_t compare_parent_id)
{
    if (curr_node)
    {
        if ((NULL != curr_node->next) && (curr_node->next->cloud_properties.parent_device_id == compare_parent_id))
        {
            ____ezlopi_device_clear_bottom_children(curr_node->next, compare_parent_id);
        }

        l_ezlopi_device_t *curr_device = l_device_head;
        while (curr_device->next)
        {
            // TRACE_D("Child-Device-ID: %08x", curr_device->next->cloud_properties.device_id);
            if (curr_device->next == curr_node)
            {
                // TRACE_E("tree_member_id: %08x", curr_device->next->cloud_properties.device_id);
                l_ezlopi_device_t *free_device = curr_device->next;
                curr_device->next = curr_device->next->next;
                free_device->next = NULL;
                ezlopi_device_free_single(free_device);
                break;
            }
            curr_device = curr_device->next;
        }
    }
}

static void __ezlopi_device_free_parent_tree(l_ezlopi_device_t *parent_device, uint32_t parent_dev_id)
{
    if (parent_device && l_device_head && (parent_dev_id > 0))
    {
        /*Clearing only the child nodes first*/
        ____ezlopi_device_clear_bottom_children(parent_device, parent_dev_id);
    }
}

void ezlopi_device_free_device(l_ezlopi_device_t *device)
{
    if (device && l_device_head)
    {
        if ((NULL != device->next) &&
            (device->cloud_properties.device_id == device->next->cloud_properties.parent_device_id) &&
            (0 == device->cloud_properties.parent_device_id))
        {
            TRACE_W("PARENT_TREE_ID: [%#x]", device->cloud_properties.device_id);

            __ezlopi_device_free_parent_tree(device, device->cloud_properties.device_id);
        }
        else
        {
            if (l_device_head == device)
            {
                l_device_head = l_device_head->next;
                device->next = NULL;
                TRACE_D("Head Device-ID: %08x", device->cloud_properties.device_id);
                ezlopi_device_free_single(device);
            }
            else
            {
                l_ezlopi_device_t *curr_device = l_device_head;
                while (curr_device->next)
                {
                    // TRACE_D("Device-ID: %08x", curr_device->next->cloud_properties.device_id);
                    if (curr_device->next == device)
                    {
                        TRACE_E("To free Device-ID: %08x", curr_device->next->cloud_properties.device_id);
                        l_ezlopi_device_t *free_device = curr_device->next;
                        curr_device->next = curr_device->next->next;
                        free_device->next = NULL;
                        ezlopi_device_free_single(free_device);
                        break;
                    }
                    curr_device = curr_device->next;
                }
            }
        }
    }
}

void ezlopi_device_free_device_by_item(l_ezlopi_item_t *item)
{
    if (item)
    {
        l_ezlopi_device_t *device_node = l_device_head;
        while (device_node)
        {
            l_ezlopi_item_t *item_node = device_node->items;
            while (item_node)
            {
                if (item_node == item)
                {
                    ezlopi_device_free_device(device_node);
                    return;
                }

                item_node = item_node->next;
            }

            device_node = device_node->next;
        }
    }
}

l_ezlopi_item_t *ezlopi_device_get_item_by_id(uint32_t item_id)
{
    l_ezlopi_item_t *item_to_return = NULL;
    l_ezlopi_device_t *device_node = l_device_head;

    while (device_node)
    {
        l_ezlopi_item_t *item_node = device_node->items;
        while (item_node)
        {
            if (item_id == item_node->cloud_properties.item_id)
            {
                item_to_return = item_node;
                break;
            }
            item_node = item_node->next;
        }

        if (item_to_return)
        {
            break;
        }

        device_node = device_node->next;
    }

    return item_to_return;
}

l_ezlopi_item_t *ezlopi_device_add_item_to_device(l_ezlopi_device_t *device, ezlopi_error_t (*item_func)(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg))
{
    l_ezlopi_item_t *new_item = NULL;
    if (device)
    {
        new_item = ezlopi_malloc(__FUNCTION__, sizeof(l_ezlopi_item_t));
        if (new_item)
        {
            memset(new_item, 0, sizeof(l_ezlopi_item_t));
            new_item->func = item_func;
            new_item->cloud_properties.device_id = device->cloud_properties.device_id;

            if (NULL == device->items)
            {
                device->items = new_item;
            }
            else
            {
                l_ezlopi_item_t *curr_item = device->items;
                while (curr_item->next)
                {
                    curr_item = curr_item->next;
                }

                curr_item->next = new_item;
            }
        }
    }

    return new_item;
}

void ezlopi_device_prepare(void)
{
    s_controller_information.battery_powered = false;
    snprintf(s_controller_information.device_type_id, sizeof(s_controller_information.device_type_id), ezlopi_ezlopi_str);
    s_controller_information.gateway_id[0] = '\0';
    s_controller_information.parent_device_id[0] = '\0';
    s_controller_information.persistent = true;
    s_controller_information.reachable = true;
    // s_controller_information.room_id[0] = '\0';
    s_controller_information.security = "no";
    s_controller_information.service_notification = false;
    s_controller_information.ready = true;
    s_controller_information.status = "idle";

#if (EZLOPI_DEVICE_TYPE_TEST_DEVICE == EZLOPI_DEVICE_TYPE)
    char *config_string = ezlopi_config_test;
#else
    char *config_string = ezlopi_factory_info_v3_get_ezlopi_config();
#endif

    if (config_string)
    {
        cJSON *cj_config = cJSON_ParseWithRef(__FUNCTION__, config_string);

        if (cj_config)
        {

            if (EZPI_SUCCESS != ezlopi_device_parse_json_v3(cj_config))
            {
                TRACE_E("parsing devices-config failed!!!!");

#if defined(CONFIG_IDF_TARGET_ESP32)
                cJSON_AddStringToObject(__FUNCTION__, cj_config, ezlopi_chipset_str, ezlopi_ESP32_str);
#elif defined(CONFIG_IDF_TARGET_ESP32S3)
                cJSON_AddStringToObject(__FUNCTION__, cj_config, ezlopi_chipset_str, ezlopi_ESP32S3_str);
#elif defined(CONFIG_IDF_TARGET_ESP32S2)
                cJSON_AddStringToObject(__FUNCTION__, cj_config, ezlopi_chipset_str, ezlopi_ESP32S2_str);
#elif defined(CONFIG_IDF_TARGET_ESP32C3)
                cJSON_AddStringToObject(__FUNCTION__, cj_config, ezlopi_chipset_str, ezlopi_ESP32C3_str);
#endif
                char *tmp_str = cJSON_PrintUnformatted(__FUNCTION__, cj_config);
                if (tmp_str)
                {
                    ezlopi_factory_info_v3_set_ezlopi_config(cj_config);
                    TRACE_D("added-chipset: %s", tmp_str);
                    vTaskDelay(1000);
                    free(tmp_str);
                    EZPI_CORE_reset_reboot();
                }
            }
            else
            {
                ezlopi_factory_info_v3_set_ezlopi_config(cj_config);
            }

            cJSON_Delete(__FUNCTION__, cj_config);
        }

#if (EZLOPI_DEVICE_TYPE_TEST_DEVICE != EZLOPI_DEVICE_TYPE)
        ezlopi_free(__FUNCTION__, config_string);
#endif
    }
    else
    {
        TRACE_E("device-config: null");
    }
}

///////// Print functions start here ////////////
#if (1 == ENABLE_TRACE)

#if 0 // Defined but not used 
static void ezlopi_device_print_controller_cloud_information_v3(void)
{
    TRACE_I("Armed: %d", s_controller_information.armed);
    TRACE_I("Battery Powered: %d", s_controller_information.battery_powered);
    TRACE_I("Device Type Id: %.*s", sizeof(s_controller_information.device_type_id), s_controller_information.device_type_id);
    TRACE_I("Gateway Id: %.*s", sizeof(s_controller_information.gateway_id), s_controller_information.gateway_id);
    TRACE_I("Parent Device Id: %.*s", sizeof(s_controller_information.parent_device_id), s_controller_information.parent_device_id);
    TRACE_I("Persistent: %d", s_controller_information.persistent);
    TRACE_I("Reachable: %d", s_controller_information.reachable);
    TRACE_I("Room Id: %.*s", sizeof(s_controller_information.room_id), s_controller_information.room_id);
    TRACE_I("Security: %s", s_controller_information.security ? s_controller_information.security : ezlopi_null_str);
    TRACE_I("Service Notification: %d", s_controller_information.service_notification);
    TRACE_I("Ready: %d", s_controller_information.ready);
    TRACE_I("Status: %s", s_controller_information.status ? s_controller_information.status : ezlopi_null_str);
}

static void ezlopi_device_print_interface_digital_io(l_ezlopi_item_t *item)
{
    _D(" |~~~|- item->interface.gpio.gpio_in.enable: %s", item->interface.gpio.gpio_in.enable ? ezlopi_true_str : ezlopi_false_str);
    TRACE_D(" |~~~|- item->interface.gpio.gpio_in.gpio_num: %d", item->interface.gpio.gpio_in.gpio_num);
    TRACE_D(" |~~~|- item->interface.gpio.gpio_in.invert: %s", item->interface.gpio.gpio_in.invert ? ezlopi_true_str : ezlopi_false_str);
    TRACE_D(" |~~~|- item->interface.gpio.gpio_in.value: %d", item->interface.gpio.gpio_in.value);
    TRACE_D(" |~~~|- item->interface.gpio.gpio_in.pull: %d", item->interface.gpio.gpio_in.pull);
    TRACE_D(" |~~~|- item->interface.gpio.gpio_in.interrupt: %d", item->interface.gpio.gpio_in.interrupt);

    TRACE_D(" |~~~|- item->interface.gpio.gpio_out.enable: %s", item->interface.gpio.gpio_out.enable ? ezlopi_true_str : ezlopi_false_str);
    TRACE_D(" |~~~|- item->interface.gpio.gpio_out.gpio_num: %d", item->interface.gpio.gpio_out.gpio_num);
    TRACE_D(" |~~~|- item->interface.gpio.gpio_out.invert: %s", item->interface.gpio.gpio_out.invert ? ezlopi_true_str : ezlopi_false_str);
    TRACE_D(" |~~~|- item->interface.gpio.gpio_out.value: %d", item->interface.gpio.gpio_out.value);
    TRACE_D(" |~~~|- item->interface.gpio.gpio_out.pull: %d", item->interface.gpio.gpio_out.pull);
    TRACE_D(" |~~~|- item->interface.gpio.gpio_in.interrupt: %d", item->interface.gpio.gpio_in.interrupt);
}

static void ezlopi_device_print_interface_analogue_input(l_ezlopi_item_t *item)
{
    TRACE_D(" |~~~|- item->interface.adc.gpio_num: %d", item->interface.adc.gpio_num);
    TRACE_D(" |~~~|- item->interface.adc.resln_bit: %d", item->interface.adc.resln_bit);
}

static void ezlopi_device_print_interface_analogue_output(l_ezlopi_item_t *item) {}

static void ezlopi_device_print_interface_pwm(l_ezlopi_item_t *item)
{
    TRACE_D(" |~~~|- item->interface.pwm.gpio_num: %d", item->interface.pwm.gpio_num);
    TRACE_D(" |~~~|- item->interface.pwm.channel: %d", item->interface.pwm.channel);
    TRACE_D(" |~~~|- item->interface.pwm.speed_mode: %d", item->interface.pwm.speed_mode);
    TRACE_D(" |~~~|- item->interface.pwm.pwm_resln: %d", item->interface.pwm.pwm_resln);
    TRACE_D(" |~~~|- item->interface.pwm.freq_hz: %d", item->interface.pwm.freq_hz);
    TRACE_D(" |~~~|- item->interface.pwm.duty_cycle: %d", item->interface.pwm.duty_cycle);
}

static void ezlopi_device_print_interface_uart(l_ezlopi_item_t *item)
{
    TRACE_D(" |~~~|- item->interface.uart.channel: %d", item->interface.uart.channel);
    TRACE_D(" |~~~|- item->interface.uart.baudrate: %d", item->interface.uart.baudrate);
    TRACE_D(" |~~~|- item->interface.uart.tx: %d", item->interface.uart.tx);
    TRACE_D(" |~~~|- item->interface.uart.rx: %d", item->interface.uart.rx);
    TRACE_D(" |~~~|- item->interface.uart.enable: %d", item->interface.uart.enable);
}

static void ezlopi_device_print_interface_i2c_master(l_ezlopi_item_t *item)
{
    TRACE_D("|~~~|- item->interface.i2c_master.enable: %s", item->interface.i2c_master.enable ? ezlopi_true_str : ezlopi_false_str);
    TRACE_D("|~~~|- item->interface.i2c_master.channel: %d", item->interface.i2c_master.channel);
    TRACE_D("|~~~|- item->interface.i2c_master.clock_speed: %d", item->interface.i2c_master.clock_speed);
    TRACE_D("|~~~|- item->interface.i2c_master.scl: %d", item->interface.i2c_master.scl);
    TRACE_D("|~~~|- item->interface.i2c_master.sda: %d", item->interface.i2c_master.sda);
}

static void ezlopi_device_print_interface_spi_master(l_ezlopi_item_t *item)
{
    TRACE_D(" |~~~|- item->interface.spi_master.enable: %d", item->interface.spi_master.enable);
    TRACE_D(" |~~~|- item->interface.spi_master.channel: %d", item->interface.spi_master.channel);
    TRACE_D(" |~~~|- item->interface.spi_master.mode: %d", item->interface.spi_master.mode);
    TRACE_D(" |~~~|- item->interface.spi_master.mosi: %d", item->interface.spi_master.mosi);
    TRACE_D(" |~~~|- item->interface.spi_master.miso: %d", item->interface.spi_master.miso);
    TRACE_D(" |~~~|- item->interface.spi_master.sck: %d", item->interface.spi_master.sck);
    TRACE_D(" |~~~|- item->interface.spi_master.cs: %d", item->interface.spi_master.cs);
    TRACE_D(" |~~~|- item->interface.spi_master.clock_speed_mhz: %d", item->interface.spi_master.clock_speed_mhz);
    TRACE_D(" |~~~|- item->interface.spi_master.command_bits: %d", item->interface.spi_master.command_bits);
    TRACE_D(" |~~~|- item->interface.spi_master.addr_bits: %d", item->interface.spi_master.addr_bits);
    TRACE_D(" |~~~|- item->interface.spi_master.queue_size: %d", item->interface.spi_master.queue_size);
    TRACE_D(" |~~~|- item->interface.spi_master.transfer_sz: %d", item->interface.spi_master.transfer_sz);
    TRACE_D(" |~~~|- item->interface.spi_master.flags: %d", item->interface.spi_master.flags);
}

static void ezlopi_device_print_interface_onewire_master(l_ezlopi_item_t *item)
{
    TRACE_D(" |~~~|- item->interface.onewire_master.enable: %d", item->interface.onewire_master.enable);
    TRACE_D(" |~~~|- item->interface.onewire_master.onewire_pin: %d", item->interface.onewire_master.onewire_pin);
}


static void ezlopi_device_print_interface_type(l_ezlopi_item_t *item)
{
    switch (item->interface_type)
    {
    case EZLOPI_DEVICE_INTERFACE_DIGITAL_OUTPUT:
    case EZLOPI_DEVICE_INTERFACE_DIGITAL_INPUT:
    {
        ezlopi_device_print_interface_digital_io(item);
        break;
    }
    case EZLOPI_DEVICE_INTERFACE_ANALOG_INPUT:
    {
        ezlopi_device_print_interface_analogue_input(item);
        break;
    }
    case EZLOPI_DEVICE_INTERFACE_ANALOG_OUTPUT:
    {
        ezlopi_device_print_interface_analogue_output(item);
        break;
    }
    case EZLOPI_DEVICE_INTERFACE_PWM:
    {
        ezlopi_device_print_interface_pwm(item);
        break;
    }
    case EZLOPI_DEVICE_INTERFACE_UART:
    {
        ezlopi_device_print_interface_uart(item);
        break;
    }
    case EZLOPI_DEVICE_INTERFACE_ONEWIRE_MASTER:
    {
        ezlopi_device_print_interface_onewire_master(item);
        break;
    }
    case EZLOPI_DEVICE_INTERFACE_I2C_MASTER:
    {
        ezlopi_device_print_interface_i2c_master(item);
        break;
    }
    case EZLOPI_DEVICE_INTERFACE_SPI_MASTER:
    {
        ezlopi_device_print_interface_spi_master(item);
        break;
    }
    default:
    {
        break;
    }
    }
}
#endif
#endif
//////////////////// Print functions end here /////////////////////////
///////////////////////////////////////////////////////////////////////
static ezlopi_error_t ezlopi_device_parse_json_v3(cJSON *cjson_config)
{
    ezlopi_error_t error = EZPI_SUCCESS;

    if (cjson_config)
    {
        CJSON_TRACE("cjson-config", cjson_config);
        cJSON *cjson_chipset = cJSON_GetObjectItem(__FUNCTION__, cjson_config, ezlopi_chipset_str);

        if (cjson_chipset)
        {
            if (cJSON_IsString(cjson_chipset) && (cjson_chipset->valuestring != NULL))
            {
                char chipset_name[10];
                strncpy(chipset_name, cjson_chipset->valuestring, cjson_chipset->str_value_len);

#if defined(CONFIG_IDF_TARGET_ESP32)
                uint32_t compare_len = strlen(ezlopi_ESP32_str) > cjson_chipset->str_value_len ? strlen(ezlopi_ESP32_str) : cjson_chipset->str_value_len;
                if (strncmp(chipset_name, ezlopi_ESP32_str, compare_len) == 0)
#elif defined(CONFIG_IDF_TARGET_ESP32S3)
                uint32_t compare_len = strlen(ezlopi_ESP32S3_str) > cjson_chipset->str_value_len ? strlen(ezlopi_ESP32S3_str) : cjson_chipset->str_value_len;
                if (strncmp(chipset_name, ezlopi_ESP32S3_str, compare_len) == 0)
#elif defined(CONFIG_IDF_TARGET_ESP32C3)
                uint32_t compare_len = strlen(ezlopi_ESP32C3_str) > cjson_chipset->str_value_len ? strlen(ezlopi_ESP32C3_str) : cjson_chipset->str_value_len;
                if (strncmp(chipset_name, ezlopi_ESP32C3_str, compare_len) == 0)
#endif // Chipset
                {
                    cJSON *cjson_device_list = cJSON_GetObjectItem(__FUNCTION__, cjson_config, ezlopi_dev_detail_str);

                    if (cjson_device_list)
                    {
                        int config_dev_idx = 0;
                        cJSON *cjson_device = NULL;

                        TRACE_I("---------------------------------------------");
                        // while (NULL != (cjson_device = cJSON_GetArrayItem(cjson_device_list, config_dev_idx)))
                        cJSON_ArrayForEach(cjson_device, cjson_device_list)
                        {
                            TRACE_I("Device-%d:", config_dev_idx);

                            int id_item = 0;
                            cJSON *cj_device_id = cJSON_GetObjectItem(__FUNCTION__, cjson_device, ezlopi_device_id_str);
                            if (NULL == cj_device_id || NULL == cj_device_id->valuestring)
                            {
                                error = EZPI_ERR_JSON_PARSE_FAILED;
                            }

                            CJSON_GET_VALUE_DOUBLE(cjson_device, ezlopi_id_item_str, id_item);

                            if (0 != id_item)
                            {
                                s_ezlopi_device_v3_t *v3_device_list = ezlopi_devices_list_get_list_v3();
                                int dev_idx = 0;

                                while (NULL != v3_device_list[dev_idx].func)
                                {
                                    if (id_item == v3_device_list[dev_idx].id)
                                    {
                                        s_ezlopi_prep_arg_t device_prep_arg = {.device = &v3_device_list[dev_idx], .cjson_device = cjson_device};
                                        v3_device_list[dev_idx].func(EZLOPI_ACTION_PREPARE, NULL, (void *)&device_prep_arg, NULL);
                                        error = EZPI_SUCCESS;
                                    }
                                    dev_idx++;
                                }
                            }

                            config_dev_idx++;
                            TRACE_I("---------------------------------------------");
                        }
                    }
                }

#if (defined(CONFIG_IDF_TARGET_ESP32) || defined(CONFIG_IDF_TARGET_ESP32S3) || defined(CONFIG_IDF_TARGET_ESP32C3))
                else
                {
                    error = EZPI_ERR_JSON_PARSE_FAILED;
                    TRACE_E("Device configuration and chipset mismatch ! Device and Item assignment aborted !");
                }
#endif // CONFIG_IDF_TARGET_ESP32 OR CONFIG_IDF_TARGET_ESP32S3 OR CONFIG_IDF_TARGET_ESP32C3
            }
            else
            {
                error = EZPI_ERR_JSON_PARSE_FAILED;
                TRACE_E("Error, could not identify the chipset in the config!");
            }
        }
        else
        {
            error = EZPI_ERR_JSON_PARSE_FAILED;
            TRACE_E("Chipset not defined in the config, Device and Item assignment aborted !");
        }
    }

    return error;
}

static void ezlopi_device_free_item(l_ezlopi_item_t *items)
{
    if (items)
    {
        if (NULL != (items->next))
        {
            ezlopi_device_free_item(items->next);
        }
        // now start to clear each node from 'bottom-up'
        if (NULL != (items->user_arg) && (true == items->is_user_arg_unique))
        {
            TRACE_D("free :- 'item->user_arg' ");
            ezlopi_free(__FUNCTION__, items->user_arg);
            items->user_arg = NULL;
        }
        // TRACE_I("free item");
        ezlopi_free(__FUNCTION__, items);
    }
}

static void ezlopi_device_free_setting(l_ezlopi_device_settings_v3_t *settings)
{
    if (settings)
    {
        if (settings->next)
        {
            ezlopi_device_free_setting(settings->next);
        }
        ezlopi_free(__FUNCTION__, settings);
    }
}

static void ezlopi_device_free_single(l_ezlopi_device_t *device)
{
    if (device)
    {
        TRACE_E("free single Device-ID: %08x", device->cloud_properties.device_id);
        if (device->items)
        {
            ezlopi_device_free_item(device->items);
            device->items = NULL;
        }

        // if (device->settings)
        // {
        //     ezlopi_device_free_setting(device->settings);
        //     device->settings = NULL;
        // }
        // if (device->cloud_properties.device_type_id)
        // {
        //     ezlopi_free(__FUNCTION__, device->cloud_properties.device_type_id);
        // }
        // if (NULL != device->cloud_properties.info)
        // {
        //     cJSON_Delete(__FUNCTION__, device->cloud_properties.info);
        //     device->cloud_properties.info = NULL;
        // }

        // TRACE_S("free...device");
        ezlopi_free(__FUNCTION__, device);
    }
}

static void ezlopi_device_free_all_device_setting(l_ezlopi_device_t *curr_device)
{
    if (curr_device)
    {
        ezlopi_device_free_all_device_setting(curr_device->next);
        ezlopi_device_free_setting(curr_device->settings); // unlink settings from devices, items, rooms, etc.
    }
}

void ezlopi_device_factory_info_reset(void)
{
    // clear all 'devices', along with their 'items & settings'
    l_ezlopi_device_t *curr_device = l_device_head;
    if (curr_device)
    {
        ezlopi_device_free_all_device_setting(curr_device);
    }
}

l_ezlopi_device_settings_v3_t *ezlopi_device_add_settings_to_device_v3(l_ezlopi_device_t *device, int (*setting_func)(e_ezlopi_settings_action_t action, struct l_ezlopi_device_settings_v3 *setting, void *arg, void *user_arg))
{
    l_ezlopi_device_settings_v3_t *new_setting = NULL;
    if (device)
    {
        new_setting = ezlopi_malloc(__FUNCTION__, sizeof(l_ezlopi_device_settings_v3_t));
        if (new_setting)
        {
            memset(new_setting, 0, sizeof(l_ezlopi_device_settings_v3_t));
            new_setting->func = setting_func;

            if (NULL == device->settings)
            {
                device->settings = new_setting;
            }
            else
            {
                l_ezlopi_device_settings_v3_t *curr_setting = device->settings;
                while (curr_setting->next)
                {
                    curr_setting = curr_setting->next;
                }

                curr_setting->next = new_setting;
            }
        }
    }

    return new_setting;
}

cJSON *ezlopi_device_create_device_table_from_prop(l_ezlopi_device_t *device_prop)
{
    cJSON *cj_device = NULL;

    if (device_prop)
    {
        cj_device = cJSON_CreateObject(__FUNCTION__);
        if (cj_device)
        {
            char tmp_string[64];
            snprintf(tmp_string, sizeof(tmp_string), "%08x", device_prop->cloud_properties.device_id);
            cJSON_AddStringToObject(__FUNCTION__, cj_device, ezlopi__id_str, tmp_string);
            if (device_prop->cloud_properties.device_type_id)
            {
                cJSON_AddStringToObject(__FUNCTION__, cj_device, ezlopi_deviceTypeId_str, device_prop->cloud_properties.device_type_id);
            }
            else
            {
                cJSON_AddStringToObject(__FUNCTION__, cj_device, ezlopi_deviceTypeId_str, ezlopi_ezlopi_str);
            }

            if (device_prop->cloud_properties.parent_device_id >= DEVICE_ID_START)
            {
                snprintf(tmp_string, sizeof(tmp_string), "%08x", device_prop->cloud_properties.parent_device_id);
            }
            else
            {
                tmp_string[0] = '\0';
            }
            cJSON_AddStringToObject(__FUNCTION__, cj_device, ezlopi_parentDeviceId_str, tmp_string);
            cJSON_AddStringToObject(__FUNCTION__, cj_device, ezlopi_category_str, device_prop->cloud_properties.category);
            cJSON_AddStringToObject(__FUNCTION__, cj_device, ezlopi_subcategory_str, device_prop->cloud_properties.subcategory);
            snprintf(tmp_string, sizeof(tmp_string), "%08x", ezlopi_cloud_generate_gateway_id());
            cJSON_AddStringToObject(__FUNCTION__, cj_device, ezlopi_gatewayId_str, tmp_string);
            cJSON_AddBoolToObject(__FUNCTION__, cj_device, ezlopi_batteryPowered_str, false);
            cJSON_AddStringToObject(__FUNCTION__, cj_device, ezlopi_name_str, device_prop->cloud_properties.device_name);
            cJSON_AddStringToObject(__FUNCTION__, cj_device, ezlopi_type_str, device_prop->cloud_properties.device_type);
            cJSON_AddBoolToObject(__FUNCTION__, cj_device, ezlopi_reachable_str, true);
            cJSON_AddBoolToObject(__FUNCTION__, cj_device, ezlopi_persistent_str, true);
            cJSON_AddBoolToObject(__FUNCTION__, cj_device, ezlopi_serviceNotification_str, false);
            cJSON_AddBoolToObject(__FUNCTION__, cj_device, ezlopi_armed_str, device_prop->cloud_properties.armed);

            if (device_prop->cloud_properties.room_id)
            {
                snprintf(tmp_string, sizeof(tmp_string), "%08x", device_prop->cloud_properties.room_id);
                cJSON_AddStringToObject(__FUNCTION__, cj_device, ezlopi_roomId_str, tmp_string);
            }
            else
            {
                cJSON_AddStringToObject(__FUNCTION__, cj_device, ezlopi_roomId_str, ezlopi__str);
            }

            cJSON_AddStringToObject(__FUNCTION__, cj_device, ezlopi_security_str, ezlopi_no_str);
            cJSON_AddBoolToObject(__FUNCTION__, cj_device, ezlopi_ready_str, true);
            cJSON_AddStringToObject(__FUNCTION__, cj_device, ezlopi_status_str, ezlopi_idle_str);
            cJSON_AddBoolToObject(__FUNCTION__, cj_device, ezlopi_parent_room_str, device_prop->cloud_properties.parent_room);
            if (device_prop->cloud_properties.protect_config && (strlen(device_prop->cloud_properties.protect_config) > 0))
            {
                cJSON_AddStringToObject(__FUNCTION__, cj_device, ezlopi_protect_config_str, device_prop->cloud_properties.protect_config);
            }
            if (NULL != device_prop->cloud_properties.info)
            {
                cJSON_AddItemReferenceToObject(__FUNCTION__, cj_device, ezlopi_info_str, device_prop->cloud_properties.info);
            }
        }
    }

    return cj_device;
}
