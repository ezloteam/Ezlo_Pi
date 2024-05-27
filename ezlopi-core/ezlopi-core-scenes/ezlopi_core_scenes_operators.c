#include "../../build/config/sdkconfig.h"

#ifdef CONFIG_EZPI_SERV_ENABLE_MESHBOTS

#include <string.h>
#include "cjext.h"

#include "ezlopi_util_trace.h"

#include "ezlopi_core_devices.h"
#include "ezlopi_core_scenes_v2.h"
#include "ezlopi_core_scenes_operators.h"
#include "ezlopi_core_scenes_expressions.h"

#include "ezlopi_cloud_constants.h"

//-------------------------------------------------
#define MAX_STRING_LEN(str1, str2) ((strlen(str1) > strlen(str2)) ? strlen(str1) : strlen(str2))
#define OPERATE_ON_STRINGS(STR1, op, STR2) (strncmp(STR1, STR2, MAX_STRING_LEN(STR1, STR2)) op 0) // logical-arrangement of '__ op 0' returns correct results
#define STR_OP_COMP(STR1, op, STR2) \
    ((NULL == STR1)   ? false       \
     : (NULL == STR2) ? false       \
                      : OPERATE_ON_STRINGS(STR1, op, STR2))

static const char* const ezlopi_scenes_value_numeric_range_value_types[] = {
#define EZLOPI_VALUE_TYPE(type, name) name,
#include "ezlopi_core_scenes_value_types.h"
#undef EZLOPI_VALUE_TYPE
};

//-------------------------------------------------

/************* Numeric ************/
static const char* const ezlopi_scenes_num_cmp_operators_op[] = {
#define SCENES_NUM_COMP_OPERATORS(OPERATOR, op, name, method) op,
#include "__operators_macros/__numeric_comparision_operators_macros.h"
#undef SCENES_NUM_COMP_OPERATORS
};

static const char* const ezlopi_scenes_num_cmp_operators_name[] = {
#define SCENES_NUM_COMP_OPERATORS(OPERATOR, op, name, method) name,
#include "__operators_macros/__numeric_comparision_operators_macros.h"
#undef SCENES_NUM_COMP_OPERATORS
};

static const char* const ezlopi_scenes_num_cmp_operators_method[] = {
#define SCENES_NUM_COMP_OPERATORS(OPERATOR, op, name, method) method,
#include "__operators_macros/__numeric_comparision_operators_macros.h"
#undef SCENES_NUM_COMP_OPERATORS
};

e_scene_num_cmp_operators_t ezlopi_scenes_numeric_comparator_operators_get_enum(char* operator_str)
{
    e_scene_num_cmp_operators_t ret = SCENES_NUM_COMP_OPERATORS_NONE + 1;
    if (operator_str)
    {
        while ((ret <= SCENES_NUM_COMP_OPERATORS_MAX) && ezlopi_scenes_num_cmp_operators_op[ret])
        {
            if (0 == strcmp(ezlopi_scenes_num_cmp_operators_op[ret], operator_str))
            {
                break;
            }
            ret++;
        }
    }
    return ret;
}

const char* ezlopi_scenes_numeric_comparator_operators_get_op(e_scene_num_cmp_operators_t operator)
{
    const char* ret = NULL;
    if ((operator> SCENES_NUM_COMP_OPERATORS_NONE) && (operator<SCENES_NUM_COMP_OPERATORS_MAX))
    {
        ret = ezlopi_scenes_num_cmp_operators_op[operator];
    }
    return ret;
}

const char* ezlopi_scenes_numeric_comparator_operators_get_name(e_scene_num_cmp_operators_t operator)
{
    const char* ret = NULL;
    if ((operator> SCENES_NUM_COMP_OPERATORS_NONE) && (operator<SCENES_NUM_COMP_OPERATORS_MAX))
    {
        // TRACE_D("Name: %s", ezlopi_scenes_num_cmp_operators_name[operator]);
        ret = ezlopi_scenes_num_cmp_operators_name[operator];
    }
    return ret;
}

const char* ezlopi_scenes_numeric_comparator_operators_get_method(e_scene_num_cmp_operators_t operator)
{
    const char* ret = NULL;
    if ((operator> SCENES_NUM_COMP_OPERATORS_NONE) && (operator<SCENES_NUM_COMP_OPERATORS_MAX))
    {
        // TRACE_D("Method: %s", ezlopi_scenes_operators_method[operator]);
        ret = ezlopi_scenes_num_cmp_operators_method[operator];
    }
    return ret;
}

double ezlopi_core_scenes_operator_get_item_double_value_current(uint32_t item_id)
{
    double item_value = 0.0;
    l_ezlopi_device_t* device = ezlopi_device_get_head();
    while (device)
    {
        l_ezlopi_item_t* item = device->items;
        while (item)
        {
            if (item->cloud_properties.item_id == item_id)
            {
                cJSON* cj_item_value = cJSON_CreateObject(__FUNCTION__);
                if (cj_item_value)
                {
                    item->func(EZLOPI_ACTION_GET_EZLOPI_VALUE, item, (void*)cj_item_value, NULL);
                    cJSON* cj_value = cJSON_GetObjectItem(__FUNCTION__, cj_item_value, ezlopi_value_str);
                    if (cj_value)
                    {
                        item_value = cj_value->valuedouble;
                    }

                    cJSON_Delete(__FUNCTION__, cj_item_value);
                }

                break;
            }

            item = item->next;
        }

        device = device->next;
    }

    return item_value;
}

int ezlopi_scenes_operators_value_number_operations(l_fields_v2_t* item_exp_field, l_fields_v2_t* value_field, l_fields_v2_t* comparator_field)
{
    int ret = 0;
    if (item_exp_field && value_field && comparator_field)
    {

        double item_exp_value = 0;
        double value_to_compare_with = 0;
        //---------------------- LHS -------------------------
        if (EZLOPI_VALUE_TYPE_EXPRESSION == item_exp_field->value_type)
        {
            // s_ezlopi_expressions_t* curr_expr_left = ezlopi_scenes_get_expression_node_by_name(item_exp_field->field_value.u_value.value_string);
            #warning "extract the double from expression";
            item_exp_value = 0;
        }
        else
        {
            uint32_t item_id = strtoul(item_exp_field->field_value.u_value.value_string, NULL, 16);
            item_exp_value = ezlopi_core_scenes_operator_get_item_double_value_current(item_id);
        }
        //---------------------- RHS -------------------------
        if (EZLOPI_VALUE_TYPE_EXPRESSION == value_field->value_type)
        {
            // s_ezlopi_expressions_t* curr_expr_right = ezlopi_scenes_get_expression_node_by_name(value_field->field_value.u_value.value_string);
            #warning "extract the double from expression";
            value_to_compare_with = 0;
        }
        else
        {
            value_to_compare_with = value_field->field_value.u_value.value_double;
        }
        //----------------------------------------------------
        if (item_exp_value && value_to_compare_with)
        {
            switch (ezlopi_scenes_numeric_comparator_operators_get_enum(comparator_field->field_value.u_value.value_string))
            {
            case SCENES_NUM_COMP_OPERATORS_LESS:
            {
                ret = (item_exp_value < value_to_compare_with);
                break;
            }
            case SCENES_NUM_COMP_OPERATORS_LESS_EQUAL:
            {
                ret = (item_exp_value <= value_to_compare_with);
                break;
            }
            case SCENES_NUM_COMP_OPERATORS_GREATER:
            {
                ret = (item_exp_value > value_to_compare_with);
                break;
            }
            case SCENES_NUM_COMP_OPERATORS_GREATER_EQUAL:
            {
                ret = (item_exp_value >= value_to_compare_with);
                break;
            }
            case SCENES_NUM_COMP_OPERATORS_EQUAL:
            {
                ret = (item_exp_value == value_to_compare_with);
                break;
            }
            case SCENES_NUM_COMP_OPERATORS_NOT_EQUAL:
            {
                ret = (item_exp_value != value_to_compare_with);
                break;
            }
#if 0
            case SCENES_NUM_COMP_OPERATORS_BETWEEN:
            {
                TRACE_W("'SCENES_OPERATORS_BETWEEN' not implemented!");
                break;
            }
            case SCENES_NUM_COMP_OPERATORS_NOT_BETWEEN:
            {
                TRACE_W("'SCENES_OPERATORS_NOT_BETWEEN' not implemented!");
                break;
            }
            case SCENES_NUM_COMP_OPERATORS_ANY_OF:
            {
                TRACE_W("'SCENES_OPERATORS_ANY_OF' not implemented!");
                break;
            }
            case SCENES_NUM_COMP_OPERATORS_NONE_OF:
            {
                TRACE_W("'SCENES_OPERATORS_NONE_OF' not implemented!");
                break;
            }
#endif
            default:
            {
                break;
            }
            }
        }
    }

    return ret;
}

