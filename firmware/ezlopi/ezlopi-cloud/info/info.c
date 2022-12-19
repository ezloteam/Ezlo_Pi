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

cJSON *info_get(const char *payload, uint32_t len, struct json_token *method_tok, uint32_t msg_count)
{
    char *string_response = NULL;
    cJSON *cjson_response = cJSON_CreateObject();
    cJSON *cjson_request = cJSON_ParseWithLength(payload, len);

    if (cjson_request)
    {
        cJSON *id = cJSON_GetObjectItem(cjson_request, ezlopi_id_str);
        cJSON *sender = cJSON_GetObjectItem(cjson_request, ezlopi_sender_str);

        if (cjson_response)
        {
            cJSON_AddStringToObject(cjson_response, ezlopi_key_method_str, method_hub_info_get);
            cJSON_AddNumberToObject(cjson_response, ezlopi_msg_id_str, msg_count);
            cJSON_AddStringToObject(cjson_response, ezlopi_id_str, id ? (id->valuestring ? id->valuestring : "") : "");
            cJSON_AddStringToObject(cjson_response, ezlopi_sender_str, sender ? (sender->valuestring ? sender->valuestring : "{}") : "{}");
            cJSON_AddNullToObject(cjson_response, "error");

            cJSON *cjson_result = cJSON_AddObjectToObject(cjson_response, "result");
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

        cJSON_Delete(cjson_request);
    }

    return cjson_response;
}
