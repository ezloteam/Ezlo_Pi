#ifndef _EZLOPI_CORE_SCENES_WHEN_METHODS_HELPER_FUNCTIONS_H_
#define _EZLOPI_CORE_SCENES_WHEN_METHODS_HELPER_FUNCTIONS_H_
#include <time.h>
#include <stdint.h>
#include <cJSON.h>

typedef enum e_issunstate_offset_type
{
    ISSUNSTATE_INTIME_MODE = 0,
    ISSUNSTATE_BEFORE_MODE,
    ISSUNSTATE_AFTER_MODE,
    ISSUNSTATE_UNDEFINED,
} e_issunstate_offset_type_t;
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

typedef struct s_isdate_func
{
    const char *field_name;
    uint8_t (*field_func)(e_isdate_modes_t mode_type, struct tm *info, cJSON *arg);
} s_isdate_func_t;
//------------------------------- ezlopi_scene_when_is_date -----------------------------------------------
e_isdate_modes_t isdate_field_type_check(const char *check_type_name);
uint8_t isdate_tm_check(e_isdate_modes_t mode_type, struct tm *info, cJSON *cj_time_arr);
uint8_t isdate_weekdays_check(e_isdate_modes_t mode_type, struct tm *info, cJSON *cj_weekdays_arr);
uint8_t isdate_mdays_check(e_isdate_modes_t mode_type, struct tm *info, cJSON *cj_days_arr);
uint8_t isdate_check_endweek_conditon(e_isdate_modes_t mode_type, struct tm *info);
uint8_t isdate_find_nth_week_curr_month(struct tm *info);
uint8_t isdate_year_weeks_check(e_isdate_modes_t mode_type, struct tm *info, cJSON *cj_weeks_arr);

//------------------------------- ezlopi_scene_when_is_SunState -------------------------------------------
e_issunstate_offset_type_t issunstate_offset_type(const char *check_type_name);
void issunsate_update_sunstate_tm(int tm_mday, struct tm *sunrise_time, struct tm *sunset_time);
void issunstate_add_tm_offset(e_issunstate_offset_type_t sunstate_offset, struct tm *sunstate_time, struct tm *defined_moment, const char *time_offset);
uint8_t issunstate_midnight_check(uint8_t sunstate_mode, const char *range_type, struct tm *curr_time, struct tm *defined_moment);

//------------------------------- ezlopi_scene_when_is_date_range -----------------------------------------
uint8_t isdate_range_check_tm(struct tm *start, struct tm *end, struct tm *info);
uint8_t isdate_range_check_day(struct tm *start, struct tm *end, struct tm *info);
uint8_t isdate_range_check_month(struct tm *start, struct tm *end, struct tm *info);
uint8_t isdate_range_check_year(struct tm *start, struct tm *end, struct tm *info);

#endif /*_EZLOPI_CORE_SCENES_WHEN_METHODS_HELPER_FUNCTIONS_H_*/