#ifndef __EZLOPI_SCENES_H__
#define __EZLOPI_SCENES_H__

// https://api.ezlo.com/hub/scenes/local_scenes_api/index.html#hubsceneslist

#include "string.h"
#include "ctype.h"
#include "stdint.h"
#include "stdbool.h"
#include "cJSON.h"

typedef enum e_scenes_block_type
{
    SCENE_BLOCK_TYPE_NONE = 0,
    SCENE_BLOCK_TYPE_WHEN = 1,
    SCENE_BLOCK_TYPE_THEN = 2,
    SCENE_BLOCK_TYPE_MAX,
} e_scenes_block_type_t;

typedef enum e_scene_value_type
{
#define EZLOPI_VALUE_TYPE(type, name) EZLOPI_VALUE_TYPE_##type,
#include "ezlopi_scenes_value_types.h"
#undef EZLOPI_VALUE_TYPE
} e_scene_value_type_t;

typedef enum e_method_type
{
#define EZLOPI_SCENE(method, name, func) EZLOPI_SCENE_##method,
#include "ezlopi_scenes_method_types.h"
#undef EZLOPI_SCENE
} e_method_type_t;

typedef struct s_method
{
    char name[32];
    e_method_type_t type;
#if 0
    union
    {
        s_is_item_state_arg_t is_item_state_arg;
        s_is_item_state_changed_arg_t is_item_state_changed_arg;
        s_is_button_state_arg_t is_button_state_arg;

        // s_arg_http_request_t http_request;
        // s_arg_house_mode_t house_mode;
        // s_arg_lua_script_t lua;
    } u_arg;
#endif
} s_method_t;

typedef struct s_block_options
{
    s_method_t method;
} s_block_options_t;

typedef struct s_action_delay
{
    uint16_t days;
    uint16_t hours;
    uint16_t minutes;
    uint16_t seconds;
} s_action_delay_t;

typedef union u_field_value
{
    char *value_string;
    double value_double;
    int value_int;
    uint32_t value_uint;
    bool value_bool;
} u_field_value_t;

typedef struct l_fields
{
    char name[32];
    // char type[32];

    // e_value_type_t value_type;
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
    e_scenes_block_type_t block_type;
    s_block_options_t block_options;
    s_action_delay_t delay;
    l_fields_t *fields;
    struct l_then_block *next;
} l_then_block_t;

typedef struct l_when_block
{
    e_scenes_block_type_t block_type;
    s_block_options_t block_options;
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
l_scenes_list_t *ezlopi_scenes_get_scenes_list(void);
l_scenes_list_t *ezlopi_scenes_get_by_id(uint32_t id);
l_scenes_list_t *ezlopi_scenes_pop_by_id(uint32_t _id);
void ezlopi_scenes_update_by_id(uint32_t _id, cJSON *cj_scene);

void ezlopi_scenes_print(l_scenes_list_t *scene_link_list);
void ezlopi_print_when_blocks(l_when_block_t *when_blocks);
void ezlopi_print_user_notifications(l_user_notification_t *user_notification);
void ezlopi_print_house_modes(l_house_modes_t *house_modes);
void ezlopi_print_then_blocks(l_then_block_t *then_blocks);
void ezlopi_print_fields(l_fields_t *fields);
void ezlopi_print_block_options(s_block_options_t *block_options, l_fields_t *fields);
void ezlopi_print_methods(s_method_t *methods);
void ezlopi_print_args(s_method_t *method);

cJSON *ezlopi_scenes_cjson_create_then_block(l_then_block_t *then_block);
cJSON *ezlopi_scenes_cjson_create_when_block(l_when_block_t *when_block);
void ezlopi_scenes_cjson_add_then_blocks(cJSON *root, l_then_block_t *then_blocks);
void ezlopi_scenes_cjson_add_when_blocks(cJSON *root, l_when_block_t *when_blocks);
cJSON *ezlopi_scenes_create_cjson_scene(l_scenes_list_t *scene);
cJSON *ezlopi_scenes_create_cjson_scene_list(l_scenes_list_t *scenes_list);
char *ezlopi_scenes_create_json_string(l_scenes_list_t *scenes_list);

void ezlopi_scenes_delete(l_scenes_list_t *scenes_list);
void ezlopi_scenes_delete_by_id(uint32_t _id);

#endif //  __EZLOPI_SCENES_H__
