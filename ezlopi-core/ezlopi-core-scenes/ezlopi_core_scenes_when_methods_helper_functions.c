/* ===========================================================================
** Copyright (C) 2025 Ezlo Innovation Inc
**
** Under EZLO AVAILABLE SOURCE LICENSE (EASL) AGREEMENT
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met:
**
** 1. Redistributions of source code must retain the above copyright notice,
**    this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. Neither the name of the copyright holder nor the names of its
**    contributors may be used to endorse or promote products derived from
**    this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
** AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
** ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
** LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
** CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
** SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
** INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
** CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
** ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
** POSSIBILITY OF SUCH DAMAGE.
** ===========================================================================
*/
/**
 * @file    ezlopi_core_scenes_when_methods_helper_functions.c
 * @brief   Functions that operates on scene-when-methods
 * @author  Krishna Kumar Sah (work.krishnasah@gmail.com)
 * @version 1.0
 * @date    12th DEC 2024
 */

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include "../../build/config/sdkconfig.h"

#ifdef CONFIG_EZPI_SERV_ENABLE_MESHBOTS

// #include "ezlopi_core_devices.h"
// #include "ezlopi_core_scenes_v2.h"
#include "ezlopi_util_trace.h"
#include "ezlopi_core_nvs.h"
#include "ezlopi_core_http.h"
#include "ezlopi_core_device_group.h"
#include "ezlopi_core_scenes_edit.h"
#include "ezlopi_core_scenes_expressions.h"
#include "ezlopi_core_scenes_when_methods_helper_functions.h"

#include "ezlopi_cloud_constants.h"
#include "EZLOPI_USER_CONFIG.h"

/*******************************************************************************
 *                          Extern Data Declarations
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Declarations
 *******************************************************************************/

/*******************************************************************************
 *                          Type & Macro Definitions
 *******************************************************************************/
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

/*******************************************************************************
 *                          Static Function Prototypes
 *******************************************************************************/
static int __compare_item_values(l_ezlopi_item_t *curr_item, l_fields_v2_t *value_field);
static uint8_t __isdate_check_endweek_conditon(e_isdate_modes_t mode_type, struct tm *info);
static uint8_t __isdate_find_nth_week_curr_month(struct tm *info);
static void __issunsate_update_sunstate_tm(int tm_mday, s_sunstate_data_t *user_data);
static void __issunstate_add_offs(e_issunstate_offset_t tmoffs_type, struct tm *choosen_suntime, struct tm *defined_moment, const char *tm_offs_val);
static bool __and_when_block_condition(l_scenes_list_v2_t *scene_node, l_when_block_v2_t *when_block);
static uint8_t __isitemState_vs_field_compare(s_item_exp_data_t *new_extract_data, l_fields_v2_t *tmp_field, uint8_t bit_mode_position);
static void __replace_old_with_new_data_val(s_item_exp_data_t *new_extract_data, s_item_exp_data_t *prev_extract_data);
static int ____old_vs_new_extract_data(s_item_exp_data_t *new_extract_data, s_item_exp_data_t *prev_extract_data);

/*******************************************************************************
 *                          Static Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Definitions
 *******************************************************************************/
int ISITEM_state_single_condition(uint32_t item_id, l_fields_v2_t *value_field)
{
    int ret = 0;
    l_ezlopi_item_t *curr_item = EZPI_core_device_get_item_by_id(item_id);
    if (curr_item)
    {
        ret = __compare_item_values(curr_item, value_field);
    }
    return ret;
}
int ISITEM_state_with_grp_condition(uint32_t device_group_id, uint32_t item_group_id, l_fields_v2_t *value_field)
{
    int ret = 0;

    l_ezlopi_device_grp_t *curr_devgrp = EZPI_core_device_group_get_by_id(device_group_id);
    if (curr_devgrp)
    {
        cJSON *cj_get_devarr = NULL;
        cJSON_ArrayForEach(cj_get_devarr, curr_devgrp->devices)
        {
            uint32_t curr_devce_id = strtoul(cj_get_devarr->valuestring, NULL, 16);
            l_ezlopi_device_t *curr_device = EZPI_core_device_get_by_id(curr_devce_id); // immediately goto "102ec000" ...
            if (curr_device)
            {
                l_ezlopi_item_t *curr_item_node = curr_device->items; // perform operation on items of above device --> "102ec000"
                while (curr_item_node)
                {
                    // compare with items_list stored in item_group_id
                    l_ezlopi_item_grp_t *curr_item_grp = EZPI_core_item_group_get_by_id(item_group_id); // get  "ll_itemgrp_node"
                    if (curr_item_grp)
                    {
                        cJSON *cj_item_names = NULL;
                        cJSON_ArrayForEach(cj_item_names, curr_item_grp->item_names)
                        {
                            uint32_t req_item_id_from_itemgrp = strtoul(cj_item_names->valuestring, NULL, 16);
                            // if the item_ids match ; Then compare the "item_values" with that of the "scene's" requirement
                            if (req_item_id_from_itemgrp == curr_item_node->cloud_properties.item_id)
                            {
                                // compare the values of "when-condition"  & the "ll_item_node"
                                ret = __compare_item_values(curr_item_node, value_field);
                                break;
                            }
                        }
                    }
                    curr_item_node = curr_item_node->next;
                }
            }
        }
    }

    return ret;
}

uint8_t ISDATE_type_check(e_isdate_modes_t *mode_type, struct tm *info, l_fields_v2_t *curr_field)
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
            if (EZPI_STRNCMP_IF_EQUAL(field_type_name[t], check_type_name, strlen(field_type_name[t]) + 1, strlen(check_type_name) + 1))
            {
                *mode_type = (e_isdate_modes_t)t; // daily , weekly , monthly,... // 0-4
                break;
            }
        }
    }
    return 0;
}
uint8_t ISDATE_tm_check(e_isdate_modes_t *mode_type, struct tm *info, l_fields_v2_t *curr_field)
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
                if (EZPI_STRNCMP_IF_EQUAL(array_item->valuestring, field_hr_mm, array_item->str_value_len, sizeof(field_hr_mm)))
                {
                    ret |= (1 << 0); // One of the TIME-condition has been met.
                    break;
                }
            }
        }
        if (!array_size) // if we are given : -"value" : []
        {
            TRACE_S("Time activate :- [00:00],  [field_hr_mm: %s]", field_hr_mm);
            if (EZPI_STRNCMP_IF_EQUAL(field_hr_mm, "00:00", sizeof(field_hr_mm), 6)) // 24-hr format
            {
                ret |= (1 << 0); // TIME-condition "00:00" has been met.
            }
        }
    }
    return ret;
}
uint8_t ISDATE_weekdays_check(e_isdate_modes_t *mode_type, struct tm *info, l_fields_v2_t *curr_field)
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
uint8_t ISDATE_mdays_check(e_isdate_modes_t *mode_type, struct tm *info, l_fields_v2_t *curr_field)
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

