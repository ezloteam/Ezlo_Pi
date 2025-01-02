#ifndef __HUB_SETTINGS_H__
#define __HUB_SETTINGS_H__

#include <string.h>
#include "cjext.h"

#include "ezlopi_core_settings.h"
#include "ezlopi_core_devices.h"

#ifdef __cplusplus
extern "C"
{
#endif

    void ezlopi_device_settings_list_v3(cJSON* cj_request, cJSON* cj_response);
    void ezlopi_device_settings_value_set_v3(cJSON* cj_request, cJSON* cj_response);
    void ezlopi_device_settings_reset_v3(cJSON* cj_request, cJSON* cj_response);
    cJSON* ezlopi_cloud_settings_updated_from_devices_v3(l_ezlopi_device_t* device, l_ezlopi_device_settings_v3_t* setting);

#ifdef __cplusplus
}
#endif

#endif // __HUB_SETTINGS_H__
