#include <string.h>
#include "lua/lua.h"
#include "lua/lauxlib.h"

#include "trace.h"

#include "core_getters_api.h"
#include "ezlopi_factory_info.h"
#include "ezlopi_devices.h"

int lcore_get_product_name(lua_State *lua_state)
{
    int ret = 0;
    char *product_name = ezlopi_factory_info_v2_get_name();
    if (product_name)
    {
        lua_pushstring(lua_state, product_name);
        free(product_name);
        ret = 1;
    }
    else
    {
        lua_pushstring(lua_state, "");
    }
    return ret;
}

// to be done
int lcore_get_gateways(lua_State *lua_state) { return 0; }
int lcore_get_gateway(lua_State *lua_state) { return 0; }

typedef struct Point
{
    int x, y;
} Point;

static void __create_device_lua_table(lua_State *lua_state, l_ezlopi_device_t *device_prop)
{
    lua_newtable(lua_state);
    // lua_settop(lua_state, 0);

    lua_pushstring(lua_state, device_prop->cloud_properties.device_name);
    lua_setfield(lua_state, -1, "name");
    lua_rawset(lua_state, -3);

    int top = lua_gettop(lua_state);
    TRACE_D("top: %d", top);
    for (int i = 1; i <= top; i++)
    {
        printf("%d\t%s\t", i, luaL_typename(lua_state, i));
        switch (lua_type(lua_state, i))
        {
        case LUA_TNUMBER:
            printf("%g\n", lua_tonumber(lua_state, i));
            break;
        case LUA_TSTRING:
            printf("%s\n", lua_tostring(lua_state, i));
            break;
        case LUA_TBOOLEAN:
            printf("%s\n", (lua_toboolean(lua_state, i) ? "true" : "false"));
            break;
        case LUA_TNIL:
            printf("%s\n", "nil");
            break;
        default:
            printf("%p\n", lua_topointer(lua_state, i));
            break;
        }
    }

    // // lua_settable(lua_state, -3);
    // TRACE_D("table index: %d", top);
    // lua_settable(lua_state, -1);
}

int lcore_get_device(lua_State *lua_state)
{
    int ret = 0;
    char *device_id_str = luaL_checkstring(lua_state, 1);
    if (device_id_str)
    {
        TRACE_E("device-id: %s", device_id_str);
        uint32_t device_id = strtoul(device_id_str, NULL, 16);
        l_ezlopi_device_t *device_node = ezlopi_device_get_head();
        while (device_node)
        {
            if (device_id == device_node->cloud_properties.device_id)
            {
                TRACE_E("Found device-id: %08x", device_id);
                __create_device_lua_table(lua_state, device_node);

#if 0
                cJSON *cj_device_prop = ezlopi_device_create_device_table_from_prop(device_node);
                if (cj_device_prop)
                {
                    char *device_prop_str = cJSON_Print(cj_device_prop);
                    if (device_prop_str)
                    {
                        lua_pushglobaltable(lua_state);
                        lua_pushlstring(lua_state, device_prop_str, strlen(device_prop_str));
                        free(device_prop_str);
                    }
                    cJSON_Delete(cj_device_prop);
                }
#endif
                break;
            }
            device_node = device_node->next;
        }
    }

    return ret;
}

int lcore_get_devices(lua_State *lua_state) { return 0; }
int lcore_get_devices_ids(lua_State *lua_state) { return 0; }
int lcore_get_root_device_id(lua_State *lua_state) { return 0; }
int lcore_get_item(lua_State *lua_state) { return 0; }
int lcore_get_items(lua_State *lua_state) { return 0; }
int lcore_get_items_by_device_id(lua_State *lua_state) { return 0; }
int lcore_get_setting(lua_State *lua_state) { return 0; }
int lcore_get_setting_ids_by_device_id(lua_State *lua_state) { return 0; }
int lcore_get_gateway_setting(lua_State *lua_state) { return 0; }
int lcore_get_gateway_setting_ids_by_gateway_id(lua_State *lua_state) { return 0; }
int lcore_get_rooms(lua_State *lua_state) { return 0; }
int lcore_get_room(lua_State *lua_state) { return 0; }