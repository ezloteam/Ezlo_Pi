#include <string.h>
#include <time.h>

#include "trace.h"
#include "ezlopi_devices.h"
#include "ezlopi_scenes_v2.h"
#include "ezlopi_meshbot_service.h"
#include "ezlopi_scenes_operators.h"
#include "ezlopi_scenes_when_methods.h"

typedef enum e_issunstate_trigger_modes
{
    ISSUNSTATE_INTIME_MODE = 0,
    ISSUNSTATE_BEFORE_MODE,
    ISSUNSTATE_AFTER_MODE,
    ISSUNSTATE_UNDEFINED,
} e_issunstate_trigger_modes_t;
typedef enum e_isdate_modes
{
    ISDATE_DAILY_MODE = 0,
    ISDATE_WEEKLY_MODE,
    ISDATE_MONTHLY_MODE,
    ISDATE_WEEKS_MODE,
    ISDATE_YEAR_WEEKS_MODE,
    ISDATE_UNDEFINED_MODE,
} e_isdate_modes_t;

typedef enum e_isdate_range_func
{
    ISDATE_RANGE_TIME = 0,
    ISDATE_RANGE_DAY,
    ISDATE_RANGE_MONTH,
    ISDATE_RANGE_YEAR,
    ISDATE_RANGE_MAX,
} e_isdate_range_func_t;

typedef struct s_field_filter
{
    const char *field_name;
    uint8_t (*field_func)(e_isdate_modes_t mode_type, struct tm *info, cJSON *arg);
} s_field_filter_t;

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

        // match the conditions
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

