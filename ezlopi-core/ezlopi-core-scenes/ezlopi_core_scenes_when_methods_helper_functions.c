#include "ezlopi_util_trace.h"
#include "ezlopi_core_http.h"
#include "ezlopi_core_scenes_v2.h"
#include "ezlopi_core_scenes_when_methods_helper_functions.h"

//------------------------------- ezlopi_scene_when_is_date ---------------------------------------------

static uint8_t isdate_check_endweek_conditon(e_isdate_modes_t mode_type, struct tm *info)
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
            // TRACE_S(" Last week of [%d] starts from [%d] :", info->tm_mon, _starting_date_of_last_week);
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
static uint8_t isdate_find_nth_week_curr_month(struct tm *info)
{
    // 2. find the fisrt day in this month
    uint8_t tmp_week_num = 1; // starts with 1 ; since are already in one of the week-count
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
    // TRACE_S("First day in current month = %d", tmp_weekday_of_curr_month);
    // TRACE_S("[1-7] : %dth_Day  lies in week[%dth] of the current month", (info->tm_wday), tmp_week_num);
    return tmp_week_num;
}

uint8_t isdate_type_check(e_isdate_modes_t *mode_type, struct tm *info, l_fields_v2_t *curr_field)
{
    const char *field_type_name[] = {
        "daily",
        "weekly",
        "monthly",
        "weeks",
        "yearWeeks",
    };
    if ((EZLOPI_VALUE_TYPE_STRING == curr_field->value_type) && (NULL != curr_field->field_value.u_value.value_string))
    {
        const char *check_type_name = curr_field->field_value.u_value.value_string;
        for (uint8_t t = 0; t < 5; t++)
        {
            if (0 == strncmp(field_type_name[t], check_type_name, strlen(check_type_name) + 1))
            {
                *mode_type = (e_isdate_modes_t)t; // daily , weekly , monthly,... // 0-4
                break;
            }
        }
    }
    return 0;
}
uint8_t isdate_tm_check(e_isdate_modes_t *mode_type, struct tm *info, l_fields_v2_t *curr_field)
{
    uint8_t ret = 0;
    cJSON *cj_time_arr = curr_field->field_value.u_value.cj_value;
    if (cj_time_arr && (cJSON_Array == cj_time_arr->type))
    {
        ret |= (1 << 4);
        char field_hr_mm[10] = {0};
        strftime(field_hr_mm, 10, "%H:%M", info);
        field_hr_mm[9] = '\0';
        // TRACE_S("[field_hr_mm: %s]", field_hr_mm);

        int array_size = cJSON_GetArraySize(cj_time_arr);
        for (int i = 0; i < array_size; i++)
        {
            cJSON *array_item = cJSON_GetArrayItem(cj_time_arr, i);
            if (array_item && cJSON_IsString(array_item))
            {
                // TRACE_S("Time activate_%d: %s,  [field_hr_mm: %s]", i, array_item->valuestring, field_hr_mm);
                if (0 == strncmp(array_item->valuestring, field_hr_mm, 10))
                {
                    ret |= (1 << 0); // One of the TIME-condition has been met.
                    break;
                }
            }
        }
        if (!array_size) // if we are given : -"value" : []
        {
            TRACE_S("Time activate :- [00:00],  [field_hr_mm: %s]", field_hr_mm);
            if (0 == strncmp(field_hr_mm, "00:00", 10)) // 24-hr format
            {
                ret |= (1 << 0); // TIME-condition "00:00" has been met.
            }
        }
    }
    return ret;
}
uint8_t isdate_weekdays_check(e_isdate_modes_t *mode_type, struct tm *info, l_fields_v2_t *curr_field)
{
    uint8_t ret = 0;
    cJSON *cj_weekdays_arr = curr_field->field_value.u_value.cj_value;
    if (cj_weekdays_arr && (cJSON_Array == cj_weekdays_arr->type))
    {
        ret |= (1 << 5);
        // Only for comparisions dont change 'info->tm_wday' -> 'sun:0,mon:1, ... , sat:6' to 'sun:7,mon:1, ... ,sat:6'
        int field_weekdays = (0 == (info->tm_wday)) ? 7 : (info->tm_wday); // sunday => 0+1 ... saturday => 6+1

        int array_size = cJSON_GetArraySize(cj_weekdays_arr);
        for (int i = 0; i < array_size; i++)
        {
            cJSON *array_item = cJSON_GetArrayItem(cj_weekdays_arr, i);
            if (array_item && cJSON_IsNumber(array_item))
            {
                // TRACE_S("Weekdays activate_[%d]: %d, [field_weekdays: %d]", i, (int)(array_item->valuedouble), field_weekdays);
                if ((int)(array_item->valuedouble) == field_weekdays)
                {
                    ret |= (1 << 1); // One of the WEEKDAYS-condition has been met.
                    break;
                }
            }
        }
    }
    return ret;
}
uint8_t isdate_mdays_check(e_isdate_modes_t *mode_type, struct tm *info, l_fields_v2_t *curr_field)
{
    uint8_t ret = 0;
    cJSON *cj_days_arr = curr_field->field_value.u_value.cj_value;
    if (cj_days_arr && (cJSON_Array == cj_days_arr->type))
    {
        ret |= (1 << 6);
        int array_size = cJSON_GetArraySize(cj_days_arr);
        int field_days = info->tm_mday; // 1-31

        for (int i = 0; i < array_size; i++)
        {
            cJSON *array_item = cJSON_GetArrayItem(cj_days_arr, i);
            if (array_item && cJSON_IsNumber(array_item))
            {
                TRACE_S("Days activate_%d: %d, [field_days: %d]", i, (int)(array_item->valuedouble), field_days);
                if ((int)(array_item->valuedouble) == field_days)
                {
                    ret |= (1 << 2); // One of the DAYS-condition has been met.
                    break;
                }
            }
        }
    }
    return ret;
}

