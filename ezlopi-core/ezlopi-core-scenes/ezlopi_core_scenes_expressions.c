#include "../../build/config/sdkconfig.h"

#ifdef CONFIG_EZPI_SERV_ENABLE_MESHBOTS

#include "cjext.h"

#include "ezlopi_core_nvs.h"
#include "ezlopi_core_devices.h"
#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_core_scenes_value.h"
#include "ezlopi_core_scenes_expressions.h"

#include "ezlopi_cloud_constants.h"
#include "EZLOPI_USER_CONFIG.h"

static s_ezlopi_expressions_t* l_expressions_head = NULL;

static uint32_t __expression_store_to_nvs(uint32_t exp_id, cJSON* cj_expression);
static void __get_expressions_value(s_ezlopi_expressions_t* exp_node, cJSON* cj_value, e_scene_value_type_v2_t value_type);
static s_exp_items_t* __expressions_items_create(cJSON* cj_item);
void __get_expressions_items(s_ezlopi_expressions_t* exp_node, cJSON* cj_items);

static s_exp_device_item_names_t* __expressions_device_item_names_create(cJSON* cj_device_item_name);
void __get_expressions_device_item_names(s_ezlopi_expressions_t* exp_node, cJSON* cj_device_item_names);
static s_ezlopi_expressions_t* __expressions_create_node(uint32_t exp_id, cJSON* cj_expression);

static bool __check_expression_type_filter(s_ezlopi_expressions_t* exp_node, e_scene_value_type_v2_t* type_filter_arr);
static e_scene_value_type_v2_t* __parse_expression_type_filter(cJSON* cj_params);
static void __add_expression_value(s_ezlopi_expressions_t* exp_node, cJSON* cj_expr);
static void __add_expression_items(s_ezlopi_expressions_t* exp_node, cJSON* cj_params);
static void __add_expression_device_item_names(s_ezlopi_expressions_t* exp_node, cJSON* cj_params);
static int __remove_exp_id_from_nvs_exp_list(uint32_t target_id);

s_ezlopi_expressions_t* ezlopi_scenes_get_expression_node_by_name(char* expression_name)
{
    s_ezlopi_expressions_t* curr_expr = l_expressions_head;
    if (expression_name && curr_expr)
    {
        while (curr_expr)
        {
            if (EZPI_STRNCMP_IF_EQUAL(curr_expr->name, expression_name, strlen(curr_expr->name), strlen(expression_name)))
            {
                break;
            }
            curr_expr = curr_expr->next;
        }
    }
    return curr_expr;
}

cJSON * generate_expression_node_in_cjson(s_ezlopi_expressions_t* exp_node)
{
    cJSON * ret_cj_exp = NULL;
    if (exp_node)
    {
        ret_cj_exp = cJSON_CreateObject(__FUNCTION__);
        if (ret_cj_exp)
        {
            cJSON_AddStringToObject(__FUNCTION__, ret_cj_exp, ezlopi_name_str, exp_node->name);
            cJSON_AddStringToObject(__FUNCTION__, ret_cj_exp, ezlopi_code_str, exp_node->code);
            cJSON_AddBoolToObject(__FUNCTION__, ret_cj_exp, ezlopi_variable_str, exp_node->variable);
            __add_expression_value(exp_node, ret_cj_exp);

            cJSON *cj_params = cJSON_AddObjectToObject(__FUNCTION__, ret_cj_exp, ezlopi_params_str);
            if (cj_params)
            {
                __add_expression_items(exp_node, cj_params);
                __add_expression_device_item_names(exp_node, cj_params);
            }

            if (exp_node->meta_data)
            {
                cJSON_AddItemToObject(__FUNCTION__, ret_cj_exp, ezlopi_metadata_str, cJSON_Duplicate(__FUNCTION__, exp_node->meta_data, 1));
            }

            // TRACE_S("New_modified ; %s[%#x]", exp_node->name, exp_node->exp_id);
            // CJSON_TRACE("cj_exp_after_changes_in_ll", ret_cj_exp);
        }
    }
    return ret_cj_exp;
}

