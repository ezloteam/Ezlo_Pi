#include <string.h>

#include "lua/lua.h"
#include "lua/lauxlib.h"

#include "trace.h"
#include "ezlopi_devices.h"
#include "core_updaters_api.h"

int lcore_update_item_value(lua_State *lua_state)
{
    int ret = 0;
    char *item_id_str = luaL_checkstring(lua_state, 1);

    if (item_id_str)
    {
        uint32_t item_id = strtoul(item_id_str, NULL, 16);
        l_ezlopi_item_t *item_node = ezlopi_device_get_item_by_id(item_id);
        if (item_node)
        {
            cJSON *cj_params = cJSON_CreateObject();
            if (cj_params)
            {
                char tmp_buffer[32];
                snprintf(tmp_buffer, sizeof(tmp_buffer), "%08x", item_node->cloud_properties.item_id);
                cJSON_AddStringToObject(cj_params, "_id", tmp_buffer);

                int check_val = lua_type(lua_state, 2);

                switch (check_val)
                {
                case LUA_TBOOLEAN:
                {
                    int bool_val = lua_toboolean(lua_state, 2);
                    cJSON_AddBoolToObject(cj_params, "value", bool_val);
                    ret = 1;
                    break;
                }
                case LUA_TNUMBER:
                {
                    double number_val = lua_tonumber(lua_state, 2);
                    cJSON_AddNumberToObject(cj_params, "value", number_val);
                    ret = 1;
                    break;
                }
                case LUA_TSTRING:
                {
                    char *string_val = lua_tostring(lua_state, 2);
                    cJSON_AddStringToObject(cj_params, "value", string_val);
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

                cJSON_Delete(cj_params);
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