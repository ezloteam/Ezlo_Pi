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
 * @file    ezlopi_core_scenes_v2_type_declaration.h
 * @brief   This file contains all scene-related Type-defines
 * @author  Krishna Kumar Sah (work.krishnasah@gmail.com)
 *          Lomas Subedi
 *          Riken Maharjan
 *          Nabin Dangi
 * @version 0.1
 * @date    12th DEC 2024
 */

#ifndef _EZLOPI_CORE_SCENES_V2_TYPE_DECLARATION_H_
#define _EZLOPI_CORE_SCENES_V2_TYPE_DECLARATION_H_

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
// #include <stdbool.h>
// #include <stdint.h>
// #include <ctype.h>
#include "cjext.h"

#include "ezlopi_core_scenes_methods.h"

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
    typedef struct s_when_function
    {
        uint32_t transtion_instant;
        uint32_t transition_count;
        bool current_state;
        bool activate_pulse_seq; /* used only in 'for_pulse_method' */
    } s_when_function_t;
    typedef enum e_scenes_block_type_v2
    {
        SCENE_BLOCK_TYPE_NONE = 0,
        SCENE_BLOCK_TYPE_WHEN = 1,
        SCENE_BLOCK_TYPE_THEN = 2,
        SCENE_BLOCK_TYPE_ELSE = 3,
        SCENE_BLOCK_TYPE_MAX,
    } e_scenes_block_type_v2_t;

    typedef enum e_scene_status_v2
    {
        EZLOPI_SCENE_STATUS_NONE = 0,
        EZLOPI_SCENE_STATUS_RUN,     // command
        EZLOPI_SCENE_STATUS_RUNNING, // state
        EZLOPI_SCENE_STATUS_STOP,    // command
        EZLOPI_SCENE_STATUS_STOPPED, // state
        EZLOPI_SCENE_STATUS_MAX
    } e_scene_status_v2_t;

    typedef struct s_method_v2
    {
        char name[32];
        e_scene_method_type_t type;
    } s_method_v2_t;

    typedef struct s_block_options_v2
    {
        s_method_v2_t method;
        cJSON *cj_function;
    } s_block_options_v2_t;

    typedef struct s_action_delay_v2
    {
        uint16_t days;
        uint16_t hours;
        uint16_t minutes;
        uint16_t seconds;
    } s_action_delay_v2_t;

    typedef enum e_value_type
    {
        VALUE_TYPE_UNDEFINED = 0,
        VALUE_TYPE_NUMBER,
        VALUE_TYPE_STRING,
        VALUE_TYPE_BOOL,
        VALUE_TYPE_CJSON,
        VALUE_TYPE_BLOCK,
        VALUE_TYPE_HOUSE_MODE_ID_ARRAY,
        VALUE_TYPE_MAX
    } e_value_type_t;

    typedef union u_field_value_v2
    {
        char *value_string;
        double value_double;
        bool value_bool;
        cJSON *cj_value;
        struct l_when_block_v2 *when_block;
        struct l_house_modes_v2 *house_modes;
    } u_field_value_v2_t;

    typedef struct s_field_value
    {
        e_value_type_t e_type;
        u_field_value_v2_t u_value;
    } s_field_value_t;

    typedef enum e_scene_value_type_v2
    {
#define EZLOPI_VALUE_TYPE(type, name) EZLOPI_VALUE_TYPE_##type,
#include "ezlopi_core_scenes_value_types.h"
#undef EZLOPI_VALUE_TYPE
    } e_scene_value_type_v2_t;

    typedef struct l_fields_v2
    {
        char name[32];
        s_field_value_t field_value;
        e_scene_value_type_v2_t value_type; // 0: double, 1: string
        char *scale;
        void *user_arg; // used by when-methods
        struct l_fields_v2 *next;

    } l_fields_v2_t;

    typedef struct l_action_block_v2
    {
        char _tempId[40];
        e_scenes_block_type_v2_t block_type;
        s_block_options_v2_t block_options;
        s_action_delay_v2_t delay;
        l_fields_v2_t *fields;
        struct l_action_block_v2 *next;
    } l_action_block_v2_t;

    typedef struct l_group_block_type
    {
        uint32_t grp_id;        // may be used in future    //  currently not-populated from nvs
        bool grp_state;         //  result of the group_block --> 1/0
        char grp_blockName[32]; //  actual -> 'groupName'   //  The Group-Name provided by UI ; to indicate a group // e.g. ["blockName" : "group-A"]
    } l_group_block_type_t;

    typedef struct l_when_block_v2
    {
        s_block_options_v2_t block_options;
        l_group_block_type_t *when_grp;      //   if(!NULL) ------------> //  indicates the 'when-block' is 'group_type'.
        l_fields_v2_t *fields;               //
        cJSON *cj_block_meta;                //   Block metadata information. Intended to save data needed for user interfaces
        e_scenes_block_type_v2_t block_type; //
        uint32_t blockId;                    //   actual -> '_ID'         //  The ID of a normal when-condition scene-block;
        bool block_enable;                   //   actual -> '_enable'     //  flag that allows blocks to return 1;
        bool block_status_reset_once;        //   NOT-NVS parameter [don't populate ; since not needed] // just a dummy flag to trigger function reset.
        struct l_when_block_v2 *next;
    } l_when_block_v2_t;

    // typedef struct l_else_block_v2
    // {
    //     char _tempId[40];
    //     e_scenes_block_type_v2_t block_type;
    //     s_block_options_v2_t block_options;
    //     s_action_delay_v2_t delay;
    //     l_fields_v2_t *fields;
    //     struct l_else_block_v2 *next;
    // } l_else_block_v2_t;

    typedef struct l_user_notification_v2
    {
        char user_id[32];
        struct l_user_notification_v2 *next;
    } l_user_notification_v2_t;

    typedef struct l_house_modes_v2
    {
        char house_mode[8];
        struct l_house_modes_v2 *next;
    } l_house_modes_v2_t;

    typedef struct l_scenes_list_v2
    {
        e_scene_status_v2_t status;
        TaskHandle_t task_handle;

        char name[32];
        char parent_id[32];
        void *thread_ctx;
        cJSON *meta;
        uint32_t _id;
        uint32_t group_id;
        uint32_t executed_date;
        bool enabled;
        bool is_group;

        l_user_notification_v2_t *user_notifications;
        l_house_modes_v2_t *house_modes;
        l_action_block_v2_t *then_block;
        l_when_block_v2_t *when_block;
        l_action_block_v2_t *else_block;

        struct l_scenes_list_v2 *next;
    } l_scenes_list_v2_t;

    typedef int (*f_scene_method_v2_t)(l_scenes_list_v2_t *curr_scene, void *arg);
    /*******************************************************************************
     *                          Extern Data Declarations
     *******************************************************************************/

    /*******************************************************************************
     *                          Extern Function Prototypes
     *******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif // _EZLOPI_CORE_SCENES_V2_TYPE_DECLARATION_H_

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
