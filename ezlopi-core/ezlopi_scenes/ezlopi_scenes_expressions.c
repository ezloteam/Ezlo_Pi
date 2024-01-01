#include <string.h>
#include <cJSON.h>

#include "ezlopi_nvs.h"
#include "ezlopi_devices.h"
#include "ezlopi_scenes_v2.h"
#include "ezlopi_cjson_macros.h"

#include "ezlopi_scenes_expressions.h"

static s_ezlopi_expressions_t *l_expressions_head = NULL;

static uint32_t __expression_store_to_nvs(uint32_t exp_id, cJSON *cj_expression);

static void __expressions_meta_data_add(s_ezlopi_expressions_t *exp_node, cJSON *cj_meta_data);
static void __expressions_get_value(s_ezlopi_expressions_t *exp_node, cJSON *cj_value, e_scene_value_type_v2_t value_type);

static s_exp_items_t *__expressions_items_create(cJSON *cj_item);
static void __expressions_items_add(s_ezlopi_expressions_t *exp_node, cJSON *cj_items);

static s_exp_device_item_names_t *__expressions_device_item_names_create(cJSON *cj_device_item_name);
static void __expressions_device_item_names_add(s_ezlopi_expressions_t *exp_node, cJSON *cj_device_item_names);

static s_ezlopi_expressions_t *__expressions_create_node(uint32_t exp_id, cJSON *cj_expression);

uint32_t ezlopi_scenes_expressions_add_to_head(uint32_t exp_id, cJSON *cj_expression)
{
    uint32_t new_exp_id = 0;

    if (l_expressions_head)
    {
        s_ezlopi_expressions_t *current_exp = l_expressions_head;
        while (current_exp->next)
        {
            current_exp = current_exp->next;
        }

        current_exp->next = __expressions_create_node(exp_id, cj_expression);
        if (current_exp->next)
        {
            new_exp_id = current_exp->next->exp_id;
        }
    }
    else
    {
        l_expressions_head = __expressions_create_node(exp_id, cj_expression);
        if (l_expressions_head)
        {
            new_exp_id = l_expressions_head->exp_id;
        }
    }

    return new_exp_id;
}

void ezlopi_scenes_expressions_delete_exp_item(s_exp_items_t *exp_items)
{
    if (exp_items)
    {
        ezlopi_scenes_expressions_delete_exp_item(exp_items->next);
        free(exp_items);
    }
}

void ezlopi_scenes_expressions_delete_exp_device_item_names(s_exp_device_item_names_t *exp_device_item_names)
{
    if (exp_device_item_names)
    {
        ezlopi_scenes_expressions_delete_exp_device_item_names(exp_device_item_names->next);
        free(exp_device_item_names);
    }
}

s_ezlopi_expressions_t *ezlopi_scenes_expressions_node_pop(void)
{
    s_ezlopi_expressions_t *popped_node = NULL;

    if (l_expressions_head)
    {
        // if ( check-condition )
        // {

        // }

        s_ezlopi_expressions_t *curr_node = l_expressions_head;
        while (curr_node)
        {
            // if ( check-condition )
            {
            }
        }
    }

    return popped_node;
}

void ezlopi_scenes_expressions_delete_node(s_ezlopi_expressions_t *exp_node)
{
    if (exp_node)
    {
        ezlopi_scenes_expressions_delete_node(exp_node->next);

        if (exp_node->code)
        {
            free(exp_node->code);
        }

        if (exp_node->meta_data)
        {
            free(exp_node->meta_data);
        }

        ezlopi_scenes_expressions_delete_exp_item(exp_node->items);
        ezlopi_scenes_expressions_delete_exp_device_item_names(exp_node->device_item_names);
        ezlopi_nvs_delete_stored_expression(exp_node->exp_id); // added nvs key-value removal
    }
}

void ezlopi_scenes_expressions_factory_info_reset(void)
{
    if (l_expressions_head)
    {
        ezlopi_scenes_expressions_delete_node(l_expressions_head);
    }
}