int ezlopi_scenes_expressions_update_nvs(char* nvs_exp_id_key, cJSON * cj_updated_exp)
{
    int ret = 0;
    if (cj_updated_exp)
    {
        char* update_exp_str = cJSON_PrintBuffered(__FUNCTION__, cj_updated_exp, 1024, false);
        TRACE_D("length of 'update_exp_str': %d", strlen(update_exp_str));

        if (update_exp_str)
        {
            ezlopi_nvs_delete_stored_data_by_name(nvs_exp_id_key);
            if (1 == (ret = ezlopi_nvs_write_str(update_exp_str, strlen(update_exp_str), nvs_exp_id_key)))
            {
                TRACE_S("successfully saved/modified expression in nvs");
            }
            ezlopi_free(__FUNCTION__, update_exp_str);
        }
    }
    return ret;
}

s_ezlopi_expressions_t* ezlopi_scenes_expressions_node_pop_by_id(uint32_t _id)
{
    s_ezlopi_expressions_t* popped_node = NULL;

    if (_id == l_expressions_head->exp_id)
    {
        popped_node = l_expressions_head;
        l_expressions_head = l_expressions_head->next;
        popped_node->next = NULL;
    }
    else
    {
        s_ezlopi_expressions_t* curr_node = l_expressions_head;
        while (curr_node->next)
        {
            if (_id == curr_node->next->exp_id)
            {
                popped_node = curr_node->next;
                curr_node->next = curr_node->next->next;
                popped_node->next = NULL;
                break;
            }
            curr_node = curr_node->next;
        }
    }

    return popped_node;
}

int ezlopi_scenes_expressions_delete_by_name(char* expression_name)
{
    int ret = 0;
    if (expression_name)
    {
        s_ezlopi_expressions_t* curr_expr = l_expressions_head;
        // s_ezlopi_expressions_t* prev_expr = NULL;
        while (curr_expr)
        {
            if (EZPI_STRNCMP_IF_EQUAL(curr_expr->name, expression_name, strlen(curr_expr->name), strlen(expression_name)))
            {
                /* Depopulating the 'exp_id' from 'expression_ll' */
                ret = ezlopi_scenes_expressions_delete_node(ezlopi_scenes_expressions_node_pop_by_id(curr_expr->exp_id));

                // s_ezlopi_expressions_t* del_expression = curr_expr;
                // if (del_expression == l_expressions_head)
                // {
                //     l_expressions_head = l_expressions_head->next;
                //     del_expression->next = NULL;
                //     ret = ezlopi_scenes_expressions_delete_node(del_expression);
                // }
                // else
                // {
                //     // prev_expr->next = curr_expr->next;
                //     del_expression->next = NULL;
                //     ret = ezlopi_scenes_expressions_delete_node(del_expression);
                // }

                break;
            }

            // prev_expr = curr_expr;
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
                cJSON* cj_expr = cJSON_CreateObject(__FUNCTION__);
                if (cj_expr)
                {
                    char exp_id[32];
                    snprintf(exp_id, sizeof(exp_id), "%08x", curr_exp->exp_id);
                    cJSON_AddStringToObject(__FUNCTION__, cj_expr, ezlopi__id_str, exp_id);
                    cJSON_AddStringToObject(__FUNCTION__, cj_expr, ezlopi_name_str, curr_exp->name);

                    if (show_code && curr_exp->code)
                    {
                        cJSON_AddStringToObject(__FUNCTION__, cj_expr, ezlopi_code_str, curr_exp->code);
                    }

                    if (curr_exp->meta_data)
                    {
                        cJSON_AddItemReferenceToObject(__FUNCTION__, cj_expr, ezlopi_metadata_str, curr_exp->meta_data);
                    }

                    cJSON* cj_params = cJSON_AddObjectToObject(__FUNCTION__, cj_expr, ezlopi_params_str);
                    if (cj_params)
                    {
                        __add_expression_items(curr_exp, cj_params);
                        __add_expression_device_item_names(curr_exp, cj_params);
                    }

                    __add_expression_value(curr_exp, cj_expr);
                }

                if (!cJSON_AddItemToArray(cj_expresson_array, cj_expr))
                {
                    cJSON_Delete(__FUNCTION__, cj_expr);
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
        ezlopi_free(__FUNCTION__, exp_items);
    }
}

void ezlopi_scenes_expressions_delete_exp_device_item_names(s_exp_device_item_names_t* exp_device_item_names)
{
    if (exp_device_item_names)
    {
        ezlopi_scenes_expressions_delete_exp_device_item_names(exp_device_item_names->next);
        ezlopi_free(__FUNCTION__, exp_device_item_names);
    }
}

int ezlopi_scenes_expressions_delete_node(s_ezlopi_expressions_t* exp_node)
{
    int ret = 0;
    if (exp_node)
    {
        ret = ezlopi_scenes_expressions_delete_node(exp_node->next);

        // 1. clear in the node in 'expression_ll'
        {
            if (exp_node->code)
            {
                ezlopi_free(__FUNCTION__, exp_node->code);
            }

            switch (exp_node->exp_value.type)
            {
            case EXPRESSION_VALUE_TYPE_STRING:
            {
                if (exp_node->exp_value.u_value.str_value)
                {
                    ezlopi_free(__FUNCTION__, exp_node->exp_value.u_value.str_value);
                    exp_node->exp_value.u_value.str_value = NULL;
                }
                break;
            }
            case EXPRESSION_VALUE_TYPE_CJ:
            {
                if (exp_node->exp_value.u_value.cj_value)
                {
                    cJSON_Delete(__FUNCTION__, exp_node->exp_value.u_value.cj_value);
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
                ezlopi_free(__FUNCTION__, exp_node->meta_data);
            }

            ezlopi_scenes_expressions_delete_exp_item(exp_node->items);
            ezlopi_scenes_expressions_delete_exp_device_item_names(exp_node->device_item_names);
        }

        // 2. clearing from NVS
        {
            ezlopi_nvs_delete_stored_data_by_id(exp_node->exp_id);// remove 'target_exp_id' from nvs
            /* Now to update 'expression_nvs_list' after removing 'exp_id' from nvs */
            ret = __remove_exp_id_from_nvs_exp_list(exp_node->exp_id);
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

        cJSON* cj_exp_id_list = cJSON_Parse(__FUNCTION__, exp_id_list_str);
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
                            cJSON* cj_exp = cJSON_Parse(__FUNCTION__, exp_str);
                            if (cj_exp)
                            {
                                ezlopi_scenes_expressions_add_to_head(exp_id, cj_exp);
                                cJSON_Delete(__FUNCTION__, cj_exp);
                            }

                            ezlopi_free(__FUNCTION__, exp_str);
                        }
                    }
                }
            }
        }

        ezlopi_free(__FUNCTION__, exp_id_list_str);
    }
}

