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
* @file    ezlopi_core_modes_cjson.c
* @brief   These functions perform operations related to house-mode cjsons
* @author  xx
* @version 0.1
* @date    12th DEC 2024
*/

/*******************************************************************************
*                          Include Files
*******************************************************************************/

// #include <time.h>
// #include "cjext.h"

// #include "ezlopi_util_trace.h"

// #include "ezlopi_core_modes.h"
// #include "ezlopi_core_errors.h"
#include "ezlopi_core_nvs.h"
#include "ezlopi_core_cloud.h"
#include "ezlopi_core_devices.h"
#include "ezlopi_core_modes_cjson.h"
#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_core_sntp.h"

#include "ezlopi_cloud_constants.h"
#include "EZLOPI_USER_CONFIG.h"

#if defined(CONFIG_EZPI_SERV_ENABLE_MODES)

/*******************************************************************************
*                          Extern Data Declarations
*******************************************************************************/

/*******************************************************************************
*                          Extern Function Declarations
*******************************************************************************/

/*******************************************************************************
*                          Type & Macro Definitions
*******************************************************************************/

/*******************************************************************************
*                          Static Function Prototypes
*******************************************************************************/
static void __cjson_add_alarmed(cJSON *cj_alarmed, s_alarmed_t *alarmed);
static void __cjson_add_mode_to_array(cJSON *cj_modes_arr, s_house_modes_t *mode);
static void __cjson_add_entry_delay(cJSON *cj_result, s_entry_delay_t *entry_delay);
static void __cjson_add_abort_window(cJSON *cj_result, s_abort_window_t *abort_delay);
static void __cjson_add_number_as_hex_string(cJSON *cj_dest, const char *obj_name, uint32_t number);
static void __cjson_duplicate_add_reference(cJSON *cj_dest, const char *item_name_str, cJSON *cj_item);
static void __cjson_add_protect_buttons(cJSON *cj_protect_buttons_arr, s_protect_buttons_t *l_protect_buttons);

/*******************************************************************************
*                          Static Data Definitions
*******************************************************************************/

/*******************************************************************************
*                          Extern Data Definitions
*******************************************************************************/

/*******************************************************************************
*                          Extern Function Definitions
*******************************************************************************/
ezlopi_error_t EZPI_core_modes_cjson_get_modes(cJSON *cj_dest)
{
    ezlopi_error_t ret = EZPI_SUCCESS;
    s_ezlopi_modes_t *_modes = EZPI_core_modes_get_custom_modes();
    if (_modes)
    {
        CJSON_ASSIGN_NUMBER_AS_STRING(cj_dest, _modes->current_mode_id, ezlopi_current_str);
        if (_modes->switch_to_mode_id)
        {
            CJSON_ASSIGN_NUMBER_AS_STRING(cj_dest, _modes->switch_to_mode_id, ezlopi_switchTo_str);
        }
        else
        {
            cJSON_AddStringToObject(__FUNCTION__, cj_dest, ezlopi_switchTo_str, ezlopi__str);
        }

        cJSON_AddNumberToObject(__FUNCTION__, cj_dest, ezlopi_timeIsLeftToSwitch_str, _modes->time_is_left_to_switch_sec);
        cJSON_AddNumberToObject(__FUNCTION__, cj_dest, ezlopi_switchToDelay_str, _modes->switch_to_delay_sec);
        cJSON_AddNumberToObject(__FUNCTION__, cj_dest, ezlopi_alarmDelay_str, _modes->alarm_delay); // utilizing the alarm-delay variables in 'alarmed-struct'

        __cjson_add_entry_delay(cj_dest, &_modes->entry_delay);
        __cjson_add_abort_window(cj_dest, &_modes->abort_delay);

        cJSON_AddNumberToObject(__FUNCTION__, cj_dest, ezlopi_switchToDelayRunsOut_str, _modes->switch_to_delay_runs_out);

        cJSON *cj_modes_array = cJSON_AddArrayToObject(__FUNCTION__, cj_dest, ezlopi_modes_str);
        if (cj_modes_array)
        {
            __cjson_add_mode_to_array(cj_modes_array, &_modes->mode_home);
            __cjson_add_mode_to_array(cj_modes_array, &_modes->mode_away);
            __cjson_add_mode_to_array(cj_modes_array, &_modes->mode_night);
            __cjson_add_mode_to_array(cj_modes_array, &_modes->mode_vacation);
        }

        __cjson_duplicate_add_reference(cj_dest, ezlopi_devices_str, _modes->cj_devices); // Adding list of security devices
        __cjson_duplicate_add_reference(cj_dest, ezlopi_alarms_str, _modes->cj_alarms);   // Array of device id which make alarms after trips
        __cjson_duplicate_add_reference(cj_dest, ezlopi_cameras_str, _modes->cj_cameras);

        cJSON *cj_protect_buttons_arr = cJSON_AddArrayToObject(__FUNCTION__, cj_dest, ezlopi_protectButtons_str);
        if (cj_protect_buttons_arr)
        {
            __cjson_add_protect_buttons(cj_protect_buttons_arr, _modes->l_protect_buttons);
        }

        cJSON *cj_alarmed = cJSON_AddObjectToObject(__FUNCTION__, cj_dest, ezlopi_alarmed_str);
        if (cj_alarmed)
        {
            __cjson_add_alarmed(cj_alarmed, &_modes->alarmed); // Marks house modes with 'armed' flag.
        }
    }

    return ret;
}

