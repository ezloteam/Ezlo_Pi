#ifndef __CORE_EXTERNAL_GATEWAY_API_H__
#define __CORE_EXTERNAL_GATEWAY_API_H__

#include "lua/lua.h"

int lcore_notify_process_started(lua_State *lua_state);
int lcore_notify_process_stopped(lua_State *lua_state);
int lcore_get_current_notified_process(lua_State *lua_state);
int lcore_generate_item_dictionary_number_id(lua_State *lua_state);
int lcore_send_ui_broadcast(lua_State *lua_state);
int lcore_send_device_checked_broadcast(lua_State *lua_state);
int lcore_send_response(lua_State *lua_state);
int lcore_subscribe(lua_State *lua_state);
int lcore_set_subscription_filters(lua_State *lua_state);
int lcore_unsubscribe(lua_State *lua_state);

#endif // __CORE_EXTERNAL_GATEWAY_API_H__