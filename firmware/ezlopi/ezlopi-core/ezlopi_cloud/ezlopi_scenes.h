#ifndef __EZLOPI_SCENES_H__
#define __EZLOPI_SCENES_H__

// https://api.ezlo.com/hub/scenes/local_scenes_api/index.html#hubsceneslist

{
    "blockOptions" : {
        "method" : {
            "args" : {
                "item" : "item",
                "value" : "value"
            },
            "name" : "setItemValue"
        }
    },
                     "blockType" : "then",
                                   "fields" : [
                                       {
                                           "name" : "item",
                                           "type" : "item",
                                           "value" : "897607_32771_1"
                                       },
                                       {
                                           "name" : "value",
                                           "type" : "int",
                                           "value" : 10
                                       }
                                   ]
}

typedef struct l_then_block
{
    struct l_then_block *next;
} l_then_block_t;

typedef struct l_when_block
{
    struct l_when_block *next;
} l_when_block_t;

typedef struct l_user_notification
{
    char user_id[32];
    struct l_user_notification *next;
} l_user_notification_t;

typedef struct l_house_modes
{
    char house_mode[4];
    struct l_house_modes *next;
} l_house_modes_t;

typedef struct l_scenes_params
{
    char _id[32];
    bool enabled;
    bool is_group;
    char group_id[32];
    char name[32];
    char parent_id[32];

    struct l_scenes_params *next;
} l_scenes_params_t;

#endif //  __EZLOPI_SCENES_H__
