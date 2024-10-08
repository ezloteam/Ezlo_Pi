#ifndef _EZLOPI_CORE_SCENES_V2_H_
#define _EZLOPI_CORE_SCENES_V2_H_

#ifdef CONFIG_EZPI_SERV_ENABLE_MESHBOTS

#include <stdbool.h>
#include <stdint.h>
#include <ctype.h>
#include "cjext.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "ezlopi_core_scenes_methods.h"
#include "ezlopi_core_errors.h"

//------------ when-block: 'cj_function' -----------------------
typedef struct s_when_function
{
    uint32_t transtion_instant;
    uint32_t transition_count;
    bool current_state;
    bool activate_pulse_seq; /* used only in 'for_pulse_method' */
} s_when_function_t;
//--------------------------------------------------------------
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
    EZLOPI_SCENE_STATUS_RUN,     // command
    EZLOPI_SCENE_STATUS_RUNNING, // state
    EZLOPI_SCENE_STATUS_STOP,    // command
    EZLOPI_SCENE_STATUS_STOPPED, // state
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
    cJSON *cj_function;
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
    char *value_string;
    double value_double;
    bool value_bool;
    cJSON *cj_value;
    struct l_when_block_v2 *when_block;
    struct l_house_modes_v2 *house_modes;
} u_field_value_v2_t;

typedef struct s_field_value
{
    e_value_type_t e_type;
    u_field_value_v2_t u_value;
} s_field_value_t;

typedef struct l_fields_v2
{
    char name[32];
    e_scene_value_type_v2_t value_type; // 0: double, 1: string
    s_field_value_t field_value;
    char *scale;
    void *user_arg; // used by when-methods
    struct l_fields_v2 *next;

} l_fields_v2_t;

typedef struct l_action_block_v2
{
    char _tempId[40];
    e_scenes_block_type_v2_t block_type;
    s_block_options_v2_t block_options;
    s_action_delay_v2_t delay;
    l_fields_v2_t *fields;
    struct l_action_block_v2 *next;
} l_action_block_v2_t;

typedef struct l_group_block_type
{
    uint32_t grp_id;        // may be used in future    //  currently not-populated from nvs
    bool grp_state;         //  result of the group_block --> 1/0
    char grp_blockName[32]; //  actual -> 'groupName'   //  The Group-Name provided by UI ; to indicate a group // e.g. ["blockName" : "group-A"]
} l_group_block_type_t;

typedef struct l_when_block_v2
{
    l_group_block_type_t *when_grp; //   if(!NULL) ------------> //  indicates the 'when-block' is 'group_type'.
    bool block_enable;              //   actual -> '_enable'     //  flag that allows blocks to return 1;
    uint32_t blockId;               //   actual -> '_ID'         //  The ID of a normal when-condition scene-block;
    bool block_status_reset_once;   //   NOT-NVS parameter [don't populate ; since not needed] // just a dummy flag to trigger function reset.
    cJSON *cj_block_meta;           //   Block metadata information. Intended to save data needed for user interfaces
    e_scenes_block_type_v2_t block_type;
    s_block_options_v2_t block_options;
    l_fields_v2_t *fields;
    struct l_when_block_v2 *next;
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
    struct l_user_notification_v2 *next;
} l_user_notification_v2_t;

typedef struct l_house_modes_v2
{
    char house_mode[8];
    struct l_house_modes_v2 *next;
} l_house_modes_v2_t;

typedef struct l_scenes_list_v2
{
    e_scene_status_v2_t status;
    TaskHandle_t task_handle;

    uint32_t _id;
    bool enabled;
    bool is_group;
    uint32_t group_id;
    char name[32];
    cJSON *meta;
    char parent_id[32];
    uint32_t executed_date;

    l_user_notification_v2_t *user_notifications;
    l_house_modes_v2_t *house_modes;
    l_action_block_v2_t *then_block;
    l_when_block_v2_t *when_block;
    l_action_block_v2_t *else_block;

    void *thread_ctx;

    struct l_scenes_list_v2 *next;
} l_scenes_list_v2_t;

typedef int (*f_scene_method_v2_t)(l_scenes_list_v2_t *curr_scene, void *arg);

