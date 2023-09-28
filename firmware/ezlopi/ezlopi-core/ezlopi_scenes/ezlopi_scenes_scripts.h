#ifndef __EZLOPI_LUA_H__
#define __EZLOPI_LUA_H__

#include "inttypes.h"

#define EZLOPI_LUA_SCRIPTS_NAMES "lua_scripts_names"

typedef struct s_ezlopi_script
{
    uint32_t id;
    char *script;
} s_ezlopi_script_t;

typedef struct s_ezlopi_lua_script
{
    char *name;
    s_ezlopi_script_t script;
} s_ezlopi_lua_script_t;

#endif // __EZLOPI_LUA_H__