/************* Strings ************/
static const char* const ezlopi_scenes_str_cmp_operators_op[] = {
#define SCENES_STRINGS_OPERATORS(OPERATOR, op, name, method) op,
#include "__operators_macros/__strings_comparision_operators_macros.h"
#undef SCENES_STRINGS_OPERATORS
};

static const char* const ezlopi_scenes_str_cmp_operators_name[] = {
#define SCENES_STRINGS_OPERATORS(OPERATOR, op, name, method) name,
#include "__operators_macros/__strings_comparision_operators_macros.h"
#undef SCENES_STRINGS_OPERATORS
};

static const char* const ezlopi_scenes_str_cmp_operators_method[] = {
#define SCENES_STRINGS_OPERATORS(OPERATOR, op, name, method) method,
#include "__operators_macros/__strings_comparision_operators_macros.h"
#undef SCENES_STRINGS_OPERATORS
};

e_scene_str_cmp_operators_t ezlopi_scenes_strings_comparator_operators_get_enum(char* operator_str)
{
    e_scene_str_cmp_operators_t ret = SCENES_STRINGS_OPERATORS_NONE + 1;
    if (operator_str)
    {
        while ((ret <= SCENES_STRINGS_OPERATORS_MAX) && ezlopi_scenes_str_cmp_operators_op[ret])
        {
            if (0 == strcmp(ezlopi_scenes_str_cmp_operators_op[ret], operator_str))
            {
                break;
            }

            ret++;
        }
    }

    return ret;
}

const char* ezlopi_scenes_strings_comparator_operators_get_op(e_scene_str_cmp_operators_t operator)
{
    const char* ret = NULL;
    if ((operator>= SCENES_STRINGS_OPERATORS_NONE) && (operator<SCENES_STRINGS_OPERATORS_MAX))
    {
        ret = ezlopi_scenes_str_cmp_operators_op[operator];
    }
    return ret;
}

const char* ezlopi_scenes_strings_comparator_operators_get_name(e_scene_str_cmp_operators_t operator)
{
    const char* ret = NULL;
    if ((operator> SCENES_STRINGS_OPERATORS_NONE) && (operator<SCENES_STRINGS_OPERATORS_MAX))
    {
        ret = ezlopi_scenes_str_cmp_operators_name[operator];
    }
    return ret;
}

const char* ezlopi_scenes_strings_comparator_operators_get_method(e_scene_str_cmp_operators_t operator)
{
    const char* ret = NULL;
    if ((operator> SCENES_STRINGS_OPERATORS_NONE) && (operator<SCENES_STRINGS_OPERATORS_MAX))
    {
        ret = ezlopi_scenes_str_cmp_operators_method[operator];
    }
    return ret;
}

char* ezlopi_core_scenes_operator_get_item_string_value_current_by_id(uint32_t item_id)
{
    char* item_value = NULL;
    l_ezlopi_device_t* device = ezlopi_device_get_head();
    while (device)
    {
        l_ezlopi_item_t* item = device->items;
        while (item)
        {
            if (item->cloud_properties.item_id == item_id)
            {
                cJSON* cj_item_value = cJSON_CreateObject(__FUNCTION__);
                if (cj_item_value)
                {
                    item->func(EZLOPI_ACTION_GET_EZLOPI_VALUE, item, (void*)cj_item_value, NULL);
                    cJSON* cj_value = cJSON_GetObjectItem(__FUNCTION__, cj_item_value, ezlopi_value_str);
                    if (cj_value)
                    {
                        if (cJSON_IsString(cj_value))
                        {
                            item_value = cJSON_GetStringValue(cj_value);
                        }
                    }
                    cJSON_Delete(__FUNCTION__, cj_item_value);
                }
                break;
            }
            item = item->next;
        }
        device = device->next;
    }
    return  item_value;
}

int ezlopi_scenes_operators_value_strings_operations(l_fields_v2_t* item_exp_field, l_fields_v2_t* value_field, l_fields_v2_t* comparator_field)
{
    int ret = 0;
    if (item_exp_field && value_field && comparator_field)
    {
        char* item_exp_value_str = NULL;
        char* value_to_compare_with = NULL;

        //---------------------- LHS -------------------------
        if (EZLOPI_VALUE_TYPE_EXPRESSION == item_exp_field->value_type)
        {
            s_ezlopi_expressions_t* curr_expr_left = ezlopi_scenes_get_expression_node_by_name(item_exp_field->field_value.u_value.value_string);
            #warning "extract the 'string' from expression";
            item_exp_value_str = NULL;
        }
        else
        {
            uint32_t item_id = strtoul(item_exp_field->field_value.u_value.value_string, NULL, 16);
            item_exp_value_str = ezlopi_core_scenes_operator_get_item_string_value_current_by_id(item_id);
        }
        //---------------------- RHS -------------------------
        if (EZLOPI_VALUE_TYPE_EXPRESSION == value_field->value_type)
        {
            s_ezlopi_expressions_t* curr_expr_right = ezlopi_scenes_get_expression_node_by_name(value_field->field_value.u_value.value_string);
            #warning "extract the 'string' from expression";
            value_to_compare_with = NULL;
        }
        else
        {
            value_to_compare_with = value_field->field_value.u_value.value_string;
        }
        //----------------------------------------------------

        if (item_exp_value_str && value_to_compare_with)
        {
            e_scene_str_cmp_operators_t string_operator = ezlopi_scenes_strings_comparator_operators_get_enum(comparator_field->field_value.u_value.value_string);
            switch (string_operator)
            {
            case SCENES_STRINGS_OPERATORS_LESS:
            {
                ret = STR_OP_COMP(item_exp_value_str, < , value_to_compare_with);
                break;
            }
            case SCENES_STRINGS_OPERATORS_GREATER:
            {
                ret = STR_OP_COMP(item_exp_value_str, > , value_to_compare_with);
                break;
            }
            case SCENES_STRINGS_OPERATORS_LESS_EQUAL:
            {
                ret = STR_OP_COMP(item_exp_value_str, <= , value_to_compare_with);
                break;
            }
            case SCENES_STRINGS_OPERATORS_GREATER_EQUAL:
            {
                ret = STR_OP_COMP(item_exp_value_str, >= , value_to_compare_with);
                break;
            }
            case SCENES_STRINGS_OPERATORS_EQUAL:
            {
                ret = STR_OP_COMP(item_exp_value_str, == , value_to_compare_with);
                break;
            }
            case SCENES_STRINGS_OPERATORS_NOT_EQUAL:
            {
                ret = STR_OP_COMP(item_exp_value_str, != , value_to_compare_with);
                break;
            }
            default:
            {
                TRACE_E("'SCENES_STRINGS_OPERATORS_* [%d]' out of range!", string_operator);
                break;
            }
            }
        }


    }

    return ret;
}

