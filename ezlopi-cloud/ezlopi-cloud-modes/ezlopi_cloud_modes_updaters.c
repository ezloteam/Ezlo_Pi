
/**
 * @file    ezlopi_cloud_modes_updaters.c
 * @brief
 * @author
 * @version
 * @date
 */
/* ===========================================================================
** Copyright (C) 2022 Ezlo Innovation Inc
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

#include <time.h>

#include "ezlopi_core_broadcast.h"
#include "ezlopi_core_sntp.h"
#include "ezlopi_core_modes.h"
#include "ezlopi_core_cjson_macros.h"

#include "ezlopi_cloud_modes.h"
#include "ezlopi_cloud_constants.h"
#include "ezlopi_cloud_modes_updaters.h"

#if defined(CONFIG_EZPI_SERV_ENABLE_MODES)
void EZPI_cloud_modes_switched(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_DeleteItemFromObject(__FUNCTION__, cj_response, ezlopi_id_str);
    cJSON_DeleteItemFromObject(__FUNCTION__, cj_response, ezlopi_method_str);

    cJSON_AddStringToObject(__FUNCTION__, cj_response, ezlopi_id_str, ezlopi_ui_broadcast_str);
    cJSON_AddStringToObject(__FUNCTION__, cj_response, ezlopi_msg_subclass_str, ezlopi_hub_modes_changed_str);

    cJSON *cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    if (cj_result)
    {
#if 0
        s_ezlopi_modes_t *curr_mode = EZPI_core_modes_get_custom_modes();
        if (curr_mode)
        {
            s_house_modes_t *curr_house_mode = EZPI_core_modes_get_current_house_modes();
            if (curr_house_mode)
            {
                // CJSON_ASSIGN_ID(cj_result, curr_mode->current_mode_id, "from"); // this can be extracted from 'request'
                // CJSON_ASSIGN_ID(cj_result, curr_mode->current_mode_id, "to");

                // cJSON_AddNumberToObject(__FUNCTION__, cj_result, ezlopi_switchToDelay_str, curr_house_mode->switch_to_delay_sec);
                // cJSON_AddNumberToObject(__FUNCTION__, cj_result, ezlopi_timestamp_str, EZPI_core_sntp_get_current_time_ms());
            }
        }
#endif
    }
}

void EZPI_cloud_modes_alarmed(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_DeleteItemFromObject(__FUNCTION__, cj_response, ezlopi_id_str);
    cJSON_DeleteItemFromObject(__FUNCTION__, cj_response, ezlopi_method_str);

    cJSON_AddStringToObject(__FUNCTION__, cj_response, ezlopi_id_str, ezlopi_ui_broadcast_str);
    cJSON_AddStringToObject(__FUNCTION__, cj_response, ezlopi_msg_subclass_str, ezlopi_hub_modes_alarmed_str);

    cJSON *cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    if (cj_result)
    {
        s_ezlopi_modes_t *curr_mode = EZPI_core_modes_get_custom_modes();
        if (curr_mode)
        {
            CJSON_ASSIGN_ID(cj_result, curr_mode->current_mode_id, ezlopi_modeId_str);
#warning "For Cloud interaction; [Cannot determine which '_id' triggered the alert]"
            // CJSON_ASSIGN_ID(cj_result, "0000", ezlopi_deviceId_str);
            CJSON_ASSIGN_ID(cj_result, curr_mode->time_is_left_to_switch_sec, ezlopi_entryDelay_str);

            (EZLOPI_MODES_ALARM_PHASE_IDLE == curr_mode->alarmed.phase)         ? cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_phase_str, ezlopi_idle_str)
            : (EZLOPI_MODES_ALARM_PHASE_BYPASS == curr_mode->alarmed.phase)     ? cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_phase_str, ezlopi_bypass_str)
            : (EZLOPI_MODES_ALARM_PHASE_ENTRYDELAY == curr_mode->alarmed.phase) ? cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_phase_str, ezlopi_entryDelay_str)
            : (EZLOPI_MODES_ALARM_PHASE_MAIN == curr_mode->alarmed.phase)       ? cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_phase_str, ezlopi_main_str)
                                                                                : cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_phase_str, ezlopi__str); // none

            (EZLOPI_MODES_ALARM_STATUS_DONE == curr_mode->alarmed.status)       ? cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_status_str, ezlopi_done_str)
            : (EZLOPI_MODES_ALARM_STATUS_BEGIN == curr_mode->alarmed.status)    ? cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_status_str, ezlopi_begin_str)
            : (EZLOPI_MODES_ALARM_STATUS_CANCELED == curr_mode->alarmed.status) ? cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_status_str, ezlopi_canceled_str)
                                                                                : cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_status_str, ezlopi__str); // none

            cJSON_AddNumberToObject(__FUNCTION__, cj_result, ezlopi_timestamp_str, EZPI_core_sntp_get_current_time_ms());
            cJSON_AddBoolToObject(__FUNCTION__, cj_result, ezlopi_silent_str, curr_mode->alarmed.silent);
#warning "need to add two-members [soundType & chime]"
        }
    }
}

void EZPI_cloud_modes_changed_alarmed(cJSON *cj_request, cJSON *cj_response)
{
    // 1. broadcast 'modes.changed'
    cJSON *cj_changed_resp = cJSON_CreateObject(__FUNCTION__);
    if (cj_changed_resp)
    {
        EZPI_cloud_modes_changed(cj_request, cj_changed_resp);
        if (EZPI_SUCCESS != EZPI_core_broadcast_add_to_queue(cj_changed_resp, EZPI_core_sntp_get_current_time_sec()))
        {
            cJSON_Delete(__FUNCTION__, cj_changed_resp);
        }
    }

    // 2. for 'modes.alarmed' - return  'cj_response'
    EZPI_cloud_modes_alarmed(cj_request, cj_response);
}

void EZPI_cloud_modes_notifications_notify_all(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
}
#if 0

void EZPI_cloud_modes_notifications_added(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
}

void EZPI_cloud_modes_notifications_removed(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
}
#endif

void EZPI_cloud_modes_disarmed_devices_added(cJSON *cj_request, cJSON *cj_response)
{
    // 1. broadcast 'modes.changed'
    cJSON *cj_changed_resp = cJSON_CreateObject(__FUNCTION__);
    if (cj_changed_resp)
    {
        EZPI_cloud_modes_changed(cj_request, cj_changed_resp);
        if (EZPI_SUCCESS != EZPI_core_broadcast_add_to_queue(cj_changed_resp, EZPI_core_sntp_get_current_time_sec()))
        {
            cJSON_Delete(__FUNCTION__, cj_changed_resp);
        }
    }

    // 2. broadcast disarmed_device.added
    cJSON_AddStringToObject(__FUNCTION__, cj_response, ezlopi_id_str, ezlopi_ui_broadcast_str);
    cJSON_AddStringToObject(__FUNCTION__, cj_response, ezlopi_msg_subclass_str, ezlopi_hub_modes_disarmed_devices_added);

    cJSON *cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    if (cj_result)
    {
        cJSON *cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
        if (cj_params)
        {
            cJSON *cj_mode_id = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_modeId_str);
            cJSON *cj_device_id = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_deviceId_str);
            if (cj_mode_id && cj_device_id)
            {
                cJSON_AddItemToObject(__FUNCTION__, cj_result, ezlopi_modeId_str, cJSON_Duplicate(__FUNCTION__, cj_mode_id, true));
                cJSON_AddItemToObject(__FUNCTION__, cj_result, ezlopi_disarmedDevice_str, cJSON_Duplicate(__FUNCTION__, cj_device_id, true));
            }
        }
    }
}

void EZPI_cloud_modes_disarmed_devices_removed(cJSON *cj_request, cJSON *cj_response)
{
    // 1. broadcast 'modes.changed'
    cJSON *cj_changed_resp = cJSON_CreateObject(__FUNCTION__);
    if (cj_changed_resp)
    {
        EZPI_cloud_modes_changed(cj_request, cj_changed_resp);
        if (EZPI_SUCCESS != EZPI_core_broadcast_add_to_queue(cj_changed_resp, EZPI_core_sntp_get_current_time_sec()))
        {
            cJSON_Delete(__FUNCTION__, cj_changed_resp);
        }
    }

    // 2. broadcast alarm_off.added
    cJSON_AddStringToObject(__FUNCTION__, cj_response, ezlopi_id_str, ezlopi_ui_broadcast_str);
    cJSON_AddStringToObject(__FUNCTION__, cj_response, ezlopi_msg_subclass_str, ezlopi_hub_modes_alarms_off_added);

    cJSON *cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    if (cj_result)
    {
        cJSON *cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
        if (cj_params)
        {
            cJSON *cj_mode_id = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_modeId_str);
            cJSON *cj_device_id = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_deviceId_str);
            if (cj_mode_id && cj_device_id)
            {
                cJSON_AddItemToObject(__FUNCTION__, cj_result, ezlopi_modeId_str, cJSON_Duplicate(__FUNCTION__, cj_mode_id, true));
                cJSON_AddItemToObject(__FUNCTION__, cj_result, ezlopi_alarmsOffDevice_str, cJSON_Duplicate(__FUNCTION__, cj_device_id, true));
            }
        }
    }
}

void EZPI_cloud_modes_alarms_off_added(cJSON *cj_request, cJSON *cj_response)
{
    // 1. broadcast 'modes.changed'
    cJSON *cj_changed_resp = cJSON_CreateObject(__FUNCTION__);
    if (cj_changed_resp)
    {
        EZPI_cloud_modes_changed(cj_request, cj_changed_resp);
        if (EZPI_SUCCESS != EZPI_core_broadcast_add_to_queue(cj_changed_resp, EZPI_core_sntp_get_current_time_sec()))
        {
            cJSON_Delete(__FUNCTION__, cj_changed_resp);
        }
    }

    // 2. broadcast alarm_off.added
    cJSON_AddStringToObject(__FUNCTION__, cj_response, ezlopi_id_str, ezlopi_ui_broadcast_str);
    cJSON_AddStringToObject(__FUNCTION__, cj_response, ezlopi_msg_subclass_str, ezlopi_hub_modes_alarms_off_added);

    cJSON *cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    if (cj_result)
    {
        cJSON *cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
        if (cj_params)
        {
            cJSON *cj_mode_id = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_modeId_str);
            cJSON *cj_device_id = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_deviceId_str);
            if (cj_mode_id && cj_device_id)
            {
                cJSON_AddItemToObject(__FUNCTION__, cj_result, ezlopi_modeId_str, cJSON_Duplicate(__FUNCTION__, cj_mode_id, true));
                cJSON_AddItemToObject(__FUNCTION__, cj_result, ezlopi_alarmsOffDevice_str, cJSON_Duplicate(__FUNCTION__, cj_device_id, true));
            }
        }
    }
}

void EZPI_cloud_modes_alarms_off_removed(cJSON *cj_request, cJSON *cj_response)
{
    // 1. broadcast 'modes.changed'
    cJSON *cj_changed_resp = cJSON_CreateObject(__FUNCTION__);
    if (cj_changed_resp)
    {
        EZPI_cloud_modes_changed(cj_request, cj_changed_resp);
        if (EZPI_SUCCESS != EZPI_core_broadcast_add_to_queue(cj_changed_resp, EZPI_core_sntp_get_current_time_sec()))
        {
            cJSON_Delete(__FUNCTION__, cj_changed_resp);
        }
    }

    // 2. broadcast alarm_off.removed
    cJSON_AddStringToObject(__FUNCTION__, cj_response, ezlopi_id_str, ezlopi_ui_broadcast_str);
    cJSON_AddStringToObject(__FUNCTION__, cj_response, ezlopi_msg_subclass_str, ezlopi_hub_modes_alarms_off_removed);

    cJSON *cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    if (cj_result)
    {
        cJSON *cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
        if (cj_params)
        {
            cJSON *cj_mode_id = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_modeId_str);
            cJSON *cj_device_id = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_deviceId_str);
            if (cj_mode_id && cj_device_id)
            {
                cJSON_AddItemToObject(__FUNCTION__, cj_result, ezlopi_modeId_str, cJSON_Duplicate(__FUNCTION__, cj_mode_id, true));
                cJSON_AddItemToObject(__FUNCTION__, cj_result, ezlopi_alarmsOffDevice_str, cJSON_Duplicate(__FUNCTION__, cj_device_id, true));
            }
        }
    }
}

void EZPI_cloud_modes_cameras_off_added(cJSON *cj_request, cJSON *cj_response)
{
    // 1. broadcast 'modes.changed'
    cJSON *cj_changed_resp = cJSON_CreateObject(__FUNCTION__);
    if (cj_changed_resp)
    {
        EZPI_cloud_modes_changed(cj_request, cj_changed_resp);
        if (EZPI_SUCCESS != EZPI_core_broadcast_add_to_queue(cj_changed_resp, EZPI_core_sntp_get_current_time_sec()))
        {
            cJSON_Delete(__FUNCTION__, cj_changed_resp);
        }
    }

    // 2. broadcast alarm_off.added
    cJSON_AddStringToObject(__FUNCTION__, cj_response, ezlopi_id_str, ezlopi_ui_broadcast_str);
    cJSON_AddStringToObject(__FUNCTION__, cj_response, ezlopi_msg_subclass_str, ezlopi_hub_modes_cameras_off_added);

    cJSON *cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    if (cj_result)
    {
        cJSON *cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
        if (cj_params)
        {
            cJSON *cj_mode_id = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_modeId_str);
            cJSON *cj_device_id = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_deviceId_str);
            if (cj_mode_id && cj_device_id)
            {
                cJSON_AddItemToObject(__FUNCTION__, cj_result, ezlopi_modeId_str, cJSON_Duplicate(__FUNCTION__, cj_mode_id, true));
                cJSON_AddItemToObject(__FUNCTION__, cj_result, ezlopi_alarmsOffDevice_str, cJSON_Duplicate(__FUNCTION__, cj_device_id, true));
            }
        }
    }
}

void EZPI_cloud_modes_cameras_off_removed(cJSON *cj_request, cJSON *cj_response)
{
    // 1. broadcast 'modes.changed'
    cJSON *cj_changed_resp = cJSON_CreateObject(__FUNCTION__);
    if (cj_changed_resp)
    {
        EZPI_cloud_modes_changed(cj_request, cj_changed_resp);
        if (EZPI_SUCCESS != EZPI_core_broadcast_add_to_queue(cj_changed_resp, EZPI_core_sntp_get_current_time_sec()))
        {
            cJSON_Delete(__FUNCTION__, cj_changed_resp);
        }
    }

    // 2. broadcast alarm_off.added
    cJSON_AddStringToObject(__FUNCTION__, cj_response, ezlopi_id_str, ezlopi_ui_broadcast_str);
    cJSON_AddStringToObject(__FUNCTION__, cj_response, ezlopi_msg_subclass_str, ezlopi_hub_modes_cameras_off_removed);

    cJSON *cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    if (cj_result)
    {
        cJSON *cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
        if (cj_params)
        {
            cJSON *cj_mode_id = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_modeId_str);
            cJSON *cj_device_id = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_deviceId_str);
            if (cj_mode_id && cj_device_id)
            {
                cJSON_AddItemToObject(__FUNCTION__, cj_result, ezlopi_modeId_str, cJSON_Duplicate(__FUNCTION__, cj_mode_id, true));
                cJSON_AddItemToObject(__FUNCTION__, cj_result, ezlopi_camerasOffDevices_str, cJSON_Duplicate(__FUNCTION__, cj_device_id, true));
            }
        }
    }
}

void EZPI_cloud_modes_bypass_devices_added(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddStringToObject(__func__, cj_response, ezlopi_id_str, ezlopi_ui_broadcast_str);
    cJSON_AddStringToObject(__func__, cj_response, ezlopi_msg_subclass_str, ezlopi_hub_modes_bypass_devices_added);

    cJSON *cj_params = cJSON_GetObjectItem(__func__, cj_request, ezlopi_params_str);
    if (cj_params)
    {
        cJSON_AddItemToObject(__func__, cj_response, ezlopi_result_str, cJSON_Duplicate(__func__, cj_params, true));
    }
}

void EZPI_cloud_modes_bypass_devices_removed(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddStringToObject(__func__, cj_response, ezlopi_id_str, ezlopi_ui_broadcast_str);
    cJSON_AddStringToObject(__func__, cj_response, ezlopi_msg_subclass_str, ezlopi_hub_modes_bypass_devices_removed);
    cJSON *cj_params = cJSON_GetObjectItem(__func__, cj_request, ezlopi_params_str);
    if (cj_params)
    {
        cJSON_AddItemToObject(__func__, cj_response, ezlopi_result_str, cJSON_Duplicate(__func__, cj_params, true));
    }
}

void EZPI_cloud_modes_changed(cJSON *cj_request, cJSON *cj_response)
{
    // cJSON_AddNumberToObject(__FUNCTION__, cj_response, ezlopi_startTime_str, EZPI_core_sntp_get_current_time_sec());

    cJSON_DeleteItemFromObject(__FUNCTION__, cj_response, ezlopi_id_str);
    cJSON_DeleteItemFromObject(__FUNCTION__, cj_response, ezlopi_method_str);

    cJSON_AddStringToObject(__FUNCTION__, cj_response, ezlopi_id_str, ezlopi_ui_broadcast_str);
    cJSON_AddStringToObject(__FUNCTION__, cj_response, ezlopi_msg_subclass_str, ezlopi_hub_modes_changed_str);

    cJSON *cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    if (cj_result)
    {
        cJSON *cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
        if (cj_params)
        {
            cJSON *cj_mode_id = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_modeId_str); // broadcast modes-info of 'updated_house_mode'
            if (cj_mode_id && cj_mode_id->valuestring)
            {
                s_house_modes_t *update_house_mode = EZPI_core_modes_get_house_mode_by_id(strtoul(cj_mode_id->valuestring, NULL, 16)); // get the 'running' house mode, indicated by 'ezlopi_mode_t'
                if (update_house_mode)
                {
                    CJSON_ASSIGN_ID(cj_result, update_house_mode->_id, ezlopi_modeId_str);
                    cJSON_AddBoolToObject(__FUNCTION__, cj_result, ezlopi_disarmedDefault_str, update_house_mode->disarmed_default);
                    cJSON_AddNumberToObject(__FUNCTION__, cj_result, ezlopi_timestamp_str, EZPI_core_sntp_get_current_time_ms());
                }
            }
        }
    }
}

void EZPI_cloud_modes_protect_buttons_added(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_DeleteItemFromObject(__FUNCTION__, cj_response, ezlopi_id_str);
    cJSON_DeleteItemFromObject(__FUNCTION__, cj_response, ezlopi_method_str);

    cJSON_AddStringToObject(__FUNCTION__, cj_response, ezlopi_id_str, ezlopi_ui_broadcast_str);
    cJSON_AddStringToObject(__FUNCTION__, cj_response, ezlopi_msg_subclass_str, ezlopi_hub_modes_protect_button_added);

    cJSON *cj_button_state = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_item_name_button_state);
    if (cj_button_state && ((1 << 0) == cj_button_state->valuedouble))
    {
        cJSON *cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
        if (cj_params)
        {
            cJSON_AddItemToObject(__FUNCTION__, cj_response, ezlopi_result_str, cJSON_Duplicate(__FUNCTION__, cj_params, true));
        }
    }

    if (NULL == cJSON_GetObjectItem(__FUNCTION__, cj_response, ezlopi_result_str))
    {
        cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    }
}

void EZPI_cloud_modes_protect_buttons_updated(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_DeleteItemFromObject(__FUNCTION__, cj_response, ezlopi_id_str);
    cJSON_DeleteItemFromObject(__FUNCTION__, cj_response, ezlopi_method_str);

    cJSON_AddStringToObject(__FUNCTION__, cj_response, ezlopi_id_str, ezlopi_ui_broadcast_str);
    cJSON_AddStringToObject(__FUNCTION__, cj_response, ezlopi_msg_subclass_str, ezlopi_hub_modes_protect_button_updated);

    cJSON *cj_button_state = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_item_name_button_state);
    if (cj_button_state && ((1 << 1) == cj_button_state->valuedouble))
    {
        cJSON *cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
        if (cj_params)
        {
            cJSON_AddItemToObject(__FUNCTION__, cj_response, ezlopi_result_str, cJSON_Duplicate(__FUNCTION__, cj_params, true));
        }
    }

    if (NULL == cJSON_GetObjectItem(__FUNCTION__, cj_response, ezlopi_result_str))
    {
        cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    }
}

void EZPI_cloud_modes_protect_buttons_removed(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_DeleteItemFromObject(__FUNCTION__, cj_response, ezlopi_id_str);
    cJSON_DeleteItemFromObject(__FUNCTION__, cj_response, ezlopi_method_str);

    cJSON_AddStringToObject(__FUNCTION__, cj_response, ezlopi_id_str, ezlopi_ui_broadcast_str);
    cJSON_AddStringToObject(__FUNCTION__, cj_response, ezlopi_msg_subclass_str, ezlopi_hub_modes_protect_button_removed);

    cJSON *cj_button_state = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_item_name_button_state);
    if (cj_button_state && ((1 << 2) == cj_button_state->valuedouble))
    {
        cJSON *cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
        if (cj_params)
        {
            cJSON *cj_device_id = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_deviceId_str);
            if (cj_device_id && cj_device_id->valuestring)
            {
                cJSON_AddStringToObject(__FUNCTION__, cj_response, ezlopi_result_str, cj_device_id->valuestring);
            }
        }
    }

    if (NULL == cJSON_GetObjectItem(__FUNCTION__, cj_response, ezlopi_result_str))
    {
        cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    }
}

void EZPI_cloud_modes_protect_button_set_broadcast(cJSON *cj_request, cJSON *cj_response)
{
    // 1. broadcast 'modes.protect.button.added'
    cJSON *cj_added_resp = cJSON_CreateObject(__FUNCTION__);
    if (cj_added_resp)
    {
        EZPI_cloud_modes_protect_buttons_added(cj_request, cj_added_resp);
        if (EZPI_SUCCESS != EZPI_core_broadcast_add_to_queue(cj_added_resp, EZPI_core_sntp_get_current_time_sec()))
        {
            cJSON_Delete(__FUNCTION__, cj_added_resp);
        }
    }

    // 2. broadcast 'modes.protect.button.updated'
    cJSON *cj_updated_resp = cJSON_CreateObject(__FUNCTION__);
    if (cj_updated_resp)
    {
        EZPI_cloud_modes_protect_buttons_updated(cj_request, cj_updated_resp);
        if (EZPI_SUCCESS != EZPI_core_broadcast_add_to_queue(cj_updated_resp, EZPI_core_sntp_get_current_time_sec()))
        {
            cJSON_Delete(__FUNCTION__, cj_updated_resp);
        }
    }

    // 3.broadcast 'modes.protect.button.removed'
    EZPI_cloud_modes_protect_buttons_removed(cj_request, cj_response);
}

void EZPI_cloud_modes_protect_devices_added(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_DeleteItemFromObject(__FUNCTION__, cj_response, ezlopi_id_str);
    cJSON_DeleteItemFromObject(__FUNCTION__, cj_response, ezlopi_method_str);

    cJSON_AddStringToObject(__FUNCTION__, cj_response, ezlopi_id_str, ezlopi_ui_broadcast_str);
    cJSON_AddStringToObject(__FUNCTION__, cj_response, ezlopi_msg_subclass_str, ezlopi_hub_modes_protect_devices_added);
    cJSON *cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
    if (cj_params)
    {
        cJSON_AddItemToObject(__FUNCTION__, cj_response, ezlopi_result_str, cJSON_Duplicate(__FUNCTION__, cj_params, true));
    }
}

void EZPI_cloud_modes_protect_devices_removed(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_DeleteItemFromObject(__FUNCTION__, cj_response, ezlopi_id_str);
    cJSON_DeleteItemFromObject(__FUNCTION__, cj_response, ezlopi_method_str);

    cJSON_AddStringToObject(__FUNCTION__, cj_response, ezlopi_id_str, ezlopi_ui_broadcast_str);
    cJSON_AddStringToObject(__FUNCTION__, cj_response, ezlopi_msg_subclass_str, ezlopi_hub_modes_protect_devices_removed);
    cJSON *cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
    if (cj_params)
    {
        cJSON_AddItemToObject(__FUNCTION__, cj_response, ezlopi_result_str, cJSON_Duplicate(__FUNCTION__, cj_params, true));
    }
}

void EZPI_cloud_modes_entry_delay_changed(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_DeleteItemFromObject(__FUNCTION__, cj_response, ezlopi_id_str);
    cJSON_DeleteItemFromObject(__FUNCTION__, cj_response, ezlopi_method_str);

    cJSON_AddStringToObject(__FUNCTION__, cj_response, ezlopi_id_str, ezlopi_ui_broadcast_str);
    cJSON_AddStringToObject(__FUNCTION__, cj_response, ezlopi_msg_subclass_str, ezlopi_hub_modes_entry_delay_changed);

    cJSON *cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    if (cj_result)
    {
        cJSON *cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
        if (cj_params)
        {
            // cJSON_AddItemToObject(__FUNCTION__, cj_result, ezlopi_entryDelay_str, cJSON_Duplicate(__FUNCTION__, cj_params, true));

            cJSON *cj_entryDelay = cJSON_AddObjectToObject(__FUNCTION__, cj_result, ezlopi_entryDelay_str);
            if (cj_entryDelay)
            {
                s_ezlopi_modes_t *curr_mode = EZPI_core_modes_get_custom_modes();
                if (curr_mode)
                {
                    cJSON_AddNumberToObject(__FUNCTION__, cj_entryDelay, ezlopi_normal_str, curr_mode->entry_delay.normal_delay_sec);
                    cJSON_AddNumberToObject(__FUNCTION__, cj_entryDelay, ezlopi_long_extended_str, curr_mode->entry_delay.long_extended_delay_sec);
                    cJSON_AddNumberToObject(__FUNCTION__, cj_entryDelay, ezlopi_extended_str, curr_mode->entry_delay.extended_delay_sec);
                    cJSON_AddNumberToObject(__FUNCTION__, cj_entryDelay, ezlopi_instant_str, curr_mode->entry_delay.instant_delay_sec);
                }
            }
            cJSON_AddItemToObject(__FUNCTION__, cj_result, ezlopi_timestamp_str, cJSON_CreateNumber(__FUNCTION__, (double)EZPI_core_sntp_get_current_time_ms()));
        }
    }
}

#endif // CONFIG_EZPI_SERV_ENABLE_MODES

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
