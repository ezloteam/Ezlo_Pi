// #include <string.h>
#include <cJSON.h>

#include "ezlopi_core_nvs.h"
#include "ezlopi_core_devices.h"
#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_core_scenes_value.h"
#include "ezlopi_core_scenes_expressions.h"

#include "ezlopi_cloud_constants.h"

static s_ezlopi_expressions_t* l_expressions_head = NULL;

static uint32_t __expression_store_to_nvs(uint32_t exp_id, cJSON* cj_expression);
static void __get_expressions_value(s_ezlopi_expressions_t* exp_node, cJSON* cj_value, e_scene_value_type_v2_t value_type);
static s_exp_items_t* __expressions_items_create(cJSON* cj_item);
static void __get_expressions_items(s_ezlopi_expressions_t* exp_node, cJSON* cj_items);

static s_exp_device_item_names_t* __expressions_device_item_names_create(cJSON* cj_device_item_name);
static void __get_expressions_device_item_names(s_ezlopi_expressions_t* exp_node, cJSON* cj_device_item_names);
static s_ezlopi_expressions_t* __expressions_create_node(uint32_t exp_id, cJSON* cj_expression);

static bool __check_expression_type_filter(s_ezlopi_expressions_t* exp_node, e_scene_value_type_v2_t* type_filter_arr);
static e_scene_value_type_v2_t* __parse_expression_type_filter(cJSON* cj_params);
static void __add_expression_value(s_ezlopi_expressions_t* exp_node, cJSON* cj_expr);
static void __add_expression_items(s_ezlopi_expressions_t* exp_node, cJSON* cj_params);
static void __add_expression_device_item_names(s_ezlopi_expressions_t* exp_node, cJSON* cj_params);

int ezlopi_scenes_expressions_delete_by_name(char* expression_name)
{
    int ret = 0;
    if (expression_name)
    {
        size_t del_name_len = strlen(expression_name);
        s_ezlopi_expressions_t* curr_expr = l_expressions_head;
        s_ezlopi_expressions_t* prev_expr = NULL;

        while (curr_expr)
        {
            size_t exp_name_len = strlen(curr_expr->name);
            size_t cmp_len = del_name_len > exp_name_len ? del_name_len : exp_name_len;
            if (0 == strncmp(curr_expr->name, expression_name, cmp_len))
            {
                s_ezlopi_expressions_t* del_expression = curr_expr;

                if (del_expression == l_expressions_head)
                {
                    l_expressions_head = l_expressions_head->next;
                    del_expression->next = NULL;
                    ret = ezlopi_scenes_expressions_delete_node(del_expression);
                }
                else
                {
                    prev_expr->next = curr_expr->next;
                    del_expression->next = NULL;
                    ret = ezlopi_scenes_expressions_delete_node(del_expression);
                }

                break;
            }

            prev_expr = curr_expr;
            curr_expr = curr_expr->next;
        }
    }

    return ret;
}

void ezlopi_scenes_expressions_list_cjson(cJSON* cj_expresson_array, cJSON* cj_params)
{
    if (cj_expresson_array)
    {
        bool show_code = false;
        e_scene_value_type_v2_t* type_filter_array = NULL;

        if (cj_params)
        {
            CJSON_GET_VALUE_BOOL(cj_params, ezlopi_showCode_str, show_code);
            type_filter_array = __parse_expression_type_filter(cj_params);
        }

        s_ezlopi_expressions_t* curr_exp = l_expressions_head;
        while (curr_exp)
        {
            if (__check_expression_type_filter(curr_exp, type_filter_array))
            {
                cJSON* cj_expr = cJSON_CreateObject();
                if (cj_expr)
                {
                    char exp_id[32];
                    snprintf(exp_id, sizeof(exp_id), "%08x", curr_exp->exp_id);
                    cJSON_AddStringToObject(cj_expr, ezlopi__id_str, exp_id);
                    cJSON_AddStringToObject(cj_expr, ezlopi_name_str, curr_exp->name);
                    if (show_code && curr_exp->code)
                    {
                        cJSON_AddStringToObject(cj_expr, ezlopi_code_str, curr_exp->code);
                    }
                    if (curr_exp->meta_data)
                    {
                        cJSON_AddItemReferenceToObject(cj_expr, ezlopi_metadata_str, curr_exp->meta_data);
                    }

                    cJSON* cj_params = cJSON_AddObjectToObject(cj_expr, ezlopi_params_str);
                    if (cj_params)
                    {
                        __add_expression_items(curr_exp, cj_params);
                        __add_expression_device_item_names(curr_exp, cj_params);
                    }

                    __add_expression_value(curr_exp, cj_expr);
                }

                if (!cJSON_AddItemToArray(cj_expresson_array, cj_expr))
                {
                    cJSON_Delete(cj_expr);
                }
            }

            curr_exp = curr_exp->next;
        }
    }
}

