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

/**
 * @brief Fetch expressions from NVS flash and populate to linklist
 * 
 */
void ezlopi_scenes_expressions_init(void);

/**
 * @brief Populate new expression to the linklist, and store it to the flash
 *
 * @param exp_id expression id, if is zero then 'cj_expression' is cosidered as new expression and stored to nvs
 * @param cj_expression cJSON pointer to the expression
 * @return uint32_t expression id, if input 'exp_id' is zero then new expression-id is created and returned
 */
uint32_t ezlopi_scenes_expressions_add_to_head(uint32_t exp_id, cJSON *cj_expression);

/**
 * @brief POP the expression from linklist
 * 
 * @return s_ezlopi_expressions_t* 
 */
s_ezlopi_expressions_t *ezlopi_scenes_expressions_node_pop(void);

/**
 * @brief Delete the expression-item and its childs
 * 
 * @param exp_items 
 */
void ezlopi_scenes_expressions_delete_exp_item(s_exp_items_t *exp_items);

/**
 * @brief Delete the expression-device item name and its childs
 * 
 * @param exp_device_item_names 
 */
void ezlopi_scenes_expressions_delete_exp_device_item_names(s_exp_device_item_names_t *exp_device_item_names);

/**
 * @brief Delete the expressions node and its childs 
 * 
 * @param exp_node 
 */
void ezlopi_scenes_expressions_delete_node(s_ezlopi_expressions_t *exp_node);

#endif // __EZLOPI_SCENES_EXPRESSIONS_H__
