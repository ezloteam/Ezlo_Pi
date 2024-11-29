#include <time.h>

#include "ezlopi_core_broadcast.h"
#include "ezlopi_core_sntp.h"
#include "ezlopi_core_modes.h"
#include "ezlopi_core_cjson_macros.h"

#include "ezlopi_cloud_modes.h"
#include "ezlopi_cloud_constants.h"
#include "ezlopi_cloud_modes_updaters.h"

#if defined(CONFIG_EZPI_SERV_ENABLE_MODES)
void ezlopi_cloud_modes_switched(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_DeleteItemFromObject(__FUNCTION__, cj_response, ezlopi_id_str);
    cJSON_DeleteItemFromObject(__FUNCTION__, cj_response, ezlopi_method_str);

    cJSON_AddStringToObject(__FUNCTION__, cj_response, ezlopi_id_str, ezlopi_ui_broadcast_str);
    cJSON_AddStringToObject(__FUNCTION__, cj_response, ezlopi_msg_subclass_str, ezlopi_hub_modes_changed_str);

    cJSON *cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    if (cj_result)
    {
#if 0
        s_ezlopi_modes_t *curr_mode = ezlopi_core_modes_get_custom_modes();
        if (curr_mode)
        {
            s_house_modes_t *curr_house_mode = ezlopi_core_modes_get_current_house_modes();
            if (curr_house_mode)
            {
                // CJSON_ASSIGN_ID(cj_result, curr_mode->current_mode_id, "from"); // this can be extracted from 'request'
                // CJSON_ASSIGN_ID(cj_result, curr_mode->current_mode_id, "to");

                // cJSON_AddNumberToObject(__FUNCTION__, cj_result, ezlopi_switchToDelay_str, curr_house_mode->switch_to_delay_sec);
                // cJSON_AddNumberToObject(__FUNCTION__, cj_result, ezlopi_timestamp_str, EZPI_CORE_sntp_get_current_time_ms());
            }
        }
#endif
    }
}

void ezlopi_cloud_modes_alarmed(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_DeleteItemFromObject(__FUNCTION__, cj_response, ezlopi_id_str);
    cJSON_DeleteItemFromObject(__FUNCTION__, cj_response, ezlopi_method_str);

    cJSON_AddStringToObject(__FUNCTION__, cj_response, ezlopi_id_str, ezlopi_ui_broadcast_str);
    cJSON_AddStringToObject(__FUNCTION__, cj_response, ezlopi_msg_subclass_str, ezlopi_hub_modes_alarmed_str);

    cJSON *cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    if (cj_result)
    {
        s_ezlopi_modes_t *curr_mode = ezlopi_core_modes_get_custom_modes();
        if (curr_mode)
        {
            CJSON_ASSIGN_ID(cj_result, curr_mode->current_mode_id, ezlopi_modeId_str);
#warning "For Cloud interaction; [Cannot to determine which '_id' triggered the alert]"
            // CJSON_ASSIGN_ID(cj_result, "0000", ezlopi_deviceId_str);
            CJSON_ASSIGN_ID(cj_result, curr_mode->time_is_left_to_switch_sec, "pendingDelay");

            (EZLOPI_MODES_ALARM_PHASE_IDLE == curr_mode->alarmed.phase)         ? cJSON_AddStringToObject(__FUNCTION__, cj_result, "phase", "idle")
            : (EZLOPI_MODES_ALARM_PHASE_BYPASS == curr_mode->alarmed.phase)     ? cJSON_AddStringToObject(__FUNCTION__, cj_result, "phase", "bypass")
            : (EZLOPI_MODES_ALARM_PHASE_ENTRYDELAY == curr_mode->alarmed.phase) ? cJSON_AddStringToObject(__FUNCTION__, cj_result, "phase", "entryDelay")
            : (EZLOPI_MODES_ALARM_PHASE_MAIN == curr_mode->alarmed.phase)       ? cJSON_AddStringToObject(__FUNCTION__, cj_result, "phase", "main")
                                                                                : cJSON_AddStringToObject(__FUNCTION__, cj_result, "phase", ""); // none

            (EZLOPI_MODES_ALARM_STATUS_DONE == curr_mode->alarmed.status)       ? cJSON_AddStringToObject(__FUNCTION__, cj_result, "status", "done")
            : (EZLOPI_MODES_ALARM_STATUS_BEGIN == curr_mode->alarmed.status)    ? cJSON_AddStringToObject(__FUNCTION__, cj_result, "status", "begin")
            : (EZLOPI_MODES_ALARM_STATUS_CANCELED == curr_mode->alarmed.status) ? cJSON_AddStringToObject(__FUNCTION__, cj_result, "status", "canceled")
                                                                                : cJSON_AddStringToObject(__FUNCTION__, cj_result, "status", ""); // none

            cJSON_AddNumberToObject(__FUNCTION__, cj_result, ezlopi_timestamp_str, EZPI_CORE_sntp_get_current_time_ms());
            cJSON_AddBoolToObject(__FUNCTION__, cj_result, "silent", curr_mode->alarmed.silent);
#warning "need to add two-members [soundType & chime]"
        }
    }
}