uint8_t isdate_year_weeks_check(e_isdate_modes_t *mode_type, struct tm *info, l_fields_v2_t *curr_field)
{
    uint8_t ret = 0;
    cJSON *cj_weeks_arr = curr_field->field_value.u_value.cj_value;
    if (cj_weeks_arr && (cJSON_Array == cj_weeks_arr->type))
    {
        ret |= (1 << 7);
        int array_size = cJSON_GetArraySize(cj_weeks_arr);
        for (int i = 0; i < array_size; i++)
        {
            // extract ;- [1,4,5,23,6,9,...,-1]
            cJSON *array_item = cJSON_GetArrayItem(cj_weeks_arr, i);
            if (array_item && cJSON_IsNumber(array_item))
            {
                if (-1 == (int)(array_item->valuedouble)) // for case :- '-1'
                {
                    if (0 != (ret = isdate_check_endweek_conditon(*mode_type, info))) // ret = (1<<3), if last-week confirmed
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
                    week_val[9] = '\0';

                    if (ISDATE_YEAR_WEEKS_MODE == *mode_type)
                    {
                        strftime(field_weeks, 10, "%W", info); // [First day => Monday] ; Week number within (00-53)
                        field_weeks[9] = '\0';
                    }
                    else if (ISDATE_WEEKS_MODE == *mode_type)
                    {
                        int tmp_week = isdate_find_nth_week_curr_month(info); // return the current week-number with 'monday' as first day
                        snprintf(field_weeks, 10, "%d", tmp_week);
                        field_weeks[9] = '\0';
                    }

                    if (0 == strncmp(week_val, field_weeks, 10)) // comparsion in string formats only
                    {
                        ret |= (1 << 3); // One of the TIME-condition has been met.
                        TRACE_I("Weeks_condition '%sth week' [reqd : %s]  has been satisfied ; ret = (1<<3)", field_weeks, week_val);
                        break;
                    }
                }
            }
        }
    }
    return ret;
}
int isdate_check_flag_result(e_isdate_modes_t mode_type, uint8_t flag_check)
{
    int ret = 0;
    const uint8_t TIME_FLAG = (1 << 0);
    const uint8_t WEEKDAYS_FLAG = (1 << 1);
    const uint8_t DAYS_FLAG = (1 << 2);
    const uint8_t WEEKS_FLAG = (1 << 3);
    const uint8_t MASK_FOR_TIME_ARG = (1 << 4);
    const uint8_t MASK_FOR_WEEKDAYS_ARG = (1 << 5);
    const uint8_t MASK_FOR_DAYS_ARG = (1 << 6);
    const uint8_t MASK_FOR_WEEKS_ARG = (1 << 7);
    switch (mode_type)
    {
    case ISDATE_DAILY_MODE:
    {
        if (((flag_check & MASK_FOR_TIME_ARG) && (flag_check & TIME_FLAG)))
        {
            TRACE_W("here! daily-time");
            ret = 1;
        }
        break;
    }
    case ISDATE_WEEKLY_MODE:
    {
        if ((((flag_check & MASK_FOR_TIME_ARG) && (flag_check & TIME_FLAG)) &&
             ((flag_check & MASK_FOR_WEEKDAYS_ARG) && (flag_check & WEEKDAYS_FLAG))))
        {
            TRACE_W("here! week_days and time");
            ret = 1;
        }
        break;
    }
    case ISDATE_MONTHLY_MODE:
    {
        if ((((flag_check & MASK_FOR_TIME_ARG) && (flag_check & TIME_FLAG)) &&
             ((flag_check & MASK_FOR_DAYS_ARG) && (flag_check & DAYS_FLAG))))
        {
            TRACE_W("here! month_days and time");
            ret = 1;
        }
        break;
    }
    case ISDATE_WEEKS_MODE:
    case ISDATE_YEAR_WEEKS_MODE:
    {
        if (((flag_check & MASK_FOR_TIME_ARG) && (flag_check & TIME_FLAG)) &&
            ((flag_check & MASK_FOR_WEEKS_ARG) && (flag_check & WEEKS_FLAG)))
        {
            TRACE_W("here! week and time");
            ret = 1;
        }
        break;
    }
    default:
    {
        break;
    }
    }
    return ret;
}

