/* ===========================================================================
** Copyright (C) 2024 Ezlo Innovation Inc
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
 * @file    ezlopi_core_scenes_when_methods_helper_functions.h
 * @brief   Functions that operates on scene-when-methods
 * @author  Krishna Kumar Sah (work.krishnasah@gmail.com)
 *          Lomas Subedi
 *          Riken Maharjan
 *          Nabin Dangi
 * @version 0.1
 * @date    12th DEC 2024
 */

#ifndef _EZLOPI_CORE_SCENES_WHEN_METHODS_HELPER_FUNCTIONS_H_
#define _EZLOPI_CORE_SCENES_WHEN_METHODS_HELPER_FUNCTIONS_H_

#ifdef CONFIG_EZPI_SERV_ENABLE_MESHBOTS

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include <time.h>
#include <stdint.h>
#include "cjext.h"

/*******************************************************************************
 *                          C++ Declaration Wrapper
 *******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

    /*******************************************************************************
     *                          Type & Macro Declarations
     *******************************************************************************/
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
        const char *field_name;
        uint8_t (*field_func)(e_isdate_modes_t *mode_type, struct tm *info, l_fields_v2_t *curr_field);
    } s_isdate_method_t;

    typedef struct s_issunstate_method
    {
        const char *field_name;
        uint8_t (*field_func)(l_scenes_list_v2_t *scene_node, l_fields_v2_t *curr_field, struct tm *info, uint8_t curr_sunstate_mode);
    } s_issunstate_method_t;

    typedef struct s_isonce_method
    {
        const char *field_name;
        uint8_t (*field_func)(l_fields_v2_t *curr_field, struct tm *info);
    } s_isonce_method_t;

    typedef struct s_isdate_range_method
    {
        const char *field_name;
        void (*field_func)(l_fields_v2_t *curr_field, struct tm *tmp_tm);
    } s_isdate_range_method_t;

    typedef struct s_function_opr
    {
        const char *opr_name;
        int (*opr_method)(l_scenes_list_v2_t *scene_node, l_when_block_v2_t *when_block, cJSON *cj_func_opr);
    } s_function_opr_t;

    typedef union s_item_expn_data
    {
        e_value_type_t e_type;      // the data's nature
        u_field_value_v2_t u_value; // reutilizing the field structure to store extracted data
    } s_item_expn_data_t;

    typedef struct s_item_exp_data
    {
        s_item_expn_data_t sample_data;     // this contains value
        e_scene_value_type_v2_t value_type; // all scenes_value_types
        uint8_t status;                     // This is used to identify what to do next. | [BIT2 =activation_flag , BIT1 =finish_flag , BIT2 =start_flag]
    } s_item_exp_data_t;

    /*******************************************************************************
     *                          Extern Data Declarations
     *******************************************************************************/

    /*******************************************************************************
     *                          Extern Function Prototypes
     *******************************************************************************/

    //------------------------------- EZPI_core_scenes_when_is_itemState ------------------------------------------
    int ISITEM_state_single_condition(uint32_t item_id, l_fields_v2_t *value_field);
    int ISITEM_state_with_grp_condition(uint32_t device_group_id, uint32_t item_group_id, l_fields_v2_t *value_field);

    //------------------------------- EZPI_core_scenes_when_is_date -----------------------------------------------
    uint8_t ISDATE_type_check(e_isdate_modes_t *mode_type, struct tm *info, l_fields_v2_t *curr_field);
    uint8_t ISDATE_tm_check(e_isdate_modes_t *mode_type, struct tm *info, l_fields_v2_t *curr_field);
    uint8_t ISDATE_weekdays_check(e_isdate_modes_t *mode_type, struct tm *info, l_fields_v2_t *curr_field);
    uint8_t ISDATE_mdays_check(e_isdate_modes_t *mode_type, struct tm *info, l_fields_v2_t *curr_field);
    uint8_t ISDATE_year_weeks_check(e_isdate_modes_t *mode_type, struct tm *info, l_fields_v2_t *curr_field);
    int ISDATE_check_flag_result(e_isdate_modes_t mode_type, uint8_t flag_check);

    //------------------------------- EZPI_core_scenes_when_is_SunState -------------------------------------------
    uint8_t ISSUNSTATE_check_mdrn(uint8_t sunstate_mode, const char *range_type, struct tm *info, struct tm *defined_moment);
    uint8_t ISSUNSTATE_get_suntime(l_scenes_list_v2_t *scene_node, l_fields_v2_t *curr_field, struct tm *info, uint8_t curr_sunstate_mode);
    uint8_t ISSUNSTATE_get_offs_tmval(l_scenes_list_v2_t *scene_node, l_fields_v2_t *curr_field, struct tm *info, uint8_t curr_sunstate_mode);
    uint8_t ISSUNSTATE_eval_weekdays(l_scenes_list_v2_t *scene_node, l_fields_v2_t *curr_field, struct tm *info, uint8_t curr_sunstate_mode);
    uint8_t ISSUNSTATE_eval_days(l_scenes_list_v2_t *scene_node, l_fields_v2_t *curr_field, struct tm *info, uint8_t curr_sunstate_mode);
    uint8_t ISSUNSTATE_eval_range(l_scenes_list_v2_t *scene_node, l_fields_v2_t *curr_field, struct tm *info, uint8_t curr_sunstate_mode);
    int ISSUNSTATE_check_flag_result(l_scenes_list_v2_t *scene_node, struct tm *info, uint8_t flag_check);

    //------------------------------- EZPI_core_scenes_when_is_once ---------------------------------------------------
    uint8_t ISONCE_tm_check(l_fields_v2_t *curr_field, struct tm *info);
    uint8_t ISONCE_day_check(l_fields_v2_t *curr_field, struct tm *info);
    uint8_t ISONCE_month_check(l_fields_v2_t *curr_field, struct tm *info);
    uint8_t ISONCE_year_check(l_fields_v2_t *curr_field, struct tm *info);
    int ISONCE_check_flag_result(l_scenes_list_v2_t *scene_node, uint8_t flag_check);

    //------------------------------- EZPI_core_scenes_when_is_date_range -----------------------------------------
    void ISDATE_range_get_tm(l_fields_v2_t *curr_field, struct tm *tmp_tm);
    void ISDATE_range_get_startday(l_fields_v2_t *curr_field, struct tm *tmp_tm);
    void ISDATE_range_get_endday(l_fields_v2_t *curr_field, struct tm *tmp_tm);
    void ISDATE_range_get_startmonth(l_fields_v2_t *curr_field, struct tm *tmp_tm);
    void ISDATE_range_get_endmonth(l_fields_v2_t *curr_field, struct tm *tmp_tm);
    void ISDATE_range_get_startyear(l_fields_v2_t *curr_field, struct tm *tmp_tm);
    void ISDATE_range_get_endyear(l_fields_v2_t *curr_field, struct tm *tmp_tm);
    uint8_t ISDATE_range_check_tm(struct tm *start, struct tm *end, struct tm *info);
    uint8_t ISDATE_range_check_day(struct tm *start, struct tm *end, struct tm *info);
    uint8_t ISDATE_range_check_month(struct tm *start, struct tm *end, struct tm *info);
    uint8_t ISDATE_range_check_year(struct tm *start, struct tm *end, struct tm *info);
    int ISDATE_range_check_flag_result(uint8_t flag_check);

    //------------------------------- EZPI_core_scene_when_function_method -----------------------------------------
    int WHEN_function_for_opr(l_scenes_list_v2_t *scene_node, l_when_block_v2_t *when_block, cJSON *cj_func_opr);
    int WHEN_function_for_repeat(l_scenes_list_v2_t *scene_node, l_when_block_v2_t *when_block, cJSON *cj_func_opr);
    int WHEN_function_for_follow(l_scenes_list_v2_t *scene_node, l_when_block_v2_t *when_block, cJSON *cj_func_opr);
    int WHEN_function_for_pulse(l_scenes_list_v2_t *scene_node, l_when_block_v2_t *when_block, cJSON *cj_func_opr);
    int WHEN_function_for_latch(l_scenes_list_v2_t *scene_node, l_when_block_v2_t *when_block, cJSON *cj_func_opr);

    //------------------------------- EZPI_core_scene_when_isDeviceItemGroup_method -----------------------------------
    int IS_deviceitem_group_value_check(l_scenes_list_v2_t *scene_node, uint32_t device_group_id, uint32_t item_group_id);

    //------------------------------- EZPI_core_scene_when_isItemStateChanged_method -----------------------------------
    int IS_itemstate_changed(s_item_exp_data_t *new_user_data, l_fields_v2_t *start_field, l_fields_v2_t *finish_field, l_scenes_list_v2_t *scene_node);

#ifdef __cplusplus
}
#endif

#endif // CONFIG_EZPI_SERV_ENABLE_MESHBOTS

#endif /*_EZLOPI_CORE_SCENES_WHEN_METHODS_HELPER_FUNCTIONS_H_*/
       /*******************************************************************************
        *                          End of File
        *******************************************************************************/