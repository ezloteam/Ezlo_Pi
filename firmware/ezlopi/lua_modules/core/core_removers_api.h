#ifndef __CORE_REMOVERS_API_H__
#define __CORE_REMOVERS_API_H__

#include "lua/lua.h"

int lcore_remove_device(lua_State *lua_state);
int lcore_remove_device_sync(lua_State *lua_state);
int lcore_remove_item(lua_State *lua_state);
int lcore_remove_gateway_devices(lua_State *lua_state);
int lcore_remove_item_dictionary_value(lua_State *lua_state);
int lcore_remove_setting(lua_State *lua_state);
int lcore_remove_gateway_setting(lua_State *lua_state);

#endif // __CORE_REMOVERS_API_H__
