#ifndef __EZLOPI_LUA_H__
#define __EZLOPI_LUA_H__

#include "inttypes.h"

typedef struct l_ezlopi_scenes_script
{
    uint32_t id;
    char *name;
    char *code;

    struct l_ezlopi_scenes_script *next;
} l_ezlopi_scenes_script_t;

void ezlopi_scenes_scripts_init(void);
uint32_t ezlopi_scenes_scripts_add_to_head(uint32_t script_id, cJSON *cj_script);

#endif // __EZLOPI_LUA_H__
