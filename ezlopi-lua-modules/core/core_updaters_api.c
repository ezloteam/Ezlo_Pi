/* ===========================================================================
** Copyright (C) 2024 Ezlo Innovation Inc
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
 * @author  John Doe
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
#include "ezlopi_core_devices.h"
#include "ezlopi_cloud_constants.h"

#include "core_updaters_api.h"

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
int lcore_update_item_value(lua_State *lua_state)
{
    int ret = 0;
    char *item_id_str = luaL_checkstring(lua_state, 1);

    if (item_id_str)
    {
        uint32_t item_id = strtoul(item_id_str, NULL, 16);
        l_ezlopi_item_t *item_node = EZPI_core_device_get_item_by_id(item_id);
        if (item_node)
        {
            cJSON *cj_params = cJSON_CreateObject(__FUNCTION__);
            if (cj_params)
            {
                char tmp_buffer[32];
                snprintf(tmp_buffer, sizeof(tmp_buffer), "%08x", item_node->cloud_properties.item_id);
                cJSON_AddStringToObject(__FUNCTION__, cj_params, ezlopi__id_str, tmp_buffer);

                int check_val = lua_type(lua_state, 2);

                switch (check_val)
                {
                case LUA_TBOOLEAN:
                {
                    int bool_val = lua_toboolean(lua_state, 2);
                    cJSON_AddBoolToObject(__FUNCTION__, cj_params, ezlopi_value_str, bool_val);
                    ret = 1;
                    break;
                }
                case LUA_TNUMBER:
                {
                    double number_val = lua_tonumber(lua_state, 2);
                    cJSON_AddNumberToObject(__FUNCTION__, cj_params, ezlopi_value_str, number_val);
                    ret = 1;
                    break;
                }
                case LUA_TSTRING:
                {
                    char *string_val = lua_tostring(lua_state, 2);
                    cJSON_AddStringToObject(__FUNCTION__, cj_params, ezlopi_value_str, string_val);
                    ret = 1;
                    break;
                }
                case LUA_TLIGHTUSERDATA:
                case LUA_TTABLE:
                case LUA_TFUNCTION:
                case LUA_TUSERDATA:
                case LUA_TTHREAD:
                default:
                {
                    TRACE_E("Unimplemented type found: %d", check_val);
                    break;
                }
                }

                if (ret)
                {
                    item_node->func(EZLOPI_ACTION_SET_VALUE, item_node, cj_params, item_node->user_arg);
                }

                cJSON_Delete(__FUNCTION__, cj_params);
            }
        }
    }

    return ret;
}

int lcore_update_item_value_with_min_max(lua_State *lua_state) { return 0; }
int lcore_update_item_dictionary_value(lua_State *lua_state) { return 0; }
int lcore_update_user_code_restriction(lua_State *lua_state) { return 0; }
int lcore_update_reachable_state(lua_State *lua_state) { return 0; }
int lcore_update_ready_state(lua_State *lua_state) { return 0; }
int lcore_update_device_status(lua_State *lua_state) { return 0; }
int lcore_update_gateway(lua_State *lua_state) { return 0; }
int lcore_modify_device(lua_State *lua_state) { return 0; }
int lcore_modify_item(lua_State *lua_state) { return 0; }
int lcore_modify_setting(lua_State *lua_state) { return 0; }
int lcore_set_setting_value(lua_State *lua_state) { return 0; }
int lcore_set_setting_status(lua_State *lua_state) { return 0; }
int lcore_set_setting_dictionary_value(lua_State *lua_state) { return 0; }
int lcore_set_setting_dictionary_status(lua_State *lua_state) { return 0; }
int lcore_set_gateway_setting_value(lua_State *lua_state) { return 0; }
int lcore_set_gateway_setting_status(lua_State *lua_state) { return 0; }

/*******************************************************************************
 *                          Static Function Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