void ezlopi_cloud_modes_changed_alarmed(cJSON *cj_request, cJSON *cj_response)
{
    // 1. broadcast 'modes.changed'
    cJSON *changed_resp = cJSON_CreateObject(__FUNCTION__);
    if (changed_resp)
    {
        ezlopi_cloud_modes_changed(cj_request, changed_resp);
        if (EZPI_SUCCESS != ezlopi_core_broadcast_add_to_queue(changed_resp))
        {
            cJSON_Delete(__FUNCTION__, changed_resp);
        }
    }

    // 2. for 'modes.alarmed' - return  'cj_response'
    TRACE_S("Triggering modes-alarm status");
    ezlopi_cloud_modes_alarmed(cj_request, cj_response);
}

void ezlopi_cloud_modes_notifications_notify_all(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
}

void ezlopi_cloud_modes_notifications_added(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
}

void ezlopi_cloud_modes_notifications_removed(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
}

void ezlopi_cloud_modes_disarmed_devices_added(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
}

void ezlopi_cloud_modes_disarmed_devices_removed(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
}

void ezlopi_cloud_modes_alarms_off_added(cJSON *cj_request, cJSON *cj_response)
{
    // cJSON_DeleteItemFromObject(__FUNCTION__, cj_response, ezlopi_id_str);
    // cJSON_DeleteItemFromObject(__FUNCTION__, cj_response, ezlopi_method_str);

    // 1. broadcast 'modes.changed'
    cJSON *changed_resp = cJSON_CreateObject(__FUNCTION__);
    if (changed_resp)
    {
        ezlopi_cloud_modes_changed(cj_request, changed_resp);
        if (EZPI_SUCCESS != ezlopi_core_broadcast_add_to_queue(changed_resp))
        {
            cJSON_Delete(__FUNCTION__, changed_resp);
        }
    }

    // 2. broadcast alarm_off.added
    cJSON_AddStringToObject(__FUNCTION__, cj_response, ezlopi_id_str, ezlopi_ui_broadcast_str);
    cJSON_AddStringToObject(__FUNCTION__, cj_response, ezlopi_msg_subclass_str, "hub.modes.alarms_off.added");

    cJSON *cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
    if (cj_params)
    {
        cJSON *cj_mode_id = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_modeId_str);
        cJSON *cj_device_id = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_deviceId_str);
        if (cj_mode_id && cj_device_id)
        {
            cJSON *cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
            if (cj_result)
            {
                cJSON_AddItemToObject(__FUNCTION__, cj_result, ezlopi_modeId_str, cJSON_Duplicate(__FUNCTION__, cj_mode_id, true));
                cJSON_AddItemToObject(__FUNCTION__, cj_result, "alarmsOffDevice", cJSON_Duplicate(__FUNCTION__, cj_device_id, true));
            }
        }
    }
}

void ezlopi_cloud_modes_alarms_off_removed(cJSON *cj_request, cJSON *cj_response)
{
    // cJSON_DeleteItemFromObject(__FUNCTION__, cj_response, ezlopi_id_str);
    // cJSON_DeleteItemFromObject(__FUNCTION__, cj_response, ezlopi_method_str);

    // 1. broadcast 'modes.changed'
    cJSON *changed_resp = cJSON_CreateObject(__FUNCTION__);
    if (changed_resp)
    {
        ezlopi_cloud_modes_changed(cj_request, changed_resp);
        if (EZPI_SUCCESS != ezlopi_core_broadcast_add_to_queue(changed_resp))
        {
            cJSON_Delete(__FUNCTION__, changed_resp);
        }
    }

    // 2. broadcast alarm_off.removed
    cJSON_AddStringToObject(__FUNCTION__, cj_response, ezlopi_id_str, ezlopi_ui_broadcast_str);
    cJSON_AddStringToObject(__FUNCTION__, cj_response, ezlopi_msg_subclass_str, "hub.modes.alarms_off.removed");

    cJSON *cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
    if (cj_params)
    {
        cJSON *cj_mode_id = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_modeId_str);
        cJSON *cj_device_id = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_deviceId_str);
        if (cj_mode_id && cj_device_id)
        {
            cJSON *cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
            if (cj_result)
            {
                cJSON_AddItemToObject(__FUNCTION__, cj_result, ezlopi_modeId_str, cJSON_Duplicate(__FUNCTION__, cj_mode_id, true));
                cJSON_AddItemToObject(__FUNCTION__, cj_result, "alarmsOffDevice", cJSON_Duplicate(__FUNCTION__, cj_device_id, true));
            }
        }
    }
}

