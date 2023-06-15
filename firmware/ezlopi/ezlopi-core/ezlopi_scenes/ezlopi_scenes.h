#ifndef __EZLOPI_SCENES_H__
#define __EZLOPI_SCENES_H__

// https://api.ezlo.com/hub/scenes/local_scenes_api/index.html#hubsceneslist

#include "string.h"
#include "ctype.h"
#include "stdint.h"
#include "cJSON.h"

typedef enum e_scene_value_type
{
    SCENE_VALUE_TYPE_UNDEFINED = 0,
    SCENE_VALUE_TYPE_FALSE = 1,
    SCENE_VALUE_TYPE_TRUE = 2,
    SCENE_VALUE_TYPE_NUMBER = 3,
    SCENE_VALUE_TYPE_STRING = 4,
    SCENE_VALUE_TYPE_MAX
} e_scene_value_type_t;

typedef struct s_args
{
    char item[32];
    char value[32];
} s_args_t;

typedef struct s_method
{
    char name[32];
    s_args_t args;
} s_method_t;

typedef struct s_block_options
{
    s_method_t method;
} s_block_options_t;

typedef struct l_fields
{
    char name[32];
    char type[32];

    e_scene_value_type_t value_type; // 0: double, 1: string
    union value
    {
        char value_string[32];
        double value_double;
    } value;
    struct l_fields *next;
} l_fields_t;

typedef struct l_then_block
{

    s_block_options_t block_options;
    char block_type[32];
    l_fields_t *fields;
    struct l_then_block *next;
} l_then_block_t;

typedef struct l_when_block
{
    s_block_options_t block_options;
    char block_type[32];
    l_fields_t *fields;
    struct l_when_block *next;
} l_when_block_t;

typedef struct l_user_notification
{
    char user_id[32];
    struct l_user_notification *next;
} l_user_notification_t;

typedef struct l_house_modes
{
    char house_mode[8];
    struct l_house_modes *next;
} l_house_modes_t;

typedef struct l_scenes_list
{
    // char _id[32];
    uint32_t _id;
    uint32_t enabled;
    uint32_t is_group;
    char group_id[32];
    char name[32];
    char parent_id[32];

    l_user_notification_t *user_notifications;
    l_house_modes_t *house_modes;
    l_then_block_t *then;
    l_when_block_t *when;

    struct l_scenes_list *next;
} l_scenes_list_t;

void ezlopi_scene_init(void);
void ezlopi_scene_update_nvs(void);
void ezlopi_scene_add(cJSON *cj_scene);
void ezlopi_scene_delete(l_scenes_list_t *scenes_list);
l_scenes_list_t *ezlopi_scene_get_scenes_list(void);
l_scenes_list_t *ezlopi_scenes_get_by_id(uint32_t id);

void ezlopi_scenes_print(l_scenes_list_t *scene_link_list);
void ezlopi_print_when_blocks(l_when_block_t *when_blocks);
void ezlopi_print_user_notifications(l_user_notification_t *user_notification);
void ezlopi_print_house_modes(l_house_modes_t *house_modes);
void ezlopi_print_then_blocks(l_then_block_t *then_blocks);
void ezlopi_print_fields(l_fields_t *fields);
void ezlopi_print_block_options(s_block_options_t *block_options);
void ezlopi_print_methods(s_method_t *methods);
void ezlopi_print_args(s_args_t *args);

cJSON *ezlopi_ezlopi_scenes_create_cjson_scene(l_scenes_list_t *scene);
cJSON *ezlopi_scenes_create_cjson_scene_list(l_scenes_list_t *scenes_list);
char *ezlopi_scenes_create_json_string(l_scenes_list_t *scenes_list);

void ezlopi_scenes_delete(l_scenes_list_t *scenes_list);

#endif //  __EZLOPI_SCENES_H__
