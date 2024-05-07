#if 0

#include <string.h>

#include "cjext.h"
#include "ezlopi_cloud_data.h"
#include "ezlopi_util_trace.h"

#include "ezlopi_core_devices_list.h"
#include "ezlopi_cloud_keywords.h"
#include "ezlopi_cloud_methods_str.h"

void data_list(cJSON* cj_request, cJSON* cj_response)
{
    cJSON* cjson_result = cJSON_AddObjectToObject(cj_response, ezlopi_result_str);
    if (cjson_result)
    {
        cJSON* cj_settings = cJSON_AddObjectToObject(cjson_result, ezlopi_settings_str);
        if (cj_settings)
        {
            cJSON* cj_first_start = cJSON_AddObjectToObject(cj_settings, ezlopi_first_start_str);
            if (cj_first_start)
            {
                cJSON_AddNumberToObject(cj_first_start, ezlopi_value_str, 0);
            }

            #warning "WARNING: work required here!"
#if 0
                l_ezlopi_configured_devices_t* registered_devices = ezlopi_devices_list_get_configured_items();
            while (NULL != registered_devices)
            {
                if (NULL != registered_devices->properties)
                {
                    break;
                }

                registered_devices = registered_devices->next;
            }
#endif
        }
    }
}

#if 0
static cJSON* ezlopi_cloud_data_create_device_list(void)
{
    cJSON* cjson_device_list = cJSON_CreateObject();

    if (cjson_device_list)
    {
        cJSON_AddNumberToObject(cjson_device_list, "ids", 1234);
    }

    return cjson_device_list;
}

static cJSON* ezlopi_cloud_data_create_settings_list(void)
{
    cJSON* cjson_device_list = cJSON_CreateObject();

    if (cjson_device_list)
    {
        cJSON_AddNumberToObject(cjson_device_list, "ids", 1234);
    }

    return cjson_device_list;
}

static cJSON* ezlopi_cloud_data_list_settings(l_ezlopi_configured_devices_t* ezlopi_device)
{
    cJSON* cjson_settings = NULL;
    if (ezlopi_device)
    {
        cjson_settings = cJSON_CreateObject();
        if (cjson_settings)
        {
            char tmp_string[64];
            snprintf(tmp_string, sizeof(tmp_string), "%08x", ezlopi_device->properties->ezlopi_cloud.device_id);
            cJSON_AddStringToObject(cjson_settings, ezlopi__id_str, tmp_string);
            cJSON_AddStringToObject(cjson_settings, ezlopi_deviceTypeId_str, ezlopi_ezlopi_str);
            cJSON_AddStringToObject(cjson_settings, ezlopi_parentDeviceId_str, ezlopi__str);
            cJSON_AddStringToObject(cjson_settings, ezlopi_category_str, ezlopi_device->properties->ezlopi_cloud.category);
            cJSON_AddStringToObject(cjson_settings, ezlopi_subcategory_str, ezlopi_device->properties->ezlopi_cloud.subcategory);
            cJSON_AddBoolToObject(cjson_settings, ezlopi_batteryPowered_str, ezlopi_device->properties->ezlopi_cloud.battery_powered);
            cJSON_AddStringToObject(cjson_settings, "name", ezlopi_device->properties->ezlopi_cloud.device_name);
            cJSON_AddStringToObject(cjson_settings, ezlopi_type_str, ezlopi_device->properties->ezlopi_cloud.device_type);
            cJSON_AddBoolToObject(cjson_settings, ezlopi_reachable_str, ezlopi_device->properties->ezlopi_cloud.reachable);
            cJSON_AddBoolToObject(cjson_settings, ezlopi_persistent_str, true);
            cJSON_AddBoolToObject(cjson_settings, ezlopi_serviceNotification_str, false);
            cJSON_AddBoolToObject(cjson_settings, "armed", false);
            snprintf(tmp_string, sizeof(tmp_string), "%08x", ezlopi_device->properties->ezlopi_cloud.room_id);
            cJSON_AddStringToObject(cjson_settings, ezlopi_roomId_str, tmp_string);
            cJSON_AddStringToObject(cjson_settings, ezlopi_security_str, ezlopi__str);
            cJSON_AddBoolToObject(cjson_settings, ezlopi_ready_str, true);
            cJSON_AddStringToObject(cjson_settings, ezlopi_status_str, ezlopi_synced_str);
            cJSON_AddStringToObject(cjson_settings, ezlopi_info_str, "{}");
        }
    }

    return cjson_settings;
}
#endif

#endif