#ifndef __HUB_MODES_H__
#define __HUB_MODES_H__

#include <string.h>

#include "cjext.h"

#ifdef __cplusplus
extern "C"
{
#endif

    void ezlopi_cloud_modes_get(cJSON* cj_request, cJSON* cj_response);
    void ezlopi_cloud_modes_current_get(cJSON* cj_request, cJSON* cj_response);
    void ezlopi_cloud_modes_switch(cJSON* cj_request, cJSON* cj_response);
    void ezlopi_cloud_modes_cancel_switch(cJSON* cj_request, cJSON* cj_response);
    void ezlopi_cloud_modes_entry_delay_cancel(cJSON* cj_request, cJSON* cj_response);
    void ezlopi_cloud_modes_entry_delay_skip(cJSON* cj_request, cJSON* cj_response);
    void ezlopi_cloud_modes_switch_to_delay_set(cJSON* cj_request, cJSON* cj_response);
    void ezlopi_cloud_modes_alarm_delay_set(cJSON* cj_request, cJSON* cj_response);
    void ezlopi_cloud_modes_notifications_set(cJSON* cj_request, cJSON* cj_response);
    void ezlopi_cloud_modes_disarmed_default_set(cJSON* cj_request, cJSON* cj_response);
    void ezlopi_cloud_modes_disarmed_devices_add(cJSON* cj_request, cJSON* cj_response);
    void ezlopi_cloud_modes_disarmed_devices_remove(cJSON* cj_request, cJSON* cj_response);
    void ezlopi_cloud_modes_alarms_off_add(cJSON* cj_request, cJSON* cj_response);
    void ezlopi_cloud_modes_alarms_off_remove(cJSON* cj_request, cJSON* cj_response);
    void ezlopi_cloud_modes_cameras_off_add(cJSON* cj_request, cJSON* cj_response);
    void ezlopi_cloud_modes_cameras_off_remove(cJSON* cj_request, cJSON* cj_response);
    void ezlopi_cloud_modes_bypass_devices_add(cJSON* cj_request, cJSON* cj_response);
    void ezlopi_cloud_modes_bypass_devices_remove(cJSON* cj_request, cJSON* cj_response);
    void ezlopi_cloud_modes_protect_set(cJSON* cj_request, cJSON* cj_response);
    void ezlopi_cloud_modes_protect_buttons_set(cJSON* cj_request, cJSON* cj_response);
    void ezlopi_cloud_modes_protect_devices_add(cJSON* cj_request, cJSON* cj_response);
    void ezlopi_cloud_modes_protect_devices_remove(cJSON* cj_request, cJSON* cj_response);
    void ezlopi_cloud_modes_entry_delay_set(cJSON* cj_request, cJSON* cj_response);
    void ezlopi_cloud_modes_entry_delay_reset(cJSON* cj_request, cJSON* cj_response);

#ifdef __cplusplus
}
#endif

#endif // __HUB_MODES_H__