/************* String_operations ************/
static const char* const ezlopi_scenes_strops_cmp_operators_op[] = {
#define SCENES_STROPS_COMP_OPERATORES(OPERATOR, op, name, method) op,
#include "__operators_macros/__string_op_comparision_operators_macros.h"
#undef SCENES_STROPS_COMP_OPERATORES
};

static const char* const ezlopi_scenes_strops_cmp_operators_name[] = {
#define SCENES_STROPS_COMP_OPERATORES(OPERATOR, op, name, method) name,
#include "__operators_macros/__string_op_comparision_operators_macros.h"
#undef SCENES_STROPS_COMP_OPERATORES
};

static const char* const ezlopi_scenes_strops_cmp_operators_method[] = {
#define SCENES_STROPS_COMP_OPERATORES(OPERATOR, op, name, method) method,
#include "__operators_macros/__string_op_comparision_operators_macros.h"
#undef SCENES_STROPS_COMP_OPERATORES
};

e_scene_strops_cmp_operators_t ezlopi_scenes_strops_comparator_operators_get_enum(char* operator_str)
{
    e_scene_strops_cmp_operators_t ret = SCENES_STROPS_COMP_OPERATORES_NONE + 1;
    if (operator_str)
    {
        while ((ret <= SCENES_STROPS_COMP_OPERATORES_MAX) && ezlopi_scenes_strops_cmp_operators_op[ret])
        {
            if (0 == strcmp(ezlopi_scenes_strops_cmp_operators_op[ret], operator_str))
            {
                break;
            }

            ret++;
        }
    }
    return ret;
}

const char* ezlopi_scenes_strops_comparator_operators_get_op(e_scene_strops_cmp_operators_t operator)
{
    const char* ret = NULL;
    if ((operator>= SCENES_STROPS_COMP_OPERATORES_NONE) && (operator<SCENES_STROPS_COMP_OPERATORES_MAX))
    {
        ret = ezlopi_scenes_strops_cmp_operators_op[operator];
    }
    return ret;
}

const char* ezlopi_scenes_strops_comparator_operators_get_name(e_scene_strops_cmp_operators_t operator)
{
    const char* ret = NULL;
    if ((operator> SCENES_STROPS_COMP_OPERATORES_NONE) && (operator<SCENES_STROPS_COMP_OPERATORES_MAX))
    {
        ret = ezlopi_scenes_strops_cmp_operators_name[operator];
    }
    return ret;
}

const char* ezlopi_scenes_strops_comparator_operators_get_method(e_scene_strops_cmp_operators_t operator)
{
    const char* ret = NULL;
    if ((operator> SCENES_STROPS_COMP_OPERATORES_NONE) && (operator<SCENES_STROPS_COMP_OPERATORES_MAX))
    {
        ret = ezlopi_scenes_strops_cmp_operators_method[operator];
    }
    return ret;
}

static char* ezlopi_scenes_laststr_comp(const char* haystack, const char* needle)
{
    char* loc = NULL;
    char* found = NULL;
    size_t pos = 0;
    while ((found = strstr(haystack + pos, needle)) != 0)
    {
        loc = found;
        pos = (found - haystack) + 1;
    }
    return loc;
}

int ezlopi_scenes_operators_value_strops_operations(l_fields_v2_t* item_exp_field, l_fields_v2_t* value_field, l_fields_v2_t* comparator_field)
{
    int ret = 0;
    if (item_exp_field && value_field && comparator_field)
    {
        char* item_exp_value_str = NULL;
        char* value_to_compare_with = NULL;
        uint32_t value_to_compare_with_num = 0;

        //---------------------- LHS -------------------------
        if (EZLOPI_VALUE_TYPE_EXPRESSION == item_exp_field->value_type)
        {
            s_ezlopi_expressions_t* curr_expr_left = ezlopi_scenes_get_expression_node_by_name(item_exp_field->field_value.u_value.value_string);
            #warning "extract the 'string' from expression";
            item_exp_value_str = NULL;
        }
        else
        {
            uint32_t item_id = strtoul(item_exp_field->field_value.u_value.value_string, NULL, 16);
            item_exp_value_str = ezlopi_core_scenes_operator_get_item_string_value_current_by_id(item_id);
        }
        //---------------------- RHS -------------------------

        if (EZLOPI_VALUE_TYPE_STRING == value_field->value_type)
        {
            value_to_compare_with = value_field->field_value.u_value.value_string;
        }
        else if (EZLOPI_VALUE_TYPE_INT == value_field->value_type)
        {
            value_to_compare_with_num = value_field->field_value.u_value.value_double;
        }
        //----------------------------------------------------

        if (item_exp_value_str && (value_to_compare_with || (value_to_compare_with_num > 0)))
        {
            e_scene_strops_cmp_operators_t strops_operator = ezlopi_scenes_strops_comparator_operators_get_enum(comparator_field->field_value.u_value.value_string);
            switch (strops_operator)
            {

            case SCENES_STROPS_COMP_OPERATORES_BEGINS_WITH:
            {
                char* str_pos = strstr(item_exp_value_str, value_to_compare_with); // finds out the position of first occurance
                if (str_pos)
                {
                    int diff = (str_pos - item_exp_value_str);
                    ret = (0 == diff) ? 1 : 0;
                }
                break;
            }
            case SCENES_STROPS_COMP_OPERATORES_NOT_BEGIN:
            {
                char* str_pos = strstr(item_exp_value_str, value_to_compare_with); // finds out the position of first occurance
                if (str_pos)
                {
                    int diff = (str_pos - item_exp_value_str);
                    ret = (0 < diff) ? 1 : 0;
                }
                break;
            }
            case SCENES_STROPS_COMP_OPERATORES_CONTAINS:
            {
                char* str_pos = strstr(item_exp_value_str, value_to_compare_with); // finds out the position of first occurance
                if (str_pos)
                {
                    int diff = (str_pos - item_exp_value_str);
                    ret = (diff <= 0) ? 1 : 0;
                }
                break;
            }
            case SCENES_STROPS_COMP_OPERATORES_NOT_CONTAIN:
            {
                char* str_pos = strstr(item_exp_value_str, value_to_compare_with); // finds out the position of first occurance
                if (NULL == str_pos)
                {
                    ret = 1;
                }
                break;
            }
            case SCENES_STROPS_COMP_OPERATORES_ENDS_WITH:
            {
                char* last = ezlopi_scenes_laststr_comp(item_exp_value_str, value_to_compare_with); // finds out the position of last occurance
                if (NULL != last)
                {
                    ret = (strlen(value_to_compare_with) == strlen(last)) ? 1 : 0;
                }
                break;
            }
            case SCENES_STROPS_COMP_OPERATORES_NOT_END:
            {
                char* last = ezlopi_scenes_laststr_comp(item_exp_value_str, value_to_compare_with); // finds out the position of last occurance
                if (NULL != last)
                {
                    ret = (strlen(value_to_compare_with) != strlen(last)) ? 1 : 0;
                }
                break;
            }
            case SCENES_STROPS_COMP_OPERATORES_LENGTH:
            {
                ret = (value_to_compare_with_num == strlen(item_exp_value_str)); // int value comparision
                break;
            }
            case SCENES_STROPS_COMP_OPERATORES_NOT_LENGTH:
            {
                ret = (value_to_compare_with_num != strlen(item_exp_value_str)); // int value comparision
                break;
            }
            default:
            {
                TRACE_E("'SCENES_STROPS_COMP_OPERATORES_* [%d]' out of range!", strops_operator);
                break;
            }
            }
        }


    }

    return ret;
}


