#include <string.h>
#include <time.h>

#include "trace.h"
#include "ezlopi_devices.h"
#include "ezlopi_scenes_v2.h"
#include "ezlopi_scenes_operators.h"
#include "ezlopi_scenes_when_methods.h"

typedef enum e_isdate_type_modes
{
    ISDATE_DAILY_MODE = 0,
    ISDATE_WEEKLY_MODE,
    ISDATE_MONTHLY_MODE,
    ISDATE_WEEKS_MODE,
    ISDATE_YEAR_WEEKS_MODE,
    ISDATE_UNDEFINED_MODE,
} e_isdate_modes_t;

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

int ezlopi_scene_when_is_sun_state(l_scenes_list_v2_t *scene_node, void *arg)
{
    TRACE_W("Warning: when-method 'is_sun_state' not implemented!");
    return 0;
}

//------------------------------- ezlopi_scene_when_is_date ---------------------------------------------------------------
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
        // TRACE_I("field_weekdays_check, arr_type[cJSON_Array:%d] ", cj_weekdays_arr->type);
        int field_weekdays = (info->tm_wday) + 1; // sunday => 0+1 ... saturday => 6+1

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
    // filter out the first 22 days ;
    // which (definately) do-not lie in last week of the month
    if (info->tm_mday > 22)
    {
        // 1. Find out the 'valid' starting date of last week ( return -1 , if invalid )
        if (-1 == _starting_date_of_last_week)
        {
            //-------------------------------------------------------------------
            // 1.1 find the nearest-prev sunday and assign it.
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
    // TRACE_I("[1-7] : %dth_Day  lies in week[%dth] of the current month", (info->tm_wday + 1), tmp_week_num);

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
    static uint8_t hhmm_daily_activation_count;   // half a minute mark counter for 'mode1'
    static uint8_t hhmm_weekly_activation_count;  // half a minute mark counter for 'mode2'
    static uint8_t hhmm_monthly_activation_count; // half a minute mark counter for 'mode3'
    static uint8_t hhmm_week_activation_count;    // half a minute mark counter for 'mode4'
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
        // TRACE_B("mode[%d], isDate:- FLAG_STATUS: %#x", mode_type, flag_check);
        // Output Filter based on date+time of activation
        switch (mode_type)
        {
        case ISDATE_DAILY_MODE:
        {
            TRACE_D("mode[%d], isDate:- FLAG_STATUS: %#x", mode_type, flag_check);
            if (((flag_check & MASK_FOR_TIME_ARG) && (flag_check & TIME_FLAG))) //&&(0 != strncmp(buffer, prev_date_time, 20)))
            {
                if (55 == hhmm_daily_activation_count++) // activate at the last second of 1 min
                {
                    TRACE_W("here! time");
                    hhmm_daily_activation_count = 0;
                    ret = 1;
                }
            }
            break;
        }
        case ISDATE_WEEKLY_MODE:
        {
            TRACE_D("mode[%d], isDate:- FLAG_STATUS: %#x", mode_type, flag_check);
            if ((((flag_check & MASK_FOR_TIME_ARG) && (flag_check & TIME_FLAG)) &&
                 ((flag_check & MASK_FOR_WEEKDAYS_ARG) && (flag_check & WEEKDAYS_FLAG))))
            {
                if (55 == hhmm_weekly_activation_count++) // activate at the last second of 1 min
                {
                    TRACE_W("here! week_days and time");
                    hhmm_weekly_activation_count = 0;
                    ret = 1;
                }
            }
            break;
        }
        case ISDATE_MONTHLY_MODE:
        {
            TRACE_D("mode[%d], isDate:- FLAG_STATUS: %#x", mode_type, flag_check);
            if ((((flag_check & MASK_FOR_TIME_ARG) && (flag_check & TIME_FLAG)) &&
                 ((flag_check & MASK_FOR_DAYS_ARG) && (flag_check & DAYS_FLAG))))
            {
                if (55 == hhmm_monthly_activation_count++) // activate at the last second of 1 min
                {
                    TRACE_W("here! mon_days and time");
                    hhmm_monthly_activation_count = 0;
                    ret = 1;
                }
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
                if (55 == hhmm_week_activation_count++) // activate at the last second of 1 min
                {
                    TRACE_W("here! week and time");
                    hhmm_week_activation_count = 0;
                    ret = 1;
                }
            }
            break;
        }
        default:
            break;
        }
    }
    return ret;
}