void ezlopi_scenes_expressions_print(s_ezlopi_expressions_t* exp_node)
{
#if (ENABLE_TRACE)
    if (exp_node)
    {
        TRACE_D("----------------- Expression --------------------");
        TRACE_D("Name: %s", exp_node->name);
        TRACE_D("Exp_id: %08x", exp_node->exp_id);
        TRACE_D("code: %s", exp_node->code ? exp_node->code : ezlopi__str);

        TRACE_D("-- Items:");
        int count = 0;
        s_exp_items_t* items = exp_node->items;
        while (items)
        {
            TRACE_D("\t-------------- item-%d ------------", ++count);
            TRACE_D("\tname: %s", items->name);
            TRACE_D("\t_id: %08x", items->_id);
            items = items->next;
        }
        TRACE_D("\t-----------------------------------");

        TRACE_D("-- Device Item Names:");
        count = 0;
        s_exp_device_item_names_t* device_item_names = exp_node->device_item_names;
        while (device_item_names)
        {
            TRACE_D("\t----------device-item-names-%d-------", ++count);
            TRACE_D("\tname: %s", device_item_names->name);
            TRACE_D("\titem-name: %s", device_item_names->item_name);
            TRACE_D("\tdevice-name: %s", device_item_names->device_name);
            device_item_names = device_item_names->next;
        }
        TRACE_D("\t-----------------------------------");

        CJSON_TRACE("meta-data", exp_node->meta_data);
        TRACE_D("Is variable: %s", exp_node->variable ? "True" : "False");
        TRACE_D("value-type: %s", ezlopi_scene_get_scene_value_type_name(exp_node->value_type));

        if (exp_node->exp_value.type > EXPRESSION_VALUE_TYPE_UNDEFINED && exp_node->exp_value.type < EXPRESSION_VALUE_TYPE_MAX)
        {
            switch (exp_node->exp_value.type)
            {
            case EXPRESSION_VALUE_TYPE_STRING:
            {
                TRACE_D("value: %s", exp_node->exp_value.u_value.str_value ? exp_node->exp_value.u_value.str_value : ezlopi__str);
                break;
            }
            case EXPRESSION_VALUE_TYPE_CJ:
            {
                CJSON_TRACE(ezlopi_value_str, exp_node->exp_value.u_value.cj_value);
                break;
            }
            case EXPRESSION_VALUE_TYPE_BOOL:
            {
                TRACE_D("value: %s", exp_node->exp_value.u_value.boolean_value ? ezlopi_true_str : ezlopi_false_str);
                break;
            }
            case EXPRESSION_VALUE_TYPE_NUMBER:
            {
                TRACE_D("value: %lf", exp_node->exp_value.u_value.number_value);
                break;
            }

            default:
            {
                TRACE_W("exp_node->exp_value.type: value type is undefined!");
                break;
            }
            }
        }

        TRACE_D("----------------- ---------- --------------------");
    }
#endif
}

uint32_t ezlopi_scenes_expressions_add_to_head(uint32_t exp_id, cJSON* cj_expression)
{
    uint32_t new_exp_id = 0;

    if (l_expressions_head)
    {
        s_ezlopi_expressions_t* current_exp = l_expressions_head;
        while (current_exp->next)
        {
            current_exp = current_exp->next;
        }

        current_exp->next = __expressions_create_node(exp_id, cj_expression);
        if (current_exp->next)
        {
            ezlopi_scenes_expressions_print(current_exp->next);
            new_exp_id = current_exp->next->exp_id;
        }
    }
    else
    {
        l_expressions_head = __expressions_create_node(exp_id, cj_expression);
        if (l_expressions_head)
        {
            ezlopi_scenes_expressions_print(l_expressions_head);
            new_exp_id = l_expressions_head->exp_id;
        }
    }

    return new_exp_id;
}