void ezlopi_cloud_modes_cameras_off_added(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
}

void ezlopi_cloud_modes_cameras_off_removed(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
}

void ezlopi_cloud_modes_bypass_devices_added(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddStringToObject(__func__, cj_response, ezlopi_id_str, ezlopi_ui_broadcast_str);
    cJSON_AddStringToObject(__func__, cj_response, ezlopi_msg_subclass_str, "hub.modes.bypass_devices.added");

    cJSON *cj_params = cJSON_GetObjectItem(__func__, cj_request, ezlopi_params_str);
    if (cj_params)
    {
        cJSON_AddItemToObject(__func__, cj_response, ezlopi_result_str, cJSON_Duplicate(__func__, cj_params, true));
    }
}

void ezlopi_cloud_modes_bypass_devices_removed(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddStringToObject(__func__, cj_response, ezlopi_id_str, ezlopi_ui_broadcast_str);
    cJSON_AddStringToObject(__func__, cj_response, ezlopi_msg_subclass_str, "hub.modes.bypass_devices.removed");
    cJSON *cj_params = cJSON_GetObjectItem(__func__, cj_request, ezlopi_params_str);
    if (cj_params)
    {
        cJSON_AddItemToObject(__func__, cj_response, ezlopi_result_str, cJSON_Duplicate(__func__, cj_params, true));
    }
}

void ezlopi_cloud_modes_changed(cJSON *cj_request, cJSON *cj_response)
{
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
                s_house_modes_t *update_house_mode = ezlopi_core_modes_get_house_mode_by_id(strtoul(cj_mode_id->valuestring, NULL, 16)); // get the 'running' house mode, indicated by 'ezlopi_mode_t'
                if (update_house_mode)
                {
                    CJSON_ASSIGN_ID(cj_result, update_house_mode->_id, ezlopi_modeId_str);
                    cJSON_AddBoolToObject(__FUNCTION__, cj_result, ezlopi_disarmedDefault_str, update_house_mode->disarmed_default);
                    cJSON_AddNumberToObject(__FUNCTION__, cj_result, ezlopi_timestamp_str, EZPI_CORE_sntp_get_current_time_ms());
                }
            }
        }
    }
}

void ezlopi_cloud_modes_protect_buttons_added(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
}

void ezlopi_cloud_modes_protect_buttons_updated(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
}

void ezlopi_cloud_modes_protect_buttons_removed(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
}

void ezlopi_cloud_modes_protect_devices_added(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
}

void ezlopi_cloud_modes_protect_devices_removed(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
}

void ezlopi_cloud_modes_entry_delay_changed(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_DeleteItemFromObject(__FUNCTION__, cj_response, ezlopi_id_str);
    cJSON_DeleteItemFromObject(__FUNCTION__, cj_response, ezlopi_method_str);

    cJSON_AddStringToObject(__FUNCTION__, cj_response, ezlopi_id_str, ezlopi_ui_broadcast_str);
    cJSON_AddStringToObject(__FUNCTION__, cj_response, ezlopi_msg_subclass_str, "hub.modes.entry_delay.changed");

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
                s_ezlopi_modes_t *curr_mode = ezlopi_core_modes_get_custom_modes();
                if (curr_mode)
                {
                    cJSON_AddNumberToObject(__FUNCTION__, cj_entryDelay, ezlopi_normal_str, curr_mode->entry_delay.normal_delay_sec);
                    cJSON_AddNumberToObject(__FUNCTION__, cj_entryDelay, ezlopi_long_extended_str, curr_mode->entry_delay.long_extended_delay_sec);
                    cJSON_AddNumberToObject(__FUNCTION__, cj_entryDelay, ezlopi_extended_str, curr_mode->entry_delay.extended_delay_sec);
                    cJSON_AddNumberToObject(__FUNCTION__, cj_entryDelay, ezlopi_instant_str, curr_mode->entry_delay.instant_delay_sec);
                }
            }
            cJSON_AddItemToObject(__FUNCTION__, cj_result, ezlopi_timestamp_str, cJSON_CreateNumber(__FUNCTION__, (double)EZPI_CORE_sntp_get_current_time_ms()));
        }
    }
}

#endif // CONFIG_EZPI_SERV_ENABLE_MODES
