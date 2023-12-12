#include <string.h>
#include <cJSON.h>

#include "trace.h"

#include "ezlopi_devices.h"
#include "ezlopi_scenes_v2.h"
#include "ezlopi_scenes_operators.h"

/************* Numeric ************/
static const char *const ezlopi_scenes_num_cmp_operators_op[] = {
#define SCENES_NUM_COMP_OPERATORS(OPERATOR, op, name, method) op,
#include "__operators_macros/__numeric_comparision_operators_macros.h"
#undef SCENES_NUM_COMP_OPERATORS
};

static const char *const ezlopi_scenes_num_cmp_operators_name[] = {
#define SCENES_NUM_COMP_OPERATORS(OPERATOR, op, name, method) name,
#include "__operators_macros/__numeric_comparision_operators_macros.h"
#undef SCENES_NUM_COMP_OPERATORS
};

static const char *const ezlopi_scenes_num_cmp_operators_method[] = {
#define SCENES_NUM_COMP_OPERATORS(OPERATOR, op, name, method) method,
#include "__operators_macros/__numeric_comparision_operators_macros.h"
#undef SCENES_NUM_COMP_OPERATORS
};

e_scene_num_cmp_operators_t ezlopi_scenes_numeric_comparator_operators_get_enum(char *operator_str)
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

const char *ezlopi_scenes_numeric_comparator_operators_get_op(e_scene_num_cmp_operators_t operator)
{
    const char *ret = NULL;
    if ((operator> SCENES_NUM_COMP_OPERATORS_NONE) && (operator<SCENES_NUM_COMP_OPERATORS_MAX))
    {
        ret = ezlopi_scenes_num_cmp_operators_op[operator];
    }
    return ret;
}

const char *ezlopi_scenes_numeric_comparator_operators_get_name(e_scene_num_cmp_operators_t operator)
{
    const char *ret = NULL;
    if ((operator> SCENES_NUM_COMP_OPERATORS_NONE) && (operator<SCENES_NUM_COMP_OPERATORS_MAX))
    {
        // TRACE_D("Name: %s", ezlopi_scenes_num_cmp_operators_name[operator]);
        ret = ezlopi_scenes_num_cmp_operators_name[operator];
    }
    return ret;
}

const char *ezlopi_scenes_numeric_comparator_operators_get_method(e_scene_num_cmp_operators_t operator)
{
    const char *ret = NULL;
    if ((operator> SCENES_NUM_COMP_OPERATORS_NONE) && (operator<SCENES_NUM_COMP_OPERATORS_MAX))
    {
        // TRACE_D("Method: %s", ezlopi_scenes_operators_method[operator]);
        ret = ezlopi_scenes_num_cmp_operators_method[operator];
    }
    return ret;
}