uint8_t ISDATE_year_weeks_check(e_isdate_modes_t *mode_type, struct tm *info, l_fields_v2_t *curr_field)
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
                    if (0 != (ret = __isdate_check_endweek_conditon(*mode_type, info))) // ret = (1<<3), if last-week confirmed
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
                        int tmp_week = __isdate_find_nth_week_curr_month(info); // return the current week-number with 'monday' as first day
                        snprintf(field_weeks, 10, "%d", tmp_week);
                        field_weeks[9] = '\0';
                    }

                    if (EZPI_STRNCMP_IF_EQUAL(week_val, field_weeks, sizeof(week_val), sizeof(field_weeks))) // comparsion in string formats only
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
int ISDATE_check_flag_result(e_isdate_modes_t mode_type, uint8_t flag_check)
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
            // TRACE_W("here! daily-time");
            ret = 1;
        }
        break;
    }
    case ISDATE_WEEKLY_MODE:
    {
        if ((((flag_check & MASK_FOR_TIME_ARG) && (flag_check & TIME_FLAG)) &&
             ((flag_check & MASK_FOR_WEEKDAYS_ARG) && (flag_check & WEEKDAYS_FLAG))))
        {
            // TRACE_W("here! week_days and time");
            ret = 1;
        }
        break;
    }
    case ISDATE_MONTHLY_MODE:
    {
        if ((((flag_check & MASK_FOR_TIME_ARG) && (flag_check & TIME_FLAG)) &&
             ((flag_check & MASK_FOR_DAYS_ARG) && (flag_check & DAYS_FLAG))))
        {
            // TRACE_W("here! month_days and time");
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
            // TRACE_W("here! week and time");
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

uint8_t ISONCE_tm_check(l_fields_v2_t *curr_field, struct tm *info)
{
    uint8_t flag_check = 0;
    if ((EZLOPI_VALUE_TYPE_24_HOURS_TIME == curr_field->value_type) && (NULL != curr_field->field_value.u_value.value_string))
    {
        char field_hr_mm[10] = {0};
        strftime(field_hr_mm, 10, "%H:%M", info);
        field_hr_mm[9] = '\0';

        if (EZPI_STRNCMP_IF_EQUAL(curr_field->field_value.u_value.value_string, field_hr_mm, strlen(curr_field->field_value.u_value.value_string) + 1, sizeof(field_hr_mm)))
        {
            flag_check |= (1 << 0); // One of the TIME-condition has been met.
        }
    }
    return flag_check;
}
uint8_t ISONCE_day_check(l_fields_v2_t *curr_field, struct tm *info)
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
uint8_t ISONCE_month_check(l_fields_v2_t *curr_field, struct tm *info)
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
uint8_t ISONCE_year_check(l_fields_v2_t *curr_field, struct tm *info)
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
int ISONCE_check_flag_result(l_scenes_list_v2_t *scene_node, uint8_t flag_check)
{
    int ret = 0;
    const uint8_t TIME_FLAG = (1 << 0);
    const uint8_t DAY_FLAG = (1 << 1);
    const uint8_t MONTH_FLAG = (1 << 2);
    const uint8_t YEAR_FLAG = (1 << 3);

    if ((flag_check & TIME_FLAG) && (flag_check & DAY_FLAG) && (flag_check & MONTH_FLAG) && (flag_check & YEAR_FLAG))
    {
        // TRACE_W("here! once and time");
        // now to disable the scene and also store in ezlopi_nvs
        scene_node->enabled = false;
        EZPI_core_scenes_enable_disable_scene_by_id_v2(scene_node->_id, false);
        ret = 1;
    }
    return ret;
}

uint8_t ISSUNSTATE_get_suntime(l_scenes_list_v2_t *scene_node, l_fields_v2_t *curr_field, struct tm *info, uint8_t curr_sunstate_mode)
{
    uint8_t flag_check = 0;
    /*Extract today's suntime via API call*/
    if ((EZLOPI_VALUE_TYPE_STRING == curr_field->value_type) && (NULL != curr_field->field_value.u_value.value_string))
    {
        flag_check |= (1 << 4); // indicates 'MASK_TIME_FLAG'

        // 1. check for valid data within 'user_arg'

        if (NULL == (scene_node->when_block->fields->user_arg))
        {
            s_sunstate_data_t *data = (s_sunstate_data_t *)ezlopi_malloc(__FUNCTION__, sizeof(s_sunstate_data_t));
            if (data)
            {
                memset(data, 0, sizeof(s_sunstate_data_t));
                scene_node->when_block->fields->user_arg = (void *)data;
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
            TRACE_S("curr_day = [%d] ; [%dth]", info->tm_mday, user_data->curr_tm_day);
            char *tmp_str = curr_field->field_value.u_value.value_string;
            size_t tmp_str_len = strlen(tmp_str);
            user_data->sunstate_mode = curr_sunstate_mode;            // this sets target sunstate for curr meshbot
            __issunsate_update_sunstate_tm(info->tm_mday, user_data); // assign 'curr_day' & 'suntime' only
            user_data->tmoffs_type = (EZPI_STRNCMP_IF_EQUAL(tmp_str, ezlopi_intime_str, tmp_str_len, 7))   ? ISSUNSTATE_INTIME_MODE
                                     : (EZPI_STRNCMP_IF_EQUAL(tmp_str, ezlopi_before_str, tmp_str_len, 7)) ? ISSUNSTATE_BEFORE_MODE
                                     : (EZPI_STRNCMP_IF_EQUAL(tmp_str, ezlopi_after_str, tmp_str_len, 6))  ? ISSUNSTATE_AFTER_MODE
                                                                                                           : ISSUNSTATE_UNDEFINED;
            // 3. check if, curr_tm_day has been updated successfully
            if ((0 == user_data->curr_tm_day) ||
                (0 == user_data->sunstate_mode) ||
                (ISSUNSTATE_UNDEFINED == user_data->tmoffs_type)) // if the curr_day or sunstate or offset is not set properly;
            {
                TRACE_E(" API extraction unsuccesful... Reseting user_args");
                ezlopi_free(__FUNCTION__, scene_node->when_block->fields->user_arg); // reset the day
                return 0;
            }
            TRACE_W("update_day = [%d][%dth] , offset[%d] : intime=0,before=1,after=2,undefined=3 , SunState[%d] : sunrise=1,sunset=2", info->tm_mday, user_data->curr_tm_day, user_data->tmoffs_type, user_data->sunstate_mode);
        }
    }
    return flag_check;
}
uint8_t ISSUNSTATE_get_offs_tmval(l_scenes_list_v2_t *scene_node, l_fields_v2_t *curr_field, struct tm *info, uint8_t curr_sunstate_mode)
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
                __issunstate_add_offs(user_data->tmoffs_type, &(user_data->choosen_suntime), &(user_data->defined_moment), curr_field->field_value.u_value.value_string);
                TRACE_S("\r\nSunMode[%d]{sunrise=1,sunset=2,0=NULL},\r\nChoosen_suntime(hh:mm:ss = %d:%d:%d),\r\ndefined_moment(hh:mm:ss = %d:%d:%d),\r\nOffset(%s)\r\n",
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
uint8_t ISSUNSTATE_eval_weekdays(l_scenes_list_v2_t *scene_node, l_fields_v2_t *curr_field, struct tm *info, uint8_t curr_sunstate_mode)
{
    uint8_t flag_check = 0;
    if ((EZLOPI_VALUE_TYPE_INT_ARRAY == curr_field->value_type) && (curr_field))
    {
        flag_check |= (1 << 5);                                      // indicates : weekdays
        flag_check |= ISDATE_weekdays_check(NULL, info, curr_field); // checks condition : (1 << 1)
    }
    return flag_check;
}
uint8_t ISSUNSTATE_eval_days(l_scenes_list_v2_t *scene_node, l_fields_v2_t *curr_field, struct tm *info, uint8_t curr_sunstate_mode)
{
    uint8_t flag_check = 0;
    if ((EZLOPI_VALUE_TYPE_INT_ARRAY == curr_field->value_type) && (curr_field))
    {
        flag_check |= (1 << 6);                                   // indicates : month_days
        flag_check |= ISDATE_mdays_check(NULL, info, curr_field); // checks condition : (1 << 2)
    }
    return flag_check;
}
uint8_t ISSUNSTATE_eval_range(l_scenes_list_v2_t *scene_node, l_fields_v2_t *curr_field, struct tm *info, uint8_t curr_sunstate_mode)
{
    uint8_t flag_check = 0;
    if ((EZLOPI_VALUE_TYPE_TOKEN == curr_field->value_type) && (NULL != curr_field->field_value.u_value.value_string))
    {
        s_sunstate_data_t *user_data = (s_sunstate_data_t *)(scene_node->when_block->fields->user_arg);
        if ((user_data) && (0 != user_data->sunstate_mode))
        {
            // TRACE_S("checking midnight range offset");
            flag_check |= (1 << 7); // indicates : midnight-range
            flag_check |= ISSUNSTATE_check_mdrn(user_data->sunstate_mode, curr_field->field_value.u_value.value_string, info, &(user_data->defined_moment));
        }
    }
    return flag_check;
}
uint8_t ISSUNSTATE_check_mdrn(uint8_t sunstate_mode, const char *range_type, struct tm *info, struct tm *defined_moment)
{
    uint8_t ret = 0;
    if (sunstate_mode && range_type && info && defined_moment)
    {
        size_t len = (range_type) ? strlen(range_type) + 1 : 0;
        if (EZPI_STRNCMP_IF_EQUAL(range_type, ezlopi_at_str, len, 3)) // at desired moment
        {
            if ((info->tm_hour == defined_moment->tm_hour) &&
                (info->tm_min == defined_moment->tm_min) &&
                (info->tm_sec == defined_moment->tm_sec))
            {
                ret = (1 << 3);
            }
        }
        else if (EZPI_STRNCMP_IF_EQUAL(range_type, ezlopi_after_str, len, 6)) // all time after defined moment till midnight of current day
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
        else if (EZPI_STRNCMP_IF_EQUAL(range_type, ezlopi_before_str, len, 7)) // all time after midnight till defined moment;
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
int ISSUNSTATE_check_flag_result(l_scenes_list_v2_t *scene_node, struct tm *info, uint8_t flag_check)
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

void ISDATE_range_get_tm(l_fields_v2_t *curr_field, struct tm *tmp_tm)
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
void ISDATE_range_get_startday(l_fields_v2_t *curr_field, struct tm *tmp_tm)
{
    if (EZLOPI_VALUE_TYPE_INT == curr_field->value_type)
    {
        tmp_tm->tm_mday = (curr_field->field_value.u_value.value_double) ? (int)(curr_field->field_value.u_value.value_double) : 1;
    }
}
void ISDATE_range_get_endday(l_fields_v2_t *curr_field, struct tm *tmp_tm)
{
    if (EZLOPI_VALUE_TYPE_INT == curr_field->value_type)
    {
        tmp_tm->tm_mday = (curr_field->field_value.u_value.value_double) ? (int)(curr_field->field_value.u_value.value_double) : 31;
    }
}
void ISDATE_range_get_startmonth(l_fields_v2_t *curr_field, struct tm *tmp_tm)
{
    if (EZLOPI_VALUE_TYPE_INT == curr_field->value_type)
    {
        tmp_tm->tm_mon = (curr_field->field_value.u_value.value_double) ? (int)(curr_field->field_value.u_value.value_double) : 1;
    }
}
void ISDATE_range_get_endmonth(l_fields_v2_t *curr_field, struct tm *tmp_tm)
{
    if (EZLOPI_VALUE_TYPE_INT == curr_field->value_type)
    {
        tmp_tm->tm_mon = (curr_field->field_value.u_value.value_double) ? (int)(curr_field->field_value.u_value.value_double) : 12;
    }
}
void ISDATE_range_get_startyear(l_fields_v2_t *curr_field, struct tm *tmp_tm)
{
    if (EZLOPI_VALUE_TYPE_INT == curr_field->value_type)
    {
        tmp_tm->tm_year = (curr_field->field_value.u_value.value_double) ? (int)(curr_field->field_value.u_value.value_double) : 1;
    }
}
void ISDATE_range_get_endyear(l_fields_v2_t *curr_field, struct tm *tmp_tm)
{
    if (EZLOPI_VALUE_TYPE_INT == curr_field->value_type)
    {
        tmp_tm->tm_year = (curr_field->field_value.u_value.value_double) ? (int)(curr_field->field_value.u_value.value_double) : 2147483647;
    }
}

uint8_t ISDATE_range_check_tm(struct tm *start, struct tm *end, struct tm *info)
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
uint8_t ISDATE_range_check_day(struct tm *start, struct tm *end, struct tm *info)
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
uint8_t ISDATE_range_check_month(struct tm *start, struct tm *end, struct tm *info)
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
uint8_t ISDATE_range_check_year(struct tm *start, struct tm *end, struct tm *info)
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
int ISDATE_range_check_flag_result(uint8_t flag_check)
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

int WHEN_function_for_opr(l_scenes_list_v2_t *scene_node, l_when_block_v2_t *when_block, cJSON *cj_func_opr)
{
    // TRACE_W("for_least");
    int ret = 0;
    if (scene_node && when_block && cj_func_opr)
    {
        cJSON *for_type = cJSON_GetObjectItem(__FUNCTION__, cj_func_opr, ezlopi_method_str);
        cJSON *for_interval = cJSON_GetObjectItem(__FUNCTION__, cj_func_opr, ezlopi_seconds_str);
        if (for_type && for_type->valuestring && for_interval)
        {
            /* first get the product of all children states*/
            bool transition_state = __and_when_block_condition(scene_node, when_block);

            /*now compare the intervals between each transtion result*/
            s_when_function_t *function_state_info = (s_when_function_t *)scene_node->when_block->fields->user_arg;

            if ((transition_state == 1) && (0 == function_state_info->current_state))
            {
                function_state_info->transtion_instant = (uint32_t)xTaskGetTickCount();
            }
            else if ((transition_state == 0) && (1 == function_state_info->current_state))
            {
                if (0 != function_state_info->transtion_instant)
                {
                    int threshold_time = for_interval->valuedouble;
                    if (EZPI_STRNCMP_IF_EQUAL(for_type->valuestring, ezlopi_less_str, for_type->str_value_len, 5))
                    {
                        if ((((uint32_t)xTaskGetTickCount() - function_state_info->transtion_instant) / 1000) < threshold_time)
                        {
                            // TRACE_S("activating-less");
                            ret = 1;
                        }
                    }
                    else if (EZPI_STRNCMP_IF_EQUAL(for_type->valuestring, ezlopi_least_str, for_type->str_value_len, 6))
                    {
                        if ((((uint32_t)xTaskGetTickCount() - function_state_info->transtion_instant) / 1000) >= threshold_time)
                        {
                            // TRACE_S("activating-atleast");
                            ret = 1;
                        }
                    }
                    // TRACE_I("end_Transtion_time[%d] , duration_time[%d] , threshold[%d]", function_state_info->transtion_instant / 1000, (uint32_t)xTaskGetTickCount() / 1000, threshold_time);
                }
            }
            function_state_info->current_state = transition_state;
        }
    }
    return ret;
}
int WHEN_function_for_repeat(l_scenes_list_v2_t *scene_node, l_when_block_v2_t *when_block, cJSON *cj_func_opr)
{
    int ret = 0;
    if (scene_node && when_block && cj_func_opr)
    {
        cJSON *for_times = cJSON_GetObjectItem(__FUNCTION__, cj_func_opr, ezlopi_times_str);
        cJSON *for_interval = cJSON_GetObjectItem(__FUNCTION__, cj_func_opr, ezlopi_seconds_str);
        if (for_times && for_interval)
        {
            /* first get the product of all children states*/
            s_when_function_t *function_state_info = (s_when_function_t *)scene_node->when_block->fields->user_arg;
            bool transition_state = __and_when_block_condition(scene_node, when_block);
            if ((transition_state) && (0 == function_state_info->current_state)) /*previous state?*/
            {
                if (0 == function_state_info->transtion_instant)
                {
                    /*store the first trigger time*/
                    // TRACE_W("Start 'repeat' activation Sequence ......!");
                    function_state_info->transtion_instant = (uint32_t)xTaskGetTickCount();
                    TRACE_W("first_trigger_time[%d] ", function_state_info->transtion_instant);
                }

                /*add the count*/
                function_state_info->transition_count++;
                // TRACE_W(" count[%d]", function_state_info->transition_count);

                /*compare with conditon*/
                int for_count = for_times->valuedouble; /*extract the type*/
                if (function_state_info->transition_count >= for_count)
                {
                    uint32_t dur = (((uint32_t)xTaskGetTickCount() - function_state_info->transtion_instant) / 1000);
                    int threshold_time = for_interval->valuedouble;
                    if (dur <= threshold_time) // from 'start' till 'now'
                    {
                        TRACE_W(" Successful activation sequence within [%dsec] ; threshold [%dsec]", dur, threshold_time);
                        ret = 1;
                    }
                    else
                    {
                        TRACE_E(" Time-Out !! consumed[%d (>%dsec)]", dur, threshold_time);
                    }
                    function_state_info->transtion_instant = 0;
                    function_state_info->transition_count = 0;
                }
            }
            function_state_info->current_state = transition_state;
        }
    }

    return ret;
}
int WHEN_function_for_follow(l_scenes_list_v2_t *scene_node, l_when_block_v2_t *when_block, cJSON *cj_func_opr)
{
    int ret = 0;
    if (scene_node && when_block && cj_func_opr)
    {
        cJSON *for_delay = cJSON_GetObjectItem(__FUNCTION__, cj_func_opr, "delayReset");
        if (for_delay)
        {
            s_when_function_t *function_state_info = (s_when_function_t *)scene_node->when_block->fields->user_arg;
            if ((0 == function_state_info->transtion_instant) && (0 == function_state_info->current_state))
            {
                /* first get the product of all children states*/
                bool transition_state = __and_when_block_condition(scene_node, when_block);
                if (transition_state) /*previous state?*/
                {
                    /* trigger phase ---> started */
                    function_state_info->current_state = transition_state;
                    function_state_info->transtion_instant = (uint32_t)xTaskGetTickCount();
                    // TRACE_W("start_follow_time");
                    ret = 1;
                }
            }
            else
            {
                int threshold_time = for_delay->valuedouble;
                if ((((uint32_t)xTaskGetTickCount() - function_state_info->transtion_instant) / 1000) <= threshold_time)
                {
                    ret = 1;
                    // TRACE_S(" Follow -> Active ");
                }
                else
                {
                    /* trigger phase ---> stop */
                    ret = 0;
                    TRACE_W("restart the trigger...");

                    /* now refreshing then block*/
                    if (false == __and_when_block_condition(scene_node, when_block))
                    {
                        function_state_info->transtion_instant = 0;
                        function_state_info->current_state = 0;
                    }
                }
            }
        }
    }

    return ret;
}
int WHEN_function_for_pulse(l_scenes_list_v2_t *scene_node, l_when_block_v2_t *when_block, cJSON *cj_func_opr)
{
    int ret = 0;
    if (scene_node && when_block && cj_func_opr)
    {
        cJSON *for_trueperiod = cJSON_GetObjectItem(__FUNCTION__, cj_func_opr, "truePeriod");
        cJSON *for_falseperiod = cJSON_GetObjectItem(__FUNCTION__, cj_func_opr, "falsePeriod");
        cJSON *for_times = cJSON_GetObjectItem(__FUNCTION__, cj_func_opr, ezlopi_times_str);
        if (for_trueperiod && for_times && for_falseperiod)
        {
            int true_time_dur = for_trueperiod->valuedouble;
            int false_time_dur = for_falseperiod->valuedouble;
            int seq_count = for_times->valuedouble;

            /*1. check for activation condition for 'pulse_flag' */
            s_when_function_t *function_state_info = (s_when_function_t *)scene_node->when_block->fields->user_arg;
            if ((0 == function_state_info->transition_count) &&
                (0 == function_state_info->current_state) &&
                (0 == function_state_info->transtion_instant) &&
                (false == function_state_info->activate_pulse_seq))
            {
                if (__and_when_block_condition(scene_node, when_block))
                {
                    TRACE_W("starting Pulse!");
                    function_state_info->activate_pulse_seq = true;
                    function_state_info->current_state = 1; // state you want to start with
                    function_state_info->transtion_instant = (uint32_t)xTaskGetTickCount();
                }
            }
            /*2. Activate pulse_sequence*/
            if (true == function_state_info->activate_pulse_seq)
            {
                if (seq_count == 0)
                {
                    function_state_info->transition_count = seq_count; /* If 'SEQ_COUNT' == 0 ; Then loop the sequence forever. */
                }
                else
                {
                    seq_count = (seq_count * 2) - 1; // 6 -> [0-5]
                }

                if (function_state_info->transition_count <= seq_count)
                {
                    uint32_t dur = (((uint32_t)xTaskGetTickCount() - function_state_info->transtion_instant) / 1000);
                    if (true == function_state_info->current_state)
                    {
                        if (dur <= true_time_dur)
                        {
                            TRACE_S("high_pulse , true[%dsec]", dur);
                        }
                        else
                        {
                            function_state_info->transtion_instant = (uint32_t)xTaskGetTickCount(); /*in sec*/
                            if (function_state_info->transition_count < seq_count)
                            {
                                function_state_info->transition_count++;
                            }
                            function_state_info->current_state = 0;
                        }
                    }
                    else if (false == function_state_info->current_state)
                    {
                        if (dur <= false_time_dur)
                        {
                            TRACE_S("low_pulse , false[%dsec]", dur);
                        }
                        else
                        {
                            function_state_info->transtion_instant = (uint32_t)xTaskGetTickCount(); /*in sec*/
                            if (function_state_info->transition_count < seq_count)
                            {
                                function_state_info->current_state = 1;
                                function_state_info->transition_count++;
                            }
                            else
                            {
                                function_state_info->current_state = 0;
                            }
                        }
                    }
                }
                else
                {
                    TRACE_E("Exceeded pulse count..............");
                    function_state_info->activate_pulse_seq = false;
                    function_state_info->current_state = 0;
                    function_state_info->transition_count = 0;
                }
            }
            else
            {
                /* trigger phase ---> stop */
                TRACE_W("restart the trigger....");

                /* now refreshing pulse*/
                if (false == __and_when_block_condition(scene_node, when_block))
                {
                    function_state_info->transtion_instant = 0;
                    function_state_info->transition_count = 0;
                }
                function_state_info->current_state = 0;
            }

            TRACE_W("trigger_state= %s , {seq_count: %d}.-----> return =>> [pulse_state = %s]",
                    (function_state_info->activate_pulse_seq) ? ezlopi_true_str : ezlopi_false_str,
                    function_state_info->transition_count,
                    (function_state_info->current_state) ? "high" : "low");
            ret = function_state_info->current_state;
        }
    }

    return ret;
}
int WHEN_function_for_latch(l_scenes_list_v2_t *scene_node, l_when_block_v2_t *when_block, cJSON *cj_func_opr)
{
    int ret = 0;
    if (scene_node && when_block && cj_func_opr)
    {
        cJSON *cj_for_enabled = cJSON_GetObjectItem(__FUNCTION__, cj_func_opr, ezlopi_enabled_str);
        if (cj_for_enabled)
        {
            s_when_function_t *function_state_info = (s_when_function_t *)scene_node->when_block->fields->user_arg;
            if ((0 == function_state_info->current_state) && (cJSON_True == cj_for_enabled->type) && (0 == function_state_info->transtion_instant)) /* if the trigger phase has not started */
            {
                /* first get the product of all children states*/
                if (__and_when_block_condition(scene_node, when_block)) /*if : previous state = 0*/
                {
                    function_state_info->current_state = 1;
                    function_state_info->transtion_instant = (uint32_t)xTaskGetTickCount();
                    TRACE_S("start_latched_time[%d]", (uint32_t)xTaskGetTickCount());
                }
            }
            else if (cJSON_False == cj_for_enabled->type)
            {
                function_state_info->current_state = 0;
                function_state_info->transtion_instant = 0;
                TRACE_S("reset_latched_time[%d]", (uint32_t)xTaskGetTickCount());
            }
            ret = function_state_info->current_state;
            TRACE_W("ret-> current_state = [%d] , active_instant[%d]", function_state_info->current_state, function_state_info->transtion_instant);
        }
    }

    return ret;
}

int IS_deviceitem_group_value_check(l_scenes_list_v2_t *scene_node, uint32_t device_group_id, uint32_t item_group_id)
{
    int ret = 0;
    if (scene_node && (0 < device_group_id) && (0 < item_group_id))
    {
        cJSON *cj_device_items_data = NULL;
        bool save_flag = false;
        if (NULL == scene_node->when_block->fields->user_arg)
        {
            cJSON *cj_device_items_data = cJSON_CreateObject(__FUNCTION__);
            if (cj_device_items_data)
            {
                save_flag = true;
            }
        }
        else
        { // extract the previous cjson_data, if exists;
            save_flag = false;
            cj_device_items_data = (cJSON *)scene_node->when_block->fields->user_arg;
        }

        // 1.
        l_ezlopi_device_grp_t *curr_devgrp = EZPI_core_device_group_get_by_id(device_group_id);
        if (curr_devgrp)
        {
            cJSON *cj_get_devarr = NULL;
            cJSON_ArrayForEach(cj_get_devarr, curr_devgrp->devices)
            {
                uint32_t curr_devce_id = strtoul(cj_get_devarr->valuestring, NULL, 16);
                l_ezlopi_device_t *curr_device = EZPI_core_device_get_by_id(curr_devce_id); // immediately goto "102ec000" ...
                if (curr_device)
                {
                    l_ezlopi_item_t *curr_item_node = curr_device->items; // perform operation on items of above device --> "102ec000"
                    while (curr_item_node)
                    {
                        // 2.
                        l_ezlopi_item_grp_t *curr_item_grp = EZPI_core_item_group_get_by_id(item_group_id); // get  "ll_itemgrp_node"
                        if (curr_item_grp)
                        {
                            cJSON *cj_item_names = NULL;
                            cJSON_ArrayForEach(cj_item_names, curr_item_grp->item_names)
                            {
                                uint32_t req_item_id_from_itemgrp = strtoul(cj_item_names->valuestring, NULL, 16);
                                // if the item_ids match ; Then compare the "item_values" with that of the "scene's" requirement
                                if (req_item_id_from_itemgrp == curr_item_node->cloud_properties.item_id) // 202ec000 == 202ec000 , ...
                                {
                                    if (cj_device_items_data) // if destination arr
                                    {
                                        cJSON *cj_tmp_value = cJSON_CreateObject(__FUNCTION__);
                                        if (cj_tmp_value)
                                        {
                                            curr_item_node->func(EZLOPI_ACTION_GET_EZLOPI_VALUE, curr_item_node, (void *)cj_tmp_value, NULL);
                                            cJSON *cj_value_new = cJSON_GetObjectItem(__FUNCTION__, cj_tmp_value, ezlopi_value_str); // this 'cj_value_new' contains latest 'item_value'.
                                            if (cj_value_new)
                                            {
                                                // save or compare?
                                                if (true == save_flag) /*save*/
                                                {
                                                    // since 'save_flag' is true we will add the latest 'item_values' into user_arg.
                                                    switch (cj_value_new->type)
                                                    {
                                                    case cJSON_True:
                                                    {
                                                        cJSON_AddBoolToObject(__FUNCTION__, cj_device_items_data, cj_item_names->valuestring, true); // store as {"202ec000" : true , ...}
                                                        break;
                                                    }
                                                    case cJSON_False:
                                                    {
                                                        cJSON_AddBoolToObject(__FUNCTION__, cj_device_items_data, cj_item_names->valuestring, false); // store as {"202ec000" : false , ...}
                                                        break;
                                                    }
                                                    case cJSON_Number:
                                                    {
                                                        cJSON_AddNumberToObject(__FUNCTION__, cj_device_items_data, cj_item_names->valuestring, cj_value_new->valuedouble); // store as {"202ec000" : 20 , ...}
                                                        break;
                                                    }
                                                    case cJSON_String:
                                                    {
                                                        cJSON_AddStringToObject(__FUNCTION__, cj_device_items_data, cj_item_names->valuestring, cj_value_new->valuestring); // store as {"202ec000" : "text" , ...}
                                                        break;
                                                    }
                                                    case cJSON_Array:
                                                    case cJSON_Object:
                                                    {
                                                        cJSON_AddItemToObject(__FUNCTION__, cj_device_items_data, cj_item_names->valuestring, cJSON_Duplicate(__FUNCTION__, cj_value_new, true)); // store as {"202ec000" : [{...},{...} ] , ...}
                                                        break;
                                                    }
                                                    default:
                                                        TRACE_E("cjson Value type mis-matched!");
                                                        break;
                                                    }
                                                }
                                                else /*compare*/
                                                {
                                                    cJSON *cj_prev_item_val = cJSON_GetObjectItem(__FUNCTION__, cj_device_items_data, cj_item_names->valuestring); // grab the specific {"202ec000" :"..."}
                                                    if (cj_prev_item_val)
                                                    {
                                                        if (cj_prev_item_val->type == cj_value_new->type) // making sure both prev & new values, are of same type.
                                                        {
                                                            // since 'save_flag' is true we will add the latest 'item_values' into user_arg.
                                                            switch (cj_value_new->type)
                                                            {
                                                            case cJSON_True:
                                                            case cJSON_False:
                                                            {
                                                                ret = (cj_prev_item_val->type != cj_value_new->type) ? 1 : 0;
                                                                break;
                                                            }
                                                            case cJSON_Number:
                                                            {
                                                                ret = (cj_prev_item_val->valuedouble != cj_value_new->valuedouble) ? 1 : 0;
                                                                break;
                                                            }
                                                            case cJSON_String:
                                                            {
                                                                ret = EZPI_STRNCMP_IF_EQUAL(cj_prev_item_val->valuestring, cj_value_new->valuestring, cj_prev_item_val->str_value_len, cj_value_new->str_value_len);
                                                                break;
                                                            }
                                                            case cJSON_Array:
                                                            case cJSON_Object:
                                                            {
                                                                ret = cJSON_Compare(cj_prev_item_val, cj_value_new, false);
                                                                break;
                                                            }
                                                            default:
                                                                TRACE_E("cj_value_new --> type mis-matched!");
                                                                break;
                                                            }
                                                        }
#ifdef CONFIG_EZPI_UTIL_TRACE_EN
                                                        else
                                                        {
                                                            TRACE_E("The old-value-type and new-value-type doesnot match");
                                                        }
#endif
                                                    }

#ifdef CONFIG_EZPI_UTIL_TRACE_EN
                                                    else
                                                    {
                                                        TRACE_E(" [%s] doesnot exist", cj_item_names->valuestring);
                                                    }
#endif
                                                }
                                            }
                                            cJSON_Delete(__FUNCTION__, cj_tmp_value);
                                        }
                                    }

                                    break;
                                }
                            }
                        }
                        curr_item_node = curr_item_node->next;
                    }
                }
            }
        }

        // store into user_arg 'save_flag == 1'
        if ((NULL == scene_node->when_block->fields->user_arg) && cj_device_items_data && (true == save_flag))
        {
#ifdef CONFIG_EZPI_UTIL_TRACE_EN
            CJSON_TRACE("first_device_item_data", cj_device_items_data);
#endif
            scene_node->when_block->fields->user_arg = (void *)cj_device_items_data;
            ret = 1;
        }
    }
    return ret;
}
int IS_itemstate_changed(s_item_exp_data_t *new_extract_data, l_fields_v2_t *start_field, l_fields_v2_t *finish_field, l_scenes_list_v2_t *scene_node)
{
    int ret = 0;
    s_item_exp_data_t *prev_extract_data = (s_item_exp_data_t *)(scene_node->when_block->fields->user_arg);
    if (new_extract_data) // new vs old data
    {
        uint32_t flag = 0;
        // ---> New flag= (000) : ---> 1. compare 'new_extract_data' with 'start_field  / ANY'  ---> if true ; start_flag =1 ; ---> store data
        // --> 1.a . if start_field = Any ; start_flag =1 & return 'true' |  Else compare
        // -----> For flag= (001): ------> 2. compare 'new_extract_data' with 'finish_field / ANY'  -----> if true ; flag = (011) ; ----> store data ;  flag = (111) : ---->
        // ------> 2.a . if finish = Any ; start_flag =1 & return 'true' |  Else compare
        // --------> For flag = (111) : --------> 3. if(start_field & final_field == NULL or 'ANY') ? exit ; else, flag = (000).

        if (NULL == prev_extract_data)
        {
            flag |= __isitemState_vs_field_compare(new_extract_data, start_field, 0);
            // TRACE_D("First-time check.... flag=%08x", flag);
        }
        else
        {
            flag = prev_extract_data->status;
            // TRACE_I("# Before comparision : status_flag=%08x", flag);
            switch (flag) // this says what to do next
            {
            case 0: // no condition match   ---> check the start condition only.
            {
                // TRACE_D("checking start conditon");
                flag |= __isitemState_vs_field_compare(prev_extract_data, start_field, 0);
                break;
            }
            case (BIT0): // Only start condition activated ; ----> so check for finish condition
            {
                // TRACE_D("checking finish conditon");
                flag |= __isitemState_vs_field_compare(new_extract_data, finish_field, 1);

                if (flag == (BIT1 | BIT0))
                {
                    // activate BIT2 in status ; if both start and finish condition are satisfied.
                    flag |= BIT2;
                }
                break;
            }
            case (BIT1 | BIT0): // condition match but not activated [or reset] ---> need to active here
            {
                // TRACE_I("====>  pre-state : (011) --> activating");
                flag |= BIT2;
                break;
            }
            case (BIT2 | BIT1 | BIT0): // all condition matched and activated  ---> check if state have changed --> if changed , reset activation flag
            {
#warning "need to optimize these switch-case";
                // comparing 'start'
                if (NULL != start_field)
                {
                    if (!(BIT0 & __isitemState_vs_field_compare(prev_extract_data, start_field, 0))) // if BIT0 = 0;
                    {
                        flag &= (0 << 2);
                        // TRACE_I(" HERE :- Reseting the 'BIT2' ");
                    }
                    else
                    {
                        // TRACE_D("start or finish condition is still satisfied.... cannot reset 'BIT2'");
                    }
                }
                else // 'ANY'
                {
                    if (!____old_vs_new_extract_data(new_extract_data, prev_extract_data))
                    {
                        flag &= (0 << 2);
                        // TRACE_I(" HERE :- Reseting the 'BIT2' ");
                    }
                    else
                    {
                        // TRACE_D(" new-data == old-data");
                    }
                }

                // comparing 'finish'
                if (NULL != finish_field)
                {
                    if (!(BIT1 & __isitemState_vs_field_compare(new_extract_data, finish_field, 1))) // if BIT0 = 0;
                    {
                        flag &= (0 << 2);
                        // TRACE_I(" HERE :- Reseting the 'BIT2' ");
                    }
                    else
                    {
                        // TRACE_D("start or finish condition is still satisfied.... cannot reset 'BIT2'");
                    }
                }
                else // 'ANY'
                {
                    if (!____old_vs_new_extract_data(new_extract_data, prev_extract_data))
                    {
                        flag &= (0 << 2);
                        // TRACE_I(" HERE :- Reseting the 'BIT2' ");
                    }
                    else
                    {
                        // TRACE_D(" new-data == old-data");
                    }
                }
                break;
            }
            default: // all remaining conditon are invalid ; So 'Reset' all flags and start again in next iteration.
            {
                // TRACE_D("Reseting.... to start fresh");
                // flag = 0;
                break;
            }
            }
        }

        // assign latest flag to new-structure.
        new_extract_data->status = flag;

        // Store the 'new-extracted' data
        if (NULL == scene_node->when_block->fields->user_arg)
        {
            // TRACE_S("---> Creating new structure");
            s_item_exp_data_t *tmp_struct = ezlopi_malloc(__FUNCTION__, sizeof(s_item_exp_data_t));
            if (tmp_struct)
            {
                memset(tmp_struct, 0, sizeof(s_item_exp_data_t));
                __replace_old_with_new_data_val(new_extract_data, tmp_struct);
                scene_node->when_block->fields->user_arg = (void *)tmp_struct;
            }
            else
            {
                TRACE_E("Malloc failed!!");
            }
        }
        else
        {
            // TRACE_W("---> Replacing old extract data");
            __replace_old_with_new_data_val(new_extract_data, (scene_node->when_block->fields->user_arg));
        }

        // TRACE_W("#### Final..... flag result=%08x  ####", flag);
        if (flag & BIT2)
        {
            // Also return 1 for 'then-method'
            // TRACE_I("====> Return 1");
            ret = 1;
        }
    }
    else
    {
        TRACE_E("new_extract_data == NULL");
    }
    return ret;
}

/*******************************************************************************
 *                         Static Function Definitions
 *******************************************************************************/
static int __compare_item_values(l_ezlopi_item_t *curr_item, l_fields_v2_t *value_field)
{
    int ret = 0;
    cJSON *cj_tmp_value = cJSON_CreateObject(__FUNCTION__);
    if (cj_tmp_value)
    {
        curr_item->func(EZLOPI_ACTION_GET_EZLOPI_VALUE, curr_item, (void *)cj_tmp_value, NULL);
        cJSON *cj_value = cJSON_GetObjectItem(__FUNCTION__, cj_tmp_value, ezlopi_value_str);
        if (cj_value)
        {
            switch (cj_value->type)
            {
            case cJSON_True:
            {
                if (true == value_field->field_value.u_value.value_bool)
                {
                    ret = 1;
                }
                break;
            }
            case cJSON_False:
            {
                if (false == value_field->field_value.u_value.value_bool)
                {
                    ret = 1;
                }
                break;
            }
            case cJSON_Number:
            {
                if (cj_value->valuedouble == value_field->field_value.u_value.value_double)
                {
                    ret = 1;
                }
                break;
            }
            case cJSON_String:
            {
                if (EZPI_STRNCMP_IF_EQUAL(cj_value->valuestring, value_field->field_value.u_value.value_string, cj_value->str_value_len, strlen(value_field->field_value.u_value.value_string) + 1))
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

        cJSON_Delete(__FUNCTION__, cj_tmp_value);
    }

    return ret;
}
static uint8_t __isdate_check_endweek_conditon(e_isdate_modes_t mode_type, struct tm *info)
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
static uint8_t __isdate_find_nth_week_curr_month(struct tm *info)
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
static void __issunsate_update_sunstate_tm(int tm_mday, s_sunstate_data_t *user_data)
{
    if (tm_mday && user_data)
    {
        char tmp_url[100] = {0};
        char tmp_headers[] = "Host: api.sunrisesunset.io\r\nAccept: */*\r\nConnection: keep-alive\r\n";
        char tmp_web_server[] = "api.sunrisesunset.io";
        char *lat_long_vals = EZPI_core_nvs_read_latidtude_longitude();
        if (lat_long_vals)
        {
            TRACE_S("long_lat_co-ordinate : %s", lat_long_vals);
            snprintf(tmp_url, 95, "%s", "https://api.sunrisesunset.io/json?lat=27.700769&lng=85.300140");
            ezlopi_free(__FUNCTION__, lat_long_vals);
        }
        else
        {
            // send httprequest to 'sunrisesunset.io' // use the latitude and longitude from NVS
            snprintf(tmp_url, 95, "%s", "https://api.sunrisesunset.io/json?lat=27.700769&lng=85.300140");
        }

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
        /*Make API call here and extract the suntime[according to 'user_data->sunstate_mode']*/

        EZPI_core_http_mbedtls_req(&tmp_config);
        // e.g. after valid extraction
        user_data->curr_tm_day = tm_mday;                                                    // this stores day for which data is extracted
        user_data->choosen_suntime.tm_hour = 5 + ((user_data->sunstate_mode == 2) ? 12 : 0); // sunrise = 1 ; sunset = 2
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
static void __issunstate_add_offs(e_issunstate_offset_t tmoffs_type, struct tm *choosen_suntime, struct tm *defined_moment, const char *tm_offs_val)
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
            defined_moment->tm_hour = (choosen_suntime->tm_hour - tmp_time.tm_hour);
            defined_moment->tm_hour = (defined_moment->tm_hour < 0) ? (24 + defined_moment->tm_hour) : defined_moment->tm_hour; // check the hour-range
            defined_moment->tm_min = (choosen_suntime->tm_min - tmp_time.tm_min);
            defined_moment->tm_min = (defined_moment->tm_min < 0) ? (60 + defined_moment->tm_min) : defined_moment->tm_min; // check the min-range
            defined_moment->tm_sec = (choosen_suntime->tm_sec - tmp_time.tm_sec);
            defined_moment->tm_sec = (defined_moment->tm_sec < 0) ? (60 + defined_moment->tm_sec) : defined_moment->tm_sec; // check the sec-range
            break;
        }
        case ISSUNSTATE_AFTER_MODE:
        {
            TRACE_S("offset : After");
            defined_moment->tm_hour = (choosen_suntime->tm_hour + tmp_time.tm_hour);
            defined_moment->tm_hour = (defined_moment->tm_hour > 23) ? (defined_moment->tm_hour - 24) : defined_moment->tm_hour; // check the hour-range
            defined_moment->tm_min = (choosen_suntime->tm_min + tmp_time.tm_min);
            defined_moment->tm_min = (defined_moment->tm_min > 59) ? (defined_moment->tm_min - 60) : defined_moment->tm_min; // check the min-range
            defined_moment->tm_sec = (choosen_suntime->tm_sec + tmp_time.tm_sec);
            defined_moment->tm_sec = (defined_moment->tm_sec > 59) ? (defined_moment->tm_sec - 60) : defined_moment->tm_sec; // check the sec-range
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
static bool __and_when_block_condition(l_scenes_list_v2_t *scene_node, l_when_block_v2_t *when_block)
{
    bool transition_state = 1; // to make valid judgements
    l_when_block_v2_t *value_when_block = when_block->fields->field_value.u_value.when_block;
    while (value_when_block)
    {
        f_scene_method_v2_t scene_method = EZPI_core_scenes_get_method_v2(value_when_block->block_options.method.type);
        if (scene_method)
        {
            transition_state &= scene_method(scene_node, (void *)value_when_block);
            if (!transition_state)
            {
                break;
            }
        }
        value_when_block = value_when_block->next;
    }
    return transition_state;
}
static uint8_t __isitemState_vs_field_compare(s_item_exp_data_t *new_extract_data, l_fields_v2_t *tmp_field, uint8_t bit_mode_position)
{
    uint8_t flag = 0;
    if (tmp_field)
    {
        switch (tmp_field->value_type)
        {
        case EZLOPI_VALUE_TYPE_ITEM:
        {
            cJSON *cj_item_value = cJSON_CreateObject(__FUNCTION__);
            if (cj_item_value)
            {
                l_ezlopi_item_t *tmp_item = EZPI_core_device_get_item_by_id(tmp_field->field_value.u_value.value_double);
                tmp_item->func(EZLOPI_ACTION_GET_EZLOPI_VALUE, tmp_item, (void *)cj_item_value, NULL);
                cJSON *cj_val = cJSON_GetObjectItem(__FUNCTION__, cj_item_value, ezlopi_value_str);
                if (cj_val)
                {
                    switch (cj_val->type)
                    {
                    case cJSON_String:
                    {
                        if (cj_val->valuestring && new_extract_data->sample_data.u_value.value_string)
                        {
                            if (EZPI_STRNCMP_IF_EQUAL(cj_val->valuestring, new_extract_data->sample_data.u_value.value_string, cj_val->str_value_len, strlen(new_extract_data->sample_data.u_value.value_string)))
                            {
                                flag |= (1 << bit_mode_position);
                            }
                        }
                        break;
                    }
                    case cJSON_Number:
                    {
                        flag |= (cj_val->valuedouble == new_extract_data->sample_data.u_value.value_double) ? (1 << bit_mode_position) : 0;
                        break;
                    }
                    case cJSON_True: // bool_values can be converted to 1/0s
                    {
                        flag |= (new_extract_data->sample_data.u_value.value_bool) ? (1 << bit_mode_position) : 0;
                        break;
                    }
                    case cJSON_False: // bool_values can be converted to 1/0s
                    {
                        flag |= (!new_extract_data->sample_data.u_value.value_bool) ? (1 << bit_mode_position) : 0;
                        break;
                    }
                    default:
                        TRACE_E("Error!! (ITEM_value) can compare only :- string / bool / number ");
                        break;
                    }
                }

                cJSON_Delete(__FUNCTION__, cj_item_value);
            }
            break;
        }
        case EZLOPI_VALUE_TYPE_EXPRESSION:
        {
            s_ezlopi_expressions_t *tmp_expr = EZPI_scenes_expressions_get_node_by_name(tmp_field->field_value.u_value.value_string);
            if (tmp_expr)
            {
                switch (tmp_expr->exp_value.type)
                {
                case EXPRESSION_VALUE_TYPE_BOOL:
                {
                    flag |= (tmp_expr->exp_value.u_value.boolean_value == new_extract_data->sample_data.u_value.value_double) ? (1 << bit_mode_position) : 0;
                    break;
                }
                case EXPRESSION_VALUE_TYPE_NUMBER:
                {
                    flag |= (tmp_expr->exp_value.u_value.number_value == new_extract_data->sample_data.u_value.value_double) ? (1 << bit_mode_position) : 0;
                    break;
                }
                case EXPRESSION_VALUE_TYPE_STRING:
                {
                    if (tmp_expr->exp_value.u_value.str_value && new_extract_data->sample_data.u_value.value_string)
                    {
                        if (EZPI_STRNCMP_IF_EQUAL(tmp_expr->exp_value.u_value.str_value,
                                                  new_extract_data->sample_data.u_value.value_string,
                                                  strlen(tmp_expr->exp_value.u_value.str_value),
                                                  strlen(new_extract_data->sample_data.u_value.value_string)))
                        {
                            flag |= (1 << bit_mode_position);
                        }
                    }
                    break;
                }
                default:
                    TRACE_E("Error!! (Expression_value) can compare only :- string / bool / number ");
                    break;
                }
            }
            break;
        }
        case EZLOPI_VALUE_TYPE_INT:
        {
            flag |= (tmp_field->field_value.u_value.value_double == new_extract_data->sample_data.u_value.value_double) ? (1 << bit_mode_position) : 0;
            break;
        }
        case EZLOPI_VALUE_TYPE_BOOL:
        {
            flag |= (tmp_field->field_value.u_value.value_bool == new_extract_data->sample_data.u_value.value_bool) ? (1 << bit_mode_position) : 0;
            break;
        }
        case EZLOPI_VALUE_TYPE_STRING:
        {
            if (tmp_field->field_value.u_value.value_string && new_extract_data->sample_data.u_value.value_string)
            {
                if (EZPI_STRNCMP_IF_EQUAL(tmp_field->field_value.u_value.value_string,
                                          new_extract_data->sample_data.u_value.value_string,
                                          strlen(tmp_field->field_value.u_value.value_string),
                                          strlen(new_extract_data->sample_data.u_value.value_string)))
                {
                    flag |= (1 << bit_mode_position);
                }
            }
            break;
        }
        default:
            TRACE_E("Error!! Field-type  only support :- Item / Expression / string / bool / number ");
            break;
        }
    }
    else
    {
        flag |= (1 << bit_mode_position);
    }
    return flag;
}
static void __replace_old_with_new_data_val(s_item_exp_data_t *new_extract_data, s_item_exp_data_t *prev_extract_data)
{
    if (new_extract_data && prev_extract_data)
    {
        prev_extract_data->status = new_extract_data->status;
        prev_extract_data->value_type = new_extract_data->value_type;
        prev_extract_data->sample_data.e_type = new_extract_data->sample_data.e_type;

        switch (new_extract_data->sample_data.e_type)
        {
        case VALUE_TYPE_BOOL:
        {
            prev_extract_data->sample_data.u_value.value_bool = new_extract_data->sample_data.u_value.value_bool;
            break;
        }
        case VALUE_TYPE_NUMBER:
        {
            prev_extract_data->sample_data.u_value.value_double = new_extract_data->sample_data.u_value.value_double;
            break;
        }
        case VALUE_TYPE_STRING:
        {
            if (new_extract_data->sample_data.u_value.value_string)
            {
                uint32_t value_len = strlen(new_extract_data->sample_data.u_value.value_string) + 1;
                {
                    if (prev_extract_data->sample_data.u_value.value_string) // erasing prev-string
                    {
                        ezlopi_free(__FUNCTION__, prev_extract_data->sample_data.u_value.value_string);
                    }
                    prev_extract_data->sample_data.u_value.value_string = ezlopi_malloc(__FUNCTION__, value_len);
                    if (prev_extract_data->sample_data.u_value.value_string)
                    {
                        snprintf(prev_extract_data->sample_data.u_value.value_string, value_len, "%s", new_extract_data->sample_data.u_value.value_string);
                    }
                }
            }
            break;
        }
        default:
            break;
        }
    }
    else
    {
        TRACE_E("Invalid operation.... null structure");
    }
}

static int ____old_vs_new_extract_data(s_item_exp_data_t *new_extract_data, s_item_exp_data_t *prev_extract_data)
{
    int ret = 0;
    switch (new_extract_data->sample_data.e_type)
    {
    case VALUE_TYPE_BOOL:
    {
        if (prev_extract_data->sample_data.u_value.value_bool == new_extract_data->sample_data.u_value.value_bool)
        {
            ret = 1;
        }
        break;
    }
    case VALUE_TYPE_NUMBER:
    {
        if (prev_extract_data->sample_data.u_value.value_double == new_extract_data->sample_data.u_value.value_double)
        {
            ret = 1;
        }
        break;
    }
    case VALUE_TYPE_STRING:
    {
        if (new_extract_data->sample_data.u_value.value_string && prev_extract_data->sample_data.u_value.value_string)
        {
            if (EZPI_STRNCMP_IF_EQUAL(
                    new_extract_data->sample_data.u_value.value_string,
                    prev_extract_data->sample_data.u_value.value_string,
                    strlen(new_extract_data->sample_data.u_value.value_string),
                    strlen(prev_extract_data->sample_data.u_value.value_string)))
            {
                ret = 1;
            }
        }
        break;
    }
    default:
        break;
    }
    return ret; // if matched --> return 1
}

#endif // CONFIG_EZPI_SERV_ENABLE_MESHBOTS

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