ezlopi_error_t EZPI_core_modes_cjson_get_current_mode(cJSON *cj_dest)
{
    ezlopi_error_t ret = EZPI_FAILED;
    s_ezlopi_modes_t *modes = EZPI_core_modes_get_custom_modes();
    if (modes)
    {
        ret = EZPI_SUCCESS;
        CJSON_ASSIGN_NUMBER_AS_STRING(cj_dest, modes->current_mode_id, ezlopi_modeId_str);
    }

    return ret;
}

s_ezlopi_modes_t *EZPI_core_modes_cjson_parse_modes(cJSON *cj_modes) // This function extracts 'mode_values' from cjson
{
    int _parsing_status = 0;
    s_ezlopi_modes_t *parsed_mode = (s_ezlopi_modes_t *)ezlopi_malloc(__FUNCTION__, sizeof(s_ezlopi_modes_t));
    if (parsed_mode)
    {
        memset(parsed_mode, 0, sizeof(s_ezlopi_modes_t));

        CJSON_GET_ID(parsed_mode->current_mode_id, cJSON_GetObjectItem(__FUNCTION__, cj_modes, ezlopi_current_str));
        CJSON_GET_ID(parsed_mode->switch_to_mode_id, cJSON_GetObjectItem(__FUNCTION__, cj_modes, ezlopi_switchTo_str));
        CJSON_GET_VALUE_DOUBLE(cj_modes, ezlopi_timeIsLeftToSwitch_str, parsed_mode->time_is_left_to_switch_sec);
        CJSON_GET_VALUE_DOUBLE(cj_modes, ezlopi_switchToDelay_str, parsed_mode->switch_to_delay_sec);
        CJSON_GET_VALUE_DOUBLE(cj_modes, ezlopi_alarmDelay_str, parsed_mode->alarm_delay); // "delay used" before switching to alarm assigned to this 'MODE'

        cJSON *cj_entry_delay = cJSON_GetObjectItem(__FUNCTION__, cj_modes, ezlopi_entryDelay_str);
        if (cj_entry_delay)
        {
            CJSON_GET_VALUE_DOUBLE(cj_entry_delay, ezlopi_normal_str, parsed_mode->entry_delay.normal_delay_sec);
            CJSON_GET_VALUE_DOUBLE(cj_entry_delay, ezlopi_long_extended_str, parsed_mode->entry_delay.long_extended_delay_sec);
            CJSON_GET_VALUE_DOUBLE(cj_entry_delay, ezlopi_extended_str, parsed_mode->entry_delay.extended_delay_sec);
            CJSON_GET_VALUE_DOUBLE(cj_entry_delay, ezlopi_instant_str, parsed_mode->entry_delay.instant_delay_sec);
        }

        cJSON *cj_abort_window = cJSON_GetObjectItem(__FUNCTION__, cj_modes, ezlopi_abortWindow_str);
        if (cj_abort_window)
        {
            CJSON_GET_VALUE_DOUBLE(cj_abort_window, ezlopi_default_str, parsed_mode->abort_delay.default_delay_sec);
            CJSON_GET_VALUE_DOUBLE(cj_abort_window, ezlopi_minimum_str, parsed_mode->abort_delay.minimum_delay_sec);
            CJSON_GET_VALUE_DOUBLE(cj_abort_window, ezlopi_maximum_str, parsed_mode->abort_delay.maximum_delay_sec);
        }

        cJSON *cj_house_modes = cJSON_GetObjectItem(__FUNCTION__, cj_modes, ezlopi_modes_str);
        if (cj_house_modes)
        {
            cJSON *cj_house_mod = NULL;

            cJSON_ArrayForEach(cj_house_mod, cj_house_modes)
            {
                CJSON_TRACE("cj_house_mod", cj_house_mod);
                uint32_t _mode_id = 0;
                CJSON_GET_ID(_mode_id, cJSON_GetObjectItem(__FUNCTION__, cj_house_mod, ezlopi__id_str));
                // TRACE_D("Mode-id: %d", _mode_id);

                s_house_modes_t *cur_house_mode = NULL;

                switch (_mode_id)
                {
                case EZLOPI_HOUSE_MODE_REF_ID_HOME:
                {
                    cur_house_mode = &parsed_mode->mode_home;
                    parsed_mode->mode_home._id = EZLOPI_HOUSE_MODE_REF_ID_HOME;
                    parsed_mode->mode_home.name = ezlopi_Home_str;
                    break;
                }
                case EZLOPI_HOUSE_MODE_REF_ID_AWAY:
                {
                    cur_house_mode = &parsed_mode->mode_away;
                    parsed_mode->mode_away._id = EZLOPI_HOUSE_MODE_REF_ID_AWAY;
                    parsed_mode->mode_away.name = ezlopi_Away_str;
                    break;
                }
                case EZLOPI_HOUSE_MODE_REF_ID_NIGHT:
                {
                    cur_house_mode = &parsed_mode->mode_night;
                    parsed_mode->mode_night._id = EZLOPI_HOUSE_MODE_REF_ID_NIGHT;
                    parsed_mode->mode_night.name = ezlopi_Night_str;
                    break;
                }
                case EZLOPI_HOUSE_MODE_REF_ID_VACATION:
                {
                    cur_house_mode = &parsed_mode->mode_vacation;
                    parsed_mode->mode_vacation._id = EZLOPI_HOUSE_MODE_REF_ID_VACATION;
                    parsed_mode->mode_vacation.name = ezlopi_Vacation_str;
                    break;
                }
                default:
                {
                    TRACE_E("Undefined house-mode-id!");
                    break;
                }
                }

                {
                    cJSON *cj_description = cJSON_GetObjectItem(__FUNCTION__, cj_house_mod, ezlopi_description_str);
                    if (cj_description && cj_description->valuestring)
                    {
                        uint32_t desc_len = (cj_description->str_value_len) + 1;
                        cur_house_mode->description = (char *)ezlopi_malloc(__FUNCTION__, desc_len);
                        if (cur_house_mode->description)
                        {
                            snprintf(cur_house_mode->description, desc_len, "%s", cj_description->valuestring);
                        }
                    }
                }

                {
                    cJSON *cj_notification = cJSON_GetObjectItem(__FUNCTION__, cj_house_mod, ezlopi_notifications_str);
                    if (cj_notification)
                    {
                        cur_house_mode->cj_notifications = cJSON_Duplicate(__FUNCTION__, cj_notification, cJSON_True);
                    }
                }

                CJSON_GET_VALUE_BOOL(cj_house_mod, ezlopi_disarmedDefault_str, cur_house_mode->disarmed_default);
                {
                    cJSON *cj_disarmed_devices = cJSON_GetObjectItem(__FUNCTION__, cj_house_mod, ezlopi_disarmedDevices_str);
                    if (cj_disarmed_devices)
                    {
                        cur_house_mode->cj_disarmed_devices = cJSON_Duplicate(__FUNCTION__, cj_disarmed_devices, cJSON_True);
                    }
                }

                {
                    cJSON *cj_alarms_off_devices = cJSON_GetObjectItem(__FUNCTION__, cj_house_mod, ezlopi_alarmsOffDevices_str);
                    if (cj_alarms_off_devices)
                    {
                        cur_house_mode->cj_alarms_off_devices = cJSON_Duplicate(__FUNCTION__, cj_alarms_off_devices, cJSON_True);
                    }
                }

                {
                    cJSON *cj_cameras_off_devices = cJSON_GetObjectItem(__FUNCTION__, cj_house_mod, ezlopi_camerasOffDevices_str);
                    if (cj_cameras_off_devices)
                    {
                        cur_house_mode->cj_cameras_off_devices = cJSON_Duplicate(__FUNCTION__, cj_cameras_off_devices, cJSON_True);
                    }
                }

                {
                    cJSON *cj_bypass_devices = cJSON_GetObjectItem(__FUNCTION__, cj_house_mod, ezlopi_bypassDevices_str);
                    if (cj_bypass_devices)
                    {
                        cur_house_mode->cj_bypass_devices = cJSON_Duplicate(__FUNCTION__, cj_bypass_devices, cJSON_True);
                    }
                }

                CJSON_GET_VALUE_BOOL(cj_house_mod, ezlopi_protect_str, cur_house_mode->protect);
                CJSON_GET_VALUE_BOOL(cj_house_mod, ezlopi_armed_str, cur_house_mode->armed);
                // CJSON_TRACE("cur_house_mode->cj_notifications", cur_house_mode->cj_notifications);
                // CJSON_TRACE("cur_house_mode->cj_disarmed_devices", cur_house_mode->cj_disarmed_devices);
                // CJSON_TRACE("cur_house_mode->cj_alarms_off_devices", cur_house_mode->cj_alarms_off_devices);
                // CJSON_TRACE("cur_house_mode->cj_cameras_off_devices", cur_house_mode->cj_cameras_off_devices);
                // CJSON_TRACE("cur_house_mode->cj_bypass_devices", cur_house_mode->cj_bypass_devices);
            }
        }

        {
            cJSON *cj_devices = cJSON_GetObjectItem(__FUNCTION__, cj_modes, ezlopi_devices_str);
            if (cj_devices)
            {
                parsed_mode->cj_devices = cJSON_Duplicate(__FUNCTION__, cj_devices, cJSON_True);
            }
        }

        {
            cJSON *cj_alarms = cJSON_GetObjectItem(__FUNCTION__, cj_modes, ezlopi_alarms_str);
            if (cj_alarms)
            {
                parsed_mode->cj_alarms = cJSON_Duplicate(__FUNCTION__, cj_alarms, cJSON_True);
            }
        }

        {
            cJSON *cj_cameras = cJSON_GetObjectItem(__FUNCTION__, cj_modes, ezlopi_cameras_str);
            if (cj_cameras)
            {
                parsed_mode->cj_cameras = cJSON_Duplicate(__FUNCTION__, cj_cameras, cJSON_True);
            }
        }

        {
            cJSON *cj_protect_buttons = cJSON_GetObjectItem(__FUNCTION__, cj_modes, ezlopi_protectButtons_str);
            if (cj_protect_buttons)
            {
                // CJSON_TRACE("protect_ll", cj_protect_buttons);
                cJSON *cj_button = NULL;
                s_protect_buttons_t *curr_button = parsed_mode->l_protect_buttons;

                cJSON_ArrayForEach(cj_button, cj_protect_buttons)
                {
                    if (curr_button)
                    {
                        curr_button->next = (s_protect_buttons_t *)ezlopi_malloc(__FUNCTION__, sizeof(s_protect_buttons_t));
                        curr_button = curr_button->next;
                    }
                    else
                    {
                        parsed_mode->l_protect_buttons = (s_protect_buttons_t *)ezlopi_malloc(__FUNCTION__, sizeof(s_protect_buttons_t));
                        curr_button = parsed_mode->l_protect_buttons;
                    }

                    if (curr_button)
                    {
                        memset(curr_button, 0, sizeof(s_protect_buttons_t));
                        CJSON_GET_ID(curr_button->device_id, cJSON_GetObjectItem(__FUNCTION__, cj_button, ezlopi_id_str));
                        CJSON_GET_VALUE_STRING_BY_COPY(cj_button, ezlopi_service_str, curr_button->service_name);
                        curr_button->next = NULL; // making sure, tail is null;
                    }
                }
            }
        }

        {
            cJSON *cj_alarmed = cJSON_GetObjectItem(__FUNCTION__, cj_modes, ezlopi_alarmed_str);
            if (cj_alarmed)
            {
                CJSON_GET_VALUE_DOUBLE(cj_alarmed, ezlopi_entryDelay_str, parsed_mode->alarmed.entry_delay_sec);
                CJSON_GET_VALUE_DOUBLE(cj_alarmed, ezlopi_timeIsLeft_str, parsed_mode->alarmed.time_is_left_sec);

                CJSON_GET_VALUE_STRING_BY_COPY(cj_alarmed, ezlopi_type_str, parsed_mode->alarmed.type);
                CJSON_GET_VALUE_BOOL(cj_alarmed, ezlopi_silent_str, parsed_mode->alarmed.silent);

                char tmp_str[32] = { 0 };
                CJSON_GET_VALUE_STRING_BY_COPY(cj_alarmed, ezlopi_phase_str, tmp_str);
                {
                    size_t tmp_len = strlen(tmp_str) + 1;
                    (EZPI_STRNCMP_IF_EQUAL(ezlopi_idle_str, tmp_str, 5, tmp_len)) ? (parsed_mode->alarmed.phase = EZLOPI_MODES_ALARM_PHASE_IDLE)
                        : (EZPI_STRNCMP_IF_EQUAL(ezlopi_bypass_str, tmp_str, 7, tmp_len)) ? (parsed_mode->alarmed.phase = EZLOPI_MODES_ALARM_PHASE_BYPASS)
                        : (EZPI_STRNCMP_IF_EQUAL(ezlopi_entryDelay_str, tmp_str, 11, tmp_len)) ? (parsed_mode->alarmed.phase = EZLOPI_MODES_ALARM_PHASE_ENTRYDELAY)
                        : (EZPI_STRNCMP_IF_EQUAL(ezlopi_main_str, tmp_str, 5, tmp_len)) ? (parsed_mode->alarmed.phase = EZLOPI_MODES_ALARM_PHASE_MAIN)
                        : (parsed_mode->alarmed.phase = EZLOPI_MODES_ALARM_PHASE_NONE); // this "NONE"-phase exists only at the beginning .
                }

                CJSON_GET_VALUE_STRING_BY_COPY(cj_alarmed, ezlopi_status_str, tmp_str);
                {
                    size_t tmp_len = strlen(tmp_str) + 1;
                    (EZPI_STRNCMP_IF_EQUAL(ezlopi_done_str, tmp_str, 5, tmp_len)) ? (parsed_mode->alarmed.status = EZLOPI_MODES_ALARM_STATUS_DONE)
                        : (EZPI_STRNCMP_IF_EQUAL(ezlopi_begin_str, tmp_str, 6, tmp_len)) ? (parsed_mode->alarmed.status = EZLOPI_MODES_ALARM_STATUS_BEGIN)
                        : (EZPI_STRNCMP_IF_EQUAL(ezlopi_canceled_str, tmp_str, 9, tmp_len)) ? (parsed_mode->alarmed.status = EZLOPI_MODES_ALARM_STATUS_CANCELED)
                        : (parsed_mode->alarmed.status = EZLOPI_MODES_ALARM_STATUS_DONE); // default State
                }

                cJSON *cj_sources_arr = cJSON_GetObjectItem(__FUNCTION__, cj_alarmed, ezlopi_sources_str);
                if (cj_sources_arr)
                {
                    cJSON *cj_source = NULL;
                    s_sources_t *curr_source = NULL;

                    cJSON_ArrayForEach(cj_source, cj_sources_arr)
                    {
                        if (parsed_mode->alarmed.sources)
                        {
                            curr_source->next = (s_sources_t *)ezlopi_malloc(__FUNCTION__, sizeof(s_sources_t));
                            curr_source = curr_source->next;
                        }
                        else
                        {
                            parsed_mode->alarmed.sources = (s_sources_t *)ezlopi_malloc(__FUNCTION__, sizeof(s_sources_t));
                            curr_source = parsed_mode->alarmed.sources;
                        }

                        if (curr_source)
                        {
                            memset(curr_source, 0, sizeof(s_sources_t));

                            CJSON_GET_ID(curr_source->device_id, cJSON_GetObjectItem(__FUNCTION__, cj_source, ezlopi_deviceId_str));
                            CJSON_GET_VALUE_DOUBLE(cj_source, ezlopi_timestamp_str, curr_source->time_stamp);
                            CJSON_GET_VALUE_DOUBLE(cj_source, ezlopi_delay_str, curr_source->delay);
                        }
                    }
                }
            }
        }
    }

    if (_parsing_status < 0)
    {
        char *empty_string = " ";
        ezlopi_nvs_write_modes(empty_string);
        TRACE_W("Reseted modes from NVS");
    }

    return parsed_mode;
}