int ezlopi_scenes_operators_value_number_operations(uint32_t item_id, l_fields_v2_t *value_field, l_fields_v2_t *comparator_field)
{
    int ret = 0;
    if (item_id && value_field && comparator_field)
    {
        double item_value = 0.0;
        cJSON *cj_item_value = cJSON_CreateObject();
        l_ezlopi_device_t *device = ezlopi_device_get_head();
        while (device)
        {
            l_ezlopi_item_t *item = device->items;
            while (item)
            {
                if (item->cloud_properties.item_id == item_id)
                {
                    if (cj_item_value)
                    {
                        item->func(EZLOPI_ACTION_GET_EZLOPI_VALUE, item, (void *)cj_item_value, NULL);
                        cJSON *cj_value = cJSON_GetObjectItem(cj_item_value, "value");
                        if (cj_value)
                        {
                            item_value = cj_value->valuedouble;
                        }
                    }

                    break;
                }
                item = item->next;
            }
            device = device->next;
        }

        switch (ezlopi_scenes_numeric_comparator_operators_get_enum(comparator_field->value.value_string))
        {
        case SCENES_NUM_COMP_OPERATORS_LESS:
        {
            ret = (item_value < value_field->value.value_double);
            break;
        }
        case SCENES_NUM_COMP_OPERATORS_LESS_EQUAL:
        {
            ret = (item_value <= value_field->value.value_double);
            break;
        }
        case SCENES_NUM_COMP_OPERATORS_GREATER:
        {
            ret = (item_value > value_field->value.value_double);
            break;
        }
        case SCENES_NUM_COMP_OPERATORS_GREATER_EQUAL:
        {
            ret = (item_value >= value_field->value.value_double);
            break;
        }
        case SCENES_NUM_COMP_OPERATORS_EQUAL:
        {
            ret = (item_value == value_field->value.value_double);
            break;
        }
        case SCENES_NUM_COMP_OPERATORS_NOT_EQUAL:
        {
            ret = (item_value != value_field->value.value_double);
            break;
        }
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
#if 0
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

        if (cj_item_value)
        {
            cJSON_Delete(cj_item_value);
        }
    }

    return ret;
}

/************* Strings ************/
static const char *const ezlopi_scenes_str_cmp_operators_op[] = {
#define SCENES_STRINGS_OPERATORS(OPERATOR, op, name, method) op,
#include "__operators_macros/__strings_comparision_operators_macros.h"
#undef SCENES_STRINGS_OPERATORS
};

static const char *const ezlopi_scenes_str_cmp_operators_name[] = {
#define SCENES_STRINGS_OPERATORS(OPERATOR, op, name, method) name,
#include "__operators_macros/__strings_comparision_operators_macros.h"
#undef SCENES_STRINGS_OPERATORS
};

static const char *const ezlopi_scenes_str_cmp_operators_method[] = {
#define SCENES_STRINGS_OPERATORS(OPERATOR, op, name, method) method,
#include "__operators_macros/__strings_comparision_operators_macros.h"
#undef SCENES_STRINGS_OPERATORS
};

e_scene_str_cmp_operators_t ezlopi_scenes_strings_comparator_operators_get_enum(char *operator_str)
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

const char *ezlopi_scenes_strings_comparator_operators_get_op(e_scene_str_cmp_operators_t operator)
{
    const char *ret = NULL;
    if ((operator>= SCENES_STRINGS_OPERATORS_NONE) && (operator<SCENES_STRINGS_OPERATORS_MAX))
    {
        ret = ezlopi_scenes_str_cmp_operators_op[operator];
    }
    return ret;
}

const char *ezlopi_scenes_strings_comparator_operators_get_name(e_scene_str_cmp_operators_t operator)
{
    const char *ret = NULL;
    if ((operator> SCENES_STRINGS_OPERATORS_NONE) && (operator<SCENES_STRINGS_OPERATORS_MAX))
    {
        ret = ezlopi_scenes_str_cmp_operators_name[operator];
    }
    return ret;
}

const char *ezlopi_scenes_strings_comparator_operators_get_method(e_scene_str_cmp_operators_t operator)
{
    const char *ret = NULL;
    if ((operator> SCENES_STRINGS_OPERATORS_NONE) && (operator<SCENES_STRINGS_OPERATORS_MAX))
    {
        ret = ezlopi_scenes_str_cmp_operators_method[operator];
    }
    return ret;
}

int ezlopi_scenes_operators_value_strings_operations(uint32_t item_id, l_fields_v2_t *value_field, l_fields_v2_t *comparator_field)
{
    int ret = 0;
    if (item_id && value_field && comparator_field)
    {
        double item_value = 0.0;
        cJSON *cj_item_value = cJSON_CreateObject();
        l_ezlopi_device_t *device = ezlopi_device_get_head();
        while (device)
        {
            l_ezlopi_item_t *item = device->items;
            while (item)
            {
                if (item->cloud_properties.item_id == item_id)
                {
                    if (cj_item_value)
                    {
                        item->func(EZLOPI_ACTION_GET_EZLOPI_VALUE, item, (void *)cj_item_value, NULL);
                        cJSON *cj_value = cJSON_GetObjectItem(cj_item_value, "value");
                        if (cj_value)
                        {
                            item_value = cj_value->valuedouble;
                        }
                    }

                    break;
                }
                item = item->next;
            }
            device = device->next;
        }

        e_scene_str_cmp_operators_t string_operator = ezlopi_scenes_strings_comparator_operators_get_enum(comparator_field->value.value_string);

        switch (string_operator)
        {
        case SCENES_STRINGS_OPERATORS_LESS:
        {
            TRACE_W("'SCENES_STRINGS_OPERATORS_LESS' not implemented!");
            break;
        }
        case SCENES_STRINGS_OPERATORS_GREATER:
        {
            TRACE_W("'SCENES_STRINGS_OPERATORS_GREATER' not implemented!");
            break;
        }
        case SCENES_STRINGS_OPERATORS_LESS_EQUAL:
        {
            TRACE_W("'SCENES_STRINGS_OPERATORS_LESS_EQUAL' not implemented!");
            break;
        }
        case SCENES_STRINGS_OPERATORS_GREATER_EQUAL:
        {
            TRACE_W("'SCENES_STRINGS_OPERATORS_GREATER_EQUAL' not implemented!");
            break;
        }
        case SCENES_STRINGS_OPERATORS_EQUAL:
        {
            TRACE_W("'SCENES_STRINGS_OPERATORS_EQUAL' not implemented!");
            break;
        }
        case SCENES_STRINGS_OPERATORS_NOT_EQUAL:
        {
            TRACE_W("'SCENES_STRINGS_OPERATORS_NOT_EQUAL' not implemented!");
            break;
        }
        case SCENES_STRINGS_OPERATORS_BEGINS_WITH:
        {
            TRACE_W("'SCENES_STRINGS_OPERATORS_BEGINS_WITH' not implemented!");
            break;
        }
        case SCENES_STRINGS_OPERATORS_ENDS_WITH:
        {
            TRACE_W("'SCENES_STRINGS_OPERATORS_ENDS_WITH' not implemented!");
            break;
        }
        case SCENES_STRINGS_OPERATORS_CONTAINS:
        {
            TRACE_W("'SCENES_STRINGS_OPERATORS_CONTAINS' not implemented!");
            break;
        }
        case SCENES_STRINGS_OPERATORS_LENGTH:
        {
            TRACE_W("'SCENES_STRINGS_OPERATORS_LENGTH' not implemented!");
            break;
        }
        case SCENES_STRINGS_OPERATORS_NOT_BEGIN:
        {
            TRACE_W("'SCENES_STRINGS_OPERATORS_NOT_BEGIN' not implemented!");
            break;
        }
        case SCENES_STRINGS_OPERATORS_NOT_END:
        {
            TRACE_W("'SCENES_STRINGS_OPERATORS_NOT_END' not implemented!");
            break;
        }
        case SCENES_STRINGS_OPERATORS_NOT_CONTAIN:
        {
            TRACE_W("'SCENES_STRINGS_OPERATORS_NOT_CONTAIN' not implemented!");
            break;
        }
        case SCENES_STRINGS_OPERATORS_NOT_LENGTH:
        {
            TRACE_W("'SCENES_STRINGS_OPERATORS_NOT_LENGTH' not implemented!");
            break;
        }
        default:
        {
            TRACE_E("'SCENES_STRINGS_OPERATORS_* [%d]' out of range!", string_operator);
            break;
        }
        }

        if (cj_item_value)
        {
            cJSON_Delete(cj_item_value);
        }
    }

    return ret;
}

/************* Values with Less ************/
static const char *const ezlopi_scenes_value_with_less_cmp_operators_op[] = {
#define SCENES_VALUES_WITH_LESS_OPERATORS(OPERATOR, op, name, method) op,
#include "__operators_macros/__value_with_less_comparision_operators_macros.h"
#undef SCENES_VALUES_WITH_LESS_OPERATORS
};

static const char *const ezlopi_scenes_value_with_less_cmp_operators_name[] = {
#define SCENES_VALUES_WITH_LESS_OPERATORS(OPERATOR, op, name, method) name,
#include "__operators_macros/__value_with_less_comparision_operators_macros.h"
#undef SCENES_VALUES_WITH_LESS_OPERATORS
};

static const char *const ezlopi_scenes_value_with_less_cmp_operators_method[] = {
#define SCENES_VALUES_WITH_LESS_OPERATORS(OPERATOR, op, name, method) method,
#include "__operators_macros/__value_with_less_comparision_operators_macros.h"
#undef SCENES_VALUES_WITH_LESS_OPERATORS
};

e_scene_value_with_less_cmp_operators_t ezlopi_scenes_value_with_less_comparator_operators_get_enum(char *operator_str)
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

const char *ezlopi_scenes_value_with_less_comparator_operators_get_op(e_scene_value_with_less_cmp_operators_t operator)
{
    const char *ret = NULL;
    if ((operator>= SCENES_VALUES_WITH_LESS_OPERATORS_NONE) && (operator<SCENES_VALUES_WITH_LESS_OPERATORS_MAX))
    {
        ret = ezlopi_scenes_value_with_less_cmp_operators_op[operator];
    }
    return ret;
}

const char *ezlopi_scenes_value_with_less_comparator_operators_get_name(e_scene_value_with_less_cmp_operators_t operator)
{
    const char *ret = NULL;
    if ((operator> SCENES_VALUES_WITH_LESS_OPERATORS_NONE) && (operator<SCENES_VALUES_WITH_LESS_OPERATORS_MAX))
    {
        ret = ezlopi_scenes_value_with_less_cmp_operators_name[operator];
    }
    return ret;
}

const char *ezlopi_scenes_value_with_less_comparator_operators_get_method(e_scene_value_with_less_cmp_operators_t operator)
{
    const char *ret = NULL;
    if ((operator> SCENES_VALUES_WITH_LESS_OPERATORS_NONE) && (operator<SCENES_VALUES_WITH_LESS_OPERATORS_MAX))
    {
        ret = ezlopi_scenes_value_with_less_cmp_operators_method[operator];
    }
    return ret;
}

int ezlopi_scenes_operators_value_with_less_operations(uint32_t item_id, l_fields_v2_t *value_field, l_fields_v2_t *comparator_field)
{
    int ret = 0;
    if (item_id && value_field && comparator_field)
    {
        double item_value = 0.0;
        cJSON *cj_item_value = cJSON_CreateObject();
        l_ezlopi_device_t *device = ezlopi_device_get_head();
        while (device)
        {
            l_ezlopi_item_t *item = device->items;
            while (item)
            {
                if (item->cloud_properties.item_id == item_id)
                {
                    if (cj_item_value)
                    {
                        item->func(EZLOPI_ACTION_GET_EZLOPI_VALUE, item, (void *)cj_item_value, NULL);
                        cJSON *cj_value = cJSON_GetObjectItem(cj_item_value, "value");
                        if (cj_value)
                        {
                            item_value = cj_value->valuedouble;
                        }
                    }

                    break;
                }
                item = item->next;
            }
            device = device->next;
        }

        e_scene_value_with_less_cmp_operators_t value_with_less_operator = ezlopi_scenes_value_with_less_comparator_operators_get_enum(comparator_field->value.value_string);

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
            cJSON_Delete(cj_item_value);
        }
    }

    return ret;
}

