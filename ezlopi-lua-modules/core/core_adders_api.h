#ifndef __CORE_ADDERS_API_H__
#define __CORE_ADDERS_API_H__

#include "lua/lua.h"

int lcore_add_device(lua_State *lua_state);
int lcore_add_item(lua_State *lua_state);
int lcore_add_setting(lua_State *lua_state);
int lcore_add_gateway_setting(lua_State *lua_state);
int lcore_add_vera_device_mapping(lua_State *lua_state);

#endif //  __CORE_ADDERS_API_H__
