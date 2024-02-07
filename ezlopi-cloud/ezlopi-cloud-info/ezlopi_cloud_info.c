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
#include "ezlopi_core_factory_info.h"

char *ezlopi_tick_to_time(uint32_t ms)
{
    uint32_t seconds = ms / 1000;
    uint32_t minutes = seconds / 60;
    uint32_t hours = minutes / 60;
    uint32_t days = hours / 24;

    seconds %= 60;
    minutes %= 60;
    hours %= 24;

    char *time_str = malloc(50);
    if (time_str)
    {
        memset(time_str, 0, 50);
        snprintf(time_str, 50, "%dd %dh %dm %ds", days, hours, minutes, seconds);
    }
    else
    {
        time_str = NULL;
    }

    return time_str;
}

void info_get(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_id_str, cJSON_GetObjectItem(cj_request, ezlopi_id_str));
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_method_str, cJSON_GetObjectItem(cj_request, ezlopi_method_str));
    cJSON *cjson_result = cJSON_AddObjectToObject(cj_response, ezlopi_result_str);
    if (cjson_result)
    {
        char *device_uuid = ezlopi_factory_info_v3_get_device_uuid();
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

        cJSON *cjson_location = cJSON_AddObjectToObject(cjson_result, "location");
        if (cjson_location)
        {
            cJSON_AddNumberToObject(cjson_location, "latitude", 0);
            cJSON_AddNumberToObject(cjson_location, "longitude", 0);
            cJSON_AddStringToObject(cjson_location, "timezone", ezlopi__str);
            cJSON_AddStringToObject(cjson_location, "state", ezlopi__str);
        }

        cJSON *cjson_build = cJSON_AddObjectToObject(cjson_result, "build");
        if (cjson_build)
        {
            cJSON_AddStringToObject(cjson_build, "time", COMPILE_TIME);
            cJSON_AddStringToObject(cjson_build, "builder", DEVELOPER);
            cJSON_AddStringToObject(cjson_build, "branch", CURRENT_BRANCH);
            cJSON_AddStringToObject(cjson_build, "commit", COMMIT_HASH);
        }

        cJSON *cjson_battery = cJSON_AddObjectToObject(cjson_result, "battery");
        if (cjson_battery)
        {
            cJSON_AddNumberToObject(cjson_battery, "stateOfCharge", 0);
            cJSON_AddNumberToObject(cjson_battery, "remainingTime", 0);
            cJSON_AddNumberToObject(cjson_battery, "health", 0);
            cJSON_AddStringToObject(cjson_battery, ezlopi_status_str, ezlopi__str);
        }

        // time_t now;
        // char strftime_buf[64];
        // struct tm timeinfo;

        // time(&now);
        // setenv("TZ", "UTC-5:45", 1);
        // tzset();
        // localtime_r(&now, &timeinfo);
        // strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);

        cJSON_AddStringToObject(cjson_result, "localtime", ezlopi__str);
        char *time_string = ezlopi_tick_to_time((uint32_t)(xTaskGetTickCount() / portTICK_PERIOD_MS));
        if (time_string)
        {
            cJSON_AddStringToObject(cjson_result, ezlopi_uptime_str, time_string);
            free(time_string);
        }
        else
        {
            cJSON_AddStringToObject(cjson_result, ezlopi_uptime_str, ezlopi__str);
        }
        ezlopi_factory_info_v3_free(device_uuid);
    }
}