void ezlopi_scenes_expressions_delete_exp_item(s_exp_items_t* exp_items)
{
    if (exp_items)
    {
        ezlopi_scenes_expressions_delete_exp_item(exp_items->next);
        free(exp_items);
    }
}

void ezlopi_scenes_expressions_delete_exp_device_item_names(s_exp_device_item_names_t* exp_device_item_names)
{
    if (exp_device_item_names)
    {
        ezlopi_scenes_expressions_delete_exp_device_item_names(exp_device_item_names->next);
        free(exp_device_item_names);
    }
}

s_ezlopi_expressions_t* ezlopi_scenes_expressions_node_pop(void)
{
    s_ezlopi_expressions_t* popped_node = NULL;

    if (l_expressions_head)
    {
        // if ( check-condition )
        // {

        // }

        s_ezlopi_expressions_t* curr_node = l_expressions_head;
        while (curr_node)
        {
            // if ( check-condition )
            {
            }
        }
    }

    return popped_node;
}

int ezlopi_scenes_expressions_delete_node(s_ezlopi_expressions_t* exp_node)
{
    int ret = 0;
    if (exp_node)
    {
        ret = ezlopi_scenes_expressions_delete_node(exp_node->next);

        if (exp_node->code)
        {
            free(exp_node->code);
        }

        switch (exp_node->exp_value.type)
        {
        case EXPRESSION_VALUE_TYPE_STRING:
        {
            if (exp_node->exp_value.u_value.str_value)
            {
                free(exp_node->exp_value.u_value.str_value);
                exp_node->exp_value.u_value.str_value = NULL;
            }
            break;
        }
        case EXPRESSION_VALUE_TYPE_CJ:
        {
            if (exp_node->exp_value.u_value.cj_value)
            {
                cJSON_Delete(exp_node->exp_value.u_value.cj_value);
                exp_node->exp_value.u_value.cj_value = NULL;
            }
            break;
        }
        default:
        {
            break;
        }
        }

        if (exp_node->meta_data)
        {
            free(exp_node->meta_data);
        }

        ezlopi_scenes_expressions_delete_exp_item(exp_node->items);
        ezlopi_scenes_expressions_delete_exp_device_item_names(exp_node->device_item_names);

        ezlopi_nvs_delete_stored_data_by_id(exp_node->exp_id);
        char* exp_ids = ezlopi_nvs_read_scenes_expressions();
        if (exp_ids)
        {
            cJSON* cj_exp_ids = cJSON_Parse(exp_ids);
            free(exp_ids);

            if (cj_exp_ids)
            {
                CJSON_TRACE("expression-ids", cj_exp_ids);

                uint32_t idx = 0;
                cJSON* cj_exp_id = NULL;
                while (NULL != (cj_exp_id = cJSON_GetArrayItem(cj_exp_ids, idx)))
                {
                    uint32_t _id = strtoul(cj_exp_id->valuestring, NULL, 16);
                    if (_id == exp_node->exp_id)
                    {
                        cJSON_DeleteItemFromArray(cj_exp_ids, idx);
                        break;
                    }

                    idx++;
                }

                char* updated_ids_str = cJSON_PrintBuffered(cj_exp_ids, 1024, false);
                TRACE_D("length of 'updated_ids_str': %d", strlen(updated_ids_str));

                cJSON_Delete(cj_exp_ids);

                if (updated_ids_str)
                {
                    TRACE_D("updated-expression-ids: %s", updated_ids_str);
                    ezlopi_nvs_write_scenes_expressions(updated_ids_str);
                    free(updated_ids_str);
                    ret = 1;
                }
            }
        }
    }

    return ret;
}

