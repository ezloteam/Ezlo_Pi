#ifndef __CORE_GETTERS_API_H__
#define __CORE_GETTERS_API_H__

#include "lua/lua.h"

int lcore_get_product_name(lua_State *lua_state);
int lcore_get_gateways(lua_State *lua_state);
int lcore_get_gateway(lua_State *lua_state);
int lcore_get_device(lua_State *lua_state);
int lcore_get_devices(lua_State *lua_state);
int lcore_get_devices_ids(lua_State *lua_state);
int lcore_get_root_device_id(lua_State *lua_state);
int lcore_get_item(lua_State *lua_state);
int lcore_get_items(lua_State *lua_state);
int lcore_get_items_by_device_id(lua_State *lua_state);
int lcore_get_setting(lua_State *lua_state);
int lcore_get_setting_ids_by_device_id(lua_State *lua_state);
int lcore_get_gateway_setting(lua_State *lua_state);
int lcore_get_gateway_setting_ids_by_gateway_id(lua_State *lua_state);
int lcore_get_rooms(lua_State *lua_state);
int lcore_get_room(lua_State *lua_state);

#endif // __CORE_GETTERS_API_H__
