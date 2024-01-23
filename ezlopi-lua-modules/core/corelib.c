
#include "lua/lua.h"
#include "lua/lauxlib.h"
#include "lua/lstate.h"

#include "ezlopi_core_factory_info.h"

#include "corelib.h"
#include "ezlopi_util_trace.h"

#include "core_getters_api.h"
#include "core_adders_api.h"
#include "core_removers_api.h"
#include "core_updaters_api.h"
#include "core_others_api.h"
#include "core_external_gateway_api.h"

static const struct luaL_Reg lcore_funcs[] = {
#define CORELIB_FUNCTION(f_name, l_func) {.name = f_name, .func = l_func},
#include "corelib_macro.h"
#undef CORELIB_FUNCTION
    {.name = NULL, .func = NULL},
};

int luaopen_lcore(lua_State *lua_state)
{
    luaL_newlib(lua_state, lcore_funcs);
    return 1;
}
