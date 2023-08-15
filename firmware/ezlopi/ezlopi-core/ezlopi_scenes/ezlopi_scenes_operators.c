#include <string.h>
#include "trace.h"
#include "cJSON.h"
#include "ezlopi_scenes.h"
#include "ezlopi_devices.h"
#include "ezlopi_scenes_operators.h"

static const char *const ezlopi_scenes_operators_op[] = {
#define SCENES_OPERATORS(OPERATOR, op, name, method) op,
#include "ezlopi_scenes_operators_macros.h"
#undef SCENES_OPERATORS
};

static const char *const ezlopi_scenes_operators_name[] = {
#define SCENES_OPERATORS(OPERATOR, op, name, method) name,
#include "ezlopi_scenes_operators.h"
#undef SCENES_OPERATORS
};

static const char *const ezlopi_scenes_operators_method[] = {
#define SCENES_OPERATORS(OPERATOR, op, name, method) method,
#include "ezlopi_scenes_operators.h"
#undef SCENES_OPERATORS
};

e_scene_cmp_operators_t ezlopi_scenes_operators_get_enum(char *operator_str)
{
    e_scene_cmp_operators_t ret = SCENES_OPERATORS_LESS;
    if (operator_str)
    {
        while ((ret <= SCENES_OPERATORS_MAX) && ezlopi_scenes_operators_op[ret])
        {
            if (0 == strcmp(ezlopi_scenes_operators_op[ret], operator_str))
            {
                break;
            }
            ret++;
        }
    }
    return ret;
}

const char *ezlopi_scenes_operators_get_op(e_scene_cmp_operators_t operator)
{
    const char *ret = NULL;
    TRACE_D("operator: %d", operator);
    if ((operator> SCENES_OPERATORS_NONE) && (operator<SCENES_OPERATORS_MAX))
    {
        ret = ezlopi_scenes_operators_op[operator];
    }
    return ret;
}

const char *ezlopi_scenes_operators_get_name(e_scene_cmp_operators_t operator)
{
    const char *ret = NULL;
    TRACE_D("operator: %d", operator);
    if ((operator> SCENES_OPERATORS_NONE) && (operator<SCENES_OPERATORS_MAX))
    {
        ret = ezlopi_scenes_operators_name[operator];
    }
    return ret;
}

const char *ezlopi_scenes_operators_get_method(e_scene_cmp_operators_t operator)
{
    const char *ret = NULL;
    TRACE_D("operator: %d", operator);
    if ((operator> SCENES_OPERATORS_NONE) && (operator<SCENES_OPERATORS_MAX))
    {
        ret = ezlopi_scenes_operators_method[operator];
    }
    return ret;
}

int ezlopi_scenes_operators_compare_value_number(uint32_t item_id, l_fields_t *value_field, l_fields_t *comparator_field)
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
                        break;
                    }
                }
                item = item->next;
            }
            device = device->next;
        }

        switch (ezlopi_scenes_operators_get_enum(comparator_field->value.value_string))
        {
        case SCENES_OPERATORS_LESS:
        {
            ret = (item_value < value_field->value.value_double);
            break;
        }
        case SCENES_OPERATORS_LESS_EQUAL:
        {
            ret = (item_value <= value_field->value.value_double);
            break;
        }
        case SCENES_OPERATORS_GREATER:
        {
            ret = (item_value > value_field->value.value_double);
            break;
        }
        case SCENES_OPERATORS_GREATER_EQUAL:
        {
            ret = (item_value >= value_field->value.value_double);
            break;
        }
        case SCENES_OPERATORS_EQUAL:
        {
            ret = (item_value == value_field->value.value_double);
            break;
        }
        case SCENES_OPERATORS_NOT_EQUAL:
        {
            ret = (item_value != value_field->value.value_double);
            break;
        }
        case SCENES_OPERATORS_BETWEEN:
        {
            TRACE_W("'SCENES_OPERATORS_BETWEEN' not implemented!");
            break;
        }
        case SCENES_OPERATORS_NOT_BETWEEN:
        {
            TRACE_W("'SCENES_OPERATORS_NOT_BETWEEN' not implemented!");
            break;
        }
        case SCENES_OPERATORS_ANY_OF:
        {
            TRACE_W("'SCENES_OPERATORS_ANY_OF' not implemented!");
            break;
        }
        case SCENES_OPERATORS_NONE_OF:
        {
            TRACE_W("'SCENES_OPERATORS_NONE_OF' not implemented!");
            break;
        }
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