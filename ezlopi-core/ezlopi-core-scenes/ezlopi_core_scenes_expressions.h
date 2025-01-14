/* ===========================================================================
** Copyright (C) 2025 Ezlo Innovation Inc
**
** Under EZLO AVAILABLE SOURCE LICENSE (EASL) AGREEMENT
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met:
**
** 1. Redistributions of source code must retain the above copyright notice,
**    this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. Neither the name of the copyright holder nor the names of its
**    contributors may be used to endorse or promote products derived from
**    this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
** AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
** ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
** LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
** CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
** SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
** INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
** CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
** ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
** POSSIBILITY OF SUCH DAMAGE.
** ===========================================================================
*/
/**
 * @file    ezlopi_core_scenes_expressions.h
 * @brief   These function performs operation on scene-expression data
 * @author  ezlopi_team_np
 * @version 0.1
 * @date    12th DEC 2024
 */

#ifndef _EZLOPI_CORE_SCENES_EXPRESSIONS_H_
#define _EZLOPI_CORE_SCENES_EXPRESSIONS_H_

#ifdef CONFIG_EZPI_SERV_ENABLE_MESHBOTS

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include <string.h>
#include <inttypes.h>
// #include <stdbool.h>

// #include "ezlopi_core_errors.h"
#include "ezlopi_core_scenes_v2.h"

/*******************************************************************************
 *                          C++ Declaration Wrapper
 *******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

    /*******************************************************************************
     *                          Type & Macro Declarations
     *******************************************************************************/
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
        char name[32];
        s_exp_value_t exp_value;
        cJSON *meta_data; // not used for now
        char *code;
        s_exp_items_t *items;
        s_exp_device_item_names_t *device_item_names;
        e_scene_value_type_v2_t value_type; // value type returned by expression
        uint32_t exp_id;
        // e_exp_value_type_t exp_value_type;
        bool variable;
        struct s_ezlopi_expressions *next;

    } s_ezlopi_expressions_t;

    /*******************************************************************************
     *                          Extern Data Declarations
     *******************************************************************************/

    /*******************************************************************************
     *                          Extern Function Prototypes
     *******************************************************************************/
    /**
     * @brief Fetch expressions from NVS flash and populate to linklist
     */
    ezlopi_error_t EZPI_scenes_expressions_init(void);

    /**
     * @brief Get 'cjson-object' from expression node.
     *
     * @param exp_node node to be converted
     * @return cJSON*
     */
    cJSON *EZPI_scenes_expressions_get_cjson(s_ezlopi_expressions_t *exp_node);

    /**
     * @brief updates expresson in nvs associated with target :- 'nvs_exp_id_key'.
     *
     * @param nvs_exp_id_key This is string of actual 'exp_id' to replace [must be in nvs_exp_list]
     * @param cj_updated_exp This 'cjson' is 'new_exp_node' to be stored in nvs
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_scenes_expressions_update_nvs(char *nvs_exp_id_key, cJSON *cj_updated_exp);

    /**
     * @brief returns expression with "targe_exp_name"
     *
     * @param target_exp_name  name of the expression
     * @return s_ezlopi_expressions_t* (Null if invalid)
     */
    s_ezlopi_expressions_t *EZPI_scenes_expression_get_by_name(char *target_exp_name);

    /**
     * @brief Populate new expression to the linklist, and store it to the flash
     *
     * @param exp_id expression id, if is zero then 'cj_expression' is cosidered as new expression and stored to nvs
     * @param cj_expression cJSON pointer to the expression
     * @return uint32_t expression id, if input 'exp_id' is zero then new expression-id is created and returned
     */
    uint32_t EZPI_scenes_expressions_add_to_head(uint32_t exp_id, cJSON *cj_expression);

    /**
     * @brief Populate 'items' information into the new_expression_node : 'exp_node'
     *
     * @param exp_node ptr to new_expression
     * @param cj_items CJSON containing :- items_ll
     */
    void EZPI_scenes_expressions_populate_items(s_ezlopi_expressions_t *exp_node, cJSON *cj_items);

    /**
     * @brief Populate 'device_item_names' information into new_expresssion_node : 'exp_node'
     *
     * @param exp_node ptr to new_expression
     * @param cj_items CJSON containing :- device_item_names_ll
     */
    void EZPI_scenes_expressions_populate_devitem_names(s_ezlopi_expressions_t *exp_node, cJSON *cj_device_item_names);

    /**
     * @brief Pop expression with '_id' from expression_ll
     *
     * @param _id target 'exp_id' to be popped from ll
     * @return s_ezlopi_expressions_t*
     */
    s_ezlopi_expressions_t *EZPI_scenes_expressions_node_pop_by_id(uint32_t _id);

    /**
     * @brief Delete the expression 'items' and its childs
     *
     * @param exp_items Pointer to 'items' block
     */
    void EZPI_scenes_expressions_delete_items(s_exp_items_t *exp_items);

    /**
     * @brief Delete the expression 'device_item_names' and its childs
     *
     * @param exp_device_item_names Pointer to 'device_item_names' block
     */
    void EZPI_scenes_expressions_delete_devitem_names(s_exp_device_item_names_t *exp_device_item_names);

    /**
     * @brief creates list of expressions stored till now & appends to 'cj_expression_array' obj.
     *
     * @param cj_expresson_array Pointer to 'cjson' containing resultant list.
     */
    void EZPI_scenes_expressions_list_cjson(cJSON *cj_expresson_array, cJSON *cj_params);

    /**
     * @brief print the informations contained in 'exp-node'
     *
     * @param exp_node Pointer to target expression_node
     */
    void EZPI_scenes_expressions_print(s_ezlopi_expressions_t *exp_node);

    /**
     * @brief Delete perticular 'expression' from linked list
     *
     * @param exp_node Pointer to target Node
     */
    int EZPI_scenes_expressions_delete_node(s_ezlopi_expressions_t *exp_node);

    /**
     * @brief Delete the expression by its name
     *
     * @param expression_name Name of the target expression_node
     * @return int return 1 on suceess and 0 on failed
     */
    int EZPI_scenes_expressions_delete_by_name(char *expression_name);

    /**
     * @brief return the desired experssion by its name
     *
     * @param expression_name  Name of the target expression_node
     * @return s_ezlopi_expressions_t*
     */
    s_ezlopi_expressions_t *EZPI_scenes_expressions_get_node_by_name(char *expression_name);

    /**
     * @brief replaces the 'old_nvs_exp' with 'new_nvs_exp'
     *
     * @param expression_node   The 'll_expression_node' containing 'target_exp_id' that indicated 'nvs_exp_target_id' to
     * @param cj_new_expression     cjson of the 'new_expn' which replace 'older_nvs_exp'
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_scenes_expressions_update_expr(s_ezlopi_expressions_t *expression_node, cJSON *cj_new_expression);

    /**
     * @brief evaluate expression for 'isItemStateChange when-method'
     *
     * @param cj_des  This contains result-values
     * @param exp_name Name of the expression
     * @param exp_code Expression Code to execute
     * @return int
     */
    int EZPI_scenes_expressions_eval_simple(cJSON *cj_des, const char *exp_name, const char *exp_code);

#ifdef __cplusplus
}
#endif

#endif // CONFIG_EZPI_SERV_ENABLE_MESHBOTS

#endif // _EZLOPI_CORE_SCENES_EXPRESSIONS_H_
/*******************************************************************************
 *                          End of File
 *******************************************************************************/