void ezlopi_scenes_expressions_init(void)
{
    char* exp_id_list_str = ezlopi_nvs_read_scenes_expressions();
    if (exp_id_list_str)
    {
        TRACE_D("exp_id_list_str: %s", exp_id_list_str);

        cJSON* cj_exp_id_list = cJSON_Parse(exp_id_list_str);
        if (cj_exp_id_list)
        {
            uint32_t exp_idx = 0;
            cJSON* cj_exp_id = NULL;

            while (NULL != (cj_exp_id = cJSON_GetArrayItem(cj_exp_id_list, exp_idx++)))
            {
                if (cj_exp_id->valuestring)
                {
                    uint32_t exp_id = strtoul(cj_exp_id->valuestring, NULL, 16);
                    if (exp_id)
                    {
                        char* exp_str = ezlopi_nvs_read_str(cj_exp_id->valuestring);
                        if (exp_str)
                        {
                            cJSON* cj_exp = cJSON_Parse(exp_str);
                            if (cj_exp)
                            {
                                ezlopi_scenes_expressions_add_to_head(exp_id, cj_exp);
                                cJSON_Delete(cj_exp);
                            }

                            free(exp_str);
                        }
                    }
                }
            }
        }

        free(exp_id_list_str);
    }
}

static s_exp_items_t* __expressions_items_create(cJSON* cj_item)
{
    s_exp_items_t* new_item_node = NULL;

    if (cj_item)
    {
        new_item_node = malloc(sizeof(s_exp_items_t));
        if (new_item_node)
        {
            memset(new_item_node, 0, sizeof(s_exp_items_t));
            CJSON_GET_VALUE_STRING_BY_COPY(cj_item, ezlopi_name_str, new_item_node->name);
            char* item_id_string = NULL;
            CJSON_GET_VALUE_STRING(cj_item, ezlopi__id_str, item_id_string);
            if (item_id_string)
            {
                new_item_node->_id = strtoul(item_id_string, NULL, 16);
            }
        }
        else
        {
            TRACE_E("Error: malloc failed!");
        }
    }

    return new_item_node;
}

static void __get_expressions_items(s_ezlopi_expressions_t* exp_node, cJSON* cj_items)
{
    if (cj_items)
    {
        uint32_t item_index = 0;
        cJSON* cj_item = NULL;
        s_exp_items_t* new_item_head = NULL;
        s_exp_items_t* curr_item_node = NULL;

        while (NULL != (cj_item = cJSON_GetArrayItem(cj_items, item_index++)))
        {
            if (new_item_head)
            {
                curr_item_node->next = __expressions_items_create(cj_item);
                curr_item_node = curr_item_node->next;
            }
            else
            {
                new_item_head = __expressions_items_create(cj_item);
                curr_item_node = new_item_head;
            }
        }

        if (exp_node->items)
        {
            s_exp_items_t* exp_item_node = exp_node->items;
            while (exp_item_node->next)
            {
                exp_item_node = exp_item_node->next;
            }

            exp_item_node->next = new_item_head;
        }
        else
        {
            exp_node->items = new_item_head;
        }
    }
}

static s_exp_device_item_names_t* __expressions_device_item_names_create(cJSON* cj_device_item_name)
{
    s_exp_device_item_names_t* new_device_item_name = NULL;

    if (cj_device_item_name)
    {
        new_device_item_name = malloc(sizeof(s_exp_device_item_names_t));
        if (new_device_item_name)
        {
            memset(new_device_item_name, 0, sizeof(s_exp_device_item_names_t));
            CJSON_GET_VALUE_STRING_BY_COPY(cj_device_item_name, ezlopi_name_str, new_device_item_name->name);
            CJSON_GET_VALUE_STRING_BY_COPY(cj_device_item_name, ezlopi_deviceName_str, new_device_item_name->device_name);
            CJSON_GET_VALUE_STRING_BY_COPY(cj_device_item_name, ezlopi_itemName_str, new_device_item_name->item_name);
        }
    }

    return new_device_item_name;
}

