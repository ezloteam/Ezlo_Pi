/* ===========================================================================
** Copyright (C) 2025 Ezlo Innovation Inc
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
 * @file    main.c
 * @brief   perform some function on data
 * @author
 * @version 0.1
 * @date    1st January 2024
 */

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include <string.h>
#include "lua/lua.h"
#include "lua/lauxlib.h"

#include "ezlopi_util_trace.h"
#include "ezlopi_util_version.h"
#include "core_getters_api.h"
#include "ezlopi_core_factory_info.h"
#include "ezlopi_core_devices.h"
#include "ezlopi_core_gateway.h"
#include "ezlopi_cloud_constants.h"
#include "lua_helper_functions.h"

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
static int __create_lua_table_for_gateway(lua_State *lua_state, s_ezlopi_gateway_t *gateway_prop);
static int __create_lua_table_for_device(lua_State *lua_state, l_ezlopi_device_t *device_prop);
static int __create_lua_table_for_item(lua_State *lua_state, l_ezlopi_item_t *item_prop, uint32_t device_id);

/*******************************************************************************
 *                          Static Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Definitions
 *******************************************************************************/

/**
 * @brief Global/extern function template example
 * Convention : Use capital letter for initial word on extern function
 * @param arg
 */
int lcore_get_product_name(lua_State *lua_state)
{
    int ret = 0;
    char *product_name = EZPI_core_factory_info_v3_get_name();
    if (product_name)
    {
        lua_pushstring(lua_state, product_name);
        ezlopi_free(__FUNCTION__, product_name);
        ret = 1;
    }
    else
    {
        lua_pushstring(lua_state, ezlopi__str);
    }
    return ret;
}

int lcore_get_gateways(lua_State *lua_state)
{
    lua_newtable(lua_state);
    lua_pushnumber(lua_state, 1);                                       // table index
    __create_lua_table_for_gateway(lua_state, EZPI_core_gateway_get()); // individual device table
    lua_settable(lua_state, -3);

    return 1;
}

int lcore_get_gateway(lua_State *lua_state)
{
    s_ezlopi_gateway_t *gateway = EZPI_core_gateway_get();
    __create_lua_table_for_gateway(lua_state, gateway);

    return 1;
}

int lcore_get_device(lua_State *lua_state)
{
    int ret = 0;
    char *device_id_str = luaL_checkstring(lua_state, 1);
    if (device_id_str)
    {
        TRACE_E("device-id: %s", device_id_str);
        uint32_t device_id = strtoul(device_id_str, NULL, 16);
        l_ezlopi_device_t *device_node = EZPI_core_device_get_head();
        while (device_node)
        {
            if (device_id == device_node->cloud_properties.device_id)
            {
                TRACE_E("Found device-id: %08x", device_id);
                ret = __create_lua_table_for_device(lua_state, device_node);
                break;
            }
            device_node = device_node->next;
        }
    }

    return ret;
}

int lcore_get_devices(lua_State *lua_state)
{
    int ret = 0;
    l_ezlopi_device_t *device_node = EZPI_core_device_get_head();
    lua_newtable(lua_state);
    while (device_node)
    {
        lua_pushnumber(lua_state, ++ret);                      // table index
        __create_lua_table_for_device(lua_state, device_node); // individual device table
        lua_settable(lua_state, -3);

        device_node = device_node->next;
    }

    return ret ? 1 : 0;
}

int lcore_get_devices_ids(lua_State *lua_state)
{
    int ret = 0;
    l_ezlopi_device_t *device_node = EZPI_core_device_get_head();
    lua_newtable(lua_state);
    while (device_node)
    {
        char tmp_buffer[32];
        snprintf(tmp_buffer, sizeof(tmp_buffer), "%08x", device_node->cloud_properties.device_id);
        lua_create_table_string_value(++ret, &tmp_buffer[0]);
        device_node = device_node->next;
    }

    return ret ? 1 : 0;
}

int lcore_get_item(lua_State *lua_state)
{
    int ret = 0;
    char *item_id_str = luaL_checkstring(lua_state, 1);
    if (item_id_str)
    {
        TRACE_D("item-id: %s", item_id_str);
        uint32_t item_id = strtoul(item_id_str, NULL, 16);

        l_ezlopi_device_t *device_node = EZPI_core_device_get_head();
        lua_newtable(lua_state);
        while (device_node)
        {
            l_ezlopi_item_t *item_node = device_node->items;
            while (item_node)
            {
                if (item_id == item_node->cloud_properties.item_id)
                {
                    ret = __create_lua_table_for_item(lua_state, item_node, device_node->cloud_properties.device_id);
                    break;
                }
                item_node = item_node->next;
            }

            device_node = device_node->next;
        }
    }

    return ret ? 1 : 0;
}