ezlopi_error_t ezlopi_scenes_init_v2(void);
uint32_t ezlopi_store_new_scene_v2(cJSON *cj_new_scene);
uint32_t ezlopi_scenes_get_list_v2(cJSON *cj_scenes_array);
int ezlopi_scene_edit_by_id(uint32_t scene_id, cJSON *cj_scene);

l_scenes_list_v2_t *ezlopi_scenes_get_scenes_head_v2(void);
f_scene_method_v2_t ezlopi_scene_get_method_v2(e_scene_method_type_t scene_method_type);

l_scenes_list_v2_t *ezlopi_scenes_get_by_id_v2(uint32_t _id);
l_scenes_list_v2_t *ezlopi_scenes_new_scene_populate(cJSON *cj_new_scene, uint32_t scene_id);

// e_scene_value_type_v2_t ezlopi_scenes_get_value_type(cJSON *cj_field);
// e_scene_value_type_v2_t ezlopi_scenes_get_expressions_value_type(cJSON *cj_value_type);

void ezlopi_scene_add_users_in_notifications(l_scenes_list_v2_t *scene_node, cJSON *cj_user);

void ezlopi_scenes_delete(l_scenes_list_v2_t *scenes_list);
void ezlopi_scenes_delete_field_value(l_fields_v2_t *field);
void ezlopi_scenes_delete_when_blocks(l_when_block_v2_t *when_blocks);
void ezlopi_scenes_delete_house_modes(l_house_modes_v2_t *house_modes);
void ezlopi_scenes_delete_action_blocks(l_action_block_v2_t *action_blocks);
void ezlopi_scenes_delete_user_notifications(l_user_notification_v2_t *user_notifications);

void ezlopi_scenes_depopulate_by_id_v2(uint32_t _id);
ezlopi_error_t ezlopi_scenes_enable_disable_scene_by_id_v2(uint32_t _id, bool enabled_flag);
void ezlopi_scenes_remove_id_from_list_v2(uint32_t _id);
l_scenes_list_v2_t *ezlopi_scenes_pop_by_id_v2(uint32_t _id);

void ezlopi_scenes_notifications_add(cJSON *cj_notifications);

#if 0 // for future usage
//-------------------------------- Only for latch operations  ----------------------------------------
/**
 * @brief This function checks for 'latch' struct within nvs_scenes. The scenes are filtered out using 'sceneId[necessary]' & 'blockId[optional]'
 *
 * @param sceneId_str contains required sceneId value
 * @param blockId_str contains required blockId value (when-condition). If (blockID == NULL) ; means to delete all latches contained within sceneId.
 * @param enable_status enable [true or false] -> [1 or 0]
 * @return successful reset => 1 / else => 0.
 */
int ezlopi_core_scene_set_reset_latch_enable(const char* sceneId_str, const char* blockId_str, bool enable_status);
#endif

// ----- # below function are for APIs # ---------
ezlopi_error_t ezlopi_core_scene_block_enable_set_reset(const char *sceneId_str, const char *blockId_str, bool enable_status);
int ezlopi_core_scene_reset_latch_state(const char *sceneId_str, const char *blockId_str);
int ezlopi_core_scene_reset_when_block(const char *sceneId_str, const char *blockId_str);
// ----- # below function are for APIs # ---------
ezlopi_error_t ezlopi_core_scene_meta_by_id(const char *sceneId_str, const char *blockId_str, cJSON *cj_meta);
int ezlopi_core_scenes_get_time_list(cJSON *cj_scenes_array);

// ----- # below function are called when 'creating' and 'editing' scene # ---------
int ezlopi_core_scene_add_group_id_if_reqd(cJSON *cj_new_scene);
int ezlopi_core_scene_add_when_blockId_if_reqd(cJSON *cj_new_scene);

l_when_block_v2_t *ezlopi_core_scene_get_group_block(uint32_t scene_id, uint32_t group_id);
int ezlopi_scenes_block_list_get_trigger_device(cJSON *cj_devices_array);
// ---------------------------------------------------------------------------------
#endif // CONFIG_EZPI_SERV_ENABLE_MESHBOTS

#endif // _EZLOPI_CORE_SCENES_V2_H_
