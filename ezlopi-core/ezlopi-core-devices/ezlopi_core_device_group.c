
#include "ezlopi_core_nvs.h"
#include "ezlopi_core_device_group.h"

#include "ezlopi_cloud_constants.h"

#include "EZLOPI_USER_CONFIG.h"

//--------------------------------------------------------------------------------
//      Static Functions.
//--------------------------------------------------------------------------------

static l_ezlopi_device_grp_t* l_device_grp_head = NULL;
static l_ezlopi_item_grp_t* l_item_grp_head = NULL;

static void __ezlopi_core_device_group_print(l_ezlopi_device_grp_t* device_grp_node)
{
#if (1 == ENABLE_TRACE)
    if (device_grp_node)
    {
        TRACE_D("\t-------------------- DEVICE-GROUP --------------------");
        TRACE_D("\t|-- Group_id: %08x", device_grp_node->_id);
        TRACE_D("\t|-- Group_name: %s", device_grp_node->name);
        CJSON_TRACE("\t|-- Categories", device_grp_node->categories);
        CJSON_TRACE("\t|-- Devices", device_grp_node->devices);
        CJSON_TRACE("\t|-- Exceptions", device_grp_node->exceptions);
        TRACE_D("\t|-- Persistent: %s", device_grp_node->persistent ? "True" : "False");
        TRACE_D("\t|-- Entry_Delay: %s", (EZLOPI_DEVICE_GRP_ENTRYDELAY_NORMAL == device_grp_node->entry_delay) ? "normal"
            : (EZLOPI_DEVICE_GRP_ENTRYDELAY_LONG_EXTENDED == device_grp_node->entry_delay) ? "long_extended"
            : (EZLOPI_DEVICE_GRP_ENTRYDELAY_EXTENDED == device_grp_node->entry_delay) ? "extended"
            : (EZLOPI_DEVICE_GRP_ENTRYDELAY_INSTANT == device_grp_node->entry_delay) ? "instant"
            : "none");
        TRACE_D("\t|-- Follow_Entry: %s", device_grp_node->follow_entry ? "True" : "False");
        TRACE_D("\t|-- Role: %s", (EZLOPI_DEVICE_GRP_ROLE_HOUSE_MODES == device_grp_node->role) ? "house_modes" : "user");
        TRACE_D("\t|-- Package_Id: %s", device_grp_node->package_id);
        TRACE_D("\t|-- ----------------- ----------- --------------------");
    }
#endif
}
static void __ezlopi_core_item_group_print(l_ezlopi_item_grp_t* item_grp_node)
{
#if (1 == ENABLE_TRACE)
    if (item_grp_node)
    {
        TRACE_D("\t|------------------- ITEM-GROUP --------------------");
        TRACE_D("\t|-- Group_id: %08x", item_grp_node->_id);
        TRACE_D("\t|-- Group_name: %s", item_grp_node->name);
        TRACE_D("\t|-- Has_Getter: %s", item_grp_node->has_getter ? "True" : "False");
        TRACE_D("\t|-- Has_Setter: %s", item_grp_node->has_setter ? "True" : "False");
        TRACE_D("\t|-- Persistent: %s", item_grp_node->persistent ? "True" : "False");
        CJSON_TRACE("\t|-- Item_Names", item_grp_node->item_names);
        TRACE_D("\t|-- Value_Type: %s", item_grp_node->value_type);
        CJSON_TRACE("\t|-- Enum", item_grp_node->enum_values);
        CJSON_TRACE("\t|-- Value_Type_Family", item_grp_node->value_type_family);
        TRACE_D("\t|-- Role: %s", (EZLOPI_ITEM_GRP_ROLE_HOUSE_MODES == item_grp_node->role) ? "house_modes" : (EZLOPI_ITEM_GRP_ROLE_LIFE_SAFETY == item_grp_node->role) ? "lifeSafety" : "empty");
        CJSON_TRACE("\t|-- Info", item_grp_node->info);
        TRACE_D("\t|-- ----------------- ----------- --------------------");
    }
#endif
}