/************* Values in_array ************/
static const char* const ezlopi_scenes_inarr_cmp_operators_op[] = {
#define SCENES_IN_ARRAY_OPERATORS(OPERATOR, op, name, method) op,
#include "__operators_macros/__in_array_comparision_operators_macros.h"
#undef SCENES_IN_ARRAY_OPERATORS
};

static const char* const ezlopi_scenes_inarr_cmp_operators_name[] = {
#define SCENES_IN_ARRAY_OPERATORS(OPERATOR, op, name, method) name,
#include "__operators_macros/__in_array_comparision_operators_macros.h"
#undef SCENES_IN_ARRAY_OPERATORS
};

static const char* const ezlopi_scenes_inarr_cmp_operators_method[] = {
#define SCENES_IN_ARRAY_OPERATORS(OPERATOR, op, name, method) method,
#include "__operators_macros/__in_array_comparision_operators_macros.h"
#undef SCENES_IN_ARRAY_OPERATORS
};

e_scene_inarr_cmp_operators_t ezlopi_scenes_inarr_comparator_operators_get_enum(char* operator_inarr)
{
    e_scene_inarr_cmp_operators_t ret = SCENES_IN_ARRAY_OPERATORS_NONE + 1;
    if (operator_inarr)
    {
        while ((ret <= SCENES_IN_ARRAY_OPERATORS_MAX) && ezlopi_scenes_inarr_cmp_operators_op[ret])
        {
            if (0 == strcmp(ezlopi_scenes_inarr_cmp_operators_op[ret], operator_inarr))
            {
                break;
            }
            ret++;
        }
    }
    return ret;
}

const char* ezlopi_scenes_inarr_comparator_operators_get_op(e_scene_inarr_cmp_operators_t operator)
{
    const char* ret = NULL;
    if ((operator>= SCENES_IN_ARRAY_OPERATORS_NONE) && (operator<SCENES_IN_ARRAY_OPERATORS_MAX))
    {
        ret = ezlopi_scenes_inarr_cmp_operators_op[operator];
    }
    return ret;
}

const char* ezlopi_scenes_inarr_comparator_operators_get_name(e_scene_inarr_cmp_operators_t operator)
{
    const char* ret = NULL;
    if ((operator>= SCENES_IN_ARRAY_OPERATORS_NONE) && (operator<SCENES_IN_ARRAY_OPERATORS_MAX))
    {
        ret = ezlopi_scenes_inarr_cmp_operators_name[operator];
    }
    return ret;
}

const char* ezlopi_scenes_inarr_comparator_operators_get_method(e_scene_inarr_cmp_operators_t operator)
{
    const char* ret = NULL;
    if ((operator>= SCENES_IN_ARRAY_OPERATORS_NONE) && (operator<SCENES_IN_ARRAY_OPERATORS_MAX))
    {
        ret = ezlopi_scenes_inarr_cmp_operators_method[operator];
    }
    return ret;
}
int ezlopi_scenes_operators_value_inarr_operations(l_fields_v2_t* item_exp_field, l_fields_v2_t* value_field, l_fields_v2_t* operation_field)
{
    int ret = 0;
    if (item_exp_field && value_field && operation_field)
    {
        char* item_exp_value_str = NULL;
        //------------------------------------------------
        if (EZLOPI_VALUE_TYPE_EXPRESSION == item_exp_field->value_type)
        {
            // s_ezlopi_expressions_t* curr_expr_left = ezlopi_scenes_get_expression_node_by_name(item_exp_field->field_value.u_value.value_string);
            #warning "extract the 'string' from expression";
            item_exp_value_str = NULL;
        }
        else
        {
            uint32_t item_id = strtoul(item_exp_field->field_value.u_value.value_string, NULL, 16);
            item_exp_value_str = ezlopi_core_scenes_operator_get_item_string_value_current_by_id(item_id);
        }
        //------------------------------------------------
        if (NULL != item_exp_value_str)
        {
            cJSON* iterator = NULL;
            char* op_str = (NULL == operation_field) ? "in" : operation_field->field_value.u_value.value_string;
            e_scene_inarr_cmp_operators_t inarr_operator = ezlopi_scenes_inarr_comparator_operators_get_enum(op_str);
            switch (inarr_operator)
            {
            case SCENES_IN_ARRAY_OPERATORS_IN:
            {
                cJSON_ArrayForEach(iterator, value_field->field_value.u_value.cj_value)
                {
                    if (cJSON_IsString(iterator))
                    {
                        const char* string_item = cJSON_GetStringValue(iterator);
                        if (NULL != string_item)
                        {
                            if (STR_OP_COMP(item_exp_value_str, == , string_item)) // check for all array elements ; if exists then break
                            {
                                ret = 1;
                                break;
                            }
                        }
                    }
                }
                break;
            }
            case SCENES_IN_ARRAY_OPERATORS_NOT_IN:
            {
                // iterate through the array elements
                cJSON_ArrayForEach(iterator, value_field->field_value.u_value.cj_value)
                {
                    if (cJSON_IsString(iterator))
                    {
                        const char* string_item = cJSON_GetStringValue(iterator);
                        if (NULL != string_item)
                        {
                            if (STR_OP_COMP(item_exp_value_str, == , string_item)) // check for all array elements ; if exists then break
                            {
                                ret = 0;
                                break;
                            }
                            else
                            {
                                ret = 1;
                            }
                        }
                    }
                }
                break;
            }

            default:
            {
                TRACE_E("'SCENES_IN_ARRAY_OPERATORS_* [%d]' out of range!", inarr_operator);
                break;
            }
            }
        }

    }

    return ret;
}

/************* Values with Less ************/

static const char* const ezlopi_scenes_value_with_less_cmp_operators_op[] = {
#define SCENES_VALUES_WITH_LESS_OPERATORS(OPERATOR, op, name, method) op,
#include "__operators_macros/__value_with_less_comparision_operators_macros.h"
#undef SCENES_VALUES_WITH_LESS_OPERATORS
};

static const char* const ezlopi_scenes_value_with_less_cmp_operators_name[] = {
#define SCENES_VALUES_WITH_LESS_OPERATORS(OPERATOR, op, name, method) name,
#include "__operators_macros/__value_with_less_comparision_operators_macros.h"
#undef SCENES_VALUES_WITH_LESS_OPERATORS
};

static const char* const ezlopi_scenes_value_with_less_cmp_operators_method[] = {
#define SCENES_VALUES_WITH_LESS_OPERATORS(OPERATOR, op, name, method) method,
#include "__operators_macros/__value_with_less_comparision_operators_macros.h"
#undef SCENES_VALUES_WITH_LESS_OPERATORS
};

e_scene_value_with_less_cmp_operators_t ezlopi_scenes_value_with_less_comparator_operators_get_enum(char* operator_str)
{
    e_scene_value_with_less_cmp_operators_t ret = SCENES_VALUES_WITH_LESS_OPERATORS_NONE + 1;
    if (operator_str)
    {
        while ((ret <= SCENES_VALUES_WITH_LESS_OPERATORS_MAX) && ezlopi_scenes_value_with_less_cmp_operators_op[ret])
        {
            if (0 == strcmp(ezlopi_scenes_value_with_less_cmp_operators_op[ret], operator_str))
            {
                break;
            }

            ret++;
        }
    }

    return ret;
}

const char* ezlopi_scenes_value_with_less_comparator_operators_get_op(e_scene_value_with_less_cmp_operators_t operator)
{
    const char* ret = NULL;
    if ((operator>= SCENES_VALUES_WITH_LESS_OPERATORS_NONE) && (operator<SCENES_VALUES_WITH_LESS_OPERATORS_MAX))
    {
        ret = ezlopi_scenes_value_with_less_cmp_operators_op[operator];
    }
    return ret;
}