static void __get_expressions_device_item_names(s_ezlopi_expressions_t* exp_node, cJSON* cj_device_item_names)
{
    if (cj_device_item_names)
    {
        uint32_t dev_item_name_index = 0;
        cJSON* cj_dev_item_name = NULL;
        s_exp_device_item_names_t* new_device_item_names_head = NULL;
        s_exp_device_item_names_t* cur_device_item_names_head = NULL;

        while (NULL != (cj_dev_item_name = cJSON_GetArrayItem(cj_device_item_names, dev_item_name_index++)))
        {
            if (new_device_item_names_head)
            {
                cur_device_item_names_head->next = __expressions_device_item_names_create(cj_dev_item_name);
                cur_device_item_names_head = cur_device_item_names_head->next;
            }
            else
            {
                new_device_item_names_head = __expressions_device_item_names_create(cj_dev_item_name);
                cur_device_item_names_head = new_device_item_names_head;
            }
        }

        if (exp_node->device_item_names)
        {
            s_exp_device_item_names_t* exp_device_item_names_node = exp_node->device_item_names;
            while (exp_device_item_names_node->next)
            {
                exp_device_item_names_node = exp_device_item_names_node->next;
            }

            exp_device_item_names_node->next = new_device_item_names_head;
        }
        else
        {
            exp_node->device_item_names = new_device_item_names_head;
        }
    }
}

static void __get_expressions_value(s_ezlopi_expressions_t* exp_node, cJSON* cj_value, e_scene_value_type_v2_t value_type)
{
    if (exp_node && cj_value && value_type)
    {
        switch (cj_value->type)
        {
        case cJSON_Number:
        {
            exp_node->exp_value.u_value.number_value = cj_value->valuedouble;
            exp_node->exp_value.type = EXPRESSION_VALUE_TYPE_NUMBER;
            TRACE_I("value: %lf", exp_node->exp_value.u_value.number_value);
            break;
        }
        case cJSON_String:
        {
            uint32_t value_len = strlen(cj_value->valuestring) + 1;
            exp_node->exp_value.type = EXPRESSION_VALUE_TYPE_STRING;
            exp_node->exp_value.u_value.str_value = malloc(value_len);
            if (exp_node->exp_value.u_value.str_value)
            {
                snprintf(exp_node->exp_value.u_value.str_value, value_len, "%s", cj_value->valuestring);
                TRACE_I("value: %s", exp_node->exp_value.u_value.str_value);
            }
            else
            {
                TRACE_E("Malloc failed!");
            }
            break;
        }
        case cJSON_True:
        {
            exp_node->exp_value.type = EXPRESSION_VALUE_TYPE_BOOL;
            exp_node->exp_value.u_value.boolean_value = true;
            TRACE_I("value: true");
            break;
        }
        case cJSON_False:
        {
            exp_node->exp_value.type = EXPRESSION_VALUE_TYPE_BOOL;
            exp_node->exp_value.u_value.boolean_value = false;
            TRACE_I("value: false");
            break;
        }
        case cJSON_Array:
        case cJSON_Object:
        {
            TRACE_E("cj_value type: %d", cj_value->type);
            exp_node->exp_value.type = EXPRESSION_VALUE_TYPE_CJ;
            exp_node->exp_value.u_value.cj_value = cJSON_Duplicate(cj_value, cJSON_True);
            break;
        }
        default:
        {
            exp_node->exp_value.u_value.number_value = 0;
            exp_node->exp_value.type = EXPRESSION_VALUE_TYPE_UNDEFINED;
            TRACE_E("Error: Undefine value type: %d", cj_value->type);
            break;
        }
        }
    }
}

