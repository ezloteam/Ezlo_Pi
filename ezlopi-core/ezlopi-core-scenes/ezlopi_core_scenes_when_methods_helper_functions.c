#include "ezlopi_util_trace.h"
#include "ezlopi_core_http.h"
#include "ezlopi_core_event_group.h"
#include "ezlopi_core_scenes_when_methods_helper_functions.h"

//------------------------------- ezlopi_scene_when_is_date -----------------------------------------------------------
e_isdate_modes_t isdate_field_type_check(const char *check_type_name)
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
uint8_t isdate_tm_check(e_isdate_modes_t mode_type, struct tm *info, cJSON *cj_time_arr)
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
uint8_t isdate_weekdays_check(e_isdate_modes_t mode_type, struct tm *info, cJSON *cj_weekdays_arr)
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
uint8_t isdate_mdays_check(e_isdate_modes_t mode_type, struct tm *info, cJSON *cj_days_arr)
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
uint8_t isdate_check_endweek_conditon(e_isdate_modes_t mode_type, struct tm *info)
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
uint8_t isdate_find_nth_week_curr_month(struct tm *info)
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
uint8_t isdate_year_weeks_check(e_isdate_modes_t mode_type, struct tm *info, cJSON *cj_weeks_arr)
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
                    if (0 != (ret = isdate_check_endweek_conditon(mode_type, info))) // ret = (1<<3), if last-week confirmed
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
                        int tmp_week = isdate_find_nth_week_curr_month(info); // return the current week-number with 'monday' as first day
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