const char* ezlopi_scenes_value_with_less_comparator_operators_get_name(e_scene_value_with_less_cmp_operators_t operator)
{
    const char* ret = NULL;
    if ((operator> SCENES_VALUES_WITH_LESS_OPERATORS_NONE) && (operator<SCENES_VALUES_WITH_LESS_OPERATORS_MAX))
    {
        ret = ezlopi_scenes_value_with_less_cmp_operators_name[operator];
    }
    return ret;
}

const char* ezlopi_scenes_value_with_less_comparator_operators_get_method(e_scene_value_with_less_cmp_operators_t operator)
{
    const char* ret = NULL;
    if ((operator> SCENES_VALUES_WITH_LESS_OPERATORS_NONE) && (operator<SCENES_VALUES_WITH_LESS_OPERATORS_MAX))
    {
        ret = ezlopi_scenes_value_with_less_cmp_operators_method[operator];
    }
    return ret;
}
#if 0
int ezlopi_scenes_operators_value_with_less_operations(uint32_t item_id, l_fields_v2_t* value_field, l_fields_v2_t* comparator_field)
{
    int ret = 0;
    if (item_id && value_field && comparator_field)
    {
        double item_value = 0.0;
        cJSON* cj_item_value = cJSON_CreateObject(__FUNCTION__);
        l_ezlopi_device_t* device = ezlopi_device_get_head();
        while (device)
        {
            l_ezlopi_item_t* item = device->items;
            while (item)
            {
                if (item->cloud_properties.item_id == item_id)
                {
                    cJSON* cj_item_value = cJSON_CreateObject(__FUNCTION__);
                    if (cj_item_value)
                    {
                        item->func(EZLOPI_ACTION_GET_EZLOPI_VALUE, item, (void*)cj_item_value, NULL);
                        cJSON* cj_value = cJSON_GetObjectItem(__FUNCTION__, cj_item_value, ezlopi_value_str);
                        if (cj_value)
                        {
                            #warning "Krishna needs to complete this"
                                item_value = cj_value->valuedouble;
                        }
                        cJSON_Delete(cj_item_value);
                    }
                    break;
                }
                item = item->next;
            }
            device = device->next;
        }

        e_scene_value_with_less_cmp_operators_t value_with_less_operator = ezlopi_scenes_value_with_less_comparator_operators_get_enum(comparator_field->field_value.u_value.value_string);

        switch (value_with_less_operator)
        {
        case SCENES_VALUES_WITH_LESS_OPERATORS_LESS:
        {
            TRACE_W("'SCENES_VALUES_WITH_LESS_OPERATORS_LESS' not implemented!");
            break;
        }
        case SCENES_VALUES_WITH_LESS_OPERATORS_GREATER:
        {
            TRACE_W("'SCENES_VALUES_WITH_LESS_OPERATORS_GREATER' not implemented!");
            break;
        }
        case SCENES_VALUES_WITH_LESS_OPERATORS_LESS_EQUAL:
        {
            TRACE_W("'SCENES_VALUES_WITH_LESS_OPERATORS_LESS_EQUAL' not implemented!");
            break;
        }
        case SCENES_VALUES_WITH_LESS_OPERATORS_GREATER_EQUAL:
        {
            TRACE_W("'SCENES_VALUES_WITH_LESS_OPERATORS_GREATER_EQUAL' not implemented!");
            break;
        }
        case SCENES_VALUES_WITH_LESS_OPERATORS_EQUAL:
        {
            TRACE_W("'SCENES_VALUES_WITH_LESS_OPERATORS_EQUAL' not implemented!");
            break;
        }
        case SCENES_VALUES_WITH_LESS_OPERATORS_NOT_EQUAL:
        {
            TRACE_W("'SCENES_VALUES_WITH_LESS_OPERATORS_NOT_EQUAL' not implemented!");
            break;
        }
        default:
        {
            TRACE_E("'SCENES_VALUES_WITH_LESS_OPERATORS_* [%d]' out of range!", value_with_less_operator);
            break;
        }
        }

        if (cj_item_value)
        {
            cJSON_Delete(__FUNCTION__, cj_item_value);
        }
    }

    return ret;
}
#endif
/************* Values without less ************/

static const char* const ezlopi_scenes_value_without_less_cmp_operators_op[] = {
#define SCENES_VALUES_WITHOUT_LESS_OPERATORS(OPERATOR, op, name, method) op,
#include "__operators_macros/__value_without_less_comparision_operators_macros.h"
#undef SCENES_VALUES_WITHOUT_LESS_OPERATORS
};

static const char* const ezlopi_scenes_value_without_less_cmp_operators_name[] = {
#define SCENES_VALUES_WITHOUT_LESS_OPERATORS(OPERATOR, op, name, method) name,
#include "__operators_macros/__value_without_less_comparision_operators_macros.h"
#undef SCENES_VALUES_WITHOUT_LESS_OPERATORS
};

static const char* const ezlopi_scenes_value_without_less_cmp_operators_method[] = {
#define SCENES_VALUES_WITHOUT_LESS_OPERATORS(OPERATOR, op, name, method) method,
#include "__operators_macros/__value_without_less_comparision_operators_macros.h"
#undef SCENES_VALUES_WITHOUT_LESS_OPERATORS
};

e_scene_value_without_less_cmp_operators_t ezlopi_scenes_value_without_less_comparator_operators_get_enum(char* operator_str)
{
    e_scene_value_without_less_cmp_operators_t ret = SCENES_VALUES_WITHOUT_LESS_OPERATORS_NONE + 1; // 1st element of enum after *_NONE
    if (operator_str)
    {
        while ((ret <= SCENES_VALUES_WITHOUT_LESS_OPERATORS_MAX) && ezlopi_scenes_value_without_less_cmp_operators_op[ret])
        {
            if (0 == strcmp(ezlopi_scenes_value_without_less_cmp_operators_op[ret], operator_str))
            {
                break;
            }

            ret++;
        }
    }

    return ret;
}

const char* ezlopi_scenes_value_without_less_comparator_operators_get_op(e_scene_value_without_less_cmp_operators_t operator)
{
    const char* ret = NULL;
    if ((operator>= SCENES_VALUES_WITHOUT_LESS_OPERATORS_NONE) && (operator<SCENES_VALUES_WITHOUT_LESS_OPERATORS_MAX))
    {
        ret = ezlopi_scenes_value_without_less_cmp_operators_op[operator];
    }
    return ret;
}

const char* ezlopi_scenes_value_without_less_comparator_operators_get_name(e_scene_value_without_less_cmp_operators_t operator)
{
    const char* ret = NULL;
    if ((operator> SCENES_VALUES_WITHOUT_LESS_OPERATORS_NONE) && (operator<SCENES_VALUES_WITHOUT_LESS_OPERATORS_MAX))
    {
        ret = ezlopi_scenes_value_without_less_cmp_operators_name[operator];
    }
    return ret;
}