cJSON *EZPI_core_modes_cjson_changed(void) //  (IN core-service-loop) // For broadcasting mode-info on active 'MODE'
{
    cJSON *cj_root = cJSON_CreateObject(__FUNCTION__);
    if (cj_root)
    {
        cJSON_AddStringToObject(__FUNCTION__, cj_root, ezlopi_id_str, ezlopi_ui_broadcast_str);
        cJSON_AddStringToObject(__FUNCTION__, cj_root, ezlopi_msg_subclass_str, ezlopi_hub_modes_changed_str);
        cJSON *cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_root, ezlopi_result_str);
        if (cj_result)
        {
            s_ezlopi_modes_t *_mode = EZPI_core_modes_get_custom_modes();
            s_house_modes_t *_current_mode = EZPI_core_modes_get_current_house_modes();
            if (_mode)
            {
                CJSON_ASSIGN_ID(cj_result, _mode->current_mode_id, ezlopi_modeId_str);
                cJSON_AddBoolToObject(__FUNCTION__, cj_result, ezlopi_disarmedDefault_str, _current_mode->disarmed_default);

                cJSON_AddNumberToObject(__FUNCTION__, cj_result, ezlopi_timestamp_str, EZPI_CORE_sntp_get_current_time_ms());
            }
        }
    }

    return cj_root;
}