static s_exp_items_t* __expressions_items_create(cJSON* cj_item)
{
    s_exp_items_t* new_item_node = NULL;

    if (cj_item)
    {
        new_item_node = ezlopi_malloc(__FUNCTION__, sizeof(s_exp_items_t));
        if (new_item_node)
        {
            memset(new_item_node, 0, sizeof(s_exp_items_t));
            CJSON_GET_VALUE_STRING_BY_COPY(cj_item, ezlopi_name_str, new_item_node->name);
            CJSON_GET_ID(new_item_node->_id, cJSON_GetObjectItem(__FUNCTION__, cj_item, ezlopi__id_str));
        }
        else
        {
            TRACE_E("Error: malloc failed!");
        }
    }

    return new_item_node;
}

void __get_expressions_items(s_ezlopi_expressions_t* exp_node, cJSON* cj_items)
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
        new_device_item_name = ezlopi_malloc(__FUNCTION__, sizeof(s_exp_device_item_names_t));
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

void __get_expressions_device_item_names(s_ezlopi_expressions_t* exp_node, cJSON* cj_device_item_names)
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
            exp_node->exp_value.u_value.str_value = ezlopi_malloc(__FUNCTION__, value_len);
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
            exp_node->exp_value.u_value.cj_value = cJSON_Duplicate(__FUNCTION__, cj_value, cJSON_True);
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
    s_ezlopi_expressions_t* new_exp_node = ezlopi_malloc(__FUNCTION__, sizeof(s_ezlopi_expressions_t));

    if (new_exp_node)
    {
        memset(new_exp_node, 0, sizeof(s_ezlopi_expressions_t));

        CJSON_GET_VALUE_STRING_BY_COPY(cj_expression, ezlopi_name_str, new_exp_node->name);

        cJSON * cj_code = cJSON_GetObjectItem(__FUNCTION__, cj_expression, ezlopi_code_str);
        if (cj_code && cj_code->valuestring && cj_code->str_value_len)
        {
            new_exp_node->code = ezlopi_malloc(__FUNCTION__, cj_code->str_value_len + 1);
            if (new_exp_node->code)
            {
                snprintf(new_exp_node->code, cj_code->str_value_len + 1, "%.*s", cj_code->str_value_len, cj_code->valuestring);
            }
        }

        cJSON* cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_expression, ezlopi_params_str);

        if (cj_params)
        {
            cJSON* cj_items = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_items_str);
            __get_expressions_items(new_exp_node, cj_items);

            cJSON* cj_device_item_names = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_device_item_names_str);
            __get_expressions_device_item_names(new_exp_node, cj_device_item_names);
        }

        CJSON_GET_VALUE_BOOL(cj_expression, ezlopi_variable_str, new_exp_node->variable);

        cJSON* cj_metaData = cJSON_GetObjectItem(__FUNCTION__, cj_expression, ezlopi_metadata_str);
        if (cj_metaData)
        {
            new_exp_node->meta_data = cJSON_Duplicate(__FUNCTION__, cj_metaData, cJSON_True);
        }


        new_exp_node->value_type = ezlopi_core_scenes_value_get_type(cj_expression, ezlopi_valueType_str);
        __get_expressions_value(new_exp_node, cJSON_GetObjectItem(__FUNCTION__, cj_expression, ezlopi_value_str), new_exp_node->value_type);

        new_exp_node->exp_id = __expression_store_to_nvs(exp_id, cj_expression);
        // ezlopi_scenes_expressions_print(new_exp_node);
    }

    return new_exp_node;
}

