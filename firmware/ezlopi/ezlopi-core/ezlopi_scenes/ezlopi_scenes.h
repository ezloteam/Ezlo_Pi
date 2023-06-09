#ifndef __EZLOPI_SCENES_H__
#define __EZLOPI_SCENES_H__

// https://api.ezlo.com/hub/scenes/local_scenes_api/index.html#hubsceneslist

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

    uint32_t value_type; // 0: double, 1: string
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
    char _id[32];
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

#endif //  __EZLOPI_SCENES_H__