cJSON *EZPI_core_modes_cjson_alarmed(const char *dev_id_str) // (IN core-service-loop) // For broadcasting alarm-info on active 'MODE'
{
    cJSON *cj_root = cJSON_CreateObject(__FUNCTION__);
    if (cj_root)
    {
        cJSON_AddStringToObject(__FUNCTION__, cj_root, ezlopi_id_str, ezlopi_ui_broadcast_str);
        cJSON_AddStringToObject(__FUNCTION__, cj_root, ezlopi_msg_subclass_str, ezlopi_hub_modes_alarmed_str);

        cJSON *cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_root, ezlopi_result_str);
        if (cj_result)
        {
            s_ezlopi_modes_t *curr_mode = EZPI_core_modes_get_custom_modes();
            if (curr_mode)
            {
                CJSON_ASSIGN_ID(cj_result, curr_mode->current_mode_id, ezlopi_modeId_str);
                cJSON_AddStringToObject(__FUNCTION__, cj_result, dev_id_str, ezlopi_deviceId_str); // 'device_id' that triggers an alarm
                if (EZLOPI_MODES_ALARM_PHASE_ENTRYDELAY == curr_mode->alarmed.phase)
                {
                    CJSON_ASSIGN_ID(cj_result, curr_mode->time_is_left_to_switch_sec, ezlopi_pendingDelay_str); // Only for 'entryDelay'
                }

                (EZLOPI_MODES_ALARM_PHASE_IDLE == curr_mode->alarmed.phase) ? cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_phase_str, ezlopi_idle_str)
                    : (EZLOPI_MODES_ALARM_PHASE_BYPASS == curr_mode->alarmed.phase) ? cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_phase_str, ezlopi_bypass_str)
                    : (EZLOPI_MODES_ALARM_PHASE_ENTRYDELAY == curr_mode->alarmed.phase) ? cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_phase_str, ezlopi_entryDelay_str)
                    : (EZLOPI_MODES_ALARM_PHASE_MAIN == curr_mode->alarmed.phase) ? cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_phase_str, ezlopi_main_str)
                    : cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_phase_str, ezlopi__str); // none

                (EZLOPI_MODES_ALARM_STATUS_DONE == curr_mode->alarmed.status) ? cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_status_str, ezlopi_done_str)
                    : (EZLOPI_MODES_ALARM_STATUS_BEGIN == curr_mode->alarmed.status) ? cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_status_str, ezlopi_begin_str)
                    : (EZLOPI_MODES_ALARM_STATUS_CANCELED == curr_mode->alarmed.status) ? cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_status_str, ezlopi_canceled_str)
                    : cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_status_str, ezlopi__str); // none

                cJSON_AddNumberToObject(__FUNCTION__, cj_result, ezlopi_timestamp_str, EZPI_CORE_sntp_get_current_time_ms());
                cJSON_AddBoolToObject(__FUNCTION__, cj_result, ezlopi_silent_str, curr_mode->alarmed.silent);

                #warning "need to add two-members [soundType & chime]"
            }
        }
    }
    return cj_root;
}