//------------------------------- ezlopi_scene_when_is_once ------------------------------------------
uint8_t isonce_tm_check(l_fields_v2_t *curr_field, struct tm *info)
{
    uint8_t flag_check = 0;
    if ((EZLOPI_VALUE_TYPE_24_HOURS_TIME == curr_field->value_type) && (NULL != curr_field->field_value.u_value.value_string))
    {
        char field_hr_mm[10] = {0};
        strftime(field_hr_mm, 10, "%H:%M", info);
        field_hr_mm[9] = '\0';

        if (0 == strncmp(curr_field->field_value.u_value.value_string, field_hr_mm, 10))
        {
            flag_check |= (1 << 0); // One of the TIME-condition has been met.
        }
    }
    return flag_check;
}
uint8_t isonce_day_check(l_fields_v2_t *curr_field, struct tm *info)
{
    uint8_t flag_check = 0;
    if (EZLOPI_VALUE_TYPE_INT == curr_field->value_type)
    {
        if ((int)(curr_field->field_value.u_value.value_double) == info->tm_mday)
        {
            flag_check |= (1 << 1);
        }
    }
    return flag_check;
}
uint8_t isonce_month_check(l_fields_v2_t *curr_field, struct tm *info)
{
    uint8_t flag_check = 0;
    if (EZLOPI_VALUE_TYPE_INT == curr_field->value_type)
    {
        if ((int)(curr_field->field_value.u_value.value_double) == (info->tm_mon + 1))
        {
            flag_check |= (1 << 2);
        }
    }
    return flag_check;
}
uint8_t isonce_year_check(l_fields_v2_t *curr_field, struct tm *info)
{
    uint8_t flag_check = 0;
    if (EZLOPI_VALUE_TYPE_INT == curr_field->value_type)
    {
        if ((int)(curr_field->field_value.u_value.value_double) == (info->tm_year + 1900))
        {
            flag_check |= (1 << 3);
        }
    }
    return flag_check;
}
int isonce_check_flag_result(l_scenes_list_v2_t *scene_node, uint8_t flag_check)
{
    int ret = 0;
    const uint8_t TIME_FLAG = (1 << 0);
    const uint8_t DAY_FLAG = (1 << 1);
    const uint8_t MONTH_FLAG = (1 << 2);
    const uint8_t YEAR_FLAG = (1 << 3);

    if ((flag_check & TIME_FLAG) && (flag_check & DAY_FLAG) && (flag_check & MONTH_FLAG) && (flag_check & YEAR_FLAG))
    {
        TRACE_W("here! once and time");
        // now to disable the scene and also store in ezlopi_nvs
        scene_node->enabled = false;
        ezlopi_scenes_enable_disable_id_from_list_v2(scene_node->_id, false);
        ret = 1;
    }
    return ret;
}

