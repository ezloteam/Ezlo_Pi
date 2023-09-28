#ifndef __EZLOPI_LUA_H__
#define __EZLOPI_LUA_H__

#include "inttypes.h"

#define EZLOPI_LUA_SCRIPTS_NAMES "lua_scripts_names"

typedef struct s_ezlopi_script
{
} s_ezlopi_script_t;

typedef struct s_ezlopi_lua_script
{
    uint32_t id;
    char *name;
    char *script;

    struct s_ezlopi_lua_script *next;
} s_ezlopi_lua_script_t;

void ezlopi_scenes_scripts_init(void);

#endif // __EZLOPI_LUA_H__
