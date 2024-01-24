#ifndef _EZLOPI_CORE_SCENES_EXPRESSIONS_H_
#define _EZLOPI_CORE_SCENES_EXPRESSIONS_H_

#include <string.h>
#include <inttypes.h>
#include <stdbool.h>

#include "ezlopi_core_scenes_v2.h"

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
    EXPRESSION_VALUE_TYPE_UNDEFINED = 0, // type not defined
    EXPRESSION_VALUE_TYPE_STRING,        // char *
    EXPRESSION_VALUE_TYPE_CJ,            // cJSON *
    EXPRESSION_VALUE_TYPE_BOOL,          // bool
    EXPRESSION_VALUE_TYPE_NUMBER,        // number

    EXPRESSION_VALUE_TYPE_MAX,
} e_exp_value_type_t;

typedef union u_exp_value
{
    char *str_value;
    cJSON *cj_value;
    bool boolean_value;
    double number_value;
} u_exp_value_t;

typedef struct s_exp_value
{
    u_exp_value_t u_value;
    e_exp_value_type_t type;
} s_exp_value_t;

typedef struct s_ezlopi_expressions
{
    uint32_t exp_id;
    char name[32];
    char *code;
    s_exp_items_t *items;
    s_exp_device_item_names_t *device_item_names;
    cJSON *meta_data; // not used for now
    bool variable;

    s_exp_value_t exp_value;
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
 * @brief construct expressions in cJSON and add it to cj_expression_array
 *
 * @param cj_expresson_array
 */
void ezlopi_scenes_expressions_list_cjson(cJSON *cj_expresson_array, cJSON *cj_params);

/**
 * @brief print the informations inside exp-node
 *
 * @param exp_node
 */
void ezlopi_scenes_expressions_print(s_ezlopi_expressions_t *exp_node);

/**
 * @brief Delete all the expressions in the linked list
 *
 * @param exp_node
 */
int ezlopi_scenes_expressions_delete_node(s_ezlopi_expressions_t *exp_node);

/**
 * @brief Delete the expression by its name
 *
 * @param expression_name
 * @return int return 1 on suceess and 0 on failed
 */
int ezlopi_scenes_expressions_delete_by_name(char *expression_name);

#endif // _EZLOPI_CORE_SCENES_EXPRESSIONS_H_