const char* ezlopi_scenes_value_without_less_comparator_operators_get_method(e_scene_value_without_less_cmp_operators_t operator)
{
    const char* ret = NULL;
    if ((operator> SCENES_VALUES_WITHOUT_LESS_OPERATORS_NONE) && (operator<SCENES_VALUES_WITHOUT_LESS_OPERATORS_MAX))
    {
        ret = ezlopi_scenes_value_without_less_cmp_operators_method[operator];
    }
    return ret;
}
#if 0
int ezlopi_scenes_operators_value_without_less_operations(uint32_t item_id, l_fields_v2_t* value_field, l_fields_v2_t* comparator_field)
{
    int ret = 0;
    if (item_id && value_field && comparator_field)
    {
        double item_value = 0.0;
        cJSON* cj_item_value = cJSON_CreateObject(__FUNCTION__);
        l_ezlopi_device_t* device = ezlopi_device_get_head();
        while (device)
        {
            l_ezlopi_item_t* item = device->items;
            while (item)
            {
                if (item->cloud_properties.item_id == item_id)
                {
                    cJSON* cj_item_value = cJSON_CreateObject(__FUNCTION__);
                    if (cj_item_value)
                    {
                        item->func(EZLOPI_ACTION_GET_EZLOPI_VALUE, item, (void*)cj_item_value, NULL);
                        cJSON* cj_value = cJSON_GetObjectItem(__FUNCTION__, cj_item_value, ezlopi_value_str);
                        if (cj_value)
                        {
                            #warning "Krishna needs to complete this"
                                item_value = cj_value->valuedouble;
                        }
                        cJSON_Delete(cj_item_value);
                    }
                    break;
                }

                item = item->next;
            }

            device = device->next;
        }

        e_scene_value_without_less_cmp_operators_t value_without_less_operator = ezlopi_scenes_value_without_less_comparator_operators_get_enum(comparator_field->field_value.u_value.value_string);

        switch (value_without_less_operator)
        {
        case SCENES_VALUES_WITHOUT_LESS_OPERATORS_EQUAL:
        {
            TRACE_W("'SCENES_VALUES_WITHOUT_LESS_OPERATORS_EQUAL' not implemented!");
            break;
        }
        case SCENES_VALUES_WITHOUT_LESS_OPERATORS_NOT_EQUAL:
        {
            TRACE_W("'SCENES_VALUES_WITHOUT_LESS_OPERATORS_NOT_EQUAL' not implemented!");
            break;
        }
        default:
        {
            TRACE_E("'SCENES_VALUES_WITHOUT_LESS_OPERATORS_* [%d]' out of range!", value_without_less_operator);
            break;
        }
        }

        if (cj_item_value)
        {
            cJSON_Delete(__FUNCTION__, cj_item_value);
        }
    }

    return ret;
}
#endif
/************* compareValues ************/
int ezlopi_scenes_operators_value_comparevalues_without_less_operations(uint32_t item_id, l_fields_v2_t* value_field, l_fields_v2_t* value_type_field, l_fields_v2_t* comparator_field)
{
    int ret = 0;
    if (item_id && value_field && value_type_field && comparator_field)
    {
        cJSON* item_value = NULL;

        l_ezlopi_device_t* device = ezlopi_device_get_head();
        while (device)
        {
            l_ezlopi_item_t* item = device->items;
            while (item)
            {
                if (item->cloud_properties.item_id == item_id)
                {
                    if (STR_OP_COMP(value_type_field->field_value.u_value.value_string, == , item->cloud_properties.value_type)) // bool == bool?
                    {
                        cJSON* cj_item_value = cJSON_CreateObject(__FUNCTION__);
                        if (cj_item_value)
                        {
                            item->func(EZLOPI_ACTION_GET_EZLOPI_VALUE, item, (void*)cj_item_value, NULL);
                            item_value = cJSON_GetObjectItem(__FUNCTION__, cj_item_value, ezlopi_value_str); // eg. double_type : 5.005  or string_type : "5.005"
                            cJSON_Delete(__FUNCTION__, cj_item_value);
                        }
                    }
                    else
                    {
                        ret = 0; // SCENES_WHEN_TYPE_MISMATCH error
                    }
                    break;
                }
                item = item->next;
            }
            device = device->next;
        }

        if (NULL != item_value)
        { // operating according to 'with-less or without-less' comparator
            char* op_str = (NULL == comparator_field) ? "==" : comparator_field->field_value.u_value.value_string;
            e_scene_value_without_less_cmp_operators_t value_without_less_operator = ezlopi_scenes_value_without_less_comparator_operators_get_enum(op_str);

            switch (value_without_less_operator)
            {
            case SCENES_VALUES_WITHOUT_LESS_OPERATORS_EQUAL:
            {

                ret = ((item_value->type == cJSON_True) ? (true == value_field->field_value.u_value.value_bool)
                    : (item_value->type == cJSON_False) ? (false == value_field->field_value.u_value.value_bool)
                    : (item_value->type == cJSON_Number) ? (item_value->valuedouble == value_field->field_value.u_value.value_double)
                    : (item_value->type == cJSON_String) ? STR_OP_COMP(item_value->valuestring, == , value_field->field_value.u_value.value_string)
                    : 0);

                break;
            }
            case SCENES_VALUES_WITHOUT_LESS_OPERATORS_NOT_EQUAL:
            {

                ret = ((item_value->type == cJSON_True) ? (true != value_field->field_value.u_value.value_bool)
                    : (item_value->type == cJSON_False) ? (false != value_field->field_value.u_value.value_bool)
                    : (item_value->type == cJSON_Number) ? (item_value->valuedouble != value_field->field_value.u_value.value_double)
                    : (item_value->type == cJSON_String) ? STR_OP_COMP(item_value->valuestring, != , value_field->field_value.u_value.value_string)
                    : 0);

                break;
            }

            default:
            {
                TRACE_E("'SCENES_VALUES_WITHOUT_LESS_OPERATORS_* [%d]' out of range!", value_without_less_operator);
                break;
            }
            }
        }


    }

    return ret;
}


