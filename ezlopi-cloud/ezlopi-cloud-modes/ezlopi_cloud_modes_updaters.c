#include <time.h>

#include "ezlopi_core_sntp.h"
#include "ezlopi_core_modes.h"
#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_core_ezlopi_broadcast.h"

#include "ezlopi_cloud_modes.h"
#include "ezlopi_cloud_constants.h"
#include "ezlopi_cloud_modes_updaters.h"

#if defined(CONFIG_EZPI_SERV_ENABLE_MODES)
void ezlopi_cloud_modes_switched(cJSON* cj_request, cJSON* cj_response)
{
    cJSON_DeleteItemFromObject(__FUNCTION__, cj_response, ezlopi_id_str);
    cJSON_DeleteItemFromObject(__FUNCTION__, cj_response, ezlopi_method_str);

    cJSON_AddStringToObject(__FUNCTION__, cj_response, ezlopi_id_str, ezlopi_ui_broadcast_str);
    cJSON_AddStringToObject(__FUNCTION__, cj_response, ezlopi_msg_subclass_str, ezlopi_hub_modes_changed_str);

    cJSON* cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    if (cj_result)
    {
        s_house_modes_t* curr_house_mode = ezlopi_core_modes_get_current_house_modes();
        if (curr_house_mode)
        {
            CJSON_ASSIGN_ID(cj_result, curr_house_mode->_id, ezlopi_modeId_str);
            cJSON_AddBoolToObject(__FUNCTION__, cj_result, ezlopi_disarmedDefault_str, curr_house_mode->disarmed_default);
            cJSON_AddNumberToObject(__FUNCTION__, cj_result, ezlopi_timestamp_str, EZPI_CORE_sntp_get_current_time_ms());
        }
    }
}

void ezlopi_cloud_modes_alarmed(cJSON* cj_request, cJSON* cj_response)
{
    cJSON* cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
}

void ezlopi_cloud_modes_notifications_notify_all(cJSON* cj_request, cJSON* cj_response)
{
    cJSON* cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
}

void ezlopi_cloud_modes_notifications_added(cJSON* cj_request, cJSON* cj_response)
{
    cJSON* cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
}

void ezlopi_cloud_modes_notifications_removed(cJSON* cj_request, cJSON* cj_response)
{
    cJSON* cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
}

void ezlopi_cloud_modes_disarmed_devices_added(cJSON* cj_request, cJSON* cj_response)
{
    cJSON* cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
}

void ezlopi_cloud_modes_disarmed_devices_removed(cJSON* cj_request, cJSON* cj_response)
{
    cJSON* cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
}

void ezlopi_cloud_modes_alarms_off_added(cJSON* cj_request, cJSON* cj_response)
{
    cJSON* cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
}

void ezlopi_cloud_modes_alarms_off_removed(cJSON* cj_request, cJSON* cj_response)
{
    cJSON* cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
}

void ezlopi_cloud_modes_cameras_off_added(cJSON* cj_request, cJSON* cj_response)
{
    cJSON* cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
}

void ezlopi_cloud_modes_cameras_off_removed(cJSON* cj_request, cJSON* cj_response)
{
    cJSON* cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
}

void ezlopi_cloud_modes_bypass_devices_added(cJSON* cj_request, cJSON* cj_response)
{
    cJSON* cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
}

void ezlopi_cloud_modes_bypass_devices_removed(cJSON* cj_request, cJSON* cj_response)
{
    cJSON* cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
}

void ezlopi_cloud_modes_changed(cJSON* cj_request, cJSON* cj_response)
{
    cJSON* cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
}

void ezlopi_cloud_modes_protect_buttons_added(cJSON* cj_request, cJSON* cj_response)
{
    cJSON* cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
}

void ezlopi_cloud_modes_protect_buttons_updated(cJSON* cj_request, cJSON* cj_response)
{
    cJSON* cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
}

void ezlopi_cloud_modes_protect_buttons_removed(cJSON* cj_request, cJSON* cj_response)
{
    cJSON* cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
}

void ezlopi_cloud_modes_protect_devices_added(cJSON* cj_request, cJSON* cj_response)
{
    cJSON* cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
}

void ezlopi_cloud_modes_protect_devices_removed(cJSON* cj_request, cJSON* cj_response)
{
    cJSON* cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
}

void ezlopi_cloud_modes_changed_updater(cJSON* cj_request, cJSON* cj_response)
{
    cJSON_AddStringToObject(__func__, cj_response, ezlopi_id_str, ezlopi_ui_broadcast_str);
    cJSON_AddStringToObject(__func__, cj_response, ezlopi_msg_subclass_str, ezlopi_modes_changed_str);
    cJSON* cj_params = cJSON_GetObjectItem(__func__, cj_request, ezlopi_params_str);
    if (cj_params)
    {
        cJSON* cj_result = cJSON_Duplicate(__func__, cj_params, true);
        if (cj_result)
        {
            uint64_t time = EZPI_CORE_sntp_get_current_time_ms();
            cJSON_AddNumberToObject(__func__, cj_result, "fwTimestampMs", time);
            cJSON_AddItemToObject(__func__, cj_response, ezlopi_result_str, cj_result);
        }

    }
}

