#ifndef __EZLOPI_SCENES_H__
#define __EZLOPI_SCENES_H__

// https://api.ezlo.com/hub/scenes/local_scenes_api/index.html#hubsceneslist

#include "string.h"
#include "ctype.h"
#include "stdint.h"
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
    SCENE_VALUE_TYPE_UNDEFINED = 0,
    SCENE_VALUE_TYPE_FALSE = 1,
    SCENE_VALUE_TYPE_TRUE = 2,
    SCENE_VALUE_TYPE_NUMBER = 3,
    SCENE_VALUE_TYPE_STRING = 4,
    SCENE_VALUE_TYPE_MAX
} e_scene_value_type_t;

typedef enum e_arg_type
{
    EZLOPI_SCENE_ARG_TYPE_NONE = 0,
    EZLOPI_SCENE_ARG_TYPE_DEVICE = 1,
    EZLOPI_SCENE_ARG_TYPE_HTTP_REQUEST = 2,
    EZLOPI_SCENE_ARG_TYPE_HOUSE_MODE = 3,
    EZLOPI_SCENE_ARG_TYPE_LUA_SCRIPT = 4,
    EZLOPI_SCENE_ARG_TYPE_MAX
} e_arg_type_t;

typedef struct s_arg_device
{
    char item[32];
    char value[32];
} s_arg_device_t;

typedef struct s_arg_http_request
{
    char content[32];
    char content_type[32];
    char credential[32];
    char headers[32];
    char skip_security[32];
    char url[64];
} s_arg_http_request_t;

typedef struct s_arg_house_mode
{
    char house_mode[32];
} s_arg_house_mode_t;

typedef struct s_arg_lua_script
{
    char dummy[32];
} s_arg_lua_script_t;

typedef struct s_method
{
    char name[32];
    e_arg_type_t arg_type;
    union
    {
        s_arg_device_t device;
        s_arg_http_request_t http_request;
        s_arg_house_mode_t house_mode;
        s_arg_lua_script_t lua;
    } u_arg;
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
    e_scenes_block_type_t block_type;
    s_block_options_t block_options;
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
l_scenes_list_t *ezlopi_scene_get_scenes_list(void);
l_scenes_list_t *ezlopi_scenes_get_by_id(uint32_t id);
l_scenes_list_t *ezlopi_scenes_pop_by_id(uint32_t _id);
void ezlopi_scenes_update_by_id(uint32_t _id, cJSON *cj_scene);

void ezlopi_scenes_print(l_scenes_list_t *scene_link_list);
void ezlopi_print_when_blocks(l_when_block_t *when_blocks);
void ezlopi_print_user_notifications(l_user_notification_t *user_notification);
void ezlopi_print_house_modes(l_house_modes_t *house_modes);
void ezlopi_print_then_blocks(l_then_block_t *then_blocks);
void ezlopi_print_fields(l_fields_t *fields);
void ezlopi_print_block_options(s_block_options_t *block_options);
void ezlopi_print_methods(s_method_t *methods);
void ezlopi_print_args(s_method_t *method);

cJSON *ezlopi_scenes_cjson_create_then_block(l_then_block_t *then_block);
cJSON *ezlopi_scenes_cjson_create_when_block(l_when_block_t *when_block);
void ezlopi_scenes_cjson_add_then_blocks(cJSON *root, l_then_block_t *then_blocks);
void ezlopi_scenes_cjson_add_when_blocks(cJSON *root, l_then_block_t *when_blocks);
cJSON *ezlopi_ezlopi_scenes_create_cjson_scene(l_scenes_list_t *scene);
cJSON *ezlopi_scenes_create_cjson_scene_list(l_scenes_list_t *scenes_list);
char *ezlopi_scenes_create_json_string(l_scenes_list_t *scenes_list);

void ezlopi_scenes_delete(l_scenes_list_t *scenes_list);
void ezlopi_scenes_delete_by_id(uint32_t _id);

#endif //  __EZLOPI_SCENES_H__