void ezlopi_scenes_expressions_init(void)
{
    char *exp_id_list_str = ezlopi_nvs_read_scenes_expressions();
    if (exp_id_list_str)
    {
        cJSON *cj_exp_id_list = cJSON_Parse(exp_id_list_str);
        if (cj_exp_id_list)
        {
            uint32_t exp_idx = 0;
            cJSON *cj_exp_id;

            while (NULL != (cj_exp_id = cJSON_GetArrayItem(cj_exp_id_list, exp_idx++)))
            {
                if (cj_exp_id->valuestring)
                {
                    uint32_t exp_id = strtoul(cj_exp_id->valuestring, NULL, 16);
                    if (exp_id)
                    {
                        char *exp_str = ezlopi_nvs_read_str(cj_exp_id->valuestring);
                        if (exp_str)
                        {
                            cJSON *cj_exp = cJSON_Parse(exp_str);
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

static s_exp_items_t *__expressions_items_create(cJSON *cj_item)
{
    s_exp_items_t *new_item_node = NULL;

    if (cj_item)
    {
        new_item_node = malloc(sizeof(s_exp_items_t));
        if (new_item_node)
        {
            memset(new_item_node, 0, sizeof(s_exp_items_t));
            CJSON_GET_VALUE_STRING_BY_COPY(cj_item, "name", new_item_node->name);
            char *item_id_string = NULL;
            CJSON_GET_VALUE_STRING(cj_item, "_id", item_id_string);
            if (item_id_string)
            {
                new_item_node->_id = strtoul(item_id_string, NULL, 16);
            }
        }
    }

    return new_item_node;
}

static void __expressions_items_add(s_ezlopi_expressions_t *exp_node, cJSON *cj_items)
{
    if (cj_items)
    {
        uint32_t item_index = 0;
        cJSON *cj_item = NULL;
        while (NULL != (cj_item = cJSON_GetArrayItem(cj_items, item_index++)))
        {
            if (exp_node->items)
            {
                s_exp_items_t *exp_item_node = exp_node->items;
                while (exp_item_node->next)
                {
                    exp_item_node = exp_item_node->next;
                }

                exp_item_node->next = __expressions_items_create(cj_item);
            }
            else
            {
                exp_node->items = __expressions_items_create(cj_item);
            }
        }
    }
}

static s_exp_device_item_names_t *__expressions_device_item_names_create(cJSON *cj_device_item_name)
{
    s_exp_device_item_names_t *new_device_item_name = NULL;

    if (cj_device_item_name)
    {
        new_device_item_name = malloc(sizeof(s_exp_device_item_names_t));
        if (new_device_item_name)
        {
            memset(new_device_item_name, 0, sizeof(s_exp_device_item_names_t));
            CJSON_GET_VALUE_STRING_BY_COPY(cj_device_item_name, "name", new_device_item_name->name);
            CJSON_GET_VALUE_STRING_BY_COPY(cj_device_item_name, "deviceName", new_device_item_name->device_name);
            CJSON_GET_VALUE_STRING_BY_COPY(cj_device_item_name, "itemName", new_device_item_name->item_name);
        }
    }

    return new_device_item_name;
}

static void __expressions_device_item_names_add(s_ezlopi_expressions_t *exp_node, cJSON *cj_device_item_names)
{
    if (cj_device_item_names)
    {
        uint32_t dev_item_name_index = 0;
        cJSON *cj_dev_item_name = NULL;
        while (NULL != (cj_dev_item_name = cJSON_GetArrayItem(cj_device_item_names, dev_item_name_index++)))
        {
            if (exp_node->device_item_names)
            {
                s_exp_device_item_names_t *curr_node = exp_node->device_item_names;
                while (curr_node->next)
                {
                    curr_node = curr_node->next;
                }

                curr_node->next = __expressions_device_item_names_create(cj_dev_item_name);
            }
            else
            {
                exp_node->device_item_names = __expressions_device_item_names_create(cj_dev_item_name);
            }
        }
    }
}

static void __expressions_meta_data_add(s_ezlopi_expressions_t *exp_node, cJSON *cj_meta_data)
{
    if (cj_meta_data)
    {
        exp_node->meta_data = cJSON_Print(cj_meta_data);
    }
}

static void __expressions_get_value(s_ezlopi_expressions_t *exp_node, cJSON *cj_value, e_scene_value_type_v2_t value_type)
{
    if (exp_node && cj_value && value_type)
    {
        switch (cj_value->type)
        {
        case cJSON_Number:
        {
            exp_node->exp_value.double_value = cj_value->valuedouble;
            TRACE_B("value: %f", exp_node->exp_value.double_value);
            break;
        }
        case cJSON_String:
        {
            uint32_t value_len = strlen(cj_value->valuestring) + 1;
            exp_node->exp_value.str_value = malloc(value_len);
            if (exp_node->exp_value.str_value)
            {
                snprintf(exp_node->exp_value.str_value, value_len, "%s", cj_value->valuestring);
                TRACE_B("value: %s", exp_node->exp_value.str_value);
            }
            else
            {
                TRACE_E("Malloc failed!");
            }
            break;
        }
        case cJSON_True:
        {
            exp_node->exp_value.boolean_value = true;
            TRACE_B("value: true");
            break;
        }
        case cJSON_False:
        {
            exp_node->exp_value.boolean_value = false;
            TRACE_B("value: false");
            break;
        }
        case cJSON_Array:
        default:
        {
            TRACE_E("cj_value type: %d", cj_value->type);
            break;
        }
        }
    }
}

static s_ezlopi_expressions_t *__expressions_create_node(uint32_t exp_id, cJSON *cj_expression)
{
    s_ezlopi_expressions_t *new_exp_node = malloc(sizeof(s_ezlopi_expressions_t));

    if (new_exp_node)
    {
        char *code_str = NULL;

        memset(new_exp_node, 0, sizeof(s_ezlopi_expressions_t));
        CJSON_GET_VALUE_BOOL(cj_expression, "variable", new_exp_node->variable);
        CJSON_GET_VALUE_STRING_BY_COPY(cj_expression, "name", new_exp_node->name);
        CJSON_GET_VALUE_STRING(cj_expression, "code", code_str);

        if (code_str)
        {
            uint32_t code_str_len = strlen(code_str) + 1;
            new_exp_node->code = malloc(code_str_len);
            if (new_exp_node->code)
            {
                memset(new_exp_node->code, 0, code_str_len);
                strcpy(new_exp_node->code, code_str);
            }
        }

        cJSON *cj_params = cJSON_GetObjectItem(cj_expression, "params");

        if (cj_params)
        {
            __expressions_items_add(new_exp_node, cJSON_GetObjectItem(cj_params, "items"));
            __expressions_device_item_names_add(new_exp_node, cJSON_GetObjectItem(cj_params, "device_item_names"));
        }

        __expressions_meta_data_add(new_exp_node, cJSON_GetObjectItem(cj_expression, "metadata"));
        new_exp_node->value_type = ezlopi_scenes_get_expressions_value_type(cJSON_GetObjectItem(cj_expression, "valueType"));
        __expressions_get_value(new_exp_node, cJSON_GetObjectItem(cj_expression, "value"), new_exp_node->value_type);

        new_exp_node->exp_id = __expression_store_to_nvs(exp_id, cj_expression);
    }

    return new_exp_node;
}

static uint32_t __expression_store_to_nvs(uint32_t exp_id, cJSON *cj_expression)
{
    if (0 == exp_id)
    {
        char *exp_string = cJSON_Print(cj_expression);
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
                    char *exp_id_list_str = ezlopi_nvs_read_scenes_expressions();
                    if (exp_id_list_str)
                    {
                        exp_id_list_str = "[]";
                        free_exp_id_list_str = 0;
                    }

                    if (exp_id_list_str)
                    {
                        cJSON *cj_exp_id_list = cJSON_Parse(exp_id_list_str);

                        if (free_exp_id_list_str)
                        {
                            free(exp_id_list_str);
                            exp_id_list_str = NULL;
                        }

                        if (cj_exp_id_list)
                        {
                            cJSON *cj_exp_id = cJSON_CreateString(exp_id_str);
                            if (cj_exp_id)
                            {
                                if (cJSON_AddItemToArray(cj_exp_id_list, cj_exp_id))
                                {
                                    exp_id_list_str = cJSON_Print(cj_exp_id_list);
                                    if (exp_id_list_str)
                                    {
                                        cJSON_Minify(exp_id_list_str);
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
                }
            }

            free(exp_string);
        }
    }

    return exp_id;
}
