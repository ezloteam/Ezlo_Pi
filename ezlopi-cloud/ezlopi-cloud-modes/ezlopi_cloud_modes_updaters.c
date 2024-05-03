#include <time.h>

#include "ezlopi_core_sntp.h"
#include "ezlopi_core_modes.h"
#include "ezlopi_core_cjson_macros.h"

#include "ezlopi_cloud_modes.h"
#include "ezlopi_cloud_constants.h"
#include "ezlopi_cloud_modes_updaters.h"

#if defined(CONFIG_EZPI_SERV_ENABLE_MODES)
void ezlopi_cloud_modes_switched(cJSON* cj_request, cJSON* cj_response)
{
    cJSON_DeleteItemFromObject(cj_response, ezlopi_id_str);
    cJSON_DeleteItemFromObject(cj_response, ezlopi_method_str);

    cJSON_AddStringToObject(cj_response, ezlopi_id_str, ezlopi_ui_broadcast_str);
    cJSON_AddStringToObject(cj_response, ezlopi_msg_subclass_str, ezlopi_hub_modes_changed_str);

    cJSON* cj_result = cJSON_AddObjectToObject(cj_response, ezlopi_result_str);
    if (cj_result)
    {
        s_house_modes_t* curr_house_mode = ezlopi_core_modes_get_current_house_modes();
        if (curr_house_mode)
        {
            CJSON_ASSIGN_ID(cj_result, curr_house_mode->_id, ezlopi_modeId_str);
            cJSON_AddBoolToObject(cj_result, ezlopi_disarmedDefault_str, curr_house_mode->disarmed_default);
            cJSON_AddNumberToObject(cj_result, ezlopi_timestamp_str, EZPI_CORE_sntp_get_current_time_ms());
        }
    }
}

void ezlopi_cloud_modes_alarmed(cJSON* cj_request, cJSON* cj_response)
{
    cJSON* cj_result = cJSON_AddObjectToObject(cj_response, ezlopi_result_str);
}

void ezlopi_cloud_modes_notifications_notify_all(cJSON* cj_request, cJSON* cj_response)
{
    cJSON* cj_result = cJSON_AddObjectToObject(cj_response, ezlopi_result_str);
}

void ezlopi_cloud_modes_notifications_added(cJSON* cj_request, cJSON* cj_response)
{
    cJSON* cj_result = cJSON_AddObjectToObject(cj_response, ezlopi_result_str);
}

void ezlopi_cloud_modes_notifications_removed(cJSON* cj_request, cJSON* cj_response)
{
    cJSON* cj_result = cJSON_AddObjectToObject(cj_response, ezlopi_result_str);
}

void ezlopi_cloud_modes_disarmed_devices_added(cJSON* cj_request, cJSON* cj_response)
{
    cJSON* cj_result = cJSON_AddObjectToObject(cj_response, ezlopi_result_str);
}

void ezlopi_cloud_modes_disarmed_devices_removed(cJSON* cj_request, cJSON* cj_response)
{
    cJSON* cj_result = cJSON_AddObjectToObject(cj_response, ezlopi_result_str);
}

void ezlopi_cloud_modes_alarms_off_added(cJSON* cj_request, cJSON* cj_response)
{
    cJSON* cj_result = cJSON_AddObjectToObject(cj_response, ezlopi_result_str);
}

void ezlopi_cloud_modes_alarms_off_removed(cJSON* cj_request, cJSON* cj_response)
{
    cJSON* cj_result = cJSON_AddObjectToObject(cj_response, ezlopi_result_str);
}

void ezlopi_cloud_modes_cameras_off_added(cJSON* cj_request, cJSON* cj_response)
{
    cJSON* cj_result = cJSON_AddObjectToObject(cj_response, ezlopi_result_str);
}

void ezlopi_cloud_modes_cameras_off_removed(cJSON* cj_request, cJSON* cj_response)
{
    cJSON* cj_result = cJSON_AddObjectToObject(cj_response, ezlopi_result_str);
}

void ezlopi_cloud_modes_bypass_devices_added(cJSON* cj_request, cJSON* cj_response)
{
    cJSON* cj_result = cJSON_AddObjectToObject(cj_response, ezlopi_result_str);
}

void ezlopi_cloud_modes_bypass_devices_removed(cJSON* cj_request, cJSON* cj_response)
{
    cJSON* cj_result = cJSON_AddObjectToObject(cj_response, ezlopi_result_str);
}

void ezlopi_cloud_modes_changed(cJSON* cj_request, cJSON* cj_response)
{
    cJSON* cj_result = cJSON_AddObjectToObject(cj_response, ezlopi_result_str);
}

void ezlopi_cloud_modes_protect_buttons_added(cJSON* cj_request, cJSON* cj_response)
{
    cJSON* cj_result = cJSON_AddObjectToObject(cj_response, ezlopi_result_str);
}

void ezlopi_cloud_modes_protect_buttons_updated(cJSON* cj_request, cJSON* cj_response)
{
    cJSON* cj_result = cJSON_AddObjectToObject(cj_response, ezlopi_result_str);
}

void ezlopi_cloud_modes_protect_buttons_removed(cJSON* cj_request, cJSON* cj_response)
{
    cJSON* cj_result = cJSON_AddObjectToObject(cj_response, ezlopi_result_str);
}

void ezlopi_cloud_modes_protect_devices_added(cJSON* cj_request, cJSON* cj_response)
{
    cJSON* cj_result = cJSON_AddObjectToObject(cj_response, ezlopi_result_str);
}

void ezlopi_cloud_modes_protect_devices_removed(cJSON* cj_request, cJSON* cj_response)
{
    cJSON* cj_result = cJSON_AddObjectToObject(cj_response, ezlopi_result_str);
}

#endif // CONFIG_EZPI_SERV_ENABLE_MODES
