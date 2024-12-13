#include <string.h>
#include <time.h>
#include "esp_sntp.h"

#include "freertos/FreeRTOS.h"

#include "../../build/config/sdkconfig.h"
#include "ezlopi_core_factory_info.h"
#include "ezlopi_cloud_info.h"
#include "ezlopi_util_trace.h"

#include "ezlopi_util_version.h"

#include "cjext.h"
#include "ezlopi_cloud_methods_str.h"
#include "ezlopi_cloud_keywords.h"
#include "ezlopi_cloud_coordinates.h"
#include "ezlopi_core_factory_info.h"
#include "ezlopi_core_sntp.h"
#include "ezlopi_core_info.h"

void info_get(cJSON *cj_request, cJSON *cj_response)
{
    cJSON *cjson_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    if (cjson_result)
    {
        char *device_uuid = ezlopi_factory_info_v3_get_device_uuid();
        // #include "esp_app_format.h"
        cJSON_AddStringToObject(__FUNCTION__, cjson_result, ezlopi_model_str, ezlopi_factory_info_v3_get_device_type());
        cJSON_AddStringToObject(__FUNCTION__, cjson_result, "architecture", CONFIG_SDK_TOOLPREFIX);
        cJSON_AddStringToObject(__FUNCTION__, cjson_result, ezlopi_firmware_str, VERSION_STR);
        cJSON_AddStringToObject(__FUNCTION__, cjson_result, "kernel", "FreeRTOS");
        cJSON_AddStringToObject(__FUNCTION__, cjson_result, "hardware", CONFIG_IDF_TARGET);
        cJSON_AddNumberToObject(__FUNCTION__, cjson_result, ezlopi_serial_str, ezlopi_factory_info_v3_get_id());

        cJSON_AddStringToObject(__FUNCTION__, cjson_result, ezlopi_uuid_str, device_uuid ? device_uuid : ezlopi__str);
        cJSON_AddBoolToObject(__FUNCTION__, cjson_result, "offlineAnonymousAccess", true);
        cJSON_AddBoolToObject(__FUNCTION__, cjson_result, "offlineInsecureAccess", true);

        cJSON *cjson_location = cJSON_AddObjectToObject(__FUNCTION__, cjson_result, "location");
        if (cjson_location)
        {
            cJSON_AddNumberToObject(__FUNCTION__, cjson_location, "latitude", EZPI_cloud_get_latitude());
            cJSON_AddNumberToObject(__FUNCTION__, cjson_location, "longitude", EZPI_cloud_get_longitude());
            char *location = EZPI_CORE_sntp_get_location();
            if (location)
            {
                cJSON_AddStringToObject(__FUNCTION__, cjson_location, "timezone", location);
                cJSON_AddStringToObject(__FUNCTION__, cjson_location, "state", ezlopi_custom_timezone_str);
                ezlopi_free(__FUNCTION__, location);
            }
            else
            {
                cJSON_AddStringToObject(__FUNCTION__, cjson_location, "timezone", ezlopi_gmt0_str);
                cJSON_AddStringToObject(__FUNCTION__, cjson_location, "state", ezlopi_default_str);
            }
        }

        cJSON *cjson_build = cJSON_AddObjectToObject(__FUNCTION__, cjson_result, "build");
        if (cjson_build)
        {
            {
                char build_time[64];
                EZPI_CORE_sntp_epoch_to_iso8601(build_time, sizeof(build_time), (time_t)BUILD_DATE);
                cJSON_AddStringToObject(__FUNCTION__, cjson_build, "time", build_time);
            }

            cJSON_AddStringToObject(__FUNCTION__, cjson_build, "builder", DEVELOPER);
            cJSON_AddStringToObject(__FUNCTION__, cjson_build, "branch", CURRENT_BRANCH);
            cJSON_AddStringToObject(__FUNCTION__, cjson_build, "commit", COMMIT_HASH);
        }

        cJSON *cjson_battery = cJSON_AddObjectToObject(__FUNCTION__, cjson_result, "battery");
        if (cjson_battery)
        {
            cJSON_AddNumberToObject(__FUNCTION__, cjson_battery, "stateOfCharge", 0);
            cJSON_AddNumberToObject(__FUNCTION__, cjson_battery, "remainingTime", 0);
            cJSON_AddNumberToObject(__FUNCTION__, cjson_battery, "health", 0);
            cJSON_AddStringToObject(__FUNCTION__, cjson_battery, ezlopi_status_str, ezlopi__str);
        }

        {
            char local_time[100];
            EZPI_CORE_sntp_get_local_time(local_time, sizeof(local_time));
            cJSON_AddStringToObject(__FUNCTION__, cjson_result, "localtime", local_time);
        }

#if 0
        {
            char local_time[100];
            EZPI_CORE_sntp_get_up_time(local_time, sizeof(local_time));
            cJSON_AddStringToObject(__FUNCTION__, cjson_result, ezlopi_uptime_str, local_time);
        }
#else
        {
            char time_string[50];
            uint32_t tick_count_ms = xTaskGetTickCount() / portTICK_PERIOD_MS;
            EZPI_CORE_info_get_tick_to_time_name(time_string, sizeof(time_string), tick_count_ms);
            cJSON_AddStringToObject(__FUNCTION__, cjson_result, ezlopi_uptime_str, time_string);
        }

#endif
        ezlopi_factory_info_v3_free(device_uuid);
    }
}
