#include <time.h>
#include "ezlopi_util_trace.h"

#include "ezlopi_core_ota.h"
#include "ezlopi_core_http.h"
#include "ezlopi_core_devices.h"
#include "ezlopi_core_event_group.h"
#include "ezlopi_core_scenes_operators.h"
#include "ezlopi_core_websocket_client.h"
#include "ezlopi_core_scenes_when_methods.h"
#include "ezlopi_core_scenes_when_methods_helper_functions.h"

#include "ezlopi_cloud_constants.h"

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
            else if (0 == strncmp(curr_field->name, ezlopi_value_str, 4))
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
                            cJSON *cj_value = cJSON_GetObjectItem(cj_tmp_value, ezlopi_value_str);
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
                scene_node->when_block->fields->user_arg = (void *)xTaskGetTickCount();
                ret = 1;
            }
        }
        else
        {
            ret = 1;
            scene_node->when_block->fields->user_arg = (void *)xTaskGetTickCount();
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
    TRACE_W("Warning: when-method 'is_button_state' not implemented!");
    return 0;
}

int ezlopi_scene_when_is_sun_state(l_scenes_list_v2_t *scene_node, void *arg)
{
    TRACE_I("IsSunState triggered");
    int ret = 0;
    l_when_block_v2_t *when_block = (l_when_block_v2_t *)arg;
    if (when_block && scene_node)
    {
        static struct tm sunrise_time = {0}; // (must be static) store the sunrise_time
        static struct tm sunset_time = {0};  // (must be static) store the sunset_time
        struct tm defined_moment = {0};      // Defined_moment (after combining : 'sunrise/sunset_time' & 'time_offset')

        e_issunstate_offset_type_t sunstate_offset = ISSUNSTATE_UNDEFINED; // what type of offset to use
        l_fields_v2_t *curr_field = when_block->fields;

        time_t rawtime = 0;
        time(&rawtime);
        struct tm *info;
        info = localtime(&rawtime);

        // temporary flags
        uint8_t flag_check = 0;
        const uint8_t TIME_FLAG = (1 << 0);
        const uint8_t WEEKDAYS_FLAG = (1 << 1);
        const uint8_t DAYS_FLAG = (1 << 2);
        const uint8_t MIDNIGHT_FLAG = (1 << 3);
        const uint8_t MASK_TIME_FLAG = (1 << 4);
        const uint8_t MASK_WEEKDAYS_FLAG = (1 << 5);
        const uint8_t MASK_DAYS_FLAG = (1 << 6);
        const uint8_t MASK_MIDNIGHT_FLAG = (1 << 7);

        // Condition checker
        uint8_t sunstate_mode = 0;
        while (NULL != curr_field)
        {
            if ((0 == strncmp(curr_field->name, "sunrise", 8)) || (0 == strncmp(curr_field->name, "sunset", 7))) // first identify 'sunstate'
            {
                sunstate_mode = (0 == strncmp(curr_field->name, "sunrise", 8)) ? 1 : (0 == strncmp(curr_field->name, "sunset", 7) ? 2 : 0);
                if ((EZLOPI_VALUE_TYPE_STRING == curr_field->value_type) && (NULL != curr_field->value.value_string))
                {
                    flag_check |= MASK_TIME_FLAG; // indicates time has been set
                    // if (0 >= ezlopi_event_group_wait_for_event(EZLOPI_EVENT_WIFI_CONNECTED, 100, false))
                    // {
                    //     TRACE_I("Waiting for wifi connection ..");
                    //     return 0;
                    // }
                    if (info->tm_mday != (uint32_t)scene_node->when_block->fields->user_arg)
                    {
                        scene_node->when_block->fields->user_arg = (void *)(info->tm_mday);
                        issunsate_update_sunstate_tm(info->tm_mday, &sunrise_time, &sunset_time); // assign today's sunrise & sunset time
                        if ((0 == sunrise_time.tm_mday) || (0 == sunset_time.tm_mday))
                        {
                            TRACE_I("............Erasing..& Waiting for wifi connection");
                            scene_node->when_block->fields->user_arg = 0; // reset the day
                            sunrise_time.tm_mday = sunset_time.tm_mday = 0;
                            return 0;
                        }
                    }

                    defined_moment = (1 == sunstate_mode) ? sunrise_time : ((2 == sunstate_mode) ? sunset_time : defined_moment);
                    sunstate_offset = issunstate_offset_type(curr_field->value.value_string);

                    TRACE_D("update_day = [%dth] , sunrise/sunset_hour = [%d] ", (uint32_t)scene_node->when_block->fields->user_arg, defined_moment.tm_hour);
                }
            }
            else if ((0 == strncmp(curr_field->name, "time", 5))) // string
            {
                if (EZLOPI_VALUE_TYPE_HMS_INTERVAL == curr_field->value_type && (NULL != curr_field->value.value_string))
                {
                    if (0 != sunstate_mode)
                    {
                        issunstate_add_tm_offset(sunstate_offset, ((1 == sunstate_mode) ? &sunrise_time : &sunset_time), &defined_moment, curr_field->value.value_string);
                    }
                }
            }
            else if ((0 == strncmp(curr_field->name, "weekdays", 5))) // weekdays // int_array
            {
                if ((EZLOPI_VALUE_TYPE_INT_ARRAY == curr_field->value_type) && (cJSON_IsArray(curr_field->value.cj_value)))
                {
                    flag_check |= MASK_WEEKDAYS_FLAG;                                                             // indicates : weekdays
                    flag_check |= isdate_weekdays_check(ISDATE_UNDEFINED_MODE, info, curr_field->value.cj_value); // (1 << 1)
                }
            }
            else if ((0 == strncmp(curr_field->name, "days", 5))) // monthdays // int_array
            {
                if ((EZLOPI_VALUE_TYPE_INT_ARRAY == curr_field->value_type) && (cJSON_IsArray(curr_field->value.cj_value)))
                {
                    flag_check |= MASK_DAYS_FLAG;                                                              // indicates : month_days
                    flag_check |= isdate_mdays_check(ISDATE_UNDEFINED_MODE, info, curr_field->value.cj_value); // (1 << 2)
                }
            }
            else if ((0 == strncmp(curr_field->name, "range", 5))) // till midnight
            {
                if (EZLOPI_VALUE_TYPE_STRING == curr_field->value_type && (NULL != curr_field->value.value_string))
                {
                    flag_check |= MASK_MIDNIGHT_FLAG; // indicates : midnight-range
                    flag_check |= issunstate_midnight_check(sunstate_mode, curr_field->value.value_string, info, &defined_moment);
                }
            }

            curr_field = curr_field->next;
        }

        if ((0 != defined_moment.tm_mday) && (0 != sunstate_mode) && (flag_check & MASK_TIME_FLAG)) // defined_moment should have the current day
        {
            // Check Time_flag status : [sunrise/sunset_moment + offsets (if any)]
            if ((info->tm_hour == defined_moment.tm_hour) &&
                (info->tm_min == defined_moment.tm_min) &&
                (info->tm_sec == defined_moment.tm_sec))
            {
                flag_check |= TIME_FLAG;
            }

            if (!(flag_check & MASK_DAYS_FLAG) && !(flag_check & MASK_WEEKDAYS_FLAG)) // without 'month_days' or 'week_days' condition
            {
                switch (flag_check)
                {
                case (MASK_MIDNIGHT_FLAG | MIDNIGHT_FLAG): // daily + midnight time?
                case (TIME_FLAG):                          // daily + exact time?
                {
                    ret = 1;
                    break;
                }
                default:
                {
                    ret = 0;
                    break;
                }
                }
            }
            else // (flag_check & MASK_DAYS_FLAG) || (flag_check & MASK_WEEKDAYS_FLAG) // month_days + week_days conditons
            {
                // Output filter for triggered in sunset/sunrise time
                switch (flag_check)
                {
                case ((MASK_DAYS_FLAG | DAYS_FLAG) | (MASK_WEEKDAYS_FLAG | WEEKDAYS_FLAG) | (MASK_MIDNIGHT_FLAG | MIDNIGHT_FLAG)): // mdays+weekday+midnight
                case ((MASK_DAYS_FLAG | DAYS_FLAG) | (MASK_WEEKDAYS_FLAG | WEEKDAYS_FLAG) | (TIME_FLAG)):                          // mdays+weekday+exact
                case ((MASK_DAYS_FLAG | DAYS_FLAG) | (MASK_MIDNIGHT_FLAG | MIDNIGHT_FLAG)):                                        // mdays+midnight
                case ((MASK_DAYS_FLAG | DAYS_FLAG) | (TIME_FLAG)):                                                                 // mdays +exact
                case ((MASK_WEEKDAYS_FLAG | WEEKDAYS_FLAG) | (MASK_MIDNIGHT_FLAG | MIDNIGHT_FLAG)):                                // weekday+midnight
                case ((MASK_WEEKDAYS_FLAG | WEEKDAYS_FLAG) | (TIME_FLAG)):                                                         // weekday+exact
                {
                    ret = 1;
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
        TRACE_I("offset[%d] : intime=0,before=1,after=2 , SunState => [%d] : sunrise=1,sunset=2  :- FLAG_STATUS: %#x", sunstate_offset, sunstate_mode, flag_check);
    }
    return ret;
}

int ezlopi_scene_when_is_date(l_scenes_list_v2_t *scene_node, void *arg)
{
    int ret = 0;
    l_when_block_v2_t *when_block = (l_when_block_v2_t *)arg;
    if (when_block)
    {
        e_isdate_modes_t mode_type = ISDATE_UNDEFINED_MODE;
        l_fields_v2_t *curr_field = when_block->fields;

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

        const s_isdate_func_t __isdate_func[] = {
            {.field_name = "time", .field_func = isdate_tm_check},           // func process ["hh:mm"]
            {.field_name = "weekdays", .field_func = isdate_weekdays_check}, // func process days in a week [1-7]
            {.field_name = "days", .field_func = isdate_mdays_check},        //
            {.field_name = "weeks", .field_func = isdate_year_weeks_check},
            {.field_name = NULL, .field_func = NULL},
        };

        // Condition checker
        while (NULL != curr_field)
        {
            if (0 == strncmp(curr_field->name, "type", 5))
            {
                if ((EZLOPI_VALUE_TYPE_STRING == curr_field->value_type) && (NULL != curr_field->value.value_string))
                {
                    mode_type = isdate_field_type_check(curr_field->value.value_string);
                }
            }
            else
            {
                for (uint8_t i = 0; i < ((sizeof(__isdate_func) / sizeof(__isdate_func[i]))); i++)
                {
                    if (0 == strncmp(__isdate_func[i].field_name, curr_field->name, strlen(curr_field->name) + 1))
                    {
                        flag_check |= (1 << (i + 4));                                                             // bit4 - bit7
                        flag_check |= (__isdate_func[i].field_func)(mode_type, info, curr_field->value.cj_value); // bit0 - bit3
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
            if (((flag_check & MASK_FOR_TIME_ARG) && (flag_check & TIME_FLAG)))
            {
                if (57 == (uint32_t)(scene_node->when_block->fields->user_arg)++) // 57 sec mark
                {
                    // TRACE_W("here! daily-time");
                    ret = 1;
                }
            }
            else
            {
                (scene_node->when_block->fields->user_arg) = 0;
            }
            break;
        }
        case ISDATE_WEEKLY_MODE:
        {
            if ((((flag_check & MASK_FOR_TIME_ARG) && (flag_check & TIME_FLAG)) &&
                 ((flag_check & MASK_FOR_WEEKDAYS_ARG) && (flag_check & WEEKDAYS_FLAG))))
            {
                if (57 == (uint32_t)(scene_node->when_block->fields->user_arg)++) // 57 sec mark
                {
                    // TRACE_W("here! week_days and time");
                    ret = 1;
                }
            }
            else
            {
                (scene_node->when_block->fields->user_arg) = 0;
            }
            break;
        }
        case ISDATE_MONTHLY_MODE:
        {
            if ((((flag_check & MASK_FOR_TIME_ARG) && (flag_check & TIME_FLAG)) &&
                 ((flag_check & MASK_FOR_DAYS_ARG) && (flag_check & DAYS_FLAG))))
            {
                if (57 == (uint32_t)(scene_node->when_block->fields->user_arg)++) // 57 sec mark
                {
                    // TRACE_W("here! month_days and time");
                    ret = 1;
                }
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
            if (((flag_check & MASK_FOR_TIME_ARG) && (flag_check & TIME_FLAG)) &&
                ((flag_check & MASK_FOR_WEEKS_ARG) && (flag_check & WEEKS_FLAG)))
            {
                if (57 == (uint32_t)(scene_node->when_block->fields->user_arg)++) // 57 sec mark
                {
                    // TRACE_W("here! week and time");
                    ret = 1;
                }
            }
            else
            {
                (scene_node->when_block->fields->user_arg) = 0;
            }
            break;
        }
        default:
        {
            break;
        }
        }
        TRACE_D("mode[%d], isDate:- FLAG_STATUS: %#x", mode_type, flag_check);
        TRACE_B("user_arg = [%d]", (uint32_t)(scene_node->when_block->fields->user_arg));
    }
    return ret;
}

int ezlopi_scene_when_is_once(l_scenes_list_v2_t *scene_node, void *arg)
{
    int ret = 0;

    l_when_block_v2_t *when_block = (l_when_block_v2_t *)arg;
    if (when_block && scene_node)
    {
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
            if (57 == (uint32_t)(scene_node->when_block->fields->user_arg)++)
            {
                // TRACE_W("here! once and time");
                scene_node->enabled = false;
                ezlopi_scenes_enable_disable_id_from_list_v2(scene_node->_id, false);
                ret = 1;
            }
            TRACE_B("user_arg = [%d]", (uint32_t)(scene_node->when_block->fields->user_arg));
        }
        else
        {
            (scene_node->when_block->fields->user_arg) = 0;
        }
    }
    return ret;
}

int ezlopi_scene_when_is_date_range(l_scenes_list_v2_t *scene_node, void *arg)
{
    int ret = 0;
    l_when_block_v2_t *when_block = (l_when_block_v2_t *)arg;
    if (when_block && scene_node)
    {
        // temporary flags
        uint8_t flag_check = 0;
        const uint8_t TIME_FLAG = (1 << 0);
        const uint8_t DAY_FLAG = (1 << 1);
        const uint8_t MONTH_FLAG = (1 << 2);
        const uint8_t YEAR_FLAG = (1 << 3);

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

        uint8_t (*_isdate_range_func[])(struct tm *start, struct tm *end, struct tm *info) = {
            isdate_range_check_tm,
            isdate_range_check_day,
            isdate_range_check_month,
            isdate_range_check_year,
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

        time_t rawtime = 0;
        time(&rawtime);
        struct tm *info;
        info = localtime(&rawtime);

        // 1. Check the time,day,month and year validity
        for (uint8_t i = 0; i < ISDATE_RANGE_MAX; i++)
        {
            flag_check |= _isdate_range_func[i](&start, &end, info);
        }

        TRACE_D("isdate_range flag_check [0x0%x]", flag_check);
        if ((flag_check & TIME_FLAG) && (flag_check & DAY_FLAG) && (flag_check & MONTH_FLAG) && (flag_check & YEAR_FLAG))
        {
            if (57 == (uint32_t)(scene_node->when_block->fields->user_arg)++)
            {
                // TRACE_W("here! isDate_range");
                ret = 1;
            }
            TRACE_B("user_arg = [%d]", (uint32_t)(scene_node->when_block->fields->user_arg));
        }
        else
        {
            (scene_node->when_block->fields->user_arg) = 0;
        }
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
    TRACE_I("isDevice_state.");
    int ret = 0;
    l_when_block_v2_t *when_block = (l_when_block_v2_t *)arg;
    if (when_block && scene_node)
    {
        uint32_t device_id = 0;
        bool value_armed = false;
        bool value_reachable = false;

        l_fields_v2_t *curr_field = when_block->fields;
        while (curr_field)
        {
            if (0 == strncmp(curr_field->name, "device", 7))
            {
                device_id = strtoul(curr_field->value.value_string, NULL, 16);
            }
            else if (0 == strncmp(curr_field->name, "armed", 6))
            {
                if (EZLOPI_VALUE_TYPE_BOOL == curr_field->value_type)
                {
                    value_armed = curr_field->value.value_bool;
                }
            }
            else if (0 == strncmp(curr_field->name, "reachable", 10))
            {
                if (EZLOPI_VALUE_TYPE_BOOL == curr_field->value_type)
                {
                    value_reachable = curr_field->value.value_bool;
                }
            }
            curr_field = curr_field->next;
        }
        if (device_id)
        {
            l_ezlopi_device_t *curr_device = ezlopi_device_get_head();
            while (curr_device)
            {
                if (device_id == curr_device->cloud_properties.device_id)
                {
                    s_ezlopi_cloud_controller_t *controller_info = ezlopi_device_get_controller_information();
                    if (controller_info)
                    {
                        /* we need to check device_id specific*/
                        // This is controller specific
                        ret = ((value_armed == controller_info->armed) ? 1 : 0);
                        ret = ((value_reachable == controller_info->service_notification) ? 1 : 0);
                    }
                }

                curr_device = curr_device->next;
            }
        }
#warning "need to check device_group condition"
    }

    return ret;
}

int ezlopi_scene_when_is_network_state(l_scenes_list_v2_t *scene_node, void *arg)
{
    TRACE_W("Warning: when-method 'is_network_state' not implemented!");
    return 0;
}

int ezlopi_scene_when_is_scene_state(l_scenes_list_v2_t *scene_node, void *arg)
{
    int ret = 0;
    l_when_block_v2_t *when_block = (l_when_block_v2_t *)arg;
    if (when_block && scene_node)
    {
        uint32_t scene_id = 0;
        l_fields_v2_t *value_field = NULL;

        l_fields_v2_t *curr_field = when_block->fields;
        while (curr_field)
        {
            if (0 == strncmp(curr_field->name, "scene", 6))
            {
                scene_id = strtoul(curr_field->value.value_string, NULL, 16);
            }
            else if (0 == strncmp(curr_field->name, "state", 6))
            {
                if (EZLOPI_VALUE_TYPE_TOKEN == curr_field->value_type)
                {
                    value_field = curr_field;
                }
            }
            curr_field = curr_field->next;
        }

        if (scene_id && value_field)
        {
            l_scenes_list_v2_t *curr_scene = ezlopi_scenes_get_scenes_head_v2();
            while (curr_scene)
            {
                if (curr_scene->_id == scene_id)
                {
                    if (0 == strncmp("any_result", value_field->value.value_string, 11))
                    {
                        ret = 1;
                    }
                    else if (0 == strncmp("scene_enabled", value_field->value.value_string, 14))
                    {
                        ret = (true == curr_scene->enabled);
                    }
                    else if (0 == strncmp("scene_disabled", value_field->value.value_string, 14))
                    {
                        ret = (false == curr_scene->enabled);
                    }
                    else if (0 == strncmp("finished", value_field->value.value_string, 9))
                    {
                        ret = (EZLOPI_SCENE_STATUS_STOP == curr_scene->status);
                    }
                    else if (0 == strncmp("partially_finished", value_field->value.value_string, 19))
                    {
                        ret = (EZLOPI_SCENE_STATUS_RUNNING == curr_scene->status);
                    }
                    else if (0 == strncmp("stopped", value_field->value.value_string, 8))
                    {
                        ret = (EZLOPI_SCENE_STATUS_STOPPED == curr_scene->status);
                    }
#warning "need to add 'FAILED' status for scene"
                    // else if (0 == strncmp("failed", value_field->value.value_string, 7))
                    // {
                    //     ret = (false == curr_scene->enabled);
                    // }
                    break;
                }
                curr_scene = curr_scene->next;
            }
        }
    }

    return ret;
}

int ezlopi_scene_when_is_group_state(l_scenes_list_v2_t *scene_node, void *arg)
{
    TRACE_W("Warning: when-method 'is_group_state' not implemented!");
    return 0;
}

int ezlopi_scene_when_is_cloud_state(l_scenes_list_v2_t *scene_node, void *arg)
{
    TRACE_I("isCloud_state.");
    int ret = 0;
    l_when_block_v2_t *when_block = (l_when_block_v2_t *)arg;
    if (when_block && scene_node)
    {
        l_fields_v2_t *value_field = NULL;
        l_fields_v2_t *curr_field = when_block->fields;
        while (curr_field)
        {
            if (0 == strncmp(curr_field->name, "state", 6))
            {
                if (EZLOPI_VALUE_TYPE_TOKEN == curr_field->value_type && (NULL != curr_field->value.value_string))
                {
                    value_field = curr_field;
                }
            }
            curr_field = curr_field->next;
        }

        if (value_field)
        {
            if (ezlopi_websocket_client_is_connected())
            {
                ret = (0 == strncmp(value_field->value.value_string, "connected", 10));
            }
            else
            {
                ret = (0 == strncmp(value_field->value.value_string, "disconnected", 14));
            }
        }
    }

    return ret;
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
            else if (0 == strncmp(curr_field->name, ezlopi_value_str, 4))
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
    int ret = 0;
    l_when_block_v2_t *when_block = (l_when_block_v2_t *)arg;

    if (when_block && scene_node)
    {
        uint32_t item_id = 0;

        l_fields_v2_t *start_value_field = NULL;
        l_fields_v2_t *end_value_field = NULL;

        l_fields_v2_t *comparator_field = NULL;

        l_fields_v2_t *curr_field = when_block->fields;
        while (curr_field)
        {
            if (0 == strncmp(curr_field->name, "item", 5))
            {
                item_id = strtoul(curr_field->value.value_string, NULL, 16);
            }
            else if (0 == strncmp(curr_field->name, "startValue", 11))
            {
                start_value_field = curr_field;
            }
            else if (0 == strncmp(curr_field->name, "endValue", 9))
            {
                comparator_field = curr_field;
            }
            else if (0 == strncmp(curr_field->name, "comparator", 11))
            {
                comparator_field = curr_field;
            }
            curr_field = curr_field->next;
        }

        if (item_id && start_value_field && end_value_field)
        {
            // check if both 'value_type' and 'scales' match.
            if ((start_value_field->value_type == end_value_field->value_type) &&
                (0 == strcmp(start_value_field->scale, end_value_field->scale)))
            {
                ret = ezlopi_scenes_operators_value_number_range_operations(item_id, start_value_field, end_value_field, comparator_field);
            }
        }
    }
    return ret;
}

int ezlopi_scene_when_compare_strings(l_scenes_list_v2_t *scene_node, void *arg)
{
    int ret = 0;
    l_when_block_v2_t *when_block = (l_when_block_v2_t *)arg;
    if (when_block && scene_node)
    {
        uint32_t item_id = 0;
        // l_fields_v2_t *expression_field = NULL;
        l_fields_v2_t *value_field = NULL;
        l_fields_v2_t *comparator_field = NULL;

        l_fields_v2_t *curr_field = when_block->fields;
        while (curr_field)
        {
            if (0 == strncmp(curr_field->name, "item", 5))
            {
                item_id = strtoul(curr_field->value.value_string, NULL, 16); // item or expression_id
            }
            else if (0 == strncmp(curr_field->name, "comparator", 11))
            {
                comparator_field = curr_field;
            }
            else if (0 == strncmp(curr_field->name, ezlopi_value_str, 6))
            {
                if (EZLOPI_VALUE_TYPE_STRING == curr_field->value_type && NULL != curr_field->value.value_string)
                {

                    value_field = curr_field; // this field has string
                }
                // else if (EZLOPI_VALUE_TYPE_EXPRESSION == curr_field->value_type && NULL != curr_field->value.value_string)
                // {
                //     expression_field = curr_field; // this field has expression_name
                // }
            }
            curr_field = curr_field->next;
        }

        if (item_id && value_field && comparator_field) // only for item_value 'string comparisions'
        {
            ret = ezlopi_scenes_operators_value_strings_operations(item_id, value_field, comparator_field);
        }
        // else if (item_id && expression_field && comparator_field) // only for expression 'string comparisions'
        // {
        //     // ret = ezlopi_scenes_operators_value_expn_strings_operations(item_id, value_field, comparator_field);
        // }
    }

    return ret;
}

int ezlopi_scene_when_string_operation(l_scenes_list_v2_t *scene_node, void *arg)
{
    int ret = 0;
    l_when_block_v2_t *when_block = (l_when_block_v2_t *)arg;
    if (when_block && scene_node)
    {
        uint32_t item_id = 0;
        // l_fields_v2_t *expression_field = NULL;
        l_fields_v2_t *value_field = NULL;
        l_fields_v2_t *comparator_field = NULL;

        l_fields_v2_t *curr_field = when_block->fields;
        while (curr_field)
        {
            if (0 == strncmp(curr_field->name, "item", 5))
            {
                item_id = strtoul(curr_field->value.value_string, NULL, 16); // ID extraction [item or expression]
            }
            else if (0 == strncmp(curr_field->name, "operation", 11))
            {
                comparator_field = curr_field;
            }
            else if (0 == strncmp(curr_field->name, ezlopi_value_str, 6))
            {
                if (EZLOPI_VALUE_TYPE_STRING == curr_field->value_type && NULL != curr_field->value.value_string)
                {
                    value_field = curr_field; // this field has string
                }
                else if (EZLOPI_VALUE_TYPE_INT == curr_field->value_type)
                {
                    value_field = curr_field; // this field has double/int value
                }
                // else if (EZLOPI_VALUE_TYPE_EXPRESSION == curr_field->value_type && NULL != curr_field->value.value_string)
                // {
                //     expression_field = curr_field; // this field has expression_name
                // }
            }
            curr_field = curr_field->next;
        }

        if (item_id && value_field && comparator_field) // only for item_value 'string comparisions'
        {
            ret = ezlopi_scenes_operators_value_strings_operations(item_id, value_field, comparator_field);
        }
        // else if (item_id && expression_field && comparator_field) // only for expression 'string comparisions'
        // {
        //     // ret = ezlopi_scenes_operators_value_expn_strings_operations(item_id, value_field, comparator_field);
        // }
    }

    return ret;
}

int ezlopi_scene_when_in_array(l_scenes_list_v2_t *scene_node, void *arg)
{
    int ret = 0;
    l_when_block_v2_t *when_block = (l_when_block_v2_t *)arg;
    if (when_block && scene_node)
    {
        uint32_t item_id = 0;
        // l_fields_v2_t *expression_field = NULL;
        l_fields_v2_t *value_field = NULL;
        l_fields_v2_t *comparator_field = NULL;

        l_fields_v2_t *curr_field = when_block->fields;
        while (curr_field)
        {
            if (0 == strncmp(curr_field->name, "item", 5))
            {
                item_id = strtoul(curr_field->value.value_string, NULL, 16); // ID extraction [item or expression]
            }
            else if (0 == strncmp(curr_field->name, "operation", 11))
            {
                comparator_field = curr_field;
            }
            else if (0 == strncmp(curr_field->name, ezlopi_value_str, 6))
            {
                if (EZLOPI_VALUE_TYPE_ARRAY == curr_field->value_type && (cJSON_IsArray(curr_field->value.cj_value)))
                {
                    value_field = curr_field; // this field has double/int value
                }
            }
            curr_field = curr_field->next;
        }

        if (item_id && value_field) // only for item_value 'string comparisions'
        {
            ret = ezlopi_scenes_operators_value_inarr_operations(item_id, value_field, comparator_field);
        }
        // else if (item_id && expression_field && comparator_field) // only for expression 'string comparisions'
        // {
        // ret = ezlopi_scenes_operators_value_expn_inarr_operations(item_id, value_field, comparator_field);
        // }
    }

    return ret;
}

int ezlopi_scene_when_compare_values(l_scenes_list_v2_t *scene_node, void *arg)
{
    int ret = 0;
    l_when_block_v2_t *when_block = (l_when_block_v2_t *)arg;
    if (when_block && scene_node)
    {
        uint32_t item_id = 0;
        l_fields_v2_t *value_type_field = NULL;
        l_fields_v2_t *value_field = NULL;
        // l_fields_v2_t *expression_field = NULL;
        l_fields_v2_t *comparator_field = NULL;

        // e_scene_value_type_v2_t value_type = EZLOPI_VALUE_TYPE_NONE;

        l_fields_v2_t *curr_field = when_block->fields;
        while (curr_field)
        {
            if (0 == strncmp(curr_field->name, "item", 5))
            {
                item_id = strtoul(curr_field->value.value_string, NULL, 16); // ID extraction [item or expression]
            }

            else if (0 == strncmp(curr_field->name, ezlopi_value_type_str, 11))
            {
                if (EZLOPI_VALUE_TYPE_STRING == curr_field->value_type && (NULL != (curr_field->value.value_string)))
                {
                    value_type_field = curr_field; // this field has double/int value
                }
            }
            else if (0 == strncmp(curr_field->name, ezlopi_value_str, 6))
            {
                if (EZLOPI_VALUE_TYPE_STRING == curr_field->value_type && (NULL != (curr_field->value.value_string)))
                {
                    value_field = curr_field; // this field has double/int value
                }
            }
            curr_field = curr_field->next;
        }

        if (item_id && value_field && value_type_field) // only for item_value 'string comparisions'
        {
            ret = ezlopi_scenes_operators_value_comparevalues_with_less_operations(item_id, value_field, value_type_field, comparator_field);
        }
        // else if (item_id && expression_field ) // only for expression 'string comparisions'
        // {
        // ret = ezlopi_scenes_operators_expn_comparevalues_with_less_operations(item_id, value_field, comparator_field);
        // }
    }
    return ret;
}

int ezlopi_scene_when_has_atleast_one_dictionary_value(l_scenes_list_v2_t *scene_node, void *arg)
{
    int ret = 0;
    l_when_block_v2_t *when_block = (l_when_block_v2_t *)arg;
    if (scene_node && when_block)
    {
        uint32_t item_id = 0;
        l_fields_v2_t *value_field = NULL;

        l_fields_v2_t *curr_field = when_block->fields;
        while (curr_field)
        {
            if (0 == strncmp(curr_field->name, "item", 5))
            {
                item_id = strtoul(curr_field->value.value_string, NULL, 16);
            }
            else if (0 == strncmp(curr_field->name, ezlopi_value_str, 6))
            {
                value_field = curr_field; // this contains "options [array]" & 'value': to be checked
            }
            curr_field = curr_field->next;
        }

        // now to extract the
        if (item_id && value_field)
        {
            ret = ezlopi_scenes_operators_has_atleastone_dictionary_value_operations(item_id, value_field);
        }
    }

    return ret;
}

int ezlopi_scene_when_is_firmware_update_state(l_scenes_list_v2_t *scene_node, void *arg)
{
    int ret = 0;
    l_when_block_v2_t *when_block = (l_when_block_v2_t *)arg;
    if (scene_node && when_block)
    {
        uint32_t item_id = 0;
        char *state_value = NULL;

        l_fields_v2_t *curr_field = when_block->fields;
        while (curr_field)
        {
            if (0 == strncmp(curr_field->name, "state", 6))
            {
                if (EZLOPI_VALUE_TYPE_TOKEN == curr_field->value_type)
                {
                    state_value = curr_field->value.value_string; // started / updating / done
                }
            }
            curr_field = curr_field->next;
        }

        // now to extract the
        if (item_id && (NULL != state_value))
        {
            if (0 == strncmp("done", state_value, 5) && (0 == __get_ota_state()))
            {
                ret = 1;
            }
            else if (0 == strncmp("started", state_value, 8) && (1 == __get_ota_state()))
            {
                ret = 1;
            }
            else if (0 == strncmp("updating", state_value, 9) && (2 == __get_ota_state()))
            {
                ret = 1;
            }
        }
    }
    return ret;
}

int ezlopi_scene_when_is_dictionary_changed(l_scenes_list_v2_t *scene_node, void *arg)
{
    int ret = 0;
    l_when_block_v2_t *when_block = (l_when_block_v2_t *)arg;
    if (scene_node && when_block)
    {
        uint32_t item_id = 0;
        l_fields_v2_t *key_field = NULL;
        l_fields_v2_t *operation_field = NULL;

        l_fields_v2_t *curr_field = when_block->fields;
        while (curr_field)
        {
            if (0 == strncmp(curr_field->name, "item", 5))
            {
                item_id = strtoul(curr_field->value.value_string, NULL, 16);
            }
            else if (0 == strncmp(curr_field->name, "key", 4))
            {
                key_field = curr_field; // this contains "options [array]" & 'value': to be checked
            }
            else if (0 == strncmp(curr_field->name, "operation", 10))
            {
                operation_field = curr_field; // this contains "options [array]" & 'value': to be checked
            }
            curr_field = curr_field->next;
        }

        if (item_id && key_field && operation_field)
        {
            ret = ezlopi_scenes_operators_is_dictionary_changed_operations(scene_node, item_id, key_field, operation_field);
        }
    }
    return ret;
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
    int ret = 0;
    l_when_block_v2_t *when_block = (l_when_block_v2_t *)arg;
    if (when_block)
    {
        ret = 1; // required for the first case
        l_when_block_v2_t *value_when_block = when_block->fields->value.when_block;
        while (value_when_block)
        {
            f_scene_method_v2_t scene_method = ezlopi_scene_get_method_v2(value_when_block->block_options.method.type);
            if (scene_method)
            {
                // iterate through all '_when_blocks_'
                ret &= !(scene_method(scene_node, (void *)value_when_block)); // if all the block-calls are false, then return 1;
            }

            value_when_block = value_when_block->next;
        }
    }
    return ret;
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
                // iterate through all the '_when_blocks_'
                ret ^= scene_method(scene_node, (void *)value_when_block);
                // return 1 ; if odd no of '_when_block_' conditions are true
            }

            value_when_block = value_when_block->next;
        }
    }

    return ret;
}

int ezlopi_scene_when_function(l_scenes_list_v2_t *scene_node, void *arg)
{
    TRACE_W("Warning: when-method not implemented!");
    return 0;
}
