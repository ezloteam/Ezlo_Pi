#ifndef __CORELIB_H__
#define __CORELIB_H__

#include "lua/lua.h"
#include "lua/lauxlib.h"
#include "lua/lstate.h"

int luaopen_lcore(lua_State *lua_state);

#endif // __CORELIB_H__