//------------------------------- ezlopi_scene_when_is_SunState -----------------------------------------------------------
e_issunstate_offset_type_t issunstate_offset_type(const char *check_type_name)
{
    e_issunstate_offset_type_t ret = ISSUNSTATE_UNDEFINED;
    ret = (0 == strncmp(check_type_name, "intime", 7))   ? ISSUNSTATE_INTIME_MODE
          : (0 == strncmp(check_type_name, "before", 7)) ? ISSUNSTATE_BEFORE_MODE
          : (0 == strncmp(check_type_name, "after", 6))  ? ISSUNSTATE_AFTER_MODE
                                                         : ISSUNSTATE_UNDEFINED;

    return ret;
}
void issunsate_update_sunstate_tm(int tm_mday, struct tm *sunrise_time, struct tm *sunset_time)
{
    if (tm_mday && sunrise_time && sunset_time)
    {
        // send httprequest to 'sunrisesunset.io' // use the latitude and longitude from NVS
        // char tmp_url[] = "https://api.sunrisesunset.io/json?lat=27.700769&lng=85.300140";
        // char tmp_headers[] = "Host: api.sunrisesunset.io\r\nAccept: */*\r\nConnection: close\r\n";
        // char tmp_web_server[] = "api.sunrisesunset.io";

        char tmp_url[] = "https://official-joke-api.appspot.com/random_joke";
        char tmp_headers[] = "Host: official-joke-api.appspot.com\r\nAccept: */*\r\nConnection: close\r\n";
        char tmp_web_server[] = "official-joke-api.appspot.com";

        s_ezlopi_core_http_mbedtls_t tmp_config = {
            .method = HTTP_METHOD_GET,
            .url = tmp_url,
            .url_maxlen = sizeof(tmp_url),
            .web_port = 443,
            .header = tmp_headers,
            .header_maxlen = sizeof(tmp_headers),
            .web_server = tmp_web_server,
            .web_server_maxlen = sizeof(tmp_web_server),
            .response = NULL,
            .response_maxlen = 0,
        };

        // must return 'true' if success
        if (NULL == tmp_config.mbedtls_task_handle)
        {
            if (0 == ezlopi_event_group_wait_for_event(EZLOPI_EVENT_MBEDTLS_TASK_BUSY, 100, 0)) // required 'not_set' // checking if mbed_task is occupied
            {
                function_to_call_mbedtlshttp(&tmp_config);
                
                uint8_t retry = 0;
                while (1 == ezlopi_event_group_wait_for_event(EZLOPI_EVENT_MBEDTLS_TASK_BUSY, 100, 0)) // wait till 10sec
                {
                    TRACE_I("MbedTask is busy...mbedtls_task_handle => %d\n", (int)tmp_config.mbedtls_task_handle);
                    if (retry++ > 10)
                    {
                        if (NULL != tmp_config.mbedtls_task_handle)
                        {
                            vTaskDelete(tmp_config.mbedtls_task_handle);
                            TRACE_E("Deleted the mbedtls_task_handle => %d\n", (int)tmp_config.mbedtls_task_handle);
                            ezlopi_event_group_clear_event(EZLOPI_EVENT_MBEDTLS_TASK_BUSY);
                        }
                        break;
                    }
                    vTaskDelay(1000 / portTICK_PERIOD_MS);
                }
            }
            else
            {
                TRACE_E("+++++ ERR : MbedTask is already active +++++\n");
            }
        }
#if 0
        if (tmp_config.response)
        {
            // sunrise_time->tm_mday = sunset_time->tm_mday = tm_mday;
            // sunrise_time->tm_hour = 6;
            // sunset_time->tm_hour = 5 + 12; // 24-hr
            // sunrise_time->tm_min = 49;
            // sunset_time->tm_min = 48;
            // sunrise_time->tm_sec = 31;
            // sunset_time->tm_sec = 42;
            TRACE_I("isSunState : [%p]response = [%d]%s.", tmp_config.response, strlen(tmp_config.response), tmp_config.response);
            free(tmp_config.response);
        }
#endif
        // get the time // for Example
        sunrise_time->tm_mday = sunset_time->tm_mday = tm_mday;
        sunrise_time->tm_hour = 6;
        sunset_time->tm_hour = 5 + 12; // 24-hr
        sunrise_time->tm_min = 49;
        sunset_time->tm_min = 48;
        sunrise_time->tm_sec = 31;
        sunset_time->tm_sec = 42;

        // now check if sunset and sunrise time are not zero
        if ((0 == sunrise_time->tm_hour) &&
            (0 == sunset_time->tm_hour) &&
            (0 == sunrise_time->tm_min) &&
            (0 == sunset_time->tm_min) &&
            (0 == sunrise_time->tm_sec) &&
            (0 == sunset_time->tm_sec))
        {
            TRACE_E(" Failed... clearing 'sunrise/sunset tm_mday'.. ");
            sunrise_time->tm_mday = sunset_time->tm_mday = 0;
        }
        /*Note : no need to free ; since the structure is static*/
        // free_http_mbedtls_struct(tmp_http_data);
    }
    else
    {
        TRACE_E("NULL -> 'sunrise_time' or 'sunset_time' Structs.");
    }
}
void issunstate_add_tm_offset(e_issunstate_offset_type_t sunstate_offset, struct tm *sunstate_time, struct tm *defined_moment, const char *time_offset)
{
    if (sunstate_time && defined_moment && time_offset) // sunstate_time => sunrise or sunset
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
        switch (sunstate_offset)
        {
        case ISSUNSTATE_BEFORE_MODE:
        {
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
            defined_moment->tm_hour = sunstate_time->tm_hour;
            defined_moment->tm_min = sunstate_time->tm_min;
            defined_moment->tm_sec = sunstate_time->tm_sec;
            break;
        }
        case ISSUNSTATE_UNDEFINED:
        {
            TRACE_D(" INVALID isSunState_offset_type ... No time-offset added.");
            break;
        }
        }
    }
}
uint8_t issunstate_midnight_check(uint8_t sunstate_mode, const char *range_type, struct tm *curr_time, struct tm *defined_moment)
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

//--------------------------- ezlopi_scene_when_is_date_range --------------------------------------------------------
uint8_t isdate_range_check_tm(struct tm *start, struct tm *end, struct tm *info)
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
uint8_t isdate_range_check_day(struct tm *start, struct tm *end, struct tm *info)
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
uint8_t isdate_range_check_month(struct tm *start, struct tm *end, struct tm *info)
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
uint8_t isdate_range_check_year(struct tm *start, struct tm *end, struct tm *info)
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

//-------------------------------------------------------------------------------------------------------------------