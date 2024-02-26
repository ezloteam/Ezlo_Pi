#ifndef _EZLOPI_CORE_SCENES_V2_H_
#define _EZLOPI_CORE_SCENES_V2_H_

#include <stdbool.h>
#include <stdint.h>
#include <ctype.h>
#include <cJSON.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "ezlopi_core_scenes_methods.h"

typedef enum e_scenes_block_type_v2
{
    SCENE_BLOCK_TYPE_NONE = 0,
    SCENE_BLOCK_TYPE_WHEN = 1,
    SCENE_BLOCK_TYPE_THEN = 2,
    SCENE_BLOCK_TYPE_ELSE = 3,
    SCENE_BLOCK_TYPE_MAX,
} e_scenes_block_type_v2_t;

typedef enum e_scene_status_v2
{
    EZLOPI_SCENE_STATUS_NONE = 0,
    EZLOPI_SCENE_STATUS_RUN,
    EZLOPI_SCENE_STATUS_RUNNING,
    EZLOPI_SCENE_STATUS_STOP,
    EZLOPI_SCENE_STATUS_STOPPED,
    EZLOPI_SCENE_STATUS_MAX
} e_scene_status_v2_t;

typedef enum e_scene_value_type_v2
{
#define EZLOPI_VALUE_TYPE(type, name) EZLOPI_VALUE_TYPE_##type,
#include "ezlopi_core_scenes_value_types.h"
#undef EZLOPI_VALUE_TYPE
} e_scene_value_type_v2_t;

typedef struct s_method_v2
{
    char name[32];
    e_scene_method_type_t type;
} s_method_v2_t;

typedef struct s_block_options_v2
{
    s_method_v2_t method;
} s_block_options_v2_t;

typedef struct s_action_delay_v2
{
    uint16_t days;
    uint16_t hours;
    uint16_t minutes;
    uint16_t seconds;
} s_action_delay_v2_t;

typedef enum e_value_type
{
    VALUE_TYPE_UNDEFINED = 0,
    VALUE_TYPE_NUMBER,
    VALUE_TYPE_STRING,
    VALUE_TYPE_BOOL,
    VALUE_TYPE_CJSON,
    VALUE_TYPE_BLOCK,
    VALUE_TYPE_HOUSE_MODE_ID_ARRAY,
    VALUE_TYPE_MAX
} e_value_type_t;

typedef union u_field_value_v2
{
    char* value_string;
    double value_double;
    bool value_bool;
    cJSON* cj_value;
    struct l_when_block_v2* when_block;
    struct l_house_modes_v2_t* house_modes;
} u_field_value_v2_t;

typedef struct s_field_value {
    e_value_type_t e_type;
    u_field_value_v2_t u_value;
} s_field_value_t;

typedef struct l_fields_v2
{
    char name[32];
    e_scene_value_type_v2_t value_type; // 0: double, 1: string
    s_field_value_t field_value;
    char* scale;
    void* user_arg; // user by when-methods
    struct l_fields_v2* next;

} l_fields_v2_t;

typedef struct l_action_block_v2
{
    char _tempId[40];
    e_scenes_block_type_v2_t block_type;
    s_block_options_v2_t block_options;
    s_action_delay_v2_t delay;
    l_fields_v2_t* fields;
    struct l_action_block_v2* next;
} l_action_block_v2_t;

typedef struct l_when_block_v2
{
    e_scenes_block_type_v2_t block_type;
    s_block_options_v2_t block_options;
    l_fields_v2_t* fields;
    struct l_when_block_v2* next;
} l_when_block_v2_t;

// typedef struct l_else_block_v2
// {
//     char _tempId[40];
//     e_scenes_block_type_v2_t block_type;
//     s_block_options_v2_t block_options;
//     s_action_delay_v2_t delay;
//     l_fields_v2_t *fields;
//     struct l_else_block_v2 *next;
// } l_else_block_v2_t;

typedef struct l_user_notification_v2
{
    char user_id[32];
    struct l_user_notification_v2* next;
} l_user_notification_v2_t;

typedef struct l_house_modes_v2
{
    char house_mode[8];
    struct l_house_modes_v2* next;
} l_house_modes_v2_t;

typedef struct l_scenes_list_v2
{
    // char _id[32];

    e_scene_status_v2_t status;
    TaskHandle_t task_handle;

    uint32_t _id;
    bool enabled;
    uint32_t is_group;
    char group_id[32];
    char name[32];
    char parent_id[32];

    l_user_notification_v2_t* user_notifications;
    l_house_modes_v2_t* house_modes;
    l_action_block_v2_t* then_block;
    l_when_block_v2_t* when_block;
    l_action_block_v2_t* else_block;

    struct l_scenes_list_v2* next;
} l_scenes_list_v2_t;

typedef int (*f_scene_method_v2_t)(l_scenes_list_v2_t* curr_scene, void* arg);

void ezlopi_scenes_init_v2(void);
uint32_t ezlopi_store_new_scene_v2(cJSON* cj_new_scene);
uint32_t ezlopi_scenes_get_list_v2(cJSON* cj_scenes_array);
int ezlopi_scene_edit_by_id(uint32_t scene_id, cJSON* cj_scene);

l_scenes_list_v2_t* ezlopi_scenes_get_scenes_head_v2(void);
f_scene_method_v2_t ezlopi_scene_get_method_v2(e_scene_method_type_t scene_method_type);

l_scenes_list_v2_t* ezlopi_scenes_get_by_id_v2(uint32_t _id);
l_scenes_list_v2_t* ezlopi_scenes_new_scene_populate(cJSON* cj_new_scene, uint32_t scene_id);

// e_scene_value_type_v2_t ezlopi_scenes_get_value_type(cJSON *cj_field);
// e_scene_value_type_v2_t ezlopi_scenes_get_expressions_value_type(cJSON *cj_value_type);

void ezlopi_scene_add_users_in_notifications(l_scenes_list_v2_t* scene_node, cJSON* cj_user);

void ezlopi_scenes_delete(l_scenes_list_v2_t* scenes_list);
void ezlopi_scenes_delete_field_value(l_fields_v2_t* field);
void ezlopi_scenes_delete_when_blocks(l_when_block_v2_t* when_blocks);
void ezlopi_scenes_delete_house_modes(l_house_modes_v2_t* house_modes);
void ezlopi_scenes_delete_action_blocks(l_action_block_v2_t* action_blocks);
void ezlopi_scenes_delete_user_notifications(l_user_notification_v2_t* user_notifications);

void ezlopi_scenes_depopulate_by_id_v2(uint32_t _id);
void ezlopi_scenes_enable_disable_id_from_list_v2(uint32_t _id, bool enabled_flag);
void ezlopi_scenes_remove_id_from_list_v2(uint32_t _id);
l_scenes_list_v2_t* ezlopi_scenes_pop_by_id_v2(uint32_t _id);

void ezlopi_scenes_print(l_scenes_list_v2_t* scene_link_list);
void ezlopi_scenes_notifications_add(cJSON* cj_notifications);

#endif // _EZLOPI_CORE_SCENES_V2_H_