/************* Values without less ************/
static const char *const ezlopi_scenes_value_without_less_cmp_operators_op[] = {
#define SCENES_VALUES_WITHOUT_LESS_OPERATORS(OPERATOR, op, name, method) op,
#include "__operators_macros/__value_without_less_comparision_operators_macros.h"
#undef SCENES_VALUES_WITHOUT_LESS_OPERATORS
};

static const char *const ezlopi_scenes_value_without_less_cmp_operators_name[] = {
#define SCENES_VALUES_WITHOUT_LESS_OPERATORS(OPERATOR, op, name, method) name,
#include "__operators_macros/__value_without_less_comparision_operators_macros.h"
#undef SCENES_VALUES_WITHOUT_LESS_OPERATORS
};

static const char *const ezlopi_scenes_value_without_less_cmp_operators_method[] = {
#define SCENES_VALUES_WITHOUT_LESS_OPERATORS(OPERATOR, op, name, method) method,
#include "__operators_macros/__value_without_less_comparision_operators_macros.h"
#undef SCENES_VALUES_WITHOUT_LESS_OPERATORS
};

e_scene_value_without_less_cmp_operators_t ezlopi_scenes_value_without_less_comparator_operators_get_enum(char *operator_str)
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

const char *ezlopi_scenes_value_without_less_comparator_operators_get_op(e_scene_value_without_less_cmp_operators_t operator)
{
    const char *ret = NULL;
    if ((operator>= SCENES_VALUES_WITHOUT_LESS_OPERATORS_NONE) && (operator<SCENES_VALUES_WITHOUT_LESS_OPERATORS_MAX))
    {
        ret = ezlopi_scenes_value_without_less_cmp_operators_op[operator];
    }
    return ret;
}