void ezlopi_cloud_modes_bypass_device_added_updater(cJSON* cj_request, cJSON* cj_response)
{
    cJSON_AddStringToObject(__func__, cj_response, ezlopi__id_str, ezlopi_ui_broadcast_str);
    cJSON_AddStringToObject(__func__, cj_response, ezlopi_msg_subclass_str, ezlopi_modes_bypass_device_added);
    cJSON* cj_params = cJSON_GetObjectItem(__func__, cj_request, ezlopi_params_str);
    if (cj_params)
    {
        cJSON* cj_result = cJSON_Duplicate(__func__, cj_params, true);
        if (cj_result)
        {
            cJSON_AddItemToObject(__func__, cj_response, ezlopi_result_str, cj_result);
        }
    }
}

void ezlopi_cloud_modes_bypass_device_removed_updater(cJSON* cj_request, cJSON* cj_response)
{
    cJSON_AddStringToObject(__func__, cj_response, ezlopi__id_str, ezlopi_ui_broadcast_str);
    cJSON_AddStringToObject(__func__, cj_response, ezlopi_msg_subclass_str, ezlopi_modes_bypass_device_removed);
    cJSON* cj_params = cJSON_GetObjectItem(__func__, cj_request, ezlopi_params_str);
    if (cj_params)
    {
        cJSON* cj_result = cJSON_Duplicate(__func__, cj_params, true);
        if (cj_result)
        {
            cJSON_AddItemToObject(__func__, cj_response, ezlopi_result_str, cj_result);
        }
    }
}

static void ezlopi_cloud_modes_send_protect_buttons_add_boradcast(cJSON* cj_request)
{
    cJSON* cj_response = cJSON_CreateObject(__FUNCTION__);
    if (cj_response)
    {
        cJSON_AddStringToObject(__FUNCTION__, cj_response, ezlopi__id_str, ezlopi_ui_broadcast_str);
        cJSON_AddStringToObject(__FUNCTION__, cj_response, ezlopi_msg_subclass_str, "hub.modes.protect.buttons.added");
        cJSON* cj_params = cJSON_GetObjectItem(__func__, cj_request, ezlopi_params_str);
        if (cj_params)
        {
            cJSON* cj_result = cJSON_Duplicate(__func__, cj_params, true);
            if (cj_result)
            {
                cJSON_AddItemToObject(__func__, cj_response, ezlopi_result_str, cj_result);
                if (!ezlopi_core_ezlopi_broadcast_add_to_queue(cj_response))
                {
                    cJSON_Delete(__func__, cj_response);
                }
            }
        }
    }
}


void ezlopi_cloud_modes_protect_button_set_updater(cJSON* cj_request, cJSON* cj_response)
{
    ezlopi_cloud_modes_send_protect_buttons_add_boradcast(cj_request);
    if (cj_response)
    {
        cJSON_AddStringToObject(__FUNCTION__, cj_response, ezlopi__id_str, ezlopi_ui_broadcast_str);
        cJSON_AddStringToObject(__FUNCTION__, cj_response, ezlopi_msg_subclass_str, "hub.modes.protect.buttons.updated");
        cJSON* cj_params = cJSON_GetObjectItem(__func__, cj_request, ezlopi_params_str);
        if (cj_params)
        {
            cJSON* cj_result = cJSON_Duplicate(__func__, cj_params, true);
            if (cj_result)
            {
                cJSON_AddItemToObject(__func__, cj_response, ezlopi_result_str, cj_result);
            }
        }
    }
    #warning("hub.modes.protect.buttons removed udpated")
}

void ezlopi_cloud_modes_protect_device_add_updater(cJSON* cj_reuest, cJSON* cj_response)
{
    if(cj_response)
    {
        cJSON_AddStringToObject(__FUNCTION__, cj_response, ezlopi__id_str, ezlopi_ui_broadcast_str);
        cJSON_AddStringToObject(__FUNCTION__, cj_response, ezlopi_msg_subclass_str, "hub.modes.protect.devices.added");
        cJSON* cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_reuest, ezlopi_params_str);
        if(cj_params)
        {
            cJSON* cj_result = cJSON_Duplicate(__FUNCTION__, cj_params, true);
            if(cj_result)
            {
                cJSON_AddItemToObject(__FUNCTION__, cj_response, ezlopi_result_str, cj_result);
            }
        }
    }
}

#endif // CONFIG_EZPI_SERV_ENABLE_MODES