int ezlopi_scenes_operators_value_comparevalues_with_less_operations(l_fields_v2_t* item_exp_field, l_fields_v2_t* value_field, l_fields_v2_t* value_type_field, l_fields_v2_t* comparator_field)
{
    int ret = 0;
    if (item_exp_field && value_field && value_type_field && comparator_field)
    {
        cJSON* item_exp_value = NULL;

        //---------------------- LHS -------------------------
        if (EZLOPI_VALUE_TYPE_EXPRESSION == item_exp_field->value_type)
        {
            s_ezlopi_expressions_t* curr_expr_left = ezlopi_scenes_get_expression_node_by_name(item_exp_field->field_value.u_value.value_string);

            cJSON* cj_item_value = cJSON_CreateObject(__FUNCTION__);
            if (cj_item_value)
            {
                if (0 == strncmp(value_type_field->field_value.u_value.value_string, "bool", 5))
                {
                    item_exp_value = cJSON_AddBoolToObject(__FUNCTION__, cj_item_value, ezlopi_value_str, curr_expr_left->exp_value.u_value.boolean_value);
                }
                else if (0 == strncmp(value_type_field->field_value.u_value.value_string, "number", 7))
                {
                    item_exp_value = cJSON_AddNumberToObject(__FUNCTION__, cj_item_value, ezlopi_value_str, curr_expr_left->exp_value.u_value.number_value);
                }
                else if (0 == strncmp(value_type_field->field_value.u_value.value_string, "string", 7))
                {
                    item_exp_value = cJSON_AddStringToObject(__FUNCTION__, cj_item_value, ezlopi_value_str, curr_expr_left->exp_value.u_value.str_value);
                }
                else
                {
                    ret = 0; // SCENES_WHEN_TYPE_MISMATCH error
                }
                cJSON_Delete(__FUNCTION__, cj_item_value);
            }

        }
        else
        {
            uint32_t item_id = 0;
            item_id = strtoul(item_exp_field->field_value.u_value.value_string, NULL, 16);

            l_ezlopi_device_t* device = ezlopi_device_get_head();
            while (device)
            {
                l_ezlopi_item_t* item = device->items;
                while (item)
                {
                    if (item->cloud_properties.item_id == item_id) // unique
                    {
                        if (STR_OP_COMP(value_type_field->field_value.u_value.value_string, == , item->cloud_properties.value_type)) // bool == bool?
                        {
                            cJSON* cj_item_value = cJSON_CreateObject(__FUNCTION__);
                            if (cj_item_value)
                            {
                                item->func(EZLOPI_ACTION_GET_EZLOPI_VALUE, item, (void*)cj_item_value, NULL);
                                item_exp_value = cJSON_GetObjectItem(__FUNCTION__, cj_item_value, ezlopi_value_str); // "5.0"
                                cJSON_Delete(__FUNCTION__, cj_item_value);
                            }
                        }
                        else
                        {
                            ret = 0; // SCENES_WHEN_TYPE_MISMATCH error
                        }
                        break;
                    }
                    item = item->next;
                }
                device = device->next;
            }
        }
        //----------------------------------------------------

        if (NULL != item_exp_value)
        { // operating according to 'with-less or without-less' comparator

            char* op_str = (NULL == comparator_field) ? "==" : comparator_field->field_value.u_value.value_string;
            e_scene_value_with_less_cmp_operators_t value_with_less_operator = ezlopi_scenes_value_with_less_comparator_operators_get_enum(op_str);

            switch (value_with_less_operator)
            {

            case SCENES_VALUES_WITH_LESS_OPERATORS_LESS:
            {
                ret = ((item_exp_value->type == cJSON_Number) ? (item_exp_value->valuedouble < value_field->field_value.u_value.value_double)
                    : (item_exp_value->type == cJSON_String) ? STR_OP_COMP(item_exp_value->valuestring, < , value_field->field_value.u_value.value_string)
                    : 0);
                if (0 == ret)
                {
                    TRACE_E("Value type mis-matched!");
                }
                break;
            }
            case SCENES_VALUES_WITH_LESS_OPERATORS_GREATER:
            {
                ret = ((item_exp_value->type == cJSON_Number) ? (item_exp_value->valuedouble > value_field->field_value.u_value.value_double)
                    : (item_exp_value->type == cJSON_String) ? STR_OP_COMP(item_exp_value->valuestring, > , value_field->field_value.u_value.value_string)
                    : 0);
                if (0 == ret)
                {
                    TRACE_E("Value type mis-matched!");
                }

                break;
            }
            case SCENES_VALUES_WITH_LESS_OPERATORS_LESS_EQUAL:
            {
                ret = ((item_exp_value->type == cJSON_Number) ? (item_exp_value->valuedouble <= value_field->field_value.u_value.value_double)
                    : (item_exp_value->type == cJSON_String) ? STR_OP_COMP(item_exp_value->valuestring, <= , value_field->field_value.u_value.value_string)
                    : 0);
                if (0 == ret)
                {
                    TRACE_E("Value type mis-matched!");
                }

                break;
            }
            case SCENES_VALUES_WITH_LESS_OPERATORS_GREATER_EQUAL:
            {
                ret = ((item_exp_value->type == cJSON_Number) ? (item_exp_value->valuedouble >= value_field->field_value.u_value.value_double)
                    : (item_exp_value->type == cJSON_String) ? STR_OP_COMP(item_exp_value->valuestring, >= , value_field->field_value.u_value.value_string)
                    : 0);
                if (0 == ret)
                {
                    TRACE_E("Value type mis-matched!");
                }

                break;
            }

            case SCENES_VALUES_WITH_LESS_OPERATORS_EQUAL:
            {
                ret = ((item_exp_value->type == cJSON_True) ? (true == value_field->field_value.u_value.value_bool)
                    : (item_exp_value->type == cJSON_False) ? (false == value_field->field_value.u_value.value_bool)
                    : (item_exp_value->type == cJSON_Number) ? (item_exp_value->valuedouble == value_field->field_value.u_value.value_double)
                    : (item_exp_value->type == cJSON_String) ? STR_OP_COMP(item_exp_value->valuestring, == , value_field->field_value.u_value.value_string)
                    : 0);
                if (0 == ret)
                {
                    TRACE_E("Value type mis-matched!");
                }

                break;
            }

            case SCENES_VALUES_WITH_LESS_OPERATORS_NOT_EQUAL:
            {
                ret = ((item_exp_value->type == cJSON_True) ? (true != value_field->field_value.u_value.value_bool)
                    : (item_exp_value->type == cJSON_False) ? (false != value_field->field_value.u_value.value_bool)
                    : (item_exp_value->type == cJSON_Number) ? (item_exp_value->valuedouble != value_field->field_value.u_value.value_double)
                    : (item_exp_value->type == cJSON_String) ? STR_OP_COMP(item_exp_value->valuestring, != , value_field->field_value.u_value.value_string)
                    : 0);
                if (0 == ret)
                {
                    TRACE_E("Value type mis-matched!");
                }

                break;
            }
            default:
            {
                TRACE_E("'SCENES_VALUES_WITH_LESS_OPERATORS_* [%d]' out of range!", value_with_less_operator);
                break;
            }
            }
        }

    }

    return ret;
}


/************* NUMERIC RANGE *************/
int ezlopi_scenes_operators_value_number_range_operations(uint32_t item_id, l_fields_v2_t* start_value_field, l_fields_v2_t* end_value_field, l_fields_v2_t* comparator_field)
{
    int ret = 0;
    if (item_id && start_value_field && end_value_field)
    {
        cJSON* item_value = NULL;
        l_ezlopi_device_t* device = ezlopi_device_get_head();
        while (device)
        {
            l_ezlopi_item_t* item = device->items;
            while (item)
            {
                if (item->cloud_properties.item_id == item_id) // find the correct " item " within the device
                {
                    cJSON* cj_item_value = cJSON_CreateObject(__FUNCTION__);
                    if (cj_item_value)
                    {
                        item->func(EZLOPI_ACTION_GET_EZLOPI_VALUE, item, (void*)cj_item_value, NULL);
                        cJSON* item_valuetype = cJSON_GetObjectItem(__FUNCTION__, cj_item_value, ezlopi_valueType_str);
                        const char* str_item_type = NULL;
                        if (item_valuetype && cJSON_IsString(item_valuetype) && (NULL != (str_item_type = cJSON_GetStringValue(item_valuetype))))
                        {
                            const char* tmp_valuetype = "";
                            if ((start_value_field->value_type > EZLOPI_VALUE_TYPE_NONE) && (start_value_field->value_type < EZLOPI_VALUE_TYPE_MAX))
                            {
                                tmp_valuetype = ezlopi_scenes_value_numeric_range_value_types[start_value_field->value_type]; // bool ? token ? int  ?
                            }
                            if (STR_OP_COMP(tmp_valuetype, == , str_item_type)) // 'int' == 'int'?
                            {
                                // now check if scale matches
                                cJSON* item_scale = cJSON_GetObjectItem(__FUNCTION__, cj_item_value, ezlopi_scale_str);
                                const char* str_scale_tmp = NULL;
                                if (item_scale && cJSON_IsString(item_scale) && (NULL != (str_scale_tmp = cJSON_GetStringValue(item_scale))))
                                {
                                    if (STR_OP_COMP(start_value_field->scale, == , str_scale_tmp)) // 'NULL' == 'NULL'
                                    {
                                        cJSON* cj_value = cJSON_GetObjectItem(__FUNCTION__, cj_item_value, ezlopi_value_str); // extract the value from " item " within the device
                                        if (cj_value)
                                        {                          // extract the item_value ;
                                            item_value = cj_value; // here the value maybe (int , float , string , bool)
                                        }
                                    }
                                    else
                                    {
                                        TRACE_E("Scale didnot match..");
                                    }
                                }
                            }
                        }
                        cJSON_Delete(__FUNCTION__, cj_item_value);
                    }
                    break;
                }
                item = item->next;
            }
            device = device->next;
        }

        if (NULL != item_value)
        {
            char* op_str = (NULL == comparator_field) ? "between" : comparator_field->field_value.u_value.value_string;

            switch (ezlopi_scenes_numeric_comparator_operators_get_enum(op_str))
            {

            case SCENES_NUM_COMP_OPERATORS_BETWEEN:
            {
                if (item_value->type == cJSON_Number)
                {
                    ret = ((item_value->valuedouble >= start_value_field->field_value.u_value.value_double) &&
                        (item_value->valuedouble <= end_value_field->field_value.u_value.value_double))
                        ? 1
                        : 0;
                }
                else if (item_value->type == cJSON_String)
                {
                    ret = ((STR_OP_COMP(item_value->valuestring, >= , start_value_field->field_value.u_value.value_string)) &&
                        (STR_OP_COMP(item_value->valuestring, <= , end_value_field->field_value.u_value.value_string)))
                        ? 1
                        : 0;
                }
                else
                {
                    TRACE_E("Value type mis-matched!");
                }
                break;
            }
            case SCENES_NUM_COMP_OPERATORS_NOT_BETWEEN:
            {
                if (item_value->type == cJSON_Number)
                {
                    ret = ((item_value->valuedouble < start_value_field->field_value.u_value.value_double) &&
                        (item_value->valuedouble > end_value_field->field_value.u_value.value_double))
                        ? 1
                        : 0;
                }
                else if (item_value->type == cJSON_String)
                {
                    ret = ((STR_OP_COMP(item_value->valuestring, < , start_value_field->field_value.u_value.value_string)) &&
                        (STR_OP_COMP(item_value->valuestring, > , end_value_field->field_value.u_value.value_string)))
                        ? 1
                        : 0;
                }
                else
                {
                    TRACE_E("Value type mis-matched!");
                }
                break;
            }

            default:
            {
                break;
            }
            }
        }

    }

    return ret;
}