const char *ezlopi_scenes_value_without_less_comparator_operators_get_name(e_scene_value_without_less_cmp_operators_t operator)
{
    const char *ret = NULL;
    if ((operator> SCENES_VALUES_WITHOUT_LESS_OPERATORS_NONE) && (operator<SCENES_VALUES_WITHOUT_LESS_OPERATORS_MAX))
    {
        ret = ezlopi_scenes_value_without_less_cmp_operators_name[operator];
    }
    return ret;
}

const char *ezlopi_scenes_value_without_less_comparator_operators_get_method(e_scene_value_without_less_cmp_operators_t operator)
{
    const char *ret = NULL;
    if ((operator> SCENES_VALUES_WITHOUT_LESS_OPERATORS_NONE) && (operator<SCENES_VALUES_WITHOUT_LESS_OPERATORS_MAX))
    {
        ret = ezlopi_scenes_value_without_less_cmp_operators_method[operator];
    }
    return ret;
}

int ezlopi_scenes_operators_value_without_less_operations(uint32_t item_id, l_fields_v2_t *value_field, l_fields_v2_t *comparator_field)
{
    int ret = 0;
    if (item_id && value_field && comparator_field)
    {
        double item_value = 0.0;
        cJSON *cj_item_value = cJSON_CreateObject();
        l_ezlopi_device_t *device = ezlopi_device_get_head();
        while (device)
        {
            l_ezlopi_item_t *item = device->items;
            while (item)
            {
                if (item->cloud_properties.item_id == item_id)
                {
                    if (cj_item_value)
                    {
                        item->func(EZLOPI_ACTION_GET_EZLOPI_VALUE, item, (void *)cj_item_value, NULL);
                        cJSON *cj_value = cJSON_GetObjectItem(cj_item_value, "value");
                        if (cj_value)
                        {
                            item_value = cj_value->valuedouble;
                        }
                    }

                    break;
                }

                item = item->next;
            }

            device = device->next;
        }

        e_scene_value_without_less_cmp_operators_t value_without_less_operator = ezlopi_scenes_value_without_less_comparator_operators_get_enum(comparator_field->value.value_string);

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
            cJSON_Delete(cj_item_value);
        }
    }

    return ret;
}