//------------------------------- ezlopi_scene_when_is_date -----------------------------------------------------------
static e_isdate_modes_t __field_type_check(const char *check_type_name)
{
    const char *field_type_name[] = {
        "daily",
        "weekly",
        "monthly",
        "weeks",
        "yearWeeks",
    };

    e_isdate_modes_t ret = ISDATE_UNDEFINED_MODE;
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
static uint8_t __field_time_check(e_isdate_modes_t mode_type, struct tm *info, cJSON *cj_time_arr)
{
    uint8_t ret = 0;
    if (cj_time_arr && (cJSON_Array == cj_time_arr->type))
    {
        char field_hr_mm[10] = {0};
        strftime(field_hr_mm, 10, "%H:%M", info);
        field_hr_mm[10] = '\0';

        int array_size = cJSON_GetArraySize(cj_time_arr);
        for (int i = 0; i < array_size; i++)
        {
            cJSON *array_item = cJSON_GetArrayItem(cj_time_arr, i);
            if (array_item && cJSON_IsString(array_item))
            {
                // TRACE_B("Time activate_%d: %s,  [field_hr_mm: %s]", i, array_item->valuestring, field_hr_mm);
                if (0 == strncmp(array_item->valuestring, field_hr_mm, 10))
                {
                    ret = (1 << 0); // One of the TIME-condition has been met.
                    break;
                }
            }
        }
        if (!array_size) // if we are given : -"value" : []
        {
            TRACE_B("Time activate :- [00:00],  [field_hr_mm: %s]", field_hr_mm);
            if (0 == strncmp(field_hr_mm, "00:00", 10)) // 24-hr format
            {
                ret = (1 << 0); // TIME-condition "00:00" has been met.
            }
        }
    }
    return ret;
}
static uint8_t __field_weekdays_check(e_isdate_modes_t mode_type, struct tm *info, cJSON *cj_weekdays_arr)
{
    uint8_t ret = 0;
    if (cj_weekdays_arr && (cJSON_Array == cj_weekdays_arr->type))
    {
        // Only for comparisions dont change 'info->tm_wday' -> 'sun:0,mon:1, ... , sat:6' to 'sun:7,mon:1, ... ,sat:6'
        int field_weekdays = (0 == (info->tm_wday)) ? 7 : (info->tm_wday); // sunday => 0+1 ... saturday => 6+1

        int array_size = cJSON_GetArraySize(cj_weekdays_arr);
        for (int i = 0; i < array_size; i++)
        {
            cJSON *array_item = cJSON_GetArrayItem(cj_weekdays_arr, i);
            if (array_item && cJSON_IsNumber(array_item))
            {
                // TRACE_B("Weekdays activate_%d: %d, [field_weekdays: %d]", i, (int)(array_item->valuedouble), field_weekdays);
                if ((int)(array_item->valuedouble) == field_weekdays)
                {
                    ret = (1 << 1); // One of the WEEKDAYS-condition has been met.
                    break;
                }
            }
        }
    }
    return ret;
}
static uint8_t __field_days_check(e_isdate_modes_t mode_type, struct tm *info, cJSON *cj_days_arr)
{
    uint8_t ret = 0;
    if (cj_days_arr && (cJSON_Array == cj_days_arr->type))
    {
        int array_size = cJSON_GetArraySize(cj_days_arr);
        int field_days = info->tm_mday; // 1-31

        for (int i = 0; i < array_size; i++)
        {
            cJSON *array_item = cJSON_GetArrayItem(cj_days_arr, i);
            if (array_item && cJSON_IsNumber(array_item))
            {
                // TRACE_B("Days activate_%d: %d, [field_days: %d]", i, (int)(array_item->valuedouble), field_days);
                if ((int)(array_item->valuedouble) == field_days)
                {
                    ret = (1 << 2); // One of the DAYS-condition has been met.
                    break;
                }
            }
        }
    }
    return ret;
}
static uint8_t __compare_end_week_date(e_isdate_modes_t mode_type, struct tm *info)
{
    uint8_t ret = 0;
    static uint8_t _last_day_of_curr_month = 0;
    static int _starting_date_of_last_week = -1;
    // filter out the first 22 days ; which do-not lie in last week of the month
    if (info->tm_mday > 22)
    {
        // 1. Find out the 'valid' starting date of last week ( return -1 , if invalid )
        if (-1 == _starting_date_of_last_week)
        {
            //-------------------------------------------------------------------
            // 1.1 find the nearest-prev sunday and assign it.
            // for this calculation dont change default 'info->tm_wday' -> '0-6'
            switch (info->tm_wday)
            {
            case 1: // monday
                _starting_date_of_last_week = info->tm_mday - 1;
                break;
            case 2: // tuesday
                _starting_date_of_last_week = info->tm_mday - 2;
                break;
            case 3: // wednasday
                _starting_date_of_last_week = info->tm_mday - 3;
                break;
            case 4: // thursday
                _starting_date_of_last_week = info->tm_mday - 4;
                break;
            case 5: // friday
                _starting_date_of_last_week = info->tm_mday - 5;
                break;
            case 6: // saturday
                _starting_date_of_last_week = info->tm_mday - 6;
                break;
            default:
                _starting_date_of_last_week = info->tm_mday;
                break;
            }
            TRACE_I("sunday :%dth_Day ", _starting_date_of_last_week);
            //-------------------------------------------------------------------
            // 1.2 validate the generated sunday number
            switch (info->tm_mon)
            {
            case 1: // FEB
            {
                int year = info->tm_year + 1900;
                if ((0 == (year % 4)) && ((0 == (year % 400)) || (0 != (year % 100))))
                {
                    _starting_date_of_last_week = ((29 - _starting_date_of_last_week) < 7) ? _starting_date_of_last_week : -1; // if this sunday's day_num lies within last 7-days of the month ; return this sunday's date
                    _last_day_of_curr_month = 29;
                }
                else // non-leap years
                {
                    _starting_date_of_last_week = ((28 - _starting_date_of_last_week) < 7) ? _starting_date_of_last_week : -1; // if this sunday's day_num lies within last 7-days of the month ; return this sunday's date
                    _last_day_of_curr_month = 28;
                }

                break;
            }
            case 3:  // APR
            case 5:  // JUN
            case 8:  // SEP
            case 10: // NOV
            {
                _starting_date_of_last_week = ((30 - _starting_date_of_last_week) < 7) ? _starting_date_of_last_week : -1;
                _last_day_of_curr_month = 30;
                break;
            }
            default: // JAN , MAR , MAY , JULY , AUG , OCT , DEC
            {
                _starting_date_of_last_week = ((31 - _starting_date_of_last_week) < 7) ? _starting_date_of_last_week : -1;
                _last_day_of_curr_month = 31;
                break;
            }
            }
        }

        // 2. Now to compare today's date and produce result ( if not -1 )
        if (-1 != _starting_date_of_last_week)
        {
            TRACE_B(" Last week of [%d] starts from [%d] :", info->tm_mon, _starting_date_of_last_week);

            if (ISDATE_YEAR_WEEKS_MODE == mode_type) // last week of the year
            {
                if ((11 == info->tm_mon) && (info->tm_mday >= _starting_date_of_last_week))
                {
                    if (info->tm_mday >= _last_day_of_curr_month)
                    { // refresh the calculation flag '_starting_date_of_last_week'
                        _starting_date_of_last_week = -1;
                    }
                    ret = (1 << 3);
                }
            }
            else if (ISDATE_WEEKS_MODE == mode_type) // last week of the month
            {
                if (info->tm_mday >= _starting_date_of_last_week)
                {
                    if (info->tm_mday >= _last_day_of_curr_month)
                    { // refresh the calculation flag '_starting_date_of_last_week'
                        _starting_date_of_last_week = -1;
                    }
                    ret = (1 << 3);
                }
            }
        }
    }
    return ret;
}
static uint8_t __find_nth_week_of_curr_month(struct tm *info)
{
    // 2. find the fisrt day in this month
    uint8_t tmp_week_num = 1;                        // starts with 1 ; since are already in one of the week-count
                                                     // for this calculation dont change default 'info->tm_wday' -> '0-6'
    int tmp_weekday_of_curr_month = (info->tm_wday); // 0-6 ; sun = 0
    for (uint8_t i = (info->tm_mday); i > 1; i--)    // total_days_in_curr_month - 1
    {
        if (1 == tmp_weekday_of_curr_month) // if 'monday' ; add count
        {
            tmp_week_num++;
        }
        tmp_weekday_of_curr_month--;
        if (0 > tmp_weekday_of_curr_month)
        {
            tmp_weekday_of_curr_month = 6; // sunday -> saturday
        }
    }

    // TRACE_B("First day in current month = %d", tmp_weekday_of_curr_month);
    // TRACE_B("[1-7] : %dth_Day  lies in week[%dth] of the current month", (info->tm_wday), tmp_week_num);

    return tmp_week_num;
}
static uint8_t __field_weeks_check(e_isdate_modes_t mode_type, struct tm *info, cJSON *cj_weeks_arr)
{
    uint8_t ret = 0;
    if (cj_weeks_arr && (cJSON_Array == cj_weeks_arr->type))
    {
        int array_size = cJSON_GetArraySize(cj_weeks_arr);
        for (int i = 0; i < array_size; i++)
        {
            // extract ;- [1,4,5,23,6,9,...,-1]
            cJSON *array_item = cJSON_GetArrayItem(cj_weeks_arr, i);
            if (array_item && cJSON_IsNumber(array_item))
            {
                if (-1 == (int)(array_item->valuedouble)) // for case :- '-1'
                {
                    if (0 != (ret = __compare_end_week_date(mode_type, info))) // ret = (1<<3), if last-week confirmed
                    {
                        TRACE_I("Weeks_condition : '-1' has been satisfied ; ret = %#x", ret);
                        break;
                    }
                }
                else // for case :- 'n'
                {
                    char field_weeks[10] = {0}; // week_value extracted from ESP32.
                    char week_val[10] = {0};    // week_value given to us from cloud.

                    // reducing array values by -1, for easier comparison ::==>  [1_54]--->[0_53]      or      [1_6]-->[0_5]
                    snprintf(week_val, 10, "%d", (int)(array_item->valuedouble - 1));
                    week_val[10] = '\0';

                    if (ISDATE_YEAR_WEEKS_MODE == mode_type)
                    {
                        strftime(field_weeks, 10, "%W", info); // [First day => Monday] ; Week number within (00-53)
                        field_weeks[10] = '\0';
                    }
                    else if (ISDATE_WEEKS_MODE == mode_type)
                    {
                        int tmp_week = __find_nth_week_of_curr_month(info); // return the current week-number with 'monday' as first day
                        snprintf(field_weeks, 10, "%d", tmp_week);
                        field_weeks[10] = '\0';
                    }

                    if (0 == strncmp(week_val, field_weeks, 10)) // comparsion in string formats only
                    {
                        ret = (1 << 3); // One of the TIME-condition has been met.
                        // TRACE_I("Weeks_condition '%sth week' [reqd : %s]  has been satisfied ; ret = (1<<3)", field_weeks, week_val);
                        break;
                    }
                }
            }
        }
    }
    return ret;
}

static const s_field_filter_t field_isdate_filter_arr[] = {
    {.field_name = "time", .field_func = __field_time_check},         // func process ["hh:mm"]
    {.field_name = "weekdays", .field_func = __field_weekdays_check}, // func process days in a week [1-7]
    {.field_name = "days", .field_func = __field_days_check},         //
    {.field_name = "weeks", .field_func = __field_weeks_check},
    {.field_name = NULL, .field_func = NULL},
};
int ezlopi_scene_when_is_date(l_scenes_list_v2_t *scene_node, void *arg)
{
    int ret = 0;
    l_when_block_v2_t *when_block = (l_when_block_v2_t *)arg;
    if (when_block)
    {
        e_isdate_modes_t mode_type = ISDATE_UNDEFINED_MODE;
        l_fields_v2_t *curr_field = when_block->fields;

        // calculate the rawtime
        time_t rawtime = 0;
        time(&rawtime);
        struct tm *info;
        info = localtime(&rawtime);

        // temporary flags
        uint8_t flag_check = 0;
        const uint8_t TIME_FLAG = (1 << 0);
        const uint8_t WEEKDAYS_FLAG = (1 << 1);
        const uint8_t DAYS_FLAG = (1 << 2);
        const uint8_t WEEKS_FLAG = (1 << 3);
        const uint8_t MASK_FOR_TIME_ARG = (1 << 4);
        const uint8_t MASK_FOR_WEEKDAYS_ARG = (1 << 5);
        const uint8_t MASK_FOR_DAYS_ARG = (1 << 6);
        const uint8_t MASK_FOR_WEEKS_ARG = (1 << 7);

        // Condition checker
        while (NULL != curr_field)
        {
            if (0 == strncmp(curr_field->name, "type", 5))
            {
                if ((EZLOPI_VALUE_TYPE_STRING == curr_field->value_type) && (NULL != curr_field->value.value_string))
                {
                    mode_type = __field_type_check(curr_field->value.value_string);
                }
            }
            else
            {
                for (uint8_t i = 0; i < ((sizeof(field_isdate_filter_arr) / sizeof(field_isdate_filter_arr[i]))); i++)
                {
                    if (0 == strncmp(field_isdate_filter_arr[i].field_name, curr_field->name, strlen(curr_field->name) + 1))
                    {
                        flag_check |= (1 << (i + 4));                                                                         // bit4 - bit7
                        flag_check |= (field_isdate_filter_arr[i].field_func)(mode_type, info, curr_field->value.value_json); // bit0 - bit3
                        break;
                    }
                }
            }
            curr_field = curr_field->next;
        }

        // Output Filter based on date+time of activation
        switch (mode_type)
        {
        case ISDATE_DAILY_MODE:
        {
            TRACE_D("mode[%d], isDate:- FLAG_STATUS: %#x", mode_type, flag_check);
            if (((flag_check & MASK_FOR_TIME_ARG) && (flag_check & TIME_FLAG))) //&&(0 != strncmp(buffer, prev_date_time, 20)))
            {
                if (57 == (int)(scene_node->when_block->fields->user_arg)++) // 57 sec mark
                {
                    TRACE_W("here! time");
                    ret = 1;
                }
                int _is_date_counter = (int)(scene_node->when_block->fields->user_arg);
                TRACE_B("user_arg = [%d]", _is_date_counter);
            }
            else
            {
                (scene_node->when_block->fields->user_arg) = 0;
            }
            break;
        }
        case ISDATE_WEEKLY_MODE:
        {
            TRACE_D("mode[%d], isDate:- FLAG_STATUS: %#x", mode_type, flag_check);
            if ((((flag_check & MASK_FOR_TIME_ARG) && (flag_check & TIME_FLAG)) &&
                 ((flag_check & MASK_FOR_WEEKDAYS_ARG) && (flag_check & WEEKDAYS_FLAG))))
            {
                if (57 == (int)(scene_node->when_block->fields->user_arg)++) // 57 sec mark
                {
                    TRACE_W("here! week_days and time");
                    ret = 1;
                }
                int _is_date_counter = (int)(scene_node->when_block->fields->user_arg);
                TRACE_B("user_arg = [%d]", _is_date_counter);
            }
            else
            {
                (scene_node->when_block->fields->user_arg) = 0;
            }
            break;
        }
        case ISDATE_MONTHLY_MODE:
        {
            TRACE_D("mode[%d], isDate:- FLAG_STATUS: %#x", mode_type, flag_check);
            if ((((flag_check & MASK_FOR_TIME_ARG) && (flag_check & TIME_FLAG)) &&
                 ((flag_check & MASK_FOR_DAYS_ARG) && (flag_check & DAYS_FLAG))))
            {
                if (57 == (int)(scene_node->when_block->fields->user_arg)++) // 57 sec mark
                {
                    TRACE_W("here! mon_days and time");
                    ret = 1;
                }
                int _is_date_counter = (int)(scene_node->when_block->fields->user_arg);
                TRACE_B("user_arg = [%d]", _is_date_counter);
            }
            else
            {
                (scene_node->when_block->fields->user_arg) = 0;
            }
            break;
        }
        case ISDATE_WEEKS_MODE:
        case ISDATE_YEAR_WEEKS_MODE:
        {
            TRACE_D("mode[%d], isDate:- FLAG_STATUS: %#x", mode_type, flag_check);
            if (((flag_check & MASK_FOR_TIME_ARG) && (flag_check & TIME_FLAG)) &&
                ((flag_check & MASK_FOR_WEEKS_ARG) && (flag_check & WEEKS_FLAG))) // && (0 != strncmp(buffer, prev_date_time, 20)))
            {
                if (57 == (int)(scene_node->when_block->fields->user_arg)++) // 57 sec mark
                {
                    TRACE_W("here! week and time");
                    ret = 1;
                }
                int _is_date_counter = (int)(scene_node->when_block->fields->user_arg);
                TRACE_B("user_arg = [%d]", _is_date_counter);
            }
            else
            {
                (scene_node->when_block->fields->user_arg) = 0;
            }
            break;
        }
        default:
            break;
        }
    }
    return ret;
}

//------------------------------- ezlopi_scene_when_is_SunState -----------------------------------------------------------
static int __update_todays_sunrise_sunset_time(struct tm *sunrise_time, struct tm *sunset_time)
{
    int ret = 0;
    if ((NULL != sunrise_time) && (NULL != sunset_time))
    {
        // send httprequest to weather api

        // fill the structures
        ret = 1;
    }
    else
    {
        TRACE_E("Invalid 'sunrise_time' or 'sunset_time' Structures.");
    }
    return ret;
}

static e_issunstate_trigger_modes_t __field_sunstate_check(const char *check_type_name)
{
    const char *field_type_name[] = {
        "intime", // order according to enum
        "before",
        "after",
    };

    e_issunstate_trigger_modes_t ret = ISSUNSTATE_UNDEFINED;
    for (uint8_t t = 0; t < 3; t++)
    {
        if (0 == strncmp(field_type_name[t], check_type_name, strlen(check_type_name) + 1))
        {
            ret = t; // intime , before , after // 0-2
            break;
        }
    }
    return ret;
}

static uint8_t __issunstate_time_offset_check(e_issunstate_trigger_modes_t sunstate_type, struct tm *sunstate_time, struct tm *curr_time, struct tm *defined_moment, const char *time_offset)
{
    uint8_t ret = 0;
    if (sunstate_time && curr_time && defined_moment && time_offset) // sunstate_time => sunrise or sunset
    {
        // Default values to store start and end boundries
        struct tm tmp_time = {0};
        char time_diff[10];

        // now to extract the time_offsets
        snprintf(time_diff, 10, "%s", time_offset);
        time_diff[10] = '\0';
        char *ptr1 = NULL;
        char *ptr2 = NULL;
        if (0 != strlen(time_diff))
        {
            tmp_time.tm_hour = strtoul(time_diff, &ptr1, 10);
            if (NULL != ptr1)
            {
                tmp_time.tm_min = strtoul(ptr1 + 1, &ptr2, 10);
                if (NULL != ptr2)
                {
                    tmp_time.tm_sec = strtoul(ptr2 + 1, NULL, 10);
                }
                else // only has minutes and seconds
                {
                    tmp_time.tm_sec = tmp_time.tm_min;
                    tmp_time.tm_min = tmp_time.tm_hour;
                    tmp_time.tm_hour = 0;
                }
            }
            else // only has seconds
            {
                tmp_time.tm_sec = tmp_time.tm_hour;
                tmp_time.tm_min = 0;
                tmp_time.tm_hour = 0;
            }
        }
        // Combined the time_offset and sunrise/sunset timing
        switch (sunstate_type)
        {
        case ISSUNSTATE_BEFORE_MODE:
        {
            ret = 1;
            defined_moment->tm_hour = (sunstate_time->tm_hour - tmp_time.tm_hour);
            defined_moment->tm_hour = (defined_moment->tm_hour < 0) ? (24 + defined_moment->tm_hour) : defined_moment->tm_hour; // check the hour-range
            defined_moment->tm_min = (sunstate_time->tm_min - tmp_time.tm_min);
            defined_moment->tm_min = (defined_moment->tm_min < 0) ? (60 + defined_moment->tm_min) : defined_moment->tm_min; // check the min-range
            defined_moment->tm_sec = (sunstate_time->tm_sec - tmp_time.tm_sec);
            defined_moment->tm_sec = (defined_moment->tm_sec < 0) ? (60 + defined_moment->tm_sec) : defined_moment->tm_sec; // check the sec-range
            break;
        }
        case ISSUNSTATE_AFTER_MODE:
        {
            ret = 1;
            defined_moment->tm_hour = (sunstate_time->tm_hour + tmp_time.tm_hour);
            defined_moment->tm_hour = (defined_moment->tm_hour > 23) ? (defined_moment->tm_hour - 24) : defined_moment->tm_hour; // check the hour-range
            defined_moment->tm_min = (sunstate_time->tm_min + tmp_time.tm_min);
            defined_moment->tm_min = (defined_moment->tm_min > 59) ? (defined_moment->tm_min - 60) : defined_moment->tm_min; // check the min-range
            defined_moment->tm_sec = (sunstate_time->tm_sec + tmp_time.tm_sec);
            defined_moment->tm_sec = (defined_moment->tm_sec > 59) ? (defined_moment->tm_sec - 60) : defined_moment->tm_sec; // check the sec-range
            break;
        }
        case ISSUNSTATE_INTIME_MODE:
        {
            ret = 1;
            defined_moment->tm_hour = (sunstate_time->tm_hour);
            defined_moment->tm_min = (sunstate_time->tm_min);
            defined_moment->tm_sec = (sunstate_time->tm_sec);
            break;
        }
        case ISSUNSTATE_UNDEFINED:
        {
            ret = 0;
            break;
        }
        }
    }
    return ret;
}

static uint8_t __issunstate_midnight_time_range_check(uint8_t sunstate_mode, const char *range_type, struct tm *curr_time, struct tm *defined_moment)
{
    uint8_t ret = 0;
    if (sunstate_mode && range_type && curr_time && defined_moment)
    {
        if (0 == strncmp(range_type, "at", 3)) // at desired moment
        {
            if ((curr_time->tm_hour == defined_moment->tm_hour) &&
                (curr_time->tm_min == defined_moment->tm_min) &&
                (curr_time->tm_sec == defined_moment->tm_sec))
            {
                ret = (1 << 3);
            }
        }
        else if (0 == strncmp(range_type, "after", 6)) // all time after defined moment till midnight of current day
        {

            if ((curr_time->tm_hour < 24) && (curr_time->tm_hour > defined_moment->tm_hour))
            {
                ret = (1 << 3);
            }
            else if (((curr_time->tm_hour < 24) && (curr_time->tm_hour == defined_moment->tm_hour)) &&
                     ((curr_time->tm_min < 60) && (curr_time->tm_min >= defined_moment->tm_min)) &&
                     ((curr_time->tm_sec < 60) && (curr_time->tm_sec > defined_moment->tm_sec)))
            {
                ret = (1 << 3);
            }
        }
        else if (0 == strncmp(range_type, "before", 7)) // all time after midnight till defined moment;
        {
            if ((curr_time->tm_hour >= 0) && (curr_time->tm_hour < defined_moment->tm_hour))
            {
                ret = (1 << 3);
            }
            else if (((curr_time->tm_hour >= 0) && (curr_time->tm_hour == defined_moment->tm_hour)) &&
                     ((curr_time->tm_min >= 0) && (curr_time->tm_min <= defined_moment->tm_min)) &&
                     ((curr_time->tm_sec >= 0) && (curr_time->tm_sec <= defined_moment->tm_sec)))
            {
                ret = (1 << 3);
            }
        }
    }
    return ret;
}

int ezlopi_scene_when_is_sun_state(l_scenes_list_v2_t *scene_node, void *arg)
{
    int ret = 0;
    l_when_block_v2_t *when_block = (l_when_block_v2_t *)arg;
    if (when_block && scene_node)
    {
        static struct tm sunrise_time = {0}; // store the sunrise_time
        static struct tm sunset_time = {0};  // store the sunset_time
        struct tm defined_moment = {0};      // the actual defined_moment after combining : 'sunrise/sunset_time' & 'time_offset'

        e_issunstate_trigger_modes_t sunstate_type = ISSUNSTATE_UNDEFINED;
        l_fields_v2_t *curr_field = when_block->fields;

        // calculate the rawtime
        time_t rawtime = 0;
        time(&rawtime);
        struct tm *info;
        info = localtime(&rawtime);

        // temporary flags
        uint8_t flag_check = 0;
        const uint8_t TIME_FLAG = (1 << 0);
        const uint8_t WEEKDAYS_FLAG = (1 << 1);
        const uint8_t DAYS_FLAG = (1 << 2);
        const uint8_t VALID_RANGE_FLAG = (1 << 3);
        const uint8_t MASK_TIME = (1 << 4);
        const uint8_t MASK_WEEKDAYS = (1 << 5);
        const uint8_t MASK_DAYS = (1 << 6);
        const uint8_t MASK_VALID_RANGE = (1 << 7);

        uint8_t sunstate_mode = 0;
        // Condition checker
        while (NULL != curr_field)
        {
            if ((sunstate_mode = (0 == strncmp(curr_field->name, "sunrise", 8)) ? 1 : 0) ||
                (sunstate_mode = (0 == strncmp(curr_field->name, "sunset", 7) ? 2 : 0))) // first identify 'sunstate'
            {
                if (((uint32_t)NULL == (uint32_t)scene_node->when_block->fields->user_arg) || (info->tm_mday != (uint32_t)scene_node->when_block->fields->user_arg))
                {
                    scene_node->when_block->fields->user_arg = (void *)info->tm_mday;
                    if ((0 == sunrise_time.tm_mday) || (0 == sunset_time.tm_mday))
                    {
                        if (__update_todays_sunrise_sunset_time(&sunrise_time, &sunset_time)) // activate once per_day;
                        {
                            defined_moment = (1 == sunstate_mode) ? sunrise_time : ((2 == sunstate_mode) ? sunset_time : defined_moment); // else {0}
                        }
                    }
                }
                if ((EZLOPI_VALUE_TYPE_STRING == curr_field->value_type) && (NULL != curr_field->value.value_string))
                {
                    sunstate_type = __field_sunstate_check(curr_field->value.value_string); // calculate  and store it into 'sunrise_time & sunset_time'
                }
            }
            else if ((0 == strncmp(curr_field->name, "time", 5))) // string
            {
                flag_check |= MASK_TIME;
                if (EZLOPI_VALUE_TYPE_HMS_INTERVAL == curr_field->value_type && (NULL != curr_field->value.value_string))
                {
                    TRACE_W("SunState_mode => [%d]", sunstate_mode);
                    if (0 != sunstate_mode)
                    { // extract time interval (hms_interval)
                        flag_check |= __issunstate_time_offset_check(sunstate_type, ((1 == sunstate_mode) ? &sunrise_time : &sunset_time), info, &defined_moment, curr_field->value.value_string);
                    }
                }
            }
            else if ((0 == strncmp(curr_field->name, "weekdays", 5))) // weekdays // int_array
            {
                if ((EZLOPI_VALUE_TYPE_INT_ARRAY == curr_field->value_type) && (cJSON_IsArray(curr_field->value.value_json)))
                {
                    flag_check |= MASK_WEEKDAYS;                                                                     // indicates : check for weekdays
                    flag_check |= __field_weekdays_check(ISDATE_UNDEFINED_MODE, info, curr_field->value.value_json); // (1 << 1)
                }
            }
            else if ((0 == strncmp(curr_field->name, "days", 5))) // monthdays // int_array
            {
                if ((EZLOPI_VALUE_TYPE_INT_ARRAY == curr_field->value_type) && (cJSON_IsArray(curr_field->value.value_json)))
                {
                    flag_check |= MASK_DAYS;                                                                     // indicates : check for month_days
                    flag_check |= __field_days_check(ISDATE_UNDEFINED_MODE, info, curr_field->value.value_json); // (1 << 2)
                }
            }
            else if ((0 == strncmp(curr_field->name, "range", 5))) // till midnight
            {
                if (EZLOPI_VALUE_TYPE_STRING == curr_field->value_type && (NULL != curr_field->value.value_string))
                {
                    flag_check |= MASK_VALID_RANGE; // indicates : check for midnight-range
                    flag_check |= __issunstate_midnight_time_range_check(sunstate_mode, curr_field->value.value_string, info, &defined_moment);
                }
            }

            curr_field = curr_field->next;
        }

        if (0 != sunstate_mode)
        { // Output Filter based on date+time of activation
            TRACE_D("mode[%d], isSunState:- FLAG_STATUS: %#x", (sunstate_type), flag_check);
            switch (flag_check)
            {
            case ((MASK_DAYS | DAYS_FLAG) | (MASK_WEEKDAYS | WEEKDAYS_FLAG) | (MASK_VALID_RANGE | VALID_RANGE_FLAG)): // 0. mdays + weekdays + midnight time?
            case ((MASK_DAYS | DAYS_FLAG) | (MASK_WEEKDAYS | WEEKDAYS_FLAG) | (MASK_TIME | TIME_FLAG)):               // 0. month_days + weekdays + exact time?
            case ((MASK_WEEKDAYS | WEEKDAYS_FLAG) | (MASK_VALID_RANGE | VALID_RANGE_FLAG)):                           // 1. weekdays + midnight time?
            case ((MASK_WEEKDAYS | WEEKDAYS_FLAG) | (MASK_TIME | TIME_FLAG)):                                         // 1. weekdays + exact time?
            case ((MASK_DAYS | DAYS_FLAG) | (MASK_VALID_RANGE | VALID_RANGE_FLAG)):                                   // 2. month_days + midnight time?
            case ((MASK_DAYS | DAYS_FLAG) | (MASK_TIME | TIME_FLAG)):                                                 // 2. month_days + exact time?
            case ((MASK_VALID_RANGE | VALID_RANGE_FLAG)):                                                             // 3. Daily + midnight time?
            case ((MASK_TIME) | (TIME_FLAG)):                                                                         // 3. Daily + exact time?
            {
                ret = 1; // Activate intime option
                break;
            }
            default:
            {
                ret = 0; // don't activate if above conditions aren't satisfied
                break;
            }
            }
        }
    }
    return ret;
}

//--------------------------- ezlopi_scene_when_is_once --------------------------------------------------------------
int ezlopi_scene_when_is_once(l_scenes_list_v2_t *scene_node, void *arg)
{
    int ret = 0;

    l_when_block_v2_t *when_block = (l_when_block_v2_t *)arg;
    if (when_block && scene_node)
    {
        // calculate the rawtime
        time_t rawtime = 0;
        time(&rawtime);
        struct tm *info;
        info = localtime(&rawtime);

        // temporary flags
        uint8_t flag_check = 0;
        const uint8_t TIME_FLAG = (1 << 0);
        const uint8_t DAY_FLAG = (1 << 1);
        const uint8_t MONTH_FLAG = (1 << 2);
        const uint8_t YEAR_FLAG = (1 << 3);

        l_fields_v2_t *curr_field = when_block->fields;
        while (curr_field)
        {
            if (0 == strncmp(curr_field->name, "time", 5))
            {
                if ((EZLOPI_VALUE_TYPE_24_HOURS_TIME == curr_field->value_type) && (NULL != curr_field->value.value_string))
                {
                    char field_hr_mm[10] = {0};
                    strftime(field_hr_mm, 10, "%H:%M", info);
                    field_hr_mm[10] = '\0';

                    if (0 == strncmp(curr_field->value.value_string, field_hr_mm, 10))
                    {
                        flag_check |= TIME_FLAG; // One of the TIME-condition has been met.
                    }
                }
                goto next;
            }
            else if (0 == strncmp(curr_field->name, "day", 4))
            {
                if (EZLOPI_VALUE_TYPE_INT == curr_field->value_type)
                {
                    if ((int)(curr_field->value.value_double) == info->tm_mday)
                    {
                        flag_check |= DAY_FLAG;
                    }
                }
                goto next;
            }
            else if (0 == strncmp(curr_field->name, "month", 6))
            {
                if (EZLOPI_VALUE_TYPE_INT == curr_field->value_type)
                {
                    if ((int)(curr_field->value.value_double) == (info->tm_mon + 1))
                    {
                        flag_check |= MONTH_FLAG;
                    }
                }
                goto next;
            }
            else if (0 == strncmp(curr_field->name, "year", 5))
            {
                if (EZLOPI_VALUE_TYPE_INT == curr_field->value_type)
                {
                    if ((int)(curr_field->value.value_double) == (info->tm_year + 1900))
                    {
                        flag_check |= YEAR_FLAG;
                    }
                }
                goto next;
            }
        next:
            curr_field = curr_field->next;
        }

        // Output Filter based on date & time
        TRACE_D("isOnce :- FLAG_STATUS: 0x0%x", flag_check);
        if ((flag_check & TIME_FLAG) && (flag_check & DAY_FLAG) && (flag_check & MONTH_FLAG) && (flag_check & YEAR_FLAG))
        {
            // now to disable the scene and also store in ezlopi_nvs
            if (57 == (int)(scene_node->when_block->fields->user_arg)++)
            {
                TRACE_W("here! once and time");
                scene_node->enabled = false;
                ezlopi_scenes_enable_disable_id_from_list_v2(scene_node->_id, false);
                ret = 1;
            }
            int _is_once_counter = (int)(scene_node->when_block->fields->user_arg);
            TRACE_B("user_arg = [%d]", _is_once_counter);
        }
        else
        {
            (scene_node->when_block->fields->user_arg) = 0;
        }
    }
    return ret;
}

//--------------------------- ezlopi_scene_when_is_date_range --------------------------------------------------------
static uint8_t __check_time_range(struct tm *start, struct tm *end, struct tm *info)
{
    uint8_t ret = 0;
    // first confirm if the time range has positive or negative difference (end - start)
    if ((end->tm_hour - start->tm_hour) > 0)
    {
        if ((info->tm_hour == start->tm_hour) && (info->tm_min >= start->tm_min))
        {
            ret |= (1 << 0);
            goto end;
        }
        else if ((info->tm_hour == end->tm_hour) && (info->tm_min <= end->tm_min))
        {
            ret |= (1 << 0);
            goto end;
        }
        else if ((info->tm_hour > start->tm_hour) && (info->tm_hour < end->tm_hour))
        {
            ret |= (1 << 0);
            goto end;
        }
    }
    else
    { // for inverted hours [ie. end(say): 2pm  / start(say): 5pm ; take 'mday' into consideration]
        if ((info->tm_mday >= start->tm_mday) && (info->tm_mday <= end->tm_mday))
        {
            if (((info->tm_hour >= start->tm_hour) && (info->tm_hour <= end->tm_hour)))
            {
                ret |= (1 << 0);
            }
        }
        else
        {
            TRACE_B("Invalid day-orders : start[%d] vs end[%d]", start->tm_mday, end->tm_mday);
        }
    }
end:
    return ret;
}
static uint8_t __check_day_range(struct tm *start, struct tm *end, struct tm *info)
{
    uint8_t ret = 0;
    if ((info->tm_mday >= start->tm_mday) && (info->tm_mday <= end->tm_mday))
    {
        ret |= (1 << 1);
    }
    else
    {
        TRACE_B("Invalid day-orders : start[%d] vs end[%d]", start->tm_mday, end->tm_mday);
    }
    return ret;
}
static uint8_t __check_month_range(struct tm *start, struct tm *end, struct tm *info)
{
    uint8_t ret = 0;
    if (((info->tm_mon + 1) >= start->tm_mon) && ((info->tm_mon + 1) <= end->tm_mon))
    {
        ret |= (1 << 2);
    }
    else
    {
        TRACE_B("Invalid month-orders : start[%d] vs end[%d]", start->tm_mon, end->tm_mon);
    }
    return ret;
}
static uint8_t __check_year_range(struct tm *start, struct tm *end, struct tm *info)
{
    uint8_t ret = 0;
    if (((info->tm_year + 1900) >= start->tm_year) && ((info->tm_year + 1900) <= end->tm_year))
    {
        ret |= (1 << 2);
    }
    else
    {
        TRACE_B("Invalid year-orders : start[%d] vs end[%d]", start->tm_year, end->tm_year);
    }
    return ret;
}

static uint8_t (*_is_date_range_func[])(struct tm *start, struct tm *end, struct tm *info) = {
    __check_time_range,
    __check_day_range,
    __check_month_range,
    __check_year_range,
};

int ezlopi_scene_when_is_date_range(l_scenes_list_v2_t *scene_node, void *arg)
{
    int ret = 0;
    l_when_block_v2_t *when_block = (l_when_block_v2_t *)arg;
    if (when_block && scene_node)
    {
        // temporary flags
        uint8_t flag_check = 0;
        const uint8_t TIME_FLAG = (1 << 0);  // HH:MM
        const uint8_t DAY_FLAG = (1 << 1);   // 1-31
        const uint8_t MONTH_FLAG = (1 << 2); // 1-12
        const uint8_t YEAR_FLAG = (1 << 3);  // 1-9999

        // Default values to store start and end boundries
        struct tm start = {
            .tm_hour = 0,
            .tm_min = 0,
            .tm_mday = 1,
            .tm_mon = 1,
            .tm_year = 1,
        };
        struct tm end = {
            .tm_hour = 23,
            .tm_min = 59,
            .tm_mday = 31,
            .tm_mon = 12,
            .tm_year = 9999,
        };

        // now to fill out required date
        l_fields_v2_t *curr_field = when_block->fields;
        while (curr_field)
        {
            if (0 == strncmp(curr_field->name, "startTime", 10))
            {
                if ((EZLOPI_VALUE_TYPE_24_HOURS_TIME == curr_field->value_type) && (NULL != curr_field->value.value_string))
                {
                    char startTime[10];
                    snprintf(startTime, 10, "%s", curr_field->value.value_string);
                    startTime[10] = '\0';
                    char *ptr = NULL;
                    if (0 != strlen(startTime))
                    {
                        start.tm_hour = strtoul(startTime, &ptr, 10);
                        start.tm_min = strtoul(ptr + 1, NULL, 10);
                    }
                }
                goto next;
            }
            else if (0 == strncmp(curr_field->name, "startDay", 9))
            {
                if (EZLOPI_VALUE_TYPE_INT == curr_field->value_type)
                {
                    start.tm_mday = ((curr_field->value.value_double)) ? ((int)(curr_field->value.value_double)) : 1;
                }
                goto next;
            }
            else if (0 == strncmp(curr_field->name, "startMonth", 11))
            {
                if (EZLOPI_VALUE_TYPE_INT == curr_field->value_type)
                {
                    start.tm_mon = ((curr_field->value.value_double)) ? ((int)(curr_field->value.value_double)) : 1;
                }
                goto next;
            }
            else if (0 == strncmp(curr_field->name, "startYear", 10))
            {
                if (EZLOPI_VALUE_TYPE_INT == curr_field->value_type)
                {
                    start.tm_year = ((int)(curr_field->value.value_double));
                }
                goto next;
            }
            else if (0 == strncmp(curr_field->name, "endTime", 8))
            {
                if ((EZLOPI_VALUE_TYPE_24_HOURS_TIME == curr_field->value_type) && (NULL != curr_field->value.value_string))
                {
                    char endTime[10];
                    snprintf(endTime, 10, "%s", curr_field->value.value_string);
                    endTime[10] = '\0';
                    char *ptr = NULL;
                    if (0 != strlen(endTime))
                    {
                        end.tm_hour = strtoul(endTime, &ptr, 10);
                        end.tm_min = strtoul(ptr + 1, NULL, 10);
                    }
                }
                goto next;
            }
            else if (0 == strncmp(curr_field->name, "endDay", 7))
            {
                if (EZLOPI_VALUE_TYPE_INT == curr_field->value_type)
                {
                    end.tm_mday = ((int)(curr_field->value.value_double));
                }
                goto next;
            }
            else if (0 == strncmp(curr_field->name, "endMonth", 9))
            {
                if (EZLOPI_VALUE_TYPE_INT == curr_field->value_type)
                {
                    end.tm_mon = ((int)(curr_field->value.value_double));
                }
                goto next;
            }
            else if (0 == strncmp(curr_field->name, "endYear", 8))
            {
                if (EZLOPI_VALUE_TYPE_INT == curr_field->value_type)
                {
                    end.tm_year = ((int)(curr_field->value.value_double));
                }
                goto next;
            }
        next:
            curr_field = curr_field->next;
        }

        // calculate the rawtime
        time_t rawtime = 0;
        time(&rawtime);
        struct tm *info;
        info = localtime(&rawtime);

        // 1. Check the time,day,month and year validity
        for (uint8_t i = 0; i < ISDATE_RANGE_MAX; i++)
        {
            flag_check |= _is_date_range_func[i](&start, &end, info);
        }

        // now compare the flag status
        TRACE_D("isdate_range flag_check [0x0%x]", flag_check);
        if ((flag_check & TIME_FLAG) && (flag_check & DAY_FLAG) && (flag_check & MONTH_FLAG) && (flag_check & YEAR_FLAG))
        {
            // increment the counter and store it in 'scence->field->user_arg'.
            if (57 == (int)(scene_node->when_block->fields->user_arg)++)
            {
                TRACE_W("here! isDate_range");
                ret = 1;
            }
            int _is_date_range_counter = (int)(scene_node->when_block->fields->user_arg);
            TRACE_B("user_arg = [%d]", _is_date_range_counter);
        }
        else
        {
            (scene_node->when_block->fields->user_arg) = 0;
        }
    }
    return ret;
}

//-------------------------------------------------------------------------------------------------------------------
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
