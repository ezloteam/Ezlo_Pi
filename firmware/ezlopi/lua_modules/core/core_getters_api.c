#include <string.h>
#include "core_getters_api.h"

#include "ezlopi_factory_info.h"

int lcore_get_product_name(lua_State *lua_state)
{
    char *product_name = ezlopi_factory_info_v2_get_name();
    if (product_name)
    {
        lua_pushstring(lua_state, product_name);
        free(product_name);
    }
    else
    {
        lua_pushstring(lua_state, "");
    }
    return 1;
}

// to be done
int lcore_get_gateways(lua_State *lua_state) { return 0; }
int lcore_get_gateway(lua_State *lua_state) { return 0; }
int lcore_get_device(lua_State *lua_state) { return 0; }
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