int lcore_get_items(lua_State *lua_state)
{
    int ret = 0;
    l_ezlopi_device_t *device_node = EZPI_core_device_get_head();
    lua_newtable(lua_state);
    while (device_node)
    {
        l_ezlopi_item_t *item_node = device_node->items;
        while (item_node)
        {
            lua_pushnumber(lua_state, ++ret);                                                           // table index
            __create_lua_table_for_item(lua_state, item_node, device_node->cloud_properties.device_id); // individual item table
            lua_settable(lua_state, -3);
            item_node = item_node->next;
        }

        device_node = device_node->next;
    }

    return ret ? 1 : 0;
}

int lcore_get_items_by_device_id(lua_State *lua_state)
{
    int ret = 0;

    char *device_id_str = luaL_checkstring(lua_state, 1);
    if (device_id_str)
    {
        TRACE_E("device-id: %s", device_id_str);
        uint32_t device_id = strtoul(device_id_str, NULL, 16);

        l_ezlopi_device_t *device_node = EZPI_core_device_get_head();
        lua_newtable(lua_state);
        while (device_node)
        {
            if (device_id == device_node->cloud_properties.device_id)
            {
                l_ezlopi_item_t *item_node = device_node->items;
                while (item_node)
                {
                    lua_pushnumber(lua_state, ++ret);                                                           // table index
                    __create_lua_table_for_item(lua_state, item_node, device_node->cloud_properties.device_id); // individual item table
                    lua_settable(lua_state, -3);
                    item_node = item_node->next;
                }

                break;
            }

            device_node = device_node->next;
        }
    }

    return ret ? 1 : 0;
}

int lcore_get_root_device_id(lua_State *lua_state) { return 0; }
int lcore_get_setting(lua_State *lua_state) { return 0; }
int lcore_get_setting_ids_by_device_id(lua_State *lua_state) { return 0; }
int lcore_get_gateway_setting(lua_State *lua_state) { return 0; }
int lcore_get_gateway_setting_ids_by_gateway_id(lua_State *lua_state) { return 0; }
int lcore_get_rooms(lua_State *lua_state) { return 0; }
int lcore_get_room(lua_State *lua_state) { return 0; }

/*******************************************************************************
 *                          Static Function Definitions
 *******************************************************************************/
static int __create_lua_table_for_device(lua_State *lua_state, l_ezlopi_device_t *device_prop)
{
    char tmp_str[32];
    s_ezlopi_cloud_controller_t *controller_info = EZPI_core_device_get_controller_information();

    lua_newtable(lua_state);

    snprintf(tmp_str, sizeof(tmp_str), "%08x", device_prop->cloud_properties.device_id);
    lua_create_table_string_key_value(ezlopi_id_str, &tmp_str[0]);

    snprintf(tmp_str, sizeof(tmp_str), "%08x", EZPI_core_gateway_get()->_id);
    lua_create_table_string_key_value(ezlopi_gateway_id_str, &tmp_str[0]);

    lua_create_table_string_key_value(ezlopi_name_str, device_prop->cloud_properties.device_name);
    lua_create_table_string_key_value(ezlopi_category_str, device_prop->cloud_properties.category);
    lua_create_table_string_key_value(ezlopi_subcategory_str, device_prop->cloud_properties.subcategory);
    lua_create_table_string_key_value(ezlopi_type_str, device_prop->cloud_properties.device_type);
    lua_create_table_string_key_value(ezlopi_device_type_id_str, controller_info->device_type_id);

    if (device_prop->cloud_properties.room_id)
    {
        snprintf(tmp_str, sizeof(tmp_str), "%08x", device_prop->cloud_properties.room_id);
        lua_create_table_string_key_value(ezlopi_room_id_str, &tmp_str[0]);
    }
    else
    {
        lua_create_table_string_key_value(ezlopi_room_id_str, ezlopi__str);
    }
    // lua_create_table_string_key_value(ezlopi_room_id_str, controller_info->room_id);

    lua_create_table_string_key_value(ezlopi_parent_device_id_str, controller_info->parent_device_id);

    lua_pushstring(lua_state, ezlopi_info_str);
    lua_newtable(lua_state);

    char *manufacturer = EZPI_core_factory_info_v3_get_manufacturer();
    lua_create_table_string_key_value(ezlopi_manufacturer_str, manufacturer);
    ezlopi_free(__FUNCTION__, manufacturer);

    char *model = EZPI_core_factory_info_v3_get_model();
    lua_create_table_string_key_value(ezlopi_model_str, model);
    ezlopi_free(__FUNCTION__, model);

    char *brand = EZPI_core_factory_info_v3_get_brand();
    lua_create_table_string_key_value(ezlopi_brand_str, brand);
    ezlopi_free(__FUNCTION__, brand);
    lua_settable(lua_state, -3);

    lua_pushstring(lua_state, ezlopi_firmware_str);
    lua_newtable(lua_state);

    lua_create_table_string_key_value(ezlopi_version_str, VERSION_STR);
    lua_create_table_string_key_value(ezlopi_build__date_str, COMPILE_TIME);
    lua_create_table_string_key_value(ezlopi_hash_str, COMMIT_HASH);
    lua_create_table_string_key_value(ezlopi_branch_str, CURRENT_BRANCH);
    lua_create_table_string_key_value(ezlopi_developer_str, DEVELOPER);
    lua_settable(lua_state, -3);

    lua_create_table_bool_key_value(ezlopi_battery_powered_str, controller_info->battery_powered);
    lua_create_table_bool_key_value(ezlopi_reachable_str, controller_info->reachable);
    lua_create_table_bool_key_value(ezlopi_persistent_str, controller_info->persistent);
    lua_create_table_string_key_value(ezlopi_security_str, controller_info->security);
    lua_create_table_bool_key_value(ezlopi_ready_str, controller_info->ready);
    lua_create_table_string_key_value(ezlopi_status_str, controller_info->status ? controller_info->status : ezlopi__str);
    lua_create_table_string_key_value(ezlopi_house_modes_options_str, ezlopi__str);
    lua_create_table_string_key_value(ezlopi_parent_room_str, ezlopi__str);

    return 1;
}