static uint32_t __expression_store_to_nvs(uint32_t exp_id, cJSON* cj_expression)
{
    if (0 == exp_id)
    {
        char* exp_string = cJSON_PrintBuffered(__FUNCTION__, cj_expression, 1024, false);
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
                    cJSON* cj_exp_id_list = cJSON_Parse(__FUNCTION__, exp_id_list_str);

                    if (free_exp_id_list_str)
                    {
                        ezlopi_free(__FUNCTION__, exp_id_list_str);
                        exp_id_list_str = NULL;
                    }

                    CJSON_TRACE("cj_esp-ids", cj_exp_id_list);

                    if (cj_exp_id_list)
                    {
                        // TRACE_D("Here");
                        cJSON* cj_exp_id = cJSON_CreateString(__FUNCTION__, exp_id_str);
                        if (cj_exp_id)
                        {
                            // TRACE_D("Here");
                            if (cJSON_AddItemToArray(cj_exp_id_list, cj_exp_id))
                            {
                                exp_id_list_str = cJSON_PrintBuffered(__FUNCTION__, cj_exp_id_list, 1024, false);
                                TRACE_D("length of 'exp_id_list_str': %d", strlen(exp_id_list_str));

                                if (exp_id_list_str)
                                {
                                    ezlopi_nvs_write_scenes_expressions(exp_id_list_str);
                                    ezlopi_free(__FUNCTION__, exp_id_list_str);
                                }
                            }
                            else
                            {
                                cJSON_Delete(__FUNCTION__, cj_exp_id);
                            }
                        }

                        cJSON_Delete(__FUNCTION__, cj_exp_id_list);
                    }
                }
                else
                {
                }
            }

            ezlopi_free(__FUNCTION__, exp_string);
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
    cJSON* cj_types_filter_array = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_filterTypes_str);
    if (cj_types_filter_array)
    {
        type_filter_array = ezlopi_calloc(__FUNCTION__, sizeof(e_scene_value_type_v2_t), cJSON_GetArraySize(cj_types_filter_array) + 1);
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
        cJSON_AddStringToObject(__FUNCTION__, cj_expr, ezlopi_valueType_str, ezlopi_scene_get_scene_value_type_name(exp_node->value_type));
        switch (exp_node->exp_value.type)
        {
        case EXPRESSION_VALUE_TYPE_STRING:
        {
            cJSON_AddStringToObject(__FUNCTION__, cj_expr, ezlopi_value_str, exp_node->exp_value.u_value.str_value);
            break;
        }
        case EXPRESSION_VALUE_TYPE_BOOL:
        {
            cJSON_AddBoolToObject(__FUNCTION__, cj_expr, ezlopi_value_str, exp_node->exp_value.u_value.boolean_value);
            break;
        }
        case EXPRESSION_VALUE_TYPE_NUMBER:
        {
            cJSON_AddNumberToObject(__FUNCTION__, cj_expr, ezlopi_value_str, exp_node->exp_value.u_value.number_value);
            break;
        }
        case EXPRESSION_VALUE_TYPE_CJ:
        {
            cJSON_AddItemToObject(__FUNCTION__, cj_expr, ezlopi_value_str, cJSON_Duplicate(__FUNCTION__, exp_node->exp_value.u_value.cj_value, 1));
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
        cJSON* cj_items = cJSON_AddArrayToObject(__FUNCTION__, cj_params, ezlopi_items_str);
        if (cj_items)
        {
            s_exp_items_t* curr_item = exp_node->items;
            while (curr_item)
            {
                cJSON* cj_item = cJSON_CreateObject(__FUNCTION__);
                if (cj_item)
                {
                    cJSON_AddStringToObject(__FUNCTION__, cj_item, ezlopi_name_str, curr_item->name);

                    char id_str[32];
                    snprintf(id_str, sizeof(id_str), "%08x", curr_item->_id);
                    cJSON_AddStringToObject(__FUNCTION__, cj_item, ezlopi__id_str, id_str);
                    if (!cJSON_AddItemToArray(cj_items, cj_item))
                    {
                        cJSON_Delete(__FUNCTION__, cj_item);
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
        cJSON* cj_device_item_names = cJSON_AddArrayToObject(__FUNCTION__, cj_params, ezlopi_device_item_names_str);
        if (cj_device_item_names)
        {
            s_exp_device_item_names_t* curr_device_item_names = exp_node->device_item_names;
            while (curr_device_item_names)
            {
                cJSON* cj_device_item_name = cJSON_CreateObject(__FUNCTION__);
                if (cj_device_item_name)
                {
                    cJSON_AddStringToObject(__FUNCTION__, cj_device_item_name, ezlopi_name_str, curr_device_item_names->name);
                    cJSON_AddStringToObject(__FUNCTION__, cj_device_item_name, ezlopi_deviceName_str, curr_device_item_names->device_name);
                    cJSON_AddStringToObject(__FUNCTION__, cj_device_item_name, ezlopi_itemName_str, curr_device_item_names->item_name);

                    if (!cJSON_AddItemToArray(cj_device_item_names, cj_device_item_name))
                    {
                        cJSON_Delete(__FUNCTION__, cj_device_item_name);
                    }
                }
                curr_device_item_names = curr_device_item_names->next;
            }
        }
    }
}

static int __remove_exp_id_from_nvs_exp_list(uint32_t target_id)
{
    int ret = 0;
    /* Now to update_list_in_nvs*/
    char* exp_ids = ezlopi_nvs_read_scenes_expressions();
    if (exp_ids)
    {
        cJSON* cj_exp_ids = cJSON_Parse(__FUNCTION__, exp_ids);
        ezlopi_free(__FUNCTION__, exp_ids);

        if (cj_exp_ids)
        {
            CJSON_TRACE("expression-ids", cj_exp_ids);

            uint32_t idx = 0;
            cJSON* cj_exp_id = NULL;
            while (NULL != (cj_exp_id = cJSON_GetArrayItem(cj_exp_ids, idx)))
            {
                uint32_t _id = strtoul(cj_exp_id->valuestring, NULL, 16);
                if (_id == target_id)
                {
                    cJSON_DeleteItemFromArray(__FUNCTION__, cj_exp_ids, idx);
                    break;
                }

                idx++;
            }

            char* updated_ids_str = cJSON_PrintBuffered(__FUNCTION__, cj_exp_ids, 1024, false);
            TRACE_D("length of 'updated_ids_str': %d", strlen(updated_ids_str));

            cJSON_Delete(__FUNCTION__, cj_exp_ids);

            if (updated_ids_str)
            {
                TRACE_D("updated-expression-ids: %s", updated_ids_str);
                ezlopi_nvs_write_scenes_expressions(updated_ids_str);
                ezlopi_free(__FUNCTION__, updated_ids_str);
                ret = 1;
            }
        }
    }
    return ret;

}
#endif  // CONFIG_EZPI_SERV_ENABLE_MESHBOTS