/************* Has atleast one dictornary Value *************/
int ezlopi_scenes_operators_has_atleastone_dictionary_value_operations(uint32_t item_id, l_fields_v2_t* value_field)
{
    int ret = 0;
    if (item_id && value_field)
    {
        cJSON* item_value = NULL;
        l_ezlopi_device_t* device = ezlopi_device_get_head();
        while (device)
        {
            l_ezlopi_item_t* item = device->items;
            while (item)
            {
                if (item->cloud_properties.item_id == item_id)
                {
                    cJSON* cj_item_value = cJSON_CreateObject(__FUNCTION__);
                    if (cj_item_value)
                    {
                        item->func(EZLOPI_ACTION_GET_EZLOPI_VALUE, item, (void*)cj_item_value, NULL);
                        cJSON* cj_valuetype = cJSON_GetObjectItem(__FUNCTION__, cj_item_value, ezlopi_valueType_str); // first check the item_type -> 'valueType'
                        const char* str_item_type = NULL;
                        if (cj_valuetype && cJSON_IsString(cj_valuetype) && (NULL != (str_item_type = cJSON_GetStringValue(cj_valuetype)))) // type => dictionary
                        {
                            // " ezlopi_cloud_value_type_str.c "

                            // const char *str_item_type = "";
                            // if ((value_field->value_type > EZLOPI_VALUE_TYPE_NONE) && (value_field->value_type < EZLOPI_VALUE_TYPE_MAX))
                            // {
                            //     str_item_type = ezlopi_scenes_value_numeric_range_value_types[value_field->value_type]; // must return "dictionary"
                            // }

                            if (STR_OP_COMP("dictionary", == , str_item_type)) // 'dictionary' == 'dictionary'?
                            {
                                cJSON* cj_value = cJSON_GetObjectItem(__FUNCTION__, cj_item_value, ezlopi_value_str); // item_value -> dictionary ; [array or object]
                                if (cj_value)
                                {
                                    item_value = cj_value;
                                }
                            }
                        }
                        cJSON_Delete(__FUNCTION__, cj_item_value);
                    }
                    break;
                }
                item = item->next;
            }
            device = device->next;
        }

        if (NULL != item_value)
        {
            if (cJSON_IsObject(item_value))
            {
                // Check if ["value":"low_battery"] key is present
                cJSON* dictionaryValue = cJSON_GetObjectItem(__FUNCTION__, item_value, value_field->field_value.u_value.value_string);
                if (NULL != dictionaryValue) // if the "eg. low_battery" element exists within the dictionary
                {
                    ret = 1;
                }
            }
        }

    }

    return ret;
}


/************* isDictornary Changed *************/
int ezlopi_scenes_operators_is_dictionary_changed_operations(l_scenes_list_v2_t* scene_node, uint32_t item_id, l_fields_v2_t* key_field, l_fields_v2_t* operation_field)
{
    int ret = 0;
    if (item_id && key_field && operation_field)
    {
        cJSON* item_value = NULL;

        l_ezlopi_device_t* device = ezlopi_device_get_head();
        while (device)
        {
            l_ezlopi_item_t* item = device->items;
            while (item)
            {
                if (item->cloud_properties.item_id == item_id)
                {
                    cJSON* cj_item_value = cJSON_CreateObject(__FUNCTION__);
                    if (cj_item_value)
                    {
                        item->func(EZLOPI_ACTION_GET_EZLOPI_VALUE, item, (void*)cj_item_value, NULL);
                        cJSON* cj_valuetype = cJSON_GetObjectItem(__FUNCTION__, cj_item_value, ezlopi_valueType_str); // first check the item_type -> 'valueType'
                        const char* str_item_type = NULL;
                        if (cj_valuetype && cJSON_IsString(cj_valuetype) && (NULL != (str_item_type = cJSON_GetStringValue(cj_valuetype)))) // type => dictionary
                        {
                            if (STR_OP_COMP("dictionary", == , str_item_type)) // 'dictionary' == 'dictionary'?
                            {
                                cJSON* cj_value = cJSON_GetObjectItem(__FUNCTION__, cj_item_value, ezlopi_value_str); // item_value -> dictionary ; [array or object]
                                if (cj_value)
                                {
                                    item_value = cj_value;
                                }
                            }
                        }
                        cJSON_Delete(__FUNCTION__, cj_item_value);
                    }

                    break;
                }
                item = item->next;
            }
            device = device->next;
        }

        if (NULL != item_value)
        {
            if (cJSON_IsObject(item_value)) // c
            {
                if (0 == strncmp("added", operation_field->field_value.u_value.value_string, 6))
                {
                    // Check if ["key":"key_1"] key is present
                    cJSON* dictionaryValue = cJSON_GetObjectItem(__FUNCTION__, item_value, key_field->field_value.u_value.value_string);
                    if (NULL != dictionaryValue) // if the "eg. key_1" element exists within the dictionary
                    {
                        ret = 1;
                    }
                }
                else if (0 == strncmp("removed", operation_field->field_value.u_value.value_string, 8))
                {
                    // Check if ["key":"key_1"] key is absent
                    cJSON* dictionaryValue = cJSON_GetObjectItem(__FUNCTION__, item_value, key_field->field_value.u_value.value_string);
                    if (NULL == dictionaryValue) // if the "eg. key_1" element exists within the dictionary
                    {
                        ret = 1;
                    }
                }
                else if (0 == strncmp("updated", operation_field->field_value.u_value.value_string, 8))
                {
                    // Check if ["key":"key_1"] key is present and its elemnt
                    cJSON* dictionaryValue = cJSON_GetObjectItem(__FUNCTION__, item_value, key_field->field_value.u_value.value_string);
                    if (NULL != dictionaryValue) // if the "eg. key_1" element exists within the dictionary
                    {
                        cJSON* last_dictionary_item = (cJSON*)scene_node->when_block->fields->user_arg;
                        ret = cJSON_Compare(last_dictionary_item, item_value, 0);
                    }
                }

                // if the item exists ; Store current item_value into 'scene_node->when_block->fields->user_arg'
                scene_node->when_block->fields->user_arg = (void*)item_value;
            }
        }

    }
    return ret;
}
#endif  // CONFIG_EZPI_SERV_ENABLE_MESHBOTS