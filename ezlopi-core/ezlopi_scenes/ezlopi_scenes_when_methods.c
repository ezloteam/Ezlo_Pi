#include <string.h>
#include <time.h>

#include "trace.h"
#include "ezlopi_devices.h"
#include "ezlopi_scenes_v2.h"
#include "ezlopi_scenes_operators.h"
#include "ezlopi_scenes_when_methods.h"

#define TIME_FLAG (1 << 0)
#define WEEKDAYS_FLAG (1 << 1)
#define DAYS_FLAG (1 << 2)
#define WEEKS_FLAG (1 << 3)

#define MASK_FOR_TIME_ARG (1 << 4)
#define MASK_FOR_WEEKDAYS_ARG (1 << 5)
#define MASK_FOR_DAYS_ARG (1 << 6)
#define MASK_FOR_WEEKS_ARG (1 << 7)

int ezlopi_scene_when_is_item_state(l_scenes_list_v2_t *scene_node, void *arg)
{
    int ret = 0;
    l_when_block_v2_t *when_block = (l_when_block_v2_t *)arg;
    if (when_block)
    {
        uint32_t item_id = 0;
        l_fields_v2_t *value_field = NULL;
#warning "Warning: armed check remains"

        l_fields_v2_t *curr_field = when_block->fields;
        while (curr_field)
        {
            if (0 == strncmp(curr_field->name, "item", 4))
            {
                item_id = strtoul(curr_field->value.value_string, NULL, 16);
            }
            else if (0 == strncmp(curr_field->name, "value", 4))
            {
                value_field = curr_field;
            }
            curr_field = curr_field->next;
        }

        if (item_id && value_field)
        {
            l_ezlopi_device_t *curr_device = ezlopi_device_get_head();
            while (curr_device)
            {
                l_ezlopi_item_t *curr_item = curr_device->items;
                while (curr_item)
                {
                    if (item_id == curr_item->cloud_properties.item_id)
                    {
                        cJSON *cj_tmp_value = cJSON_CreateObject();
                        if (cj_tmp_value)
                        {
                            curr_item->func(EZLOPI_ACTION_GET_EZLOPI_VALUE, curr_item, (void *)cj_tmp_value, NULL);
                            cJSON *cj_value = cJSON_GetObjectItem(cj_tmp_value, "value");
                            if (cj_value)
                            {
                                switch (cj_value->type)
                                {
                                case cJSON_True:
                                {
                                    if (true == value_field->value.value_bool)
                                    {
                                        ret = 1;
                                    }
                                    break;
                                }
                                case cJSON_False:
                                {
                                    if (false == value_field->value.value_bool)
                                    {
                                        ret = 1;
                                    }
                                    break;
                                }
                                case cJSON_Number:
                                {
                                    if (cj_value->valuedouble == value_field->value.value_double)
                                    {
                                        ret = 1;
                                    }
                                    break;
                                }
                                case cJSON_String:
                                {
                                    uint32_t cmp_size = (strlen(cj_value->valuestring) > strlen(value_field->value.value_string)) ? strlen(cj_value->valuestring) : strlen(value_field->value.value_string);
                                    if (0 == strncmp(cj_value->valuestring, value_field->value.value_string, cmp_size))
                                    {
                                        ret = 1;
                                    }
                                    break;
                                }
                                default:
                                {
                                    TRACE_E("Value type mis-matched!");
                                }
                                }
                            }

                            cJSON_Delete(cj_tmp_value);
                        }
                        break;
                    }
                    curr_item = curr_item->next;
                }

                curr_device = curr_device->next;
            }
        }
    }

    return ret;
}

int ezlopi_scene_when_is_interval(l_scenes_list_v2_t *scene_node, void *arg)
{
    int ret = 0;

    if (scene_node)
    {
        char *end_prt = NULL;
        uint32_t interval = strtoul(scene_node->when_block->fields->value.value_string, &end_prt, 10);
        if (end_prt)
        {
            if (0 == strncmp(end_prt, "m", 1))
            {
                interval *= 60;
            }
            else if (0 == strncmp(end_prt, "h", 1))
            {
                interval *= (60 * 60);
            }
            else if (0 == strncmp(end_prt, "d", 1))
            {
                interval *= (60 * 60 * 24);
            }
        }

        uint32_t last_update = 0;
        if (scene_node->when_block->fields->user_arg)
        {
            last_update = (uint32_t)scene_node->when_block->fields->user_arg;

            if (((xTaskGetTickCount() - last_update) / 1000) > interval)
            {
                scene_node->when_block->fields->user_arg = xTaskGetTickCount();
                ret = 1;
            }
        }
        else
        {
            ret = 1;
            scene_node->when_block->fields->user_arg = xTaskGetTickCount();
        }
    }

    return ret;
}

int ezlopi_scene_when_is_item_state_changed(l_scenes_list_v2_t *scene_node, void *arg)
{
    TRACE_W("Warning: when-method 'is_item_state_changed' not implemented!");
    return 0;
}

int ezlopi_scene_when_is_button_state(l_scenes_list_v2_t *scene_node, void *arg)
{
    int ret = 0;
    l_when_block_v2_t *when_block = (l_when_block_v2_t *)arg;
    if (when_block)
    {
        uint32_t item_id = 0;
        l_fields_v2_t *value_field = NULL;

        l_fields_v2_t *curr_field = when_block->fields;
        while (curr_field)
        {
            if (0 == strncmp(curr_field->name, "item", 5))
            {
                item_id = strtoul(curr_field->value.value_string, NULL, 16); // base 16
            }
            else if (0 == strncmp(curr_field->name, "value", 6))
            {
                if (EZLOPI_VALUE_TYPE_TOKEN == curr_field->value_type)
                {
                    // Here !! token_type is a -> "string"  | eg. "released"
                    //------------------------------------------------------------------------------
                    // const char *button_actions[] = {
                    //     "idle",
                    //     "press_1_time",
                    //     "released",
                    //     "...",
                    // };
                    // for (;;)
                    // {
                    //     if (button_actions[] == curr_field->value)
                    //     {
                    //         //value_field = curr_field;
                    //     }
                    // }
                    //------------------------------------------------------------------------------

                    value_field = curr_field; // pass the pointer holding the ->  button_actions:'released'
                }
            }
            curr_field = curr_field->next;
        }

        if (item_id && value_field)
        {
            // "value": {button_number = 1, button_state = "press_1_time"},

            l_ezlopi_device_t *curr_device = ezlopi_device_get_head();
            while (curr_device)
            {
                l_ezlopi_item_t *curr_item = curr_device->items;
                while (curr_item)
                {
                    if (item_id == curr_item->cloud_properties.item_id) // perticular item with name->button_state
                    {
                        cJSON *cj_tmp_value = cJSON_CreateObject();
                        if (cj_tmp_value)
                        {
                            curr_item->func(EZLOPI_ACTION_GET_EZLOPI_VALUE, curr_item, (void *)cj_tmp_value, NULL);
                            if (EZLOPI_VALUE_TYPE_BUTTON_STATE == curr_field->value_type)
                            {
                                cJSON *cj_value = cJSON_GetObjectItem(cj_tmp_value, "value"); //
                                if (cj_value)
                                {
                                    switch (cj_value->type)
                                    {
                                    case cJSON_String:
                                    {
                                        uint32_t cmp_size = (strlen(cj_value->valuestring) > strlen(value_field->value.value_string)) ? strlen(cj_value->valuestring) : strlen(value_field->value.value_string);
                                        if (0 == strncmp(cj_value->valuestring, value_field->value.value_string, cmp_size))
                                        {
                                            ret = 1;
                                        }
                                        break;
                                    }
                                    default:
                                    {
                                        TRACE_E("Value type mis-matched!");
                                    }
                                    }
                                }
                            }
                            cJSON_Delete(cj_tmp_value);
                        }
                    }
                    curr_item = curr_item->next;
                }
                curr_device = curr_device->next;
            }
        }
    }
    return ret;
}

int ezlopi_scene_when_is_sun_state(l_scenes_list_v2_t *scene_node, void *arg)
{
    TRACE_W("Warning: when-method 'is_sun_state' not implemented!");
    return 0;
}

typedef enum e_isdate_type_modes
{
    ISDATE_DAILY_MODE = 0,
    ISDATE_WEEKLY_MODE,
    ISDATE_MONTHLY_MODE,
    ISDATE_WEEKS_MODE,
    ISDATE_YEAR_WEEKS_MODE,
    ISDATE_NONE_MODE,
} e_isdate_modes_t;

typedef struct s_field_filter
{
    const char *field_name;
    uint8_t (*field_func)(e_isdate_modes_t mode_type, struct tm *info, const void *arg);
} s_field_filter_t;

static e_isdate_modes_t field_type_check(const char *check_type_name)
{
    const char *field_type_name[] = {
        "daily",
        "weekly",
        "monthly",
        "weeks",
        "yearWeeks",
    };

    e_isdate_modes_t ret = ISDATE_NONE_MODE;
    for (uint8_t t = 0; t < 5; t++)
    {
        if (0 == strncmp(field_type_name[t], check_type_name, strlen(check_type_name) + 1))
        {
            ret = t; // daily , weekly , monthly,... // 0-4
            break;
        }
    }
    return ret;
}

static uint8_t field_time_check(e_isdate_modes_t mode_type, struct tm *info, const void *arg)
{
    uint8_t ret = 0;
    cJSON *cj_time_arr = (cJSON *)arg;
    if (cj_time_arr && (cJSON_Array == cj_time_arr->type))
    {
        TRACE_B("time__[' hh:mm ']");
        char field_hr_mm[10];
        strftime(field_hr_mm, 10, "%H:%M", info);

        cJSON *curr_value = cj_time_arr->child;
        while (curr_value)
        {
            if (0 == strncmp(cj_time_arr->valuestring, field_hr_mm, 10))
            {
                ret = TIME_FLAG;
            }
            curr_value = curr_value->next;
        }
        // if we are given :- "value": []
        if (0 == strncmp(cj_time_arr->valuestring, "00:00", 6)) // 24-hr format
        {
            ret = TIME_FLAG;
        }
    }
    return ret;
}

static uint8_t field_weekdays_check(e_isdate_modes_t mode_type, struct tm *info, const void *arg)
{
    uint8_t ret = 0;
    cJSON *cj_weekdays_arr = (cJSON *)arg;
    if (cj_weekdays_arr && (cJSON_Array == cj_weekdays_arr->type))
    {
        TRACE_B("weekdays__[ 1 - 7 ]");
        int field_weekdays = (info->tm_wday) + 1; // sunday => 0+1 ... saturday => 6+1

        cJSON *curr_value = cj_weekdays_arr->child;
        while (curr_value)
        {
            if (cj_weekdays_arr->valuedouble == field_weekdays)
            {
                ret = WEEKDAYS_FLAG;
            }
            curr_value = curr_value->next;
        }
    }
    return ret;
}

static uint8_t field_days_check(e_isdate_modes_t mode_type, struct tm *info, const void *arg)
{
    uint8_t ret = 0;
    cJSON *cj_days_arr = (cJSON *)arg;
    if (cj_days_arr && (cJSON_Array == cj_days_arr->type))
    {
        TRACE_B("days__[ 1 - 31 ]");
        int field_days = info->tm_mday; // 0-31
        cJSON *curr_value = cj_days_arr->child;
        while (curr_value)
        {
            if (cj_days_arr->valuedouble == field_days)
            {
                ret = DAYS_FLAG;
            }
            curr_value = curr_value->next;
        }
    }
    return ret;
}
static uint8_t field_weeks_check(e_isdate_modes_t mode_type, struct tm *info, const void *arg)
{
    int ret = 0;
    cJSON *cj_weeks_arr = (cJSON *)arg;
    if (cj_weeks_arr && (cJSON_Array == cj_weeks_arr->type))
    {

        switch (mode_type)
        {

        case ISDATE_YEAR_WEEKS_MODE:
            TRACE_B("days__[1, ..., 54, -1]"); // -1 = last week of the year
            break;
        case ISDATE_WEEKS_MODE:
        default:
            TRACE_B("days__[1, ..., 6, -1]"); // -1 = last week of the month
            break;
        }

        char week_val[10] = {'/0'};
        snprintf(week_val, 10, "%f", cj_weeks_arr->valuedouble); // converting to str for comparision

        char field_weeks[10];
        strftime(field_weeks, 10, "%W", info); // Week number with the first day as Monday (00-53)

        cJSON *curr_value = cj_weeks_arr->child;
        while (curr_value)
        {
            if (0 == strncmp(week_val, field_weeks, 10))
            {
                ret = WEEKS_FLAG;
            }
            curr_value = curr_value->next;
        }
        // for '-1' case
        if (0 == strncmp(week_val, "-1", 10))
        {
            ret = WEEKS_FLAG;
        }
    }
    return ret;
}

static const s_field_filter_t field_filter_arr[] = {
    {.field_name = "time", .field_func = field_time_check},         // func process ["hh:mm"]
    {.field_name = "weekdays", .field_func = field_weekdays_check}, // func process days in a week [1-7]
    {.field_name = "days", .field_func = field_days_check},         //
    {.field_name = "weeks", .field_func = field_weeks_check},
    {.field_name = NULL, .field_func = NULL},
};

int ezlopi_scene_when_is_date(l_scenes_list_v2_t *scene_node, void *arg)
{
    int ret = 0;
    if (scene_node)
    {
        l_when_block_v2_t *when_block = (l_when_block_v2_t *)arg;
        if (when_block)
        {
            e_isdate_modes_t mode_type = ISDATE_NONE_MODE;
            uint8_t flag_check = 0; //
            l_fields_v2_t *curr_field = when_block->fields;

            // calculate the rawtime
            time_t rawtime;
            time(&rawtime);
            struct tm *info;
            info = localtime(&rawtime); // assign current date+time

            // function counter

            while (NULL != curr_field)
            {
                if (0 == strncmp(curr_field->name, "type", 5))
                {
                    if ((NULL != curr_field->value_type) && (NULL != curr_field->value.value_string))
                    {
                        mode_type = field_type_check(curr_field->value.value_string);
                    }
                }
                else
                {
                    for (uint8_t i = 0; i < ((sizeof(field_filter_arr) / sizeof(field_filter_arr[i]))); i++)
                    {
                        if (0 == strncmp(field_filter_arr[i].field_name, curr_field->name, strlen(curr_field->name) + 1))
                        {
                            flag_check |= (field_filter_arr[i].field_func)(mode_type, info, curr_field->value.value_json);
                        }
                    }
                }

                curr_field = curr_field->next;
            }

            if (flag_check > 0) // if any one of the condition is satisfied
            {
            }
        }
    }
    return 0;
}

int ezlopi_scene_when_is_once(l_scenes_list_v2_t *scene_node, void *arg)
{
    TRACE_W("Warning: when-method 'is_once' not implemented!");
    return 0;
}

int ezlopi_scene_when_is_date_range(l_scenes_list_v2_t *scene_node, void *arg)
{
    TRACE_W("Warning: when-method 'is_date_range' not implemented!");
    return 0;
}

int ezlopi_scene_when_is_user_lock_operation(l_scenes_list_v2_t *scene_node, void *arg)
{
    TRACE_W("Warning: when-method 'is_user_lock_operation' not implemented!");
    return 0;
}

int ezlopi_scene_when_is_house_mode_changed_to(l_scenes_list_v2_t *scene_node, void *arg)
{
    TRACE_W("Warning: when-method 'is_house_mode_changed_to' not implemented!");
    return 0;
}

int ezlopi_scene_when_is_house_mode_changed_from(l_scenes_list_v2_t *scene_node, void *arg)
{
    TRACE_W("Warning: when-method 'is_house_mode_changed_from' not implemented!");
    return 0;
}

int ezlopi_scene_when_is_device_state(l_scenes_list_v2_t *scene_node, void *arg)
{
    TRACE_W("Warning: when-method 'is_device_state' not implemented!");
    return 0;
}

int ezlopi_scene_when_is_network_state(l_scenes_list_v2_t *scene_node, void *arg)
{
    TRACE_W("Warning: when-method 'is_network_state' not implemented!");
    return 0;
}

int ezlopi_scene_when_is_scene_state(l_scenes_list_v2_t *scene_node, void *arg)
{
    TRACE_W("Warning: when-method 'is_scene_state' not implemented!");
    return 0;
}

int ezlopi_scene_when_is_group_state(l_scenes_list_v2_t *scene_node, void *arg)
{
    TRACE_W("Warning: when-method 'is_group_state' not implemented!");
    return 0;
}

int ezlopi_scene_when_is_cloud_state(l_scenes_list_v2_t *scene_node, void *arg)
{
    TRACE_W("Warning: when-method 'is_cloud_state' not implemented!");
    return 0;
}

int ezlopi_scene_when_is_battery_state(l_scenes_list_v2_t *scene_node, void *arg)
{
    TRACE_W("Warning: when-method 'is_battery_state' not implemented!");
    return 0;
}

int ezlopi_scene_when_is_battery_level(l_scenes_list_v2_t *scene_node, void *arg)
{
    TRACE_W("Warning: when-method 'is_battery_level' not implemented!");
    return 0;
}

