#include <string.h>

#include "sdkconfig.h"
#include "ezlopi_factory_info.h"
#include "info.h"
#include "trace.h"
#include "frozen.h"
#include "version.h"

#include "cJSON.h"
#include "ezlopi_cloud_methods_str.h"
#include "ezlopi_cloud_keywords.h"

void info_get(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_id_str, cJSON_GetObjectItem(cj_request, ezlopi_id_str));
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_key_method_str, cJSON_GetObjectItem(cj_request, ezlopi_key_method_str));
    cJSON *cjson_result = cJSON_AddObjectToObject(cj_response,ezlopi_result);
    if (cjson_result)
    {
        char tmp_string[64];
        cJSON_AddStringToObject(cjson_result, "model", "");
        cJSON_AddStringToObject(cjson_result, "architecture", "");
        cJSON_AddStringToObject(cjson_result, "firmware", "");
        cJSON_AddStringToObject(cjson_result, "kernel", "");
        cJSON_AddStringToObject(cjson_result, "hardware", "");
        cJSON_AddNumberToObject(cjson_result, "serial", 0);
        cJSON_AddStringToObject(cjson_result, "uuid", "");
        cJSON_AddBoolToObject(cjson_result, "offlineAnonymousAccess", true);
        cJSON_AddBoolToObject(cjson_result, "offlineInsecureAccess", true);

        cJSON *cjson_location = cJSON_AddObjectToObject(cjson_result, "location");
        if (cjson_location)
        {
            cJSON_AddNumberToObject(cjson_location, "latitude", 0);
            cJSON_AddNumberToObject(cjson_location, "longitude", 0);
            cJSON_AddStringToObject(cjson_location, "timezone", "");
            cJSON_AddStringToObject(cjson_location, "state", "");
        }

        cJSON *cjson_build = cJSON_AddObjectToObject(cjson_result, "build");
        if (cjson_build)
        {
            cJSON_AddStringToObject(cjson_build, "time", "");
            cJSON_AddStringToObject(cjson_build, "builder", "krishna kumar sah");
            cJSON_AddStringToObject(cjson_build, "branch", "development");
            cJSON_AddStringToObject(cjson_build, "commit", "**");
        }

        cJSON *cjson_battery = cJSON_AddObjectToObject(cjson_result, "battery");
        if (cjson_battery)
        {
            cJSON_AddNumberToObject(cjson_battery, "stateOfCharge", 0);
            cJSON_AddNumberToObject(cjson_battery, "remainingTime", 0);
            cJSON_AddNumberToObject(cjson_battery, "health", 0);
            cJSON_AddStringToObject(cjson_battery, "status", "");
        }

        cJSON_AddStringToObject(cjson_result, "uptime", "");
        cJSON_AddStringToObject(cjson_result, "localtime", "");
    }
}