/*******************************************************************************
*                         Static Function Definitions
*******************************************************************************/

static void __cjson_add_number_as_hex_string(cJSON *cj_dest, const char *obj_name, uint32_t number)
{
    if (cj_dest && obj_name)
    {
        char tmp_str[32];
        snprintf(tmp_str, sizeof(tmp_str), "%08x", number);
        cJSON_AddStringToObject(__FUNCTION__, cj_dest, obj_name, tmp_str);
    }
}

static void __cjson_add_entry_delay(cJSON *cj_result, s_entry_delay_t *entry_delay)
{
    cJSON *cj_entry_delay = cJSON_AddObjectToObject(__FUNCTION__, cj_result, ezlopi_entryDelay_str);
    if (cj_entry_delay)
    {
        cJSON_AddNumberToObject(__FUNCTION__, cj_entry_delay, ezlopi_normal_str, entry_delay->normal_delay_sec);
        cJSON_AddNumberToObject(__FUNCTION__, cj_entry_delay, ezlopi_long_extended_str, entry_delay->long_extended_delay_sec);
        cJSON_AddNumberToObject(__FUNCTION__, cj_entry_delay, ezlopi_extended_str, entry_delay->extended_delay_sec);
        cJSON_AddNumberToObject(__FUNCTION__, cj_entry_delay, ezlopi_instant_str, entry_delay->instant_delay_sec);
    }
}

static void __cjson_add_abort_window(cJSON *cj_result, s_abort_window_t *abort_delay)
{
    cJSON *cj_abort_window = cJSON_AddObjectToObject(__FUNCTION__, cj_result, ezlopi_abortWindow_str);
    if (cj_abort_window)
    {
        cJSON_AddNumberToObject(__FUNCTION__, cj_abort_window, ezlopi_default_str, abort_delay->default_delay_sec);
        cJSON_AddNumberToObject(__FUNCTION__, cj_abort_window, ezlopi_minimum_str, abort_delay->minimum_delay_sec);
        cJSON_AddNumberToObject(__FUNCTION__, cj_abort_window, ezlopi_maximum_str, abort_delay->maximum_delay_sec);
    }
}

static void __cjson_duplicate_add_reference(cJSON *cj_dest, const char *item_name_str, cJSON *cj_item)
{
    if (cj_dest)
    {
        if (cj_item)
        {
            cJSON_AddItemReferenceToObject(__FUNCTION__, cj_dest, item_name_str, cj_item);
        }
        else
        {
            cJSON_AddArrayToObject(__FUNCTION__, cj_dest, item_name_str);
        }
    }
}

static void __cjson_add_mode_to_array(cJSON *cj_modes_arr, s_house_modes_t *house_mode)
{
    if (house_mode)
    {
        cJSON *cj_house_mode = cJSON_CreateObject(__FUNCTION__);
        if (cj_house_mode)
        {
            CJSON_ASSIGN_NUMBER_AS_STRING(cj_house_mode, house_mode->_id, ezlopi__id_str);

            cJSON_AddStringToObject(__FUNCTION__, cj_house_mode, ezlopi_name_str, house_mode->name);
            cJSON_AddStringToObject(__FUNCTION__, cj_house_mode, ezlopi_description_str, house_mode->description ? house_mode->description : ezlopi__str);

            cJSON_AddBoolToObject(__FUNCTION__, cj_house_mode, ezlopi_armed_str, house_mode->armed);
            cJSON_AddBoolToObject(__FUNCTION__, cj_house_mode, ezlopi_protect_str, house_mode->protect);
            cJSON_AddBoolToObject(__FUNCTION__, cj_house_mode, ezlopi_disarmedDefault_str, house_mode->disarmed_default);

            __cjson_duplicate_add_reference(cj_house_mode, ezlopi_disarmedDevices_str, house_mode->cj_disarmed_devices);
            __cjson_duplicate_add_reference(cj_house_mode, ezlopi_notifications_str, house_mode->cj_notifications);
            __cjson_duplicate_add_reference(cj_house_mode, ezlopi_alarmsOffDevices_str, house_mode->cj_alarms_off_devices);
            __cjson_duplicate_add_reference(cj_house_mode, ezlopi_camerasOffDevices_str, house_mode->cj_cameras_off_devices);
            __cjson_duplicate_add_reference(cj_house_mode, ezlopi_bypassDevices_str, house_mode->cj_bypass_devices);

            if (!cJSON_AddItemToArray(cj_modes_arr, cj_house_mode))
            {
                cJSON_Delete(__FUNCTION__, cj_house_mode);
            }
        }
    }
}

