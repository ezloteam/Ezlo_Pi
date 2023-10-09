#ifndef __CORE_UPDATERS_API_H__
#define __CORE_UPDATERS_API_H__

#include "lua/lua.h"

int lcore_update_item_value(lua_State *lua_state);
int lcore_update_item_value_with_min_max(lua_State *lua_state);
int lcore_update_item_dictionary_value(lua_State *lua_state);
int lcore_update_user_code_restriction(lua_State *lua_state);
int lcore_update_reachable_state(lua_State *lua_state);
int lcore_update_ready_state(lua_State *lua_state);
int lcore_update_device_status(lua_State *lua_state);
int lcore_update_gateway(lua_State *lua_state);
int lcore_modify_device(lua_State *lua_state);
int lcore_modify_item(lua_State *lua_state);
int lcore_modify_setting(lua_State *lua_state);
int lcore_set_setting_value(lua_State *lua_state);
int lcore_set_setting_status(lua_State *lua_state);
int lcore_set_setting_dictionary_value(lua_State *lua_state);
int lcore_set_setting_dictionary_status(lua_State *lua_state);
int lcore_set_gateway_setting_value(lua_State *lua_state);
int lcore_set_gateway_setting_status(lua_State *lua_state);

#endif // __CORE_UPDATERS_API_H__