static void __edit_devgrp_from_ll(l_ezlopi_device_grp_t* req_devgrp_node, cJSON* cj_devgrp_new)
{
    CJSON_GET_VALUE_STRING_BY_COPY(cj_devgrp_new, ezlopi_name_str, req_devgrp_node->name);
    CJSON_GET_VALUE_BOOL(cj_devgrp_new, ezlopi_persistent_str, req_devgrp_node->persistent);
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
    __ezlopi_core_device_group_print(req_devgrp_node);
}
static void __edit_itemgrp_from_ll(l_ezlopi_item_grp_t* req_itemgrp_node, cJSON* cj_itemgrp_new)
{
    CJSON_GET_VALUE_STRING_BY_COPY(cj_itemgrp_new, ezlopi_name_str, req_itemgrp_node->name);
    CJSON_GET_VALUE_BOOL(cj_itemgrp_new, "hasGetter", req_itemgrp_node->has_getter);
    CJSON_GET_VALUE_BOOL(cj_itemgrp_new, "hasSetter", req_itemgrp_node->has_setter);
    CJSON_GET_VALUE_BOOL(cj_itemgrp_new, ezlopi_persistent_str, req_itemgrp_node->persistent);
    CJSON_GET_VALUE_STRING_BY_COPY(cj_itemgrp_new, "valueType", req_itemgrp_node->value_type);

    // ------------- itemNames -------------
    {
        cJSON_Delete(__FUNCTION__, req_itemgrp_node->item_names);
        cJSON * cj_item_names = cJSON_GetObjectItem(__FUNCTION__, cj_itemgrp_new, "itemNames");
        if (cj_item_names && cJSON_IsArray(cj_item_names))
        {
            req_itemgrp_node->item_names = cJSON_Duplicate(__FUNCTION__, cj_item_names, cJSON_True);
        }
    }

    // ------------- enums -------------
    {
        cJSON_Delete(__FUNCTION__, req_itemgrp_node->enum_values);
        cJSON * cj_enums = cJSON_GetObjectItem(__FUNCTION__, cj_itemgrp_new, ezlopi_enum_str);
        if (cj_enums && cJSON_IsArray(cj_enums))
        {
            req_itemgrp_node->enum_values = cJSON_Duplicate(__FUNCTION__, cj_enums, cJSON_True);
        }
    }

    // ------------- valueType_Family -------------
    {
        cJSON_Delete(__FUNCTION__, req_itemgrp_node->enum_values);
        cJSON * cj_valuetype_family = cJSON_GetObjectItem(__FUNCTION__, cj_itemgrp_new, "valueTypeFamily");
        if (cj_valuetype_family && cJSON_IsArray(cj_valuetype_family))
        {
            req_itemgrp_node->value_type_family = cJSON_Duplicate(__FUNCTION__, cj_valuetype_family, cJSON_True);
        }
    }

    // ------------- role -------------
    {
        cJSON *cj_role = cJSON_GetObjectItem(__FUNCTION__, cj_itemgrp_new, "role");
        if (cj_role && cj_role->valuestring && cj_role->str_value_len)
        {
            req_itemgrp_node->role = ((0 == strncmp(cj_role->valuestring, "house_modes", 12)) ? EZLOPI_ITEM_GRP_ROLE_HOUSE_MODES
                : (0 == strncmp(cj_role->valuestring, "lifeSafety", 12)) ? EZLOPI_ITEM_GRP_ROLE_LIFE_SAFETY
                : EZLOPI_ITEM_GRP_ROLE_EMPTY);
        }
    }

    // ------------- info -------------
    {
        cJSON_Delete(__FUNCTION__, req_itemgrp_node->enum_values);
        cJSON * cj_info = cJSON_GetObjectItem(__FUNCTION__, cj_itemgrp_new, ezlopi_info_str);
        if (cj_info && cJSON_IsArray(cj_info))
        {
            req_itemgrp_node->info = cJSON_Duplicate(__FUNCTION__, cj_info, cJSON_True);
        }
    }
    __ezlopi_core_item_group_print(req_itemgrp_node);
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
static int __ezlopi_core_item_group_edit_update_id(uint32_t itemgrp_id, cJSON* cj_itemgrp_new)
{
    int ret = 0;
    CJSON_TRACE("cj_updated_devgrp_node :", cj_itemgrp_new);

    if (itemgrp_id && cj_itemgrp_new)
    {
        l_ezlopi_item_grp_t* req_itemgrp_node = ezlopi_core_item_group_get_by_id(itemgrp_id);
        if (req_itemgrp_node)
        {
            __edit_itemgrp_from_ll(req_itemgrp_node, cj_itemgrp_new);
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
static int __ezlopi_core_device_group_edit_store_updated_to_nvs(cJSON* cj_devgrp_new)   // input from ll
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
static int __ezlopi_core_item_group_edit_store_updated_to_nvs(cJSON* cj_itemgrp_new) // input from ll
{
    int ret = 0;
    if (cj_itemgrp_new)
    {
        char* update_itemgrp_str = cJSON_PrintBuffered(__FUNCTION__, cj_itemgrp_new, 1024, false);
        TRACE_D("length of 'update_itemgrp_str': %d", strlen(update_itemgrp_str));

        if (update_itemgrp_str)
        {
            cJSON* cj_itemgrp_id = cJSON_GetObjectItem(__FUNCTION__, cj_itemgrp_new, "id");  // according to docs --> 'id'
            if (cj_itemgrp_id && cj_itemgrp_id->valuestring)
            {
                ezlopi_nvs_delete_stored_data_by_name(cj_itemgrp_id->valuestring); // delete --> '0x0..devgrp ' : '{}'
                ret = ezlopi_nvs_write_str(update_itemgrp_str, strlen(update_itemgrp_str), cj_itemgrp_id->valuestring); // write --> '0x0..devgrp ' : '{}'
            }

            ezlopi_free(__FUNCTION__, update_itemgrp_str);
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
static l_ezlopi_item_grp_t* __item_group_pop_by_id(uint32_t _id)
{
    l_ezlopi_item_grp_t* ret_itemgrp = NULL;

    if (_id == l_item_grp_head->_id)
    {
        ret_itemgrp = l_item_grp_head;
        l_item_grp_head = l_item_grp_head->next;
        ret_itemgrp->next = NULL;
    }
    else
    {
        l_ezlopi_item_grp_t* curr_itemgrp = l_item_grp_head;
        while (curr_itemgrp->next)
        {
            if (_id == curr_itemgrp->next->_id)
            {
                ret_itemgrp = curr_itemgrp->next;
                curr_itemgrp->next = curr_itemgrp->next->next;
                ret_itemgrp->next = NULL;
                break;
            }
            curr_itemgrp = curr_itemgrp->next;
        }
    }

    return ret_itemgrp;
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
static void __item_group_delete_node(l_ezlopi_item_grp_t* itemgrp_node)
{
    if (itemgrp_node)
    {
        if (NULL != itemgrp_node->item_names)
        {
            cJSON_Delete(__FUNCTION__, itemgrp_node->item_names);
            itemgrp_node->item_names = NULL;
        }

        if (NULL != itemgrp_node->value_type_family)
        {
            cJSON_Delete(__FUNCTION__, itemgrp_node->value_type_family);
            itemgrp_node->value_type_family = NULL;
        }

        if (NULL != itemgrp_node->enum_values)
        {
            cJSON_Delete(__FUNCTION__, itemgrp_node->enum_values);
            itemgrp_node->enum_values = NULL;
        }

        if (NULL != itemgrp_node->info)
        {
            cJSON_Delete(__FUNCTION__, itemgrp_node->info);
            itemgrp_node->info = NULL;
        }

        __item_group_delete_node(itemgrp_node->next);
        itemgrp_node->next = NULL;
        ezlopi_free(__FUNCTION__, itemgrp_node);
    }
}


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
            CJSON_GET_VALUE_BOOL(cj_device_grp, ezlopi_persistent_str, new_device_grp_node->persistent);
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

            __ezlopi_core_device_group_print(new_device_grp_node);
        }
    }
    return new_device_grp_node;
}
static l_ezlopi_item_grp_t* ____item_grp_create_node(cJSON* cj_item_grp, uint32_t item_grp_id)
{
    l_ezlopi_item_grp_t* new_item_grp_node = NULL;
    if (cj_item_grp)
    {
        new_item_grp_node = ezlopi_malloc(__FUNCTION__, sizeof(l_ezlopi_item_grp_t));
        if (new_item_grp_node)
        {
            memset(new_item_grp_node, 0, sizeof(l_ezlopi_item_grp_t));

            new_item_grp_node->_id = item_grp_id;

            CJSON_GET_VALUE_STRING_BY_COPY(cj_item_grp, ezlopi_name_str, new_item_grp_node->name);
            CJSON_GET_VALUE_BOOL(cj_item_grp, "hasGetter", new_item_grp_node->has_getter);
            CJSON_GET_VALUE_BOOL(cj_item_grp, "hasSetter", new_item_grp_node->has_setter);
            CJSON_GET_VALUE_BOOL(cj_item_grp, ezlopi_persistent_str, new_item_grp_node->persistent);
            CJSON_GET_VALUE_STRING_BY_COPY(cj_item_grp, ezlopi_valueType_str, new_item_grp_node->value_type);

            {
                cJSON * cj_item_names = cJSON_GetObjectItem(__FUNCTION__, cj_item_grp, "itemNames");
                if (cj_item_names && cJSON_IsArray(cj_item_names))
                {
                    new_item_grp_node->item_names = cJSON_Duplicate(__FUNCTION__, cj_item_names, cJSON_True);
                }
            }

            {
                cJSON * cj_enum = cJSON_GetObjectItem(__FUNCTION__, cj_item_grp, ezlopi_enum_str);
                if (cj_enum && cJSON_IsArray(cj_enum))
                {
                    new_item_grp_node->enum_values = cJSON_Duplicate(__FUNCTION__, cj_enum, cJSON_True);
                }
            }

            {// valueType_Family [cjson]
                if (NULL == new_item_grp_node->value_type)
                {
                    cJSON * cj_valuetype_family_list = cJSON_GetObjectItem(__FUNCTION__, cj_item_grp, "valueTypeFamily");
                    if (cj_valuetype_family_list && cJSON_IsArray(cj_valuetype_family_list))
                    {
                        new_item_grp_node->value_type_family = cJSON_Duplicate(__FUNCTION__, cj_valuetype_family_list, cJSON_True);
                    }
                }
            }

            {
                cJSON *cj_role = cJSON_GetObjectItem(__FUNCTION__, cj_item_grp, "role");
                if (cj_role && cj_role->valuestring && cj_role->str_value_len)
                {
                    new_item_grp_node->role = ((0 == strncmp(cj_role->valuestring, "house_modes", 12)) ? EZLOPI_ITEM_GRP_ROLE_HOUSE_MODES
                        : (0 == strncmp(cj_role->valuestring, "lifeSafety", 12)) ? EZLOPI_ITEM_GRP_ROLE_LIFE_SAFETY
                        : EZLOPI_ITEM_GRP_ROLE_EMPTY);
                }
            }

            {
                cJSON * cj_info = cJSON_GetObjectItem(__FUNCTION__, cj_item_grp, ezlopi_info_str);
                if (cj_info && cJSON_IsArray(cj_info))
                {
                    new_item_grp_node->info = cJSON_Duplicate(__FUNCTION__, cj_info, cJSON_True);
                }
            }

            __ezlopi_core_item_group_print(new_item_grp_node);
        }
    }
    return new_item_grp_node;
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
        // CJSON_TRACE("new-device-group", cj_device_grp);
        l_device_grp_head = ____device_grp_create_node(cj_device_grp, device_grp_id);
        new_device_grp_node = l_device_grp_head;
    }

    return new_device_grp_node;
}
static l_ezlopi_item_grp_t* __item_group_populate(cJSON* cj_item_grp, uint32_t item_grp_id)
{
    l_ezlopi_item_grp_t* new_item_grp_node = NULL;
    if (l_item_grp_head)
    {
        l_ezlopi_item_grp_t* current_item_grp = l_item_grp_head;
        while (current_item_grp->next)
        {
            current_item_grp = current_item_grp->next;
        }

        current_item_grp->next = ____item_grp_create_node(cj_item_grp, item_grp_id);
        new_item_grp_node = current_item_grp->next;
    }
    else
    {
        CJSON_TRACE("new-item-group", cj_item_grp);
        l_item_grp_head = ____item_grp_create_node(cj_item_grp, item_grp_id);
        new_item_grp_node = l_item_grp_head;
    }

    return new_item_grp_node;
}

//--------------------------------------------------------------------------------
//      Device-Group Functions
//--------------------------------------------------------------------------------

l_ezlopi_device_grp_t* ezlopi_core_device_group_get_head(void)
{
    return l_device_grp_head;
}
l_ezlopi_item_grp_t* ezlopi_core_item_group_get_head(void)
{
    return l_item_grp_head;
}


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
l_ezlopi_item_grp_t* ezlopi_core_item_group_get_by_id(uint32_t _id)
{
    l_ezlopi_item_grp_t* ret_itemgrp_node = NULL;
    l_ezlopi_item_grp_t* itemgrp_list = l_item_grp_head;
    while (itemgrp_list)
    {
        if (_id == itemgrp_list->_id)
        {
            ret_itemgrp_node = itemgrp_list;
            break;
        }
        itemgrp_list = itemgrp_list->next;
    }

    return ret_itemgrp_node;
}


int ezlopi_core_device_group_edit_by_id(uint32_t devgrp_id, cJSON* cj_devgrp_new)
{
    int ret = 0;

    if (1 == __ezlopi_core_device_group_edit_update_id(devgrp_id, cj_devgrp_new))   // modifies the ll-node with, 'devgrp_id'
    {
        l_ezlopi_device_grp_t* req_devgrp_node = ezlopi_core_device_group_get_by_id(devgrp_id); // this extract the latest ll-node
        if (req_devgrp_node)
        {
            if (1 == __ezlopi_core_device_group_edit_store_updated_to_nvs(ezlopi_core_device_group_create_cjson(req_devgrp_node)))
            {
                ret = 1;
            }
        }
    }

    return ret;
}
int ezlopi_core_item_group_edit_by_id(uint32_t itemgrp_id, cJSON* cj_itemgrp_new)
{
    int ret = 0;

    if (1 == __ezlopi_core_item_group_edit_update_id(itemgrp_id, cj_itemgrp_new))   // modifies the ll-node with, 'itemgrp_id'
    {
        l_ezlopi_item_grp_t* req_itemgrp_node = ezlopi_core_item_group_get_by_id(itemgrp_id); // this extract the latest ll-node
        if (req_itemgrp_node)
        {
            if (1 == __ezlopi_core_item_group_edit_store_updated_to_nvs(ezlopi_core_item_group_create_cjson(req_itemgrp_node)))
            {
                ret = 1;
            }
        }
    }

    return ret;
}


void ezlopi_core_device_group_remove_id_from_list(uint32_t _id)
{
    char* devgrp_id_list = ezlopi_nvs_read_device_groups();
    if (devgrp_id_list)
    {
        cJSON* cj_devgrp_id_list = cJSON_Parse(__FUNCTION__, devgrp_id_list);
        if (cj_devgrp_id_list)
        {
            uint32_t idx = 0;
            cJSON* cj_id = NULL;
            while (NULL != (cj_id = cJSON_GetArrayItem(cj_devgrp_id_list, idx)))   // since all the elements are all-ready in 'cJSON_Number'
            {
                if (cj_id->valuedouble == _id)
                {
                    cJSON_DeleteItemFromArray(__FUNCTION__, cj_devgrp_id_list, idx);
                }
                // iterate upto the last elements (incase -->  for doubles)
                idx++;
            }

            // Now to rewrite the item_group list into NVS
            char* updated_devGrpid_list_str = cJSON_PrintBuffered(__FUNCTION__, cj_devgrp_id_list, 1024, false);
            TRACE_D("length of 'updated_devGrpid_list_str': %d", strlen(updated_devGrpid_list_str));

            if (updated_devGrpid_list_str)
            {
                ezlopi_nvs_write_device_groups(updated_devGrpid_list_str);
                ezlopi_free(__FUNCTION__, updated_devGrpid_list_str);
            }

            cJSON_Delete(__FUNCTION__, cj_devgrp_id_list);
        }
        ezlopi_free(__FUNCTION__, devgrp_id_list);
    }
}
void ezlopi_core_item_group_remove_id_from_list(uint32_t _id)
{
    char* itemgrp_id_list = ezlopi_nvs_read_item_groups();
    if (itemgrp_id_list)
    {
        cJSON* cj_itemgrp_id_list = cJSON_Parse(__FUNCTION__, itemgrp_id_list);
        if (cj_itemgrp_id_list)
        {
            uint32_t idx = 0;
            cJSON* cj_id = NULL;
            while (NULL != (cj_id = cJSON_GetArrayItem(cj_itemgrp_id_list, idx)))   // since all the elements are all-ready in 'cJSON_Number'
            {
                if (cj_id->valuedouble == _id)
                {
                    cJSON_DeleteItemFromArray(__FUNCTION__, cj_itemgrp_id_list, idx);
                }
                // iterate upto the last elements (incase -->  for doubles)
                idx++;
            }

            // Now to rewrite the item_group list into NVS
            char* updated_itemGrp_id_list_str = cJSON_PrintBuffered(__FUNCTION__, cj_itemgrp_id_list, 1024, false);
            TRACE_D("length of 'updated_itemGrp_id_list_str': %d", strlen(updated_itemGrp_id_list_str));

            if (updated_itemGrp_id_list_str)
            {
                ezlopi_nvs_write_item_groups(updated_itemGrp_id_list_str);
                ezlopi_free(__FUNCTION__, updated_itemGrp_id_list_str);
            }

            cJSON_Delete(__FUNCTION__, cj_itemgrp_id_list);
        }
        ezlopi_free(__FUNCTION__, itemgrp_id_list);
    }
}


void ezlopi_core_device_group_depopulate_by_id_v2(uint32_t _id)
{
    __device_group_delete_node(__device_group_pop_by_id(_id));
}
void ezlopi_core_item_group_depopulate_by_id_v2(uint32_t _id)
{
    __item_group_delete_node(__item_group_pop_by_id(_id));
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
            CJSON_TRACE("In-cjson :", cj_devgrp_id_list);
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
                                ret += 1;   // return total number of device-grp
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
uint32_t ezlopi_core_item_group_get_list(cJSON* cj_itemgrp_array)
{
    int ret = 0;
    char* itemgrp_id_list = ezlopi_nvs_read_item_groups();
    if (itemgrp_id_list)
    {
        cJSON* cj_itemgrp_id_list = cJSON_Parse(__FUNCTION__, itemgrp_id_list);
        if (cj_itemgrp_id_list)
        {
            CJSON_TRACE("In-cjson :", cj_itemgrp_id_list);
            int array_size = cJSON_GetArraySize(cj_itemgrp_id_list);
            for (int i = 0; i < array_size; i++)
            {
                cJSON* cj_itemgrp_id = cJSON_GetArrayItem(cj_itemgrp_id_list, i);
                if (cj_itemgrp_id && cj_itemgrp_id->valuedouble)
                {
                    char tmp_buffer[32];
                    uint32_t itemgrp_id = (uint32_t)cj_itemgrp_id->valuedouble;
                    snprintf(tmp_buffer, sizeof(tmp_buffer), "%08x", itemgrp_id);
                    char* itemgrp_str = ezlopi_nvs_read_str(tmp_buffer);
                    if (itemgrp_str)
                    {
                        cJSON* cj_itemgrp_node = cJSON_Parse(__FUNCTION__, itemgrp_str);
                        if (cj_itemgrp_node)
                        {
                            char itemgrp_id_str[32];
                            snprintf(itemgrp_id_str, sizeof(itemgrp_id_str), "%08x", (uint32_t)cj_itemgrp_id->valuedouble);
                            cJSON_AddStringToObject(__FUNCTION__, cj_itemgrp_node, ezlopi__id_str, itemgrp_id_str);

                            if (!cJSON_AddItemToArray(cj_itemgrp_array, cj_itemgrp_node))
                            {
                                cJSON_Delete(__FUNCTION__, cj_itemgrp_node);
                            }
                            else
                            {
                                ret += 1;   // return total number of item-grp
                            }
                        }

                        ezlopi_free(__FUNCTION__, itemgrp_str);
                    }
                }
            }

            cJSON_Delete(__FUNCTION__, cj_itemgrp_id_list);
        }

        ezlopi_free(__FUNCTION__, itemgrp_id_list);
    }

    return ret;
}


uint32_t ezlopi_core_device_group_store_nvs_devgrp(cJSON* cj_new_device_grp)
{
    uint32_t new_dev_grp_id = 0;
    if (cj_new_device_grp)
    {
        new_dev_grp_id = ezlopi_cloud_generate_device_group_id();
        char devgrp_id_str[32];
        snprintf(devgrp_id_str, sizeof(devgrp_id_str), "%08x", new_dev_grp_id); // convert (uint32_t) to (0xC002e....)
        cJSON_AddStringToObject(__FUNCTION__, cj_new_device_grp, ezlopi__id_str, devgrp_id_str); // _id -> group_id

        char* new_devgrp_str = cJSON_PrintBuffered(__FUNCTION__, cj_new_device_grp, 1024, false);
        TRACE_D("length of 'new_devgrp_str': %d", strlen(new_devgrp_str));

        if (new_devgrp_str)
        {
            if (ezlopi_nvs_write_str(new_devgrp_str, strlen(new_devgrp_str) + 1, devgrp_id_str))
            {
                bool free_devGrp_id_list_str = 1;
                char* device_grp_id_list_str = ezlopi_nvs_read_device_groups();
                if (NULL == device_grp_id_list_str)
                {
                    device_grp_id_list_str = "[]";
                    free_devGrp_id_list_str = 0;
                    TRACE_W("DeviceGroup ids-list not found in NVS");
                }

                cJSON* cj_device_grp_id_list = cJSON_Parse(__FUNCTION__, device_grp_id_list_str);
                if (cj_device_grp_id_list)
                {
                    cJSON* cj_devgrp_id_str = cJSON_CreateNumber(__FUNCTION__, new_dev_grp_id);
                    if (cj_devgrp_id_str)
                    {
                        if (!cJSON_AddItemToArray(cj_device_grp_id_list, cj_devgrp_id_str))
                        {   // if 'new_dev_grp_id' doesnot exist, then erase the falsly store 'new_devgrp_str' in nvs.
                            cJSON_Delete(__FUNCTION__, cj_devgrp_id_str);
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
                                    TRACE_S("Device-grp list updated.");
                                }
                                else
                                {
                                    TRACE_E("Device-grp list update failed!");
                                }

                                ezlopi_free(__FUNCTION__, updated_devgrp_id_list_str);
                            }
                        }
                    }

                    cJSON_Delete(__FUNCTION__, cj_device_grp_id_list);
                }
                if (free_devGrp_id_list_str)
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
uint32_t ezlopi_core_item_group_store_nvs_itemgrp(cJSON* cj_new_item_grp)
{
    uint32_t new_item_grp_id = 0;
    if (cj_new_item_grp)
    {
        new_item_grp_id = ezlopi_cloud_generate_item_group_id();
        char itemGrp_id_str[32];
        snprintf(itemGrp_id_str, sizeof(itemGrp_id_str), "%08x", new_item_grp_id);
        cJSON_AddStringToObject(__FUNCTION__, cj_new_item_grp, ezlopi__id_str, itemGrp_id_str); // _id -> group_id

        char* new_itemgrp_str = cJSON_PrintBuffered(__FUNCTION__, cj_new_item_grp, 1024, false);
        TRACE_D("length of 'new_itemgrp_str': %d", strlen(new_itemgrp_str));

        if (new_itemgrp_str)
        {
            if (ezlopi_nvs_write_str(new_itemgrp_str, strlen(new_itemgrp_str) + 1, itemGrp_id_str))
            {
                bool free_itemGrp_id_list_str = 1;
                char* item_grp_id_list_str = ezlopi_nvs_read_item_groups();
                if (NULL == item_grp_id_list_str)
                {
                    item_grp_id_list_str = "[]";
                    free_itemGrp_id_list_str = 0;
                    TRACE_W("ItemGroup ids-list not found in NVS");
                }

                cJSON* cj_item_grp_id_list = cJSON_Parse(__FUNCTION__, item_grp_id_list_str);
                if (cj_item_grp_id_list)
                {
                    cJSON* cj_itemGrp_id_str = cJSON_CreateNumber(__FUNCTION__, new_item_grp_id);
                    if (cj_itemGrp_id_str)
                    {
                        if (!cJSON_AddItemToArray(cj_item_grp_id_list, cj_itemGrp_id_str))
                        {   // if 'new_item_grp_id' doesnot exist, then erase the falsly store 'new_itemgrp_str' in nvs.
                            cJSON_Delete(__FUNCTION__, cj_itemGrp_id_str);
                            ezlopi_nvs_delete_stored_data_by_id(new_item_grp_id);
                            new_item_grp_id = 0;
                        }
                        else
                        {
                            char* updated_itemgrp_id_list_str = cJSON_PrintBuffered(__FUNCTION__, cj_item_grp_id_list, 1024, false);
                            TRACE_D("length of 'updated_itemgrp_id_list_str': %d", strlen(updated_itemgrp_id_list_str));

                            if (updated_itemgrp_id_list_str)
                            {
                                TRACE_D("updated_itemgrp_id_list_str: %s", updated_itemgrp_id_list_str);
                                if (ezlopi_nvs_write_item_groups(updated_itemgrp_id_list_str))
                                {
                                    TRACE_S("Item-grp list updated.");
                                }
                                else
                                {
                                    TRACE_E("Item-grp list update failed!");
                                }

                                ezlopi_free(__FUNCTION__, updated_itemgrp_id_list_str);
                            }
                        }
                    }
                    cJSON_Delete(__FUNCTION__, cj_item_grp_id_list);
                }

                if (free_itemGrp_id_list_str)
                {
                    ezlopi_free(__FUNCTION__, item_grp_id_list_str);
                }
            }
            else
            {
                new_item_grp_id = 0;
            }

            ezlopi_free(__FUNCTION__, new_itemgrp_str);
        }
        else
        {
            new_item_grp_id = 0;
        }
    }

    return new_item_grp_id;
}


l_ezlopi_device_grp_t * ezlopi_core_device_group_new_devgrp_populate(cJSON *cj_new_dev_grp, uint32_t new_device_grp_id)
{
    return __device_group_populate(cj_new_dev_grp, new_device_grp_id);
}
l_ezlopi_item_grp_t * ezlopi_core_item_group_new_itemgrp_populate(cJSON *cj_new_item_grp, uint32_t new_item_grp_id)
{
    return __item_group_populate(cj_new_item_grp, new_item_grp_id);
}


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

            cJSON_AddBoolToObject(__FUNCTION__, cj_devgrp, ezlopi_persistent_str, devgrp_node->persistent);
            cJSON_AddBoolToObject(__FUNCTION__, cj_devgrp, "followEntry", devgrp_node->follow_entry);
            cJSON_AddStringToObject(__FUNCTION__, cj_devgrp, "packageId", devgrp_node->package_id);
        }
    }

    return cj_devgrp;
}
cJSON* ezlopi_core_item_group_create_cjson(l_ezlopi_item_grp_t* itemgrp_node)
{
    cJSON* cj_itemgrp = NULL;
    if (itemgrp_node)
    {
        cj_itemgrp = cJSON_CreateObject(__FUNCTION__);
        if (cj_itemgrp)
        {
            char tmp_str[16] = { 0 };
            snprintf(tmp_str, sizeof(tmp_str), "%08x", itemgrp_node->_id);
            cJSON_AddStringToObject(__FUNCTION__, cj_itemgrp, ezlopi__id_str, tmp_str);
            cJSON_AddStringToObject(__FUNCTION__, cj_itemgrp, ezlopi_name_str, itemgrp_node->name);
            cJSON_AddBoolToObject(__FUNCTION__, cj_itemgrp, ezlopi_hasGetter_str, itemgrp_node->has_getter);
            cJSON_AddBoolToObject(__FUNCTION__, cj_itemgrp, ezlopi_hasSetter_str, itemgrp_node->has_setter);
            cJSON_AddBoolToObject(__FUNCTION__, cj_itemgrp, ezlopi_persistent_str, itemgrp_node->persistent);
            cJSON_AddStringToObject(__FUNCTION__, cj_itemgrp, ezlopi_valueType_str, itemgrp_node->value_type);

            cJSON_AddItemToObject(__FUNCTION__, cj_itemgrp, "itemNames", cJSON_Duplicate(__FUNCTION__, itemgrp_node->item_names, cJSON_True));
            cJSON_AddItemToObject(__FUNCTION__, cj_itemgrp, ezlopi_enum_str, cJSON_Duplicate(__FUNCTION__, itemgrp_node->enum_values, cJSON_True));
            cJSON_AddItemToObject(__FUNCTION__, cj_itemgrp, "valueTypeFamily", cJSON_Duplicate(__FUNCTION__, itemgrp_node->value_type_family, cJSON_True));
            cJSON_AddItemToObject(__FUNCTION__, cj_itemgrp, ezlopi_info_str, cJSON_Duplicate(__FUNCTION__, itemgrp_node->value_type_family, cJSON_True));

            {   // add 'role'
                switch (itemgrp_node->role)
                {
                case EZLOPI_ITEM_GRP_ROLE_LIFE_SAFETY:
                {
                    cJSON_AddStringToObject(__FUNCTION__, cj_itemgrp, "role", "lifeSafety");
                    break;
                }
                case EZLOPI_ITEM_GRP_ROLE_HOUSE_MODES:
                {
                    cJSON_AddStringToObject(__FUNCTION__, cj_itemgrp, "role", "house_modes");
                    break;
                }
                case EZLOPI_ITEM_GRP_ROLE_EMPTY:
                {
                    cJSON_AddStringToObject(__FUNCTION__, cj_itemgrp, "role", "empty");
                    break;
                }
                default:
                    break;
                }
            }
        }
    }

    return cj_itemgrp;
}


//------- Main Init_Function -------------------------------------------------
void ezlopi_device_group_init(void)
{
    char* devgrp_id_list_str = ezlopi_nvs_read_device_groups();
    if (devgrp_id_list_str)
    {
        TRACE_D("devGrp_id_list_str : %s", devgrp_id_list_str);
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
                                // CJSON_TRACE("new_devgrp_node", cj_new_devgrp_node);
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
}
void ezlopi_item_group_init(void)
{
    char* itemgrp_id_list_str = ezlopi_nvs_read_item_groups();
    if (itemgrp_id_list_str)
    {
        TRACE_D("itemGrp_id_list_str : %s", itemgrp_id_list_str);
        cJSON* cj_itemgrp_ids = cJSON_Parse(__FUNCTION__, itemgrp_id_list_str);
        if (cj_itemgrp_ids)
        {
            int array_size = cJSON_GetArraySize(cj_itemgrp_ids);
            for (int i = 0; i < array_size; i++)
            {
                cJSON* cj_itemgrp_id = cJSON_GetArrayItem(cj_itemgrp_ids, i);
                if (cj_itemgrp_id && cj_itemgrp_id->valuedouble)
                {
                    uint32_t tmp_itemgrp_id = (uint32_t)cj_itemgrp_id->valuedouble;

                    char tmp_buffer[32];
                    snprintf(tmp_buffer, sizeof(tmp_buffer), "%08x", tmp_itemgrp_id);
                    ezlopi_cloud_update_item_group_id((uint32_t)tmp_itemgrp_id);

                    char* itemgrp_str = ezlopi_nvs_read_str(tmp_buffer);
                    if (itemgrp_str)
                    {
                        cJSON* cj_itemgrp = cJSON_Parse(__FUNCTION__, itemgrp_str);
                        if (cj_itemgrp)
                        {
                            l_ezlopi_item_grp_t* new_itemgrp_node = __item_group_populate(cj_itemgrp, tmp_itemgrp_id);
                            cJSON_Delete(__FUNCTION__, cj_itemgrp);

                            if (new_itemgrp_node)
                            {
                                cJSON* cj_new_itemgrp_node = ezlopi_core_item_group_create_cjson(new_itemgrp_node);
                                // CJSON_TRACE("new_itemgrp_node", cj_new_itemgrp_node);
                                cJSON_Delete(__FUNCTION__, cj_new_itemgrp_node);
                            }
                        }

                        ezlopi_free(__FUNCTION__, itemgrp_str);
                    }
                }

            }

            cJSON_Delete(__FUNCTION__, cj_itemgrp_ids);
        }

        ezlopi_free(__FUNCTION__, itemgrp_id_list_str);
    }
}