int ezlopi_scene_when_compare_numbers(l_scenes_list_v2_t *scene_node, void *arg)
{
    int ret = 0;
    l_when_block_v2_t *when_block = (l_when_block_v2_t *)arg;

    if (when_block && scene_node)
    {
        uint32_t item_id = 0;
        l_fields_v2_t *value_field = NULL;
        l_fields_v2_t *comparator_field = NULL;

        l_fields_v2_t *curr_field = when_block->fields;
        while (curr_field)
        {
            if (0 == strncmp(curr_field->name, "item", 4))
            {
                item_id = strtoul(curr_field->value.value_string, NULL, 16);
            }
            else if (0 == strncmp(curr_field->name, "value", 4))
            {
                value_field = curr_field;
            }
            else if (0 == strncmp(curr_field->name, "comparator", 10))
            {
                comparator_field = curr_field;
            }
            curr_field = curr_field->next;
        }

        if (item_id && value_field && comparator_field)
        {
            ret = ezlopi_scenes_operators_value_number_operations(item_id, value_field, comparator_field);
        }
    }

    return ret;
}

int ezlopi_scene_when_compare_number_range(l_scenes_list_v2_t *scene_node, void *arg)
{
    TRACE_W("Warning: when-method 'number_range' not implemented!");
    return 0;
}

int ezlopi_scene_when_compare_strings(l_scenes_list_v2_t *scene_node, void *arg)
{
    TRACE_W("Warning: when-method 'compare_strings' not implemented!");
    return 0;
}

int ezlopi_scene_when_string_operation(l_scenes_list_v2_t *scene_node, void *arg)
{
    TRACE_W("Warning: when-method 'string_operation' not implemented!");
    return 0;
}

int ezlopi_scene_when_in_array(l_scenes_list_v2_t *scene_node, void *arg)
{
    TRACE_W("Warning: when-method 'in_array' not implemented!");
    return 0;
}

int ezlopi_scene_when_compare_values(l_scenes_list_v2_t *scene_node, void *arg)
{
    TRACE_W("Warning: when-method 'compare_values' not implemented!");
    return 0;
}

int ezlopi_scene_when_has_atleast_one_dictionary_value(l_scenes_list_v2_t *scene_node, void *arg)
{
    TRACE_W("Warning: when-method 'has_atleast_one_dictionary_value' not implemented!");
    return 0;
}

int ezlopi_scene_when_is_firmware_update_state(l_scenes_list_v2_t *scene_node, void *arg)
{
    TRACE_W("Warning: when-method 'is_firmware_update_state' not implemented!");
    return 0;
}

int ezlopi_scene_when_is_dictionary_changed(l_scenes_list_v2_t *scene_node, void *arg)
{
    TRACE_W("Warning: when-method 'is_dictionary_changed' not implemented!");
    return 0;
}

int ezlopi_scene_when_is_detected_in_hot_zone(l_scenes_list_v2_t *scene_node, void *arg)
{
    TRACE_W("Warning: when-method 'is_detected_in_hot_zone' not implemented!");
    return 0;
}

int ezlopi_scene_when_and(l_scenes_list_v2_t *scene_node, void *arg)
{
    int ret = 0;
    l_when_block_v2_t *when_block = (l_when_block_v2_t *)arg;
    if (when_block)
    {
        l_when_block_v2_t *value_when_block = when_block->fields->value.when_block;
        while (value_when_block)
        {
            f_scene_method_v2_t scene_method = ezlopi_scene_get_method_v2(value_when_block->block_options.method.type);
            if (scene_method)
            {
                ret &= scene_method(scene_node, (void *)value_when_block);
                if (!ret)
                {
                    break;
                }
            }

            value_when_block = value_when_block->next;
        }
    }

    return ret;
}

int ezlopi_scene_when_not(l_scenes_list_v2_t *scene_node, void *arg)
{
    TRACE_W("Warning: when-method not implemented!");
    return 0;
}

int ezlopi_scene_when_or(l_scenes_list_v2_t *scene_node, void *arg)
{
    int ret = 0;
    l_when_block_v2_t *when_block = (l_when_block_v2_t *)arg;
    if (when_block)
    {
        l_when_block_v2_t *value_when_block = when_block->fields->value.when_block;
        while (value_when_block)
        {
            f_scene_method_v2_t scene_method = ezlopi_scene_get_method_v2(value_when_block->block_options.method.type);
            if (scene_method)
            {
                ret |= scene_method(scene_node, (void *)value_when_block);
                if (ret)
                {
                    break;
                }
            }

            value_when_block = value_when_block->next;
        }
    }

    return ret;
}

int ezlopi_scene_when_xor(l_scenes_list_v2_t *scene_node, void *arg)
{
    TRACE_W("Warning: when-method not implemented!");
    return 0;
}

int ezlopi_scene_when_function(l_scenes_list_v2_t *scene_node, void *arg)
{
    TRACE_W("Warning: when-method not implemented!");
    return 0;
}
