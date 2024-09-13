#include <time.h>

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
        s_house_modes_t *curr_house_mode = ezlopi_core_modes_get_current_house_modes();
        if (curr_house_mode)
        {
            CJSON_ASSIGN_ID(cj_result, curr_house_mode->_id, ezlopi_modeId_str);
            cJSON_AddBoolToObject(__FUNCTION__, cj_result, ezlopi_disarmedDefault_str, curr_house_mode->disarmed_default);
            cJSON_AddNumberToObject(__FUNCTION__, cj_result, ezlopi_timestamp_str, EZPI_CORE_sntp_get_current_time_ms());
        }
    }
}

void ezlopi_cloud_modes_alarmed(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
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
    cJSON_DeleteItemFromObject(__FUNCTION__, cj_response, ezlopi_id_str);
    cJSON_DeleteItemFromObject(__FUNCTION__, cj_response, ezlopi_method_str);

#warning "need to add 'hub.modes.changed' broadcast";

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
    cJSON_DeleteItemFromObject(__FUNCTION__, cj_response, ezlopi_id_str);
    cJSON_DeleteItemFromObject(__FUNCTION__, cj_response, ezlopi_method_str);

#warning "need to add 'hub.modes.changed' broadcast";

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
    cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
}

void ezlopi_cloud_modes_bypass_devices_removed(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
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
        s_house_modes_t *curr_house_mode = ezlopi_core_modes_get_current_house_modes();
        if (curr_house_mode)
        {
            CJSON_ASSIGN_ID(cj_result, curr_house_mode->_id, ezlopi_modeId_str);
            cJSON_AddBoolToObject(__FUNCTION__, cj_result, ezlopi_disarmedDefault_str, curr_house_mode->disarmed_default);
            cJSON_AddNumberToObject(__FUNCTION__, cj_result, ezlopi_timestamp_str, EZPI_CORE_sntp_get_current_time_ms());
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
                    cJSON_AddNumberToObject(__FUNCTION__, cj_entryDelay, ezlopi_short_str, curr_mode->entry_delay.short_delay_sec);
                    cJSON_AddNumberToObject(__FUNCTION__, cj_entryDelay, ezlopi_extended_str, curr_mode->entry_delay.extended_delay_sec);
                    cJSON_AddNumberToObject(__FUNCTION__, cj_entryDelay, ezlopi_instant_str, curr_mode->entry_delay.instant_delay_sec);
                }
            }
            cJSON_AddItemToObject(__FUNCTION__, cj_result, ezlopi_timestamp_str, cJSON_CreateNumber(__FUNCTION__, (double)EZPI_CORE_sntp_get_current_time_ms()));
        }
    }
}

#endif // CONFIG_EZPI_SERV_ENABLE_MODES