static s_ezlopi_expressions_t* __expressions_create_node(uint32_t exp_id, cJSON* cj_expression)
{
    s_ezlopi_expressions_t* new_exp_node = malloc(sizeof(s_ezlopi_expressions_t));

    if (new_exp_node)
    {
        char* code_str = NULL;
        memset(new_exp_node, 0, sizeof(s_ezlopi_expressions_t));

        CJSON_GET_VALUE_STRING_BY_COPY(cj_expression, ezlopi_name_str, new_exp_node->name);
        CJSON_GET_VALUE_STRING(cj_expression, ezlopi_code_str, code_str);

        if (code_str)
        {
            uint32_t code_str_len = strlen(code_str) + 1;
            new_exp_node->code = malloc(code_str_len);
            if (new_exp_node->code)
            {
                memset(new_exp_node->code, 0, code_str_len);
                strcpy(new_exp_node->code, code_str);
            }
            else
            {
                TRACE_E("Error: Failed to alloc code-string!");
            }
        }

        cJSON* cj_params = cJSON_GetObjectItem(cj_expression, ezlopi_params_str);

        if (cj_params)
        {
            cJSON* cj_items = cJSON_GetObjectItem(cj_params, ezlopi_items_str);
            __get_expressions_items(new_exp_node, cj_items);

            cJSON* cj_device_item_names = cJSON_GetObjectItem(cj_params, ezlopi_device_item_names_str);
            __get_expressions_device_item_names(new_exp_node, cj_device_item_names);
        }

        CJSON_GET_VALUE_BOOL(cj_expression, ezlopi_variable_str, new_exp_node->variable);

        new_exp_node->meta_data = cJSON_DetachItemFromObject(cj_expression, ezlopi_metadata_str);
        new_exp_node->value_type = ezlopi_core_scenes_value_get_type(cj_expression, ezlopi_valueType_str);
        __get_expressions_value(new_exp_node, cJSON_GetObjectItem(cj_expression, ezlopi_value_str), new_exp_node->value_type);

        new_exp_node->exp_id = __expression_store_to_nvs(exp_id, cj_expression);
        ezlopi_scenes_expressions_print(new_exp_node);
    }

    return new_exp_node;
}

static uint32_t __expression_store_to_nvs(uint32_t exp_id, cJSON* cj_expression)
{
    if (0 == exp_id)
    {
        char* exp_string = cJSON_PrintBuffered(cj_expression, 1024, false);
        TRACE_D("length of 'exp_string': %d", strlen(exp_string));

        if (exp_string)
        {
            exp_id = ezlopi_cloud_generate_expression_id();
            if (exp_id)
            {
                char exp_id_str[32];
                snprintf(exp_id_str, sizeof(exp_id_str), "%08x", exp_id);

                if (ezlopi_nvs_write_str(exp_string, strlen(exp_string), exp_id_str))
                {
                    bool free_exp_id_list_str = 1;
                    char* exp_id_list_str = ezlopi_nvs_read_scenes_expressions();
                    if (NULL == exp_id_list_str)
                    {
                        exp_id_list_str = "[]";
                        free_exp_id_list_str = 0;
                        TRACE_W("Expressions ids-list not found in NVS");
                    }

                    TRACE_D("Expressions-IDs: %s", exp_id_list_str);
                    cJSON* cj_exp_id_list = cJSON_Parse(exp_id_list_str);

                    if (free_exp_id_list_str)
                    {
                        free(exp_id_list_str);
                        exp_id_list_str = NULL;
                    }

                    CJSON_TRACE("cj_esp-ids", cj_exp_id_list);

                    if (cj_exp_id_list)
                    {
                        TRACE_D("Here");
                        cJSON* cj_exp_id = cJSON_CreateString(exp_id_str);
                        if (cj_exp_id)
                        {
                            TRACE_D("Here");
                            if (cJSON_AddItemToArray(cj_exp_id_list, cj_exp_id))
                            {
                                exp_id_list_str = cJSON_PrintBuffered(cj_exp_id_list, 1024, false);
                                TRACE_D("length of 'exp_id_list_str': %d", strlen(exp_id_list_str));

                                if (exp_id_list_str)
                                {
                                    ezlopi_nvs_write_scenes_expressions(exp_id_list_str);
                                    free(exp_id_list_str);
                                }
                            }
                            else
                            {
                                cJSON_Delete(cj_exp_id);
                            }
                        }

                        cJSON_Delete(cj_exp_id_list);
                    }
                }
                else
                {
                }
            }

            free(exp_string);
        }
    }
    else
    {
        ezlopi_cloud_update_expression_id(exp_id);
    }

    return exp_id;
}

static bool __check_expression_type_filter(s_ezlopi_expressions_t* exp_node, e_scene_value_type_v2_t* type_filter_arr)
{
    bool ret = true;
    if (type_filter_arr)
    {
        ret = false;
        uint32_t idx = 0;
        while (type_filter_arr[idx])
        {
            if (type_filter_arr[idx] == exp_node->value_type)
            {
                ret = true;
                break;
            }
            idx++;
        }
    }

    return ret;
}

static e_scene_value_type_v2_t* __parse_expression_type_filter(cJSON* cj_params)
{
    e_scene_value_type_v2_t* type_filter_array = NULL;
    cJSON* cj_types_filter_array = cJSON_GetObjectItem(cj_params, ezlopi_filterTypes_str);
    if (cj_types_filter_array)
    {
        type_filter_array = calloc(sizeof(e_scene_value_type_v2_t), cJSON_GetArraySize(cj_types_filter_array) + 1);
        if (type_filter_array)
        {
            uint32_t idx = 0;
            cJSON* cj_type = NULL;
            while (NULL != (cj_type = cJSON_GetArrayItem(cj_types_filter_array, idx)))
            {
                type_filter_array[idx] = ezlopi_core_scenes_value_get_type(cj_type, NULL);
                idx++;
            }
            type_filter_array[idx] = 0;
        }
    }

    return type_filter_array;
}

static void __add_expression_value(s_ezlopi_expressions_t* exp_node, cJSON* cj_expr)
{
    if (EZLOPI_VALUE_TYPE_NONE < exp_node->value_type && EZLOPI_VALUE_TYPE_MAX > exp_node->value_type)
    {
        cJSON_AddStringToObject(cj_expr, ezlopi_valueType_str, ezlopi_scene_get_scene_value_type_name(exp_node->value_type));
        switch (exp_node->exp_value.type)
        {
        case EXPRESSION_VALUE_TYPE_STRING:
        {
            cJSON_AddStringToObject(cj_expr, ezlopi_value_str, exp_node->exp_value.u_value.str_value);
            break;
        }
        case EXPRESSION_VALUE_TYPE_BOOL:
        {
            cJSON_AddBoolToObject(cj_expr, ezlopi_value_str, exp_node->exp_value.u_value.boolean_value);
            break;
        }
        case EXPRESSION_VALUE_TYPE_NUMBER:
        {
            cJSON_AddNumberToObject(cj_expr, ezlopi_value_str, exp_node->exp_value.u_value.number_value);
            break;
        }
        case EXPRESSION_VALUE_TYPE_CJ:
        {
            break;
        }

        default:
            break;
        }
    }
}

static void __add_expression_items(s_ezlopi_expressions_t* exp_node, cJSON* cj_params)
{
    if (exp_node->items)
    {
        cJSON* cj_items = cJSON_AddArrayToObject(cj_params, ezlopi_items_str);
        if (cj_items)
        {
            s_exp_items_t* curr_item = exp_node->items;
            while (curr_item)
            {
                cJSON* cj_item = cJSON_CreateObject();
                if (cj_item)
                {
                    cJSON_AddStringToObject(cj_item, ezlopi_name_str, curr_item->name);

                    char id_str[32];
                    snprintf(id_str, sizeof(id_str), "%08x", curr_item->_id);
                    cJSON_AddStringToObject(cj_item, ezlopi__id_str, id_str);
                    if (!cJSON_AddItemToArray(cj_items, cj_item))
                    {
                        cJSON_Delete(cj_item);
                    }
                }
                curr_item = curr_item->next;
            }
        }
    }
}

static void __add_expression_device_item_names(s_ezlopi_expressions_t* exp_node, cJSON* cj_params)
{
    if (exp_node->device_item_names)
    {
        cJSON* cj_device_item_names = cJSON_AddArrayToObject(cj_params, ezlopi_device_item_names_str);
        if (cj_device_item_names)
        {
            s_exp_device_item_names_t* curr_device_item_names = exp_node->device_item_names;
            while (curr_device_item_names)
            {
                cJSON* cj_device_item_name = cJSON_CreateObject();
                if (cj_device_item_name)
                {
                    cJSON_AddStringToObject(cj_device_item_name, ezlopi_name_str, curr_device_item_names->name);
                    cJSON_AddStringToObject(cj_device_item_name, ezlopi_deviceName_str, curr_device_item_names->device_name);
                    cJSON_AddStringToObject(cj_device_item_name, ezlopi_itemName_str, curr_device_item_names->item_name);

                    if (!cJSON_AddItemToArray(cj_device_item_names, cj_device_item_name))
                    {
                        cJSON_Delete(cj_device_item_name);
                    }
                }
                curr_device_item_names = curr_device_item_names->next;
            }
        }
    }
}
