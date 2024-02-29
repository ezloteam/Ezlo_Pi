#ifndef __EZLOPI_CLOUD_MODES_UPDATERS_H__
#define __EZLOPI_CLOUD_MODES_UPDATERS_H__

#include <cJSON.h>

void ezlopi_cloud_modes_switched(cJSON *cj_request, cJSON *cj_response);
void ezlopi_cloud_modes_alarmed(cJSON *cj_request, cJSON *cj_response);
void ezlopi_cloud_modes_notifications_notify_all(cJSON *cj_request, cJSON *cj_response);
void ezlopi_cloud_modes_notifications_added(cJSON *cj_request, cJSON *cj_response);
void ezlopi_cloud_modes_notifications_removed(cJSON *cj_request, cJSON *cj_response);
void ezlopi_cloud_modes_disarmed_devices_added(cJSON *cj_request, cJSON *cj_response);
void ezlopi_cloud_modes_disarmed_devices_removed(cJSON *cj_request, cJSON *cj_response);
void ezlopi_cloud_modes_alarms_off_added(cJSON *cj_request, cJSON *cj_response);
void ezlopi_cloud_modes_alarms_off_removed(cJSON *cj_request, cJSON *cj_response);
void ezlopi_cloud_modes_cameras_off_added(cJSON *cj_request, cJSON *cj_response);
void ezlopi_cloud_modes_cameras_off_removed(cJSON *cj_request, cJSON *cj_response);
void ezlopi_cloud_modes_bypass_devices_added(cJSON *cj_request, cJSON *cj_response);
void ezlopi_cloud_modes_bypass_devices_removed(cJSON *cj_request, cJSON *cj_response);
void ezlopi_cloud_modes_changed(cJSON *cj_request, cJSON *cj_response);
void ezlopi_cloud_modes_protect_buttons_added(cJSON *cj_request, cJSON *cj_response);
void ezlopi_cloud_modes_protect_buttons_updated(cJSON *cj_request, cJSON *cj_response);
void ezlopi_cloud_modes_protect_buttons_removed(cJSON *cj_request, cJSON *cj_response);
void ezlopi_cloud_modes_protect_devices_added(cJSON *cj_request, cJSON *cj_response);
void ezlopi_cloud_modes_protect_devices_removed(cJSON *cj_request, cJSON *cj_response);

#endif // __EZLOPI_CLOUD_MODES_UPDATERS_H__