static void __cjson_add_protect_buttons(cJSON *cj_protect_buttons_arr, s_protect_buttons_t *l_protect_buttons)
{
    while (l_protect_buttons)
    {
        cJSON *cj_protect_button = cJSON_CreateObject(__FUNCTION__);
        if (cj_protect_button)
        {
            __cjson_add_number_as_hex_string(cj_protect_button, ezlopi_id_str, l_protect_buttons->device_id);
            cJSON_AddStringToObject(__FUNCTION__, cj_protect_button, ezlopi_service_str, l_protect_buttons->service_name);

            if (!cJSON_AddItemToArray(cj_protect_buttons_arr, cj_protect_button))
            {
                cJSON_Delete(__FUNCTION__, cj_protect_button);
            }
        }

        l_protect_buttons = l_protect_buttons->next;
    }
}

static void __cjson_add_alarmed(cJSON *cj_alarmed, s_alarmed_t *alarmed)
{
    if (alarmed)
    {
        cJSON_AddNumberToObject(__FUNCTION__, cj_alarmed, ezlopi_entryDelay_str, alarmed->entry_delay_sec);
        cJSON_AddNumberToObject(__FUNCTION__, cj_alarmed, ezlopi_timeIsLeft_str, alarmed->time_is_left_sec);
        cJSON_AddStringToObject(__FUNCTION__, cj_alarmed, ezlopi_type_str, alarmed->type);
        cJSON_AddBoolToObject(__FUNCTION__, cj_alarmed, ezlopi_silent_str, alarmed->silent);

        cJSON *cj_sources_arr = cJSON_AddArrayToObject(__FUNCTION__, cj_alarmed, ezlopi_sources_str);
        if (cj_sources_arr)
        {
            s_sources_t *source_node = alarmed->sources;
            while (source_node)
            {
                cJSON *cj_source = cJSON_CreateObject(__FUNCTION__);
                if (cj_source)
                {
                    CJSON_ASSIGN_NUMBER_AS_STRING(cj_source, source_node->device_id, ezlopi_deviceId_str);
                    cJSON_AddNumberToObject(__FUNCTION__, cj_source, ezlopi_delay_str, source_node->delay);
                    cJSON_AddNumberToObject(__FUNCTION__, cj_source, ezlopi_timestamp_str, source_node->time_stamp);
                }

                source_node = source_node->next;
            }
        }
    }
}

#endif // CONFIG_EZPI_SERV_ENABLE_MODES

/*******************************************************************************
*                          End of File
*******************************************************************************/