#include <string.h>
#include <time.h>
#include "esp_sntp.h"

#include "freertos/FreeRTOS.h"

#include "sdkconfig.h"
#include "ezlopi_core_factory_info.h"
#include "ezlopi_cloud_info.h"
#include "ezlopi_util_trace.h"

#include "ezlopi_util_version.h"

#include "cJSON.h"
#include "ezlopi_cloud_methods_str.h"
#include "ezlopi_cloud_keywords.h"
#include "ezlopi_cloud_coordinates.h"
#include "ezlopi_core_factory_info.h"
#include "ezlopi_core_sntp.h"

void ezlopi_tick_to_time(char* time_buff, uint32_t buff_len, uint32_t ms)
{
    uint32_t seconds = ms / 1000;
    uint32_t minutes = seconds / 60;
    uint32_t hours = minutes / 60;
    uint32_t days = hours / 24;

    seconds %= 60;
    minutes %= 60;
    hours %= 24;

    if (time_buff && buff_len)
    {
        memset(time_buff, 0, buff_len);
        snprintf(time_buff, buff_len, "%dd %dh %dm %ds", days, hours, minutes, seconds);
    }
}

void info_get(cJSON* cj_request, cJSON* cj_response)
{
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_id_str, cJSON_GetObjectItem(cj_request, ezlopi_id_str));
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_method_str, cJSON_GetObjectItem(cj_request, ezlopi_method_str));
    cJSON* cjson_result = cJSON_AddObjectToObject(cj_response, ezlopi_result_str);
    if (cjson_result)
    {
        char* device_uuid = ezlopi_factory_info_v3_get_device_uuid();
        // #include "esp_app_format.h"
        cJSON_AddStringToObject(cjson_result, ezlopi_model_str, ezlopi_factory_info_v3_get_device_type());
        cJSON_AddStringToObject(cjson_result, "architecture", CONFIG_SDK_TOOLPREFIX);
        cJSON_AddStringToObject(cjson_result, ezlopi_firmware_str, VERSION_STR);
        cJSON_AddStringToObject(cjson_result, "kernel", "FreeRTOS");
        cJSON_AddStringToObject(cjson_result, "hardware", CONFIG_IDF_TARGET);
        cJSON_AddNumberToObject(cjson_result, ezlopi_serial_str, ezlopi_factory_info_v3_get_id());

        cJSON_AddStringToObject(cjson_result, ezlopi_uuid_str, device_uuid ? device_uuid : ezlopi__str);
        cJSON_AddBoolToObject(cjson_result, "offlineAnonymousAccess", true);
        cJSON_AddBoolToObject(cjson_result, "offlineInsecureAccess", true);

        cJSON* cjson_location = cJSON_AddObjectToObject(cjson_result, "location");
        if (cjson_location)
        {
            cJSON_AddNumberToObject(cjson_location, "latitude", ezlopi_cloud_get_latitude());
            cJSON_AddNumberToObject(cjson_location, "longitude", ezlopi_cloud_get_longitude());
            char* location = EZPI_CORE_sntp_get_location();
            if (location)
            {
                cJSON_AddStringToObject(cjson_location, "timezone", location);
                cJSON_AddStringToObject(cjson_location, "state", ezlopi_custom_timezone_str);
                free(location);
            }
            else
            {
                cJSON_AddStringToObject(cjson_location, "timezone", ezlopi_gmt0_str);
                cJSON_AddStringToObject(cjson_location, "state", ezlopi_default_str);
            }
        }

        cJSON* cjson_build = cJSON_AddObjectToObject(cjson_result, "build");
        if (cjson_build)
        {
            {
                char build_time[64];
                EZPI_CORE_sntp_epoch_to_iso8601(build_time, sizeof(build_time), (time_t)BUILD_DATE);
                cJSON_AddStringToObject(cjson_build, "time", build_time);
            }

            cJSON_AddStringToObject(cjson_build, "builder", DEVELOPER);
            cJSON_AddStringToObject(cjson_build, "branch", CURRENT_BRANCH);
            cJSON_AddStringToObject(cjson_build, "commit", COMMIT_HASH);
        }

        cJSON* cjson_battery = cJSON_AddObjectToObject(cjson_result, "battery");
        if (cjson_battery)
        {
            cJSON_AddNumberToObject(cjson_battery, "stateOfCharge", 0);
            cJSON_AddNumberToObject(cjson_battery, "remainingTime", 0);
            cJSON_AddNumberToObject(cjson_battery, "health", 0);
            cJSON_AddStringToObject(cjson_battery, ezlopi_status_str, ezlopi__str);
        }

        {
            char local_time[100];
            EZPI_CORE_sntp_get_local_time(local_time, sizeof(local_time));
            cJSON_AddStringToObject(cjson_result, "localtime", local_time);
        }

#if 0
        {
            char local_time[100];
            EZPI_CORE_sntp_get_up_time(local_time, sizeof(local_time));
            cJSON_AddStringToObject(cjson_result, ezlopi_uptime_str, local_time);
        }
#else
        {
            char time_string[50];
            uint32_t tick_count_ms = xTaskGetTickCount() / portTICK_PERIOD_MS;
            ezlopi_tick_to_time(tick_count_ms, time_string, sizeof(time_string));
            cJSON_AddStringToObject(cjson_result, ezlopi_uptime_str, time_string);
        }

#endif
        ezlopi_factory_info_v3_free(device_uuid);
    }
}
