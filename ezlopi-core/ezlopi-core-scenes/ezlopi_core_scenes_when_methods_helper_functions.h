#ifndef _EZLOPI_CORE_SCENES_WHEN_METHODS_HELPER_FUNCTIONS_H_
#define _EZLOPI_CORE_SCENES_WHEN_METHODS_HELPER_FUNCTIONS_H_
#include <time.h>
#include <stdint.h>
#include <cJSON.h>

typedef enum e_isdate_modes
{
    ISDATE_DAILY_MODE = 0,
    ISDATE_WEEKLY_MODE,
    ISDATE_MONTHLY_MODE,
    ISDATE_WEEKS_MODE,
    ISDATE_YEAR_WEEKS_MODE,
    ISDATE_UNDEFINED_MODE,
} e_isdate_modes_t;

typedef enum e_isdate_range_method
{
    ISDATE_RANGE_TIME = 0,
    ISDATE_RANGE_DAY,
    ISDATE_RANGE_MONTH,
    ISDATE_RANGE_YEAR,
    ISDATE_RANGE_MAX,
} e_isdate_range_method_t;

typedef struct s_isdate_method
{
    const char* field_name;
    uint8_t(*field_func)(e_isdate_modes_t* mode_type, struct tm* info, l_fields_v2_t* curr_field);
} s_isdate_method_t;

typedef struct s_issunstate_method
{
    const char* field_name;
    uint8_t(*field_func)(l_scenes_list_v2_t* scene_node, l_fields_v2_t* curr_field, struct tm* info, uint8_t curr_sunstate_mode);
} s_issunstate_method_t;

typedef struct s_isonce_method
{
    const char* field_name;
    uint8_t(*field_func)(l_fields_v2_t* curr_field, struct tm* info);
} s_isonce_method_t;

typedef struct s_isdate_range_method
{
    const char* field_name;
    void (*field_func)(l_fields_v2_t* curr_field, struct tm* tmp_tm);
} s_isdate_range_method_t;

typedef struct s_function_opr
{
    const char* opr_name;
    int(*opr_method)(l_scenes_list_v2_t* scene_node, l_when_block_v2_t* when_block, cJSON* cj_func_opr);
} s_function_opr_t;

typedef struct s_when_function
{
    uint32_t transtion_instant;
    uint32_t transition_count;
    bool current_state;
    bool activate_pulse_seq;    /* used only in 'for_pulse_method' */
} s_when_function_t;

//------------------------------- ezlopi_scene_when_is_date -----------------------------------------------
uint8_t isdate_type_check(e_isdate_modes_t* mode_type, struct tm* info, l_fields_v2_t* curr_field);
uint8_t isdate_tm_check(e_isdate_modes_t* mode_type, struct tm* info, l_fields_v2_t* curr_field);
uint8_t isdate_weekdays_check(e_isdate_modes_t* mode_type, struct tm* info, l_fields_v2_t* curr_field);
uint8_t isdate_mdays_check(e_isdate_modes_t* mode_type, struct tm* info, l_fields_v2_t* curr_field);
uint8_t isdate_year_weeks_check(e_isdate_modes_t* mode_type, struct tm* info, l_fields_v2_t* curr_field);
int isdate_check_flag_result(e_isdate_modes_t mode_type, uint8_t flag_check);

//------------------------------- ezlopi_scene_when_is_SunState -------------------------------------------
uint8_t issunstate_check_mdrn(uint8_t sunstate_mode, const char* range_type, struct tm* info, struct tm* defined_moment);
uint8_t issunstate_get_suntime(l_scenes_list_v2_t* scene_node, l_fields_v2_t* curr_field, struct tm* info, uint8_t curr_sunstate_mode);
uint8_t issunstate_get_offs_tmval(l_scenes_list_v2_t* scene_node, l_fields_v2_t* curr_field, struct tm* info, uint8_t curr_sunstate_mode);
uint8_t issunstate_eval_weekdays(l_scenes_list_v2_t* scene_node, l_fields_v2_t* curr_field, struct tm* info, uint8_t curr_sunstate_mode);
uint8_t issunstate_eval_days(l_scenes_list_v2_t* scene_node, l_fields_v2_t* curr_field, struct tm* info, uint8_t curr_sunstate_mode);
uint8_t issunstate_eval_range(l_scenes_list_v2_t* scene_node, l_fields_v2_t* curr_field, struct tm* info, uint8_t curr_sunstate_mode);
int issunstate_check_flag_result(l_scenes_list_v2_t* scene_node, struct tm* info, uint8_t flag_check);

//------------------------------- ezlopi_scene_when_is_once ---------------------------------------------------
uint8_t isonce_tm_check(l_fields_v2_t* curr_field, struct tm* info);
uint8_t isonce_day_check(l_fields_v2_t* curr_field, struct tm* info);
uint8_t isonce_month_check(l_fields_v2_t* curr_field, struct tm* info);
uint8_t isonce_year_check(l_fields_v2_t* curr_field, struct tm* info);
int isonce_check_flag_result(l_scenes_list_v2_t* scene_node, uint8_t flag_check);

//------------------------------- ezlopi_scene_when_is_date_range -----------------------------------------
void isdate_range_get_tm(l_fields_v2_t* curr_field, struct tm* tmp_tm);
void isdate_range_get_startday(l_fields_v2_t* curr_field, struct tm* tmp_tm);
void isdate_range_get_endday(l_fields_v2_t* curr_field, struct tm* tmp_tm);
void isdate_range_get_startmonth(l_fields_v2_t* curr_field, struct tm* tmp_tm);
void isdate_range_get_endmonth(l_fields_v2_t* curr_field, struct tm* tmp_tm);
void isdate_range_get_startyear(l_fields_v2_t* curr_field, struct tm* tmp_tm);
void isdate_range_get_endyear(l_fields_v2_t* curr_field, struct tm* tmp_tm);
uint8_t isdate_range_check_tm(struct tm* start, struct tm* end, struct tm* info);
uint8_t isdate_range_check_day(struct tm* start, struct tm* end, struct tm* info);
uint8_t isdate_range_check_month(struct tm* start, struct tm* end, struct tm* info);
uint8_t isdate_range_check_year(struct tm* start, struct tm* end, struct tm* info);
int isdate_range_check_flag_result(uint8_t flag_check);

//------------------------------- ezlopi_scene_when_function_method -----------------------------------------
int when_function_for_opr(l_scenes_list_v2_t* scene_node, l_when_block_v2_t* when_block, cJSON* cj_func_opr);
int when_function_for_repeat(l_scenes_list_v2_t* scene_node, l_when_block_v2_t* when_block, cJSON* cj_func_opr);
int when_function_for_follow(l_scenes_list_v2_t* scene_node, l_when_block_v2_t* when_block, cJSON* cj_func_opr);
int when_function_for_pulse(l_scenes_list_v2_t* scene_node, l_when_block_v2_t* when_block, cJSON* cj_func_opr);
int when_function_for_latch(l_scenes_list_v2_t* scene_node, l_when_block_v2_t* when_block, cJSON* cj_func_opr);
#endif /*_EZLOPI_CORE_SCENES_WHEN_METHODS_HELPER_FUNCTIONS_H_*/