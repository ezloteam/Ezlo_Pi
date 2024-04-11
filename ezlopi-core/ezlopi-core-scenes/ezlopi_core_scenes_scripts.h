#ifndef _EZLOPI_CORE_SCENES_SCRIPTS_H_
#define _EZLOPI_CORE_SCENES_SCRIPTS_H_

#include <inttypes.h>

typedef enum e_script_state
{
    SCRIPT_STATE_NONE = 0,
    SCRIPT_STATE_RUNNING,
    SCRIPT_STATE_NOT_RUNNING,
    SCRIPT_STATE_PAUSED,
    SCRIPT_STATE_DELETE,
} e_script_state_t;

typedef struct l_ezlopi_scenes_script
{
    uint32_t id;
    char name[32];
    char* code;
    e_script_state_t state;
    void* script_handle;

    struct l_ezlopi_scenes_script* next;
} l_ezlopi_scenes_script_t;

void ezlopi_scenes_scripts_init(void);

l_ezlopi_scenes_script_t* ezlopi_scenes_scripts_get_head(void);
uint32_t ezlopi_scenes_scripts_add_to_head(uint32_t script_id, cJSON* cj_script);

void ezlopi_scenes_scripts_update(cJSON* cj_script);

void ezlopi_scenes_scripts_stop_by_id(uint32_t script_id);
void ezlopi_scenes_scripts_stop(l_ezlopi_scenes_script_t* script_node);

void ezlopi_scenes_scripts_run_by_id(uint32_t script_id);
void ezlopi_scenes_scripts_delete_by_id(uint32_t script_id);

void ezlopi_scenes_scripts_factory_info_reset(void);

#endif // _EZLOPI_CORE_SCENES_SCRIPTS_H_