//--------------------------- ezlopi_scene_when_is_once --------------------------------------------------------------

int ezlopi_scene_when_is_once(l_scenes_list_v2_t *scene_node, void *arg)
{
    int ret = 0;
    static uint8_t hhmm_is_once_counter; // half a minute mark counter for 'mode1'
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
            }
            curr_field = curr_field->next;
        }

        // Output Filter based on date & time
        TRACE_D("isOnce :- FLAG_STATUS: 0x0%x", flag_check);
        if ((flag_check & TIME_FLAG) && (flag_check & DAY_FLAG) && (flag_check & MONTH_FLAG) && (flag_check & YEAR_FLAG))
        {
            // now to disable the scene and also store in ezlopi_nvs
            if (55 == hhmm_is_once_counter++) // activate at the last second of 1 min
            {
                TRACE_W("here! once and time");
                hhmm_is_once_counter = 0;
                scene_node->enabled = 0;
                // ezlopi_meshbot_service_stop_for_scene_id(scene_node->_id);
                // ezlopi_scenes_remove_id_from_list_v2(scene_node->_id);
                
                ret = 1;
            }
        }
    }
    return ret;
}

int ezlopi_scene_when_is_date_range(l_scenes_list_v2_t *scene_node, void *arg)
{
    int ret = 0;
    static uint8_t hhmm_is_once_counter; // half a minute mark counter for 'mode1'
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
        const uint8_t TIME_FLAG = (1 << 0); // HH:MM
        const uint8_t DAY_FLAG = (1 << 1);  // 
        const uint8_t MONTH_FLAG = (1 << 2);
        const uint8_t YEAR_FLAG = (1 << 3);

        // Default values to store
        struct tm start;
        struct tm end;

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
                    else
                    {
                        start.tm_hour = 0;
                        start.tm_min = 0;
                    }
                }
            }
            else if (0 == strncmp(curr_field->name, "startDay", 9))
            {
                if (EZLOPI_VALUE_TYPE_INT == curr_field->value_type)
                {
                    start.tm_yday = ((int)(curr_field->value.value_double));
                }
            }
            else if (0 == strncmp(curr_field->name, "startMonth", 11))
            {
                if (EZLOPI_VALUE_TYPE_INT == curr_field->value_type)
                {
                    start.tm_mon = ((int)(curr_field->value.value_double));
                }
            }
            else if (0 == strncmp(curr_field->name, "startYear", 10))
            {
                if (EZLOPI_VALUE_TYPE_INT == curr_field->value_type)
                {
                    start.tm_year = ((int)(curr_field->value.value_double));
                }
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
                    else
                    {
                        end.tm_hour = 23;
                        end.tm_min = 59;
                    }
                }
            }
            else if (0 == strncmp(curr_field->name, "endDay", 7))
            {
                if (EZLOPI_VALUE_TYPE_INT == curr_field->value_type)
                {
                    end.tm_yday = ((int)(curr_field->value.value_double));
                }
            }
            else if (0 == strncmp(curr_field->name, "endMonth", 9))
            {
                if (EZLOPI_VALUE_TYPE_INT == curr_field->value_type)
                {
                    end.tm_mon = ((int)(curr_field->value.value_double));
                }
            }
            else if (0 == strncmp(curr_field->name, "endYear", 8))
            {
                if (EZLOPI_VALUE_TYPE_INT == curr_field->value_type)
                {
                    end.tm_year = ((int)(curr_field->value.value_double));
                }
            }

            curr_field = curr_field->next;
        }

        // calculate the rawtime
        time_t rawtime = 0;
        time(&rawtime);
        struct tm *info;
        info = localtime(&rawtime);

        if (info->tm_year <=)
            ;
    }
    return ret;
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