static int __create_lua_table_for_item(lua_State *lua_state, l_ezlopi_item_t *item_prop, uint32_t device_id)
{
    char tmp_str[32];
    // s_ezlopi_cloud_controller_t *controller_info = EZPI_core_device_get_controller_information();

    lua_newtable(lua_state);

    snprintf(tmp_str, sizeof(tmp_str), "%08x", item_prop->cloud_properties.item_id);
    lua_create_table_string_key_value(ezlopi_id_str, &tmp_str[0]);

    snprintf(tmp_str, sizeof(tmp_str), "%08x", device_id);
    lua_create_table_string_key_value(ezlopi_device_id_str, &tmp_str[0]);

    lua_create_table_string_key_value(ezlopi_name_str, item_prop->cloud_properties.item_name);
    lua_create_table_bool_key_value(ezlopi_has_getter_str, item_prop->cloud_properties.has_getter);
    lua_create_table_bool_key_value(ezlopi_has_setter_str, item_prop->cloud_properties.has_setter);
    lua_create_table_bool_key_value(ezlopi_show_str, item_prop->cloud_properties.show);
    lua_create_table_string_key_value(ezlopi_scale_str, item_prop->cloud_properties.scale);
    lua_create_table_string_key_value(ezlopi_value_type_str, item_prop->cloud_properties.value_type);

    if (value_type_token == item_prop->cloud_properties.value_type)
    {
        lua_pushstring(lua_state, ezlopi_enum_str);
        lua_newtable(lua_state);

        lua_create_table_string_value(1, ezlopi_none_str); // Remains for future

        lua_settable(lua_state, -3);
    }
    else
    {
        cJSON *cj_result = cJSON_CreateObject(__FUNCTION__);
        if (cj_result)
        {
            item_prop->func(EZLOPI_ACTION_GET_EZLOPI_VALUE, item_prop, cj_result, item_prop->user_arg);
            cJSON *cj_value = cJSON_GetObjectItem(__FUNCTION__, cj_result, ezlopi_value_str);
            if (cj_value)
            {
                switch (cj_value->type)
                {
                case cJSON_True:
                {
                    lua_create_table_bool_key_value(ezlopi_value_str, true);
                    break;
                }
                case cJSON_False:
                {
                    lua_create_table_bool_key_value(ezlopi_value_str, false);
                    break;
                }
                case cJSON_Number:
                {
                    lua_create_table_number_key_value(ezlopi_value_str, cj_value->valuedouble);
                    break;
                }
                case cJSON_String:
                {
                    if (cj_value->valuestring)
                    {
                        lua_create_table_string_key_value(ezlopi_value_str, cj_value->valuestring);
                    }
                    break;
                }
                default:
                {
                    break;
                }
                }
            }
            cJSON_Delete(__FUNCTION__, cj_result);
        }
    }

    return 1;
}

static int __create_lua_table_for_gateway(lua_State *lua_state, s_ezlopi_gateway_t *gateway_prop)
{
    char tmp_str[32];
    lua_newtable(lua_state);

    snprintf(tmp_str, sizeof(tmp_str), "%08x", gateway_prop->_id);
    lua_create_table_string_key_value(ezlopi_id_str, &tmp_str[0]);

    lua_create_table_string_key_value(ezlopi_name_str, gateway_prop->name);

    lua_create_table_string_key_value(ezlopi_plugin_id_str, gateway_prop->pluginid);
    lua_create_table_string_key_value(ezlopi_label_str, gateway_prop->label);
    lua_create_table_string_key_value(ezlopi_reason_str, ezlopi__str);
    lua_create_table_string_key_value(ezlopi_unreachable_reasons_str, ezlopi__str);
    lua_create_table_string_key_value(ezlopi_unreachable_actions_str, ezlopi__str);
    lua_create_table_bool_key_value(ezlopi_manual_device_adding_str, gateway_prop->manual_device_adding);

    lua_pushstring(lua_state, ezlopi_info_str);
    lua_newtable(lua_state);
    lua_create_table_bool_key_value(ezlopi_manual_device_adding_str, gateway_prop->status);
    lua_settable(lua_state, -3);

    return 1;
}

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
