#ifndef __EZLOPI_SCENES_EXPRESSIONS_H__
#define __EZLOPI_SCENES_EXPRESSIONS_H__

#include "string.h"
#include "inttypes.h"
#include "stdbool.h"

#include "ezlopi_scenes_v2.h"

typedef struct s_exp_items
{
    char name[48];
    uint32_t _id;
    struct s_exp_items *next;

} s_exp_items_t;

typedef struct s_exp_device_item_names
{
    char name[48];
    char device_name[48];
    char item_name[48];
    struct s_exp_device_item_names *next;

} s_exp_device_item_names_t;

typedef enum e_exp_value_type
{
    EXPRESSION_VALUE_TYPE_UNDEFINED = 0,
    EXPRESSION_VALUE_TYPE_STRING,
    EXPRESSION_VALUE_TYPE_U8,
    EXPRESSION_VALUE_TYPE_I8,
    EXPRESSION_VALUE_TYPE_U16,
    EXPRESSION_VALUE_TYPE_I16,
    EXPRESSION_VALUE_TYPE_U32,
    EXPRESSION_VALUE_TYPE_I32,
    EXPRESSION_VALUE_TYPE_F,
    EXPRESSION_VALUE_TYPE_D,

    EXPRESSION_VALUE_TYPE_MAX,
} e_exp_value_type_t;

typedef union u_exp_value
{
    char *str_value;
    int8_t i_8_value;
    uint8_t u_8_value;
    int16_t i_16_value;
    uint16_t u_16_value;
    int32_t i_32_value;
    uint32_t u_32_value;
    float float_value;
    double double_value;
    bool boolean_value;

} u_exp_value_t;

typedef struct s_ezlopi_expressions
{
    uint32_t exp_id;
    char name[32];
    char *code;
    s_exp_items_t *items;
    s_exp_device_item_names_t *device_item_names;
    char *meta_data; // not used for now
    bool variable;

    u_exp_value_t exp_value;
    e_scene_value_type_v2_t value_type; // value type returned by expression
    // e_exp_value_type_t exp_value_type;

    struct s_ezlopi_expressions *next;

} s_ezlopi_expressions_t;

#endif // __EZLOPI_SCENES_EXPRESSIONS_H__