//------------------------------- ezlopi_scene_when_is_SunState ---------------------------------------

typedef enum e_issunstate_offset
{
    ISSUNSTATE_INTIME_MODE = 0,
    ISSUNSTATE_BEFORE_MODE,
    ISSUNSTATE_AFTER_MODE,
    ISSUNSTATE_UNDEFINED,
} e_issunstate_offset_t;
typedef struct s_sunstate_data
{
    uint8_t sunstate_mode;             // [sunrise=1,sunset=2,0=NULL]
    e_issunstate_offset_t tmoffs_type; // [intime=0,before=1,after=2]
    int curr_tm_day;                   // today's day number
    struct tm choosen_suntime;         // unique suntime
    struct tm defined_moment;          // offset+suntime
} s_sunstate_data_t;

static void issunsate_update_sunstate_tm(int tm_mday, s_sunstate_data_t *user_data)
{
    if (tm_mday && user_data)
    {
        // send httprequest to 'sunrisesunset.io' // use the latitude and longitude from NVS
        char tmp_url[] = "https://api.sunrisesunset.io/json?lat=27.700769&lng=85.300140";
        char tmp_headers[] = "Host: api.sunrisesunset.io\r\nAccept: */*\r\nConnection: close\r\n";
        char tmp_web_server[] = "api.sunrisesunset.io";

        // char tmp_url[] = "https://official-joke-api.appspot.com/random_joke";
        // char tmp_headers[] = "Host: official-joke-api.appspot.com\r\nAccept: */*\r\nConnection: close\r\n";
        // char tmp_web_server[] = "official-joke-api.appspot.com";

        s_ezlopi_core_http_mbedtls_t tmp_config = {
            .method = HTTP_METHOD_GET,
            .url = tmp_url,
            .url_maxlen = sizeof(tmp_url),
            .web_port = 443,
            .header = tmp_headers,
            .header_maxlen = sizeof(tmp_headers),
            .web_server = tmp_web_server,
            .web_server_maxlen = sizeof(tmp_web_server),
            .response = NULL};
        /*Make API call here and extract the suntime[according to 'user_data->sunstate_mode']*/

        ezlopi_core_http_mbedtls_req(&tmp_config);
        // e.g. after valid extraction
        user_data->curr_tm_day = tm_mday;            // this stores day for which data is extracted
        user_data->choosen_suntime.tm_hour = 5 + 12; // 24-hr
        user_data->choosen_suntime.tm_min = 48;
        user_data->choosen_suntime.tm_sec = 42;

        // now check if sunset and sunrise time are not zero
        if ((0 == user_data->choosen_suntime.tm_hour) &&
            (0 == user_data->choosen_suntime.tm_min) &&
            (0 == user_data->choosen_suntime.tm_sec))
        {
            TRACE_E(" Failed... clearing 'sunrise/sunset tm_mday'.. ");
            user_data->curr_tm_day = 0;
        }
    }
}
static void issunstate_add_offs(e_issunstate_offset_t tmoffs_type, struct tm *choosen_suntime, struct tm *defined_moment, const char *tm_offs_val)
{
    if (choosen_suntime && defined_moment && tm_offs_val) // choosen_suntime => sunrise or sunset
    {
        // Default values to store start and end boundries
        struct tm tmp_time = {0};

        // Nox, extract & add :'tm_offs_val'
        char time_diff[10];
        snprintf(time_diff, 10, "%s", tm_offs_val);
        time_diff[9] = '\0';
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
        // Combined  'tm_offs_val' & 'curr_suntime'
        int hr_overflow = 0;
        int min_overflow = 0;

        switch (tmoffs_type)
        {
        case ISSUNSTATE_INTIME_MODE:
        {
            TRACE_S("offset : Intime");
            defined_moment->tm_hour = choosen_suntime->tm_hour;
            defined_moment->tm_min = choosen_suntime->tm_min;
            defined_moment->tm_sec = choosen_suntime->tm_sec;
            break;
        }
        case ISSUNSTATE_BEFORE_MODE:
        {
            TRACE_S("offset : Before");
            defined_moment->tm_sec = (choosen_suntime->tm_sec - tmp_time.tm_sec);
            if (defined_moment->tm_sec < 0)
            {
                defined_moment->tm_sec = (60 + defined_moment->tm_sec);
                min_overflow = -1;
            }
            defined_moment->tm_min = (choosen_suntime->tm_min - tmp_time.tm_min + min_overflow);
            if (defined_moment->tm_min < 0)
            {
                defined_moment->tm_min = (60 + defined_moment->tm_min);
                hr_overflow = -1;
            }
            defined_moment->tm_hour = (choosen_suntime->tm_hour - tmp_time.tm_hour + hr_overflow);
            if (defined_moment->tm_hour < 0)
            {
                defined_moment->tm_hour = (24 + defined_moment->tm_hour);
            }
            break;
        }
        case ISSUNSTATE_AFTER_MODE:
        {
            TRACE_S("offset : After");
            defined_moment->tm_sec = (choosen_suntime->tm_sec + tmp_time.tm_sec);
            if (defined_moment->tm_sec > 59)
            {
                defined_moment->tm_sec = (defined_moment->tm_sec - 60);
                min_overflow = +1;
            }
            defined_moment->tm_min = (choosen_suntime->tm_min + tmp_time.tm_min + min_overflow);
            if (defined_moment->tm_min > 59)
            {
                defined_moment->tm_min = (defined_moment->tm_min - 60);
            }
            defined_moment->tm_hour = (choosen_suntime->tm_hour + tmp_time.tm_hour + hr_overflow);
            if (defined_moment->tm_hour > 23)
            {
                defined_moment->tm_hour = (defined_moment->tm_hour - 24);
            }
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

uint8_t issunstate_get_suntime(l_scenes_list_v2_t *scene_node, l_fields_v2_t *curr_field, struct tm *info, uint8_t curr_sunstate_mode)
{
    uint8_t flag_check = 0;
    /*Extract today's suntime via API call*/
    if ((EZLOPI_VALUE_TYPE_STRING == curr_field->value_type) && (NULL != curr_field->field_value.u_value.value_string))
    {
        flag_check |= (1 << 4); // indicates 'MASK_TIME_FLAG'

        // 1. check for valid data within 'user_arg'

        if (NULL == (scene_node->when_block->fields->user_arg))
        {
            s_sunstate_data_t *data = (s_sunstate_data_t *)malloc(sizeof(s_sunstate_data_t));
            if (data)
            {
                memset(data, 0, sizeof(s_sunstate_data_t));
                scene_node->when_block->fields->user_arg = (void *)data;
                // TRACE_D("created user_data...");
            }
            else
            {
                TRACE_E("failed to create user_data...");
                return 0;
            }
        }
        // 2. Recalculate suntime for new-day or null
        s_sunstate_data_t *user_data = (s_sunstate_data_t *)(scene_node->when_block->fields->user_arg);
        if (user_data && (info->tm_mday != user_data->curr_tm_day))
        {
            // TRACE_S("curr_day = [%d] ; [%dth]", info->tm_mday, user_data->curr_tm_day);
            user_data->sunstate_mode = curr_sunstate_mode;          // this sets target sunstate for curr meshbot
            issunsate_update_sunstate_tm(info->tm_mday, user_data); // assign 'curr_day' & 'suntime' only
            user_data->tmoffs_type = (0 == strncmp(curr_field->field_value.u_value.value_string, "intime", 7))   ? ISSUNSTATE_INTIME_MODE
                                     : (0 == strncmp(curr_field->field_value.u_value.value_string, "before", 7)) ? ISSUNSTATE_BEFORE_MODE
                                     : (0 == strncmp(curr_field->field_value.u_value.value_string, "after", 6))  ? ISSUNSTATE_AFTER_MODE
                                                                                                                 : ISSUNSTATE_UNDEFINED;
            // 3. check if, curr_tm_day has been updated successfully
            if ((0 == user_data->curr_tm_day) ||
                (0 == user_data->sunstate_mode) ||
                (ISSUNSTATE_UNDEFINED == user_data->tmoffs_type)) // if the curr_day or sunstate or offset is not set properly;
            {
                TRACE_E(" API extraction unsuccesful... Reseting user_args");
                free(scene_node->when_block->fields->user_arg); // reset the day
                return 0;
            }
            TRACE_W("update_day = [%d][%dth] , offset[%d] : intime=0,before=1,after=2,undefined=3 , SunState[%d] : sunrise=1,sunset=2", info->tm_mday, user_data->curr_tm_day, user_data->tmoffs_type, user_data->sunstate_mode);
        }
    }

    return flag_check;
}

uint8_t issunstate_get_offs_tmval(l_scenes_list_v2_t *scene_node, l_fields_v2_t *curr_field, struct tm *info, uint8_t curr_sunstate_mode)
{
    uint8_t flag_check = 0;
    if ((EZLOPI_VALUE_TYPE_HMS_INTERVAL == curr_field->value_type) && (NULL != curr_field->field_value.u_value.value_string))
    {
        s_sunstate_data_t *user_data = (s_sunstate_data_t *)(scene_node->when_block->fields->user_arg);
        if ((user_data) && (0 != user_data->sunstate_mode))
        {
            if ((0 == user_data->defined_moment.tm_hour) &&
                (0 == user_data->defined_moment.tm_min) &&
                (0 == user_data->defined_moment.tm_sec))
            {
                TRACE_D(".... Adding offset:  +/- (hh:mm:ss) ....");
                issunstate_add_offs(user_data->tmoffs_type, &(user_data->choosen_suntime), &(user_data->defined_moment), curr_field->field_value.u_value.value_string);
                TRACE_S("\nSunMode[%d]{sunrise=1,sunset=2,0=NULL},\nChoosen_suntime(hh:mm:ss = %d:%d:%d),\ndefined_moment(hh:mm:ss = %d:%d:%d),\nOffset(%s)\n",
                        user_data->sunstate_mode,
                        user_data->choosen_suntime.tm_hour,
                        user_data->choosen_suntime.tm_min,
                        user_data->choosen_suntime.tm_sec,
                        user_data->defined_moment.tm_hour,
                        user_data->defined_moment.tm_min,
                        user_data->defined_moment.tm_sec,
                        curr_field->field_value.u_value.value_string);
            }
        }
    }
    return flag_check;
}
uint8_t issunstate_eval_weekdays(l_scenes_list_v2_t *scene_node, l_fields_v2_t *curr_field, struct tm *info, uint8_t curr_sunstate_mode)
{
    uint8_t flag_check = 0;
    if ((EZLOPI_VALUE_TYPE_INT_ARRAY == curr_field->value_type) && (curr_field))
    {
        flag_check |= (1 << 5);                                      // indicates : weekdays
        flag_check |= isdate_weekdays_check(NULL, info, curr_field); // checks condition : (1 << 1)
    }
    return flag_check;
}
uint8_t issunstate_eval_days(l_scenes_list_v2_t *scene_node, l_fields_v2_t *curr_field, struct tm *info, uint8_t curr_sunstate_mode)
{
    uint8_t flag_check = 0;
    if ((EZLOPI_VALUE_TYPE_INT_ARRAY == curr_field->value_type) && (curr_field))
    {
        flag_check |= (1 << 6);                                   // indicates : month_days
        flag_check |= isdate_mdays_check(NULL, info, curr_field); // checks condition : (1 << 2)
    }
    return flag_check;
}
uint8_t issunstate_eval_range(l_scenes_list_v2_t *scene_node, l_fields_v2_t *curr_field, struct tm *info, uint8_t curr_sunstate_mode)
{
    uint8_t flag_check = 0;
    if ((EZLOPI_VALUE_TYPE_TOKEN == curr_field->value_type) && (NULL != curr_field->field_value.u_value.value_string))
    {
        s_sunstate_data_t *user_data = (s_sunstate_data_t *)(scene_node->when_block->fields->user_arg);
        if ((user_data) && (0 != user_data->sunstate_mode))
        {
            // TRACE_S("checking midnight range offset");
            flag_check |= (1 << 7); // indicates : midnight-range
            flag_check |= issunstate_check_mdrn(user_data->sunstate_mode, curr_field->field_value.u_value.value_string, info, &(user_data->defined_moment));
        }
    }
    return flag_check;
}
uint8_t issunstate_check_mdrn(uint8_t sunstate_mode, const char *range_type, struct tm *info, struct tm *defined_moment)
{
    uint8_t ret = 0;
    if (sunstate_mode && range_type && info && defined_moment)
    {
        if (0 == strncmp(range_type, "at", 3)) // at desired moment
        {
            if ((info->tm_hour == defined_moment->tm_hour) &&
                (info->tm_min == defined_moment->tm_min) &&
                (info->tm_sec == defined_moment->tm_sec))
            {
                ret = (1 << 3);
            }
        }
        else if (0 == strncmp(range_type, "after", 6)) // all time after defined moment till midnight of current day
        {

            if ((info->tm_hour < 24) && (info->tm_hour > defined_moment->tm_hour))
            {
                ret = (1 << 3);
            }
            else if (((info->tm_hour < 24) && (info->tm_hour == defined_moment->tm_hour)) &&
                     ((info->tm_min < 60) && (info->tm_min >= defined_moment->tm_min)) &&
                     ((info->tm_sec < 60) && (info->tm_sec > defined_moment->tm_sec)))
            {
                ret = (1 << 3);
            }
        }
        else if (0 == strncmp(range_type, "before", 7)) // all time after midnight till defined moment;
        {
            if ((info->tm_hour >= 0) && (info->tm_hour < defined_moment->tm_hour))
            {
                ret = (1 << 3);
            }
            else if (((info->tm_hour >= 0) && (info->tm_hour == defined_moment->tm_hour)) &&
                     ((info->tm_min >= 0) && (info->tm_min <= defined_moment->tm_min)) &&
                     ((info->tm_sec >= 0) && (info->tm_sec <= defined_moment->tm_sec)))
            {
                ret = (1 << 3);
            }
        }
    }
    return ret;
}

int issunstate_check_flag_result(l_scenes_list_v2_t *scene_node, struct tm *info, uint8_t flag_check)
{
    int ret = 0;
    const uint8_t TIME_FLAG = (1 << 0);
    const uint8_t WEEKDAYS_FLAG = (1 << 1);
    const uint8_t DAYS_FLAG = (1 << 2);
    const uint8_t MIDNIGHT_FLAG = (1 << 3);
    const uint8_t MASK_TIME_FLAG = (1 << 4);
    const uint8_t MASK_WEEKDAYS_FLAG = (1 << 5);
    const uint8_t MASK_DAYS_FLAG = (1 << 6);
    const uint8_t MASK_MIDNIGHT_FLAG = (1 << 7);
    s_sunstate_data_t *user_data = (s_sunstate_data_t *)(scene_node->when_block->fields->user_arg);
    if (user_data)
    {
        if ((0 != user_data->curr_tm_day) && (0 != user_data->sunstate_mode) && (flag_check & MASK_TIME_FLAG)) // defined_moment should have the current day
        {
            // 1. Set 'time-flag', when defined moment is matched : [sunrise/sunset_moment + offsets (if any)]
            if ((info->tm_hour == user_data->defined_moment.tm_hour) &&
                (info->tm_min == user_data->defined_moment.tm_min) &&
                (info->tm_sec == user_data->defined_moment.tm_sec))
            {
                flag_check |= TIME_FLAG;
            }
            // 2. Identify :- Daily or Monthly+weeekdays case
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
        // TRACE_S("offset[%d](intime=0,before=1,after=2,undefined=3) , SunState[%d](sunrise=1,sunset=2) , FLAG_STATUS: %#x", (int)user_data->tmoffs_type, user_data->sunstate_mode, flag_check);
    }

    return ret;
}
//--------------------------- ezlopi_scene_when_is_date_range ----------------------------------------
void isdate_range_get_tm(l_fields_v2_t *curr_field, struct tm *tmp_tm)
{
    if ((EZLOPI_VALUE_TYPE_24_HOURS_TIME == curr_field->value_type) && (NULL != curr_field->field_value.u_value.value_string))
    {
        char time[10];
        snprintf(time, 10, "%s", curr_field->field_value.u_value.value_string);
        time[9] = '\0';
        char *ptr = NULL;
        if (0 != strlen(time))
        {
            tmp_tm->tm_hour = strtoul(time, &ptr, 10);
            tmp_tm->tm_min = strtoul(ptr + 1, NULL, 10);
        }
    }
}
void isdate_range_get_startday(l_fields_v2_t *curr_field, struct tm *tmp_tm)
{
    if (EZLOPI_VALUE_TYPE_INT == curr_field->value_type)
    {
        tmp_tm->tm_mday = (curr_field->field_value.u_value.value_double) ? (int)(curr_field->field_value.u_value.value_double) : 1;
    }
}
void isdate_range_get_endday(l_fields_v2_t *curr_field, struct tm *tmp_tm)
{
    if (EZLOPI_VALUE_TYPE_INT == curr_field->value_type)
    {
        tmp_tm->tm_mday = (curr_field->field_value.u_value.value_double) ? (int)(curr_field->field_value.u_value.value_double) : 31;
    }
}
void isdate_range_get_startmonth(l_fields_v2_t *curr_field, struct tm *tmp_tm)
{
    if (EZLOPI_VALUE_TYPE_INT == curr_field->value_type)
    {
        tmp_tm->tm_mon = (curr_field->field_value.u_value.value_double) ? (int)(curr_field->field_value.u_value.value_double) : 1;
    }
}
void isdate_range_get_endmonth(l_fields_v2_t *curr_field, struct tm *tmp_tm)
{
    if (EZLOPI_VALUE_TYPE_INT == curr_field->value_type)
    {
        tmp_tm->tm_mon = (curr_field->field_value.u_value.value_double) ? (int)(curr_field->field_value.u_value.value_double) : 12;
    }
}
void isdate_range_get_startyear(l_fields_v2_t *curr_field, struct tm *tmp_tm)
{
    if (EZLOPI_VALUE_TYPE_INT == curr_field->value_type)
    {
        tmp_tm->tm_year = (curr_field->field_value.u_value.value_double) ? (int)(curr_field->field_value.u_value.value_double) : 1;
    }
}
void isdate_range_get_endyear(l_fields_v2_t *curr_field, struct tm *tmp_tm)
{
    if (EZLOPI_VALUE_TYPE_INT == curr_field->value_type)
    {
        tmp_tm->tm_year = (curr_field->field_value.u_value.value_double) ? (int)(curr_field->field_value.u_value.value_double) : 2147483647;
    }
}

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
            TRACE_S("Invalid day-orders : start[%d] vs end[%d]", start->tm_mday, end->tm_mday);
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
        TRACE_S("Invalid day-orders : start[%d] vs end[%d]", start->tm_mday, end->tm_mday);
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
        TRACE_S("Invalid month-orders : start[%d] vs end[%d]", start->tm_mon, end->tm_mon);
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
        TRACE_S("Invalid year-orders : start[%d] vs end[%d]", start->tm_year, end->tm_year);
    }
    return ret;
}
int isdate_range_check_flag_result(uint8_t flag_check)
{
    const uint8_t TIME_FLAG = (1 << 0);
    const uint8_t DAY_FLAG = (1 << 1);
    const uint8_t MONTH_FLAG = (1 << 2);
    const uint8_t YEAR_FLAG = (1 << 3);

    int ret = 0;
    if ((flag_check & TIME_FLAG) && (flag_check & DAY_FLAG) && (flag_check & MONTH_FLAG) && (flag_check & YEAR_FLAG))
    {
        ret = 1;
    }
    return ret;
}
//-------------------------------------------------------------------------------------------------------------------