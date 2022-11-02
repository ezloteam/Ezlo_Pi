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

const char *info_1_start = "{\"method\":\"hub.info.get\",\"msg_id\":%d,\"api\":\"1.0\",\"error\":null,\"id\":\"%.*s\",\"result\":{"
                           "\"model\":\"%s\","
                           "\"architecture\":\"%s\","
                           "\"firmware\":\"%s\","
                           "\"kernel\":\"%s\","
                           "\"hardware\":\"%s\","
                           "\"serial\":\"%llu\","
                           "\"uuid\":\"%s\","
                           "\"offlineAnonymousAccess\":true,"
                           "\"offlineInsecureAccess\":true,"
                           "\"location\":{\"latitude\": %f, \"longitude\": %f, \"timezone\":\"%s\", \"state\":\"%s\"},"
                           "\"build\":{\"time\": \"%s\", \"builder\":\"%s\", \"branch\":\"%s\", \"commit\":\"%s\"},"
                           "\"battery\":{\"stateOfCharge\":%d, \"remainingTime\": %d, \"status\":\"%s\", \"health\": %d},"
                           "\"uptime\":\"%s\","
                           "\"localtime\":\"%s\""
                           "},"
                           "\"sender\":%.*s"
                           "}";

char *info_get(const char *payload, uint32_t len, struct json_token *method_tok, uint32_t msg_count)
{
    char *string_response = NULL;
    cJSON *cjson_request = cJSON_ParseWithLength(payload, len);

    if (cjson_request)
    {
        cJSON *id = cJSON_GetObjectItem(cjson_request, ezlopi_id_str);
        cJSON *sender = cJSON_GetObjectItem(cjson_request, ezlopi_sender_str);

        cJSON *cjson_response = cJSON_CreateObject();
        if (cjson_response)
        {
            cJSON_AddStringToObject(cjson_response, ezlopi_key_method_str, method_hub_info_get);
            cJSON_AddNumberToObject(cjson_response, ezlopi_msg_id_str, msg_count);
            cJSON_AddItemReferenceToObject(cjson_response, ezlopi_id_str, id);
            cJSON_AddItemReferenceToObject(cjson_response, ezlopi_sender_str, sender);
            cJSON_AddNullToObject(cjson_response, "error");

            cJSON *cjson_result = cJSON_CreateObject();
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

                cJSON *cjson_location = cJSON_CreateObject();
                if (cjson_location)
                {
                    cJSON_AddNumberToObject(cjson_location, "latitude", 0);
                    cJSON_AddNumberToObject(cjson_location, "longitude", 0);
                    cJSON_AddStringToObject(cjson_location, "timezone", "");
                    cJSON_AddStringToObject(cjson_location, "state", "");

                    if (!cJSON_AddItemToObject(cjson_result, "location", cjson_location))
                    {
                        cJSON_Delete(cjson_location);
                    }
                }

                cJSON *cjson_build = cJSON_CreateObject();
                if (cjson_build)
                {
                    cJSON_AddStringToObject(cjson_build, "time", "");
                    cJSON_AddStringToObject(cjson_build, "builder", "");
                    cJSON_AddStringToObject(cjson_build, "branch", "");
                    cJSON_AddStringToObject(cjson_build, "commit", "");

                    if (!cJSON_AddItemToObject(cjson_result, "build", cjson_build))
                    {
                        cJSON_Delete(cjson_build);
                    }
                }

                cJSON *cjson_battery = cJSON_CreateObject();
                if (cjson_battery)
                {
                    cJSON_AddNumberToObject(cjson_battery, "stateOfCharge", 0);
                    cJSON_AddNumberToObject(cjson_battery, "remainingTime", 0);
                    cJSON_AddNumberToObject(cjson_battery, "health", 0);
                    cJSON_AddStringToObject(cjson_battery, "status", "");

                    if (!cJSON_AddItemToObject(cjson_result, "battery", cjson_battery))
                    {
                        cJSON_Delete(cjson_battery);
                    }
                }

                cJSON_AddStringToObject(cjson_result, "uptime", "");
                cJSON_AddStringToObject(cjson_result, "localtime", "");

                if (!cJSON_AddItemToObjectCS(cjson_response, "result", cjson_result))
                {
                    cJSON_Delete(cjson_result);
                }
            }

            string_response = cJSON_Print(cjson_response);
            if (string_response)
            {
                TRACE_B("'%s' response:\r\n%s\r\n", method_hub_info_get, string_response);
                cJSON_Minify(string_response);
            }

            cJSON_Delete(cjson_response);
        }

        cJSON_Delete(cjson_request);
    }

    return string_response;
}

#if 0
char *info_get(const char *payload, uint32_t len, struct json_token *method, uint32_t msg_count)
{
    uint32_t buf_len = 1024;
    char *send_buf = (char *)malloc(buf_len);

    if (send_buf)
    {
        memset(send_buf, 0, buf_len);
        // factory_info *factory = factory_info::get_instance();
        s_ezlopi_factory_info_t *factory = ezlopi_factory_info_get_info();

        struct json_token msg_id = JSON_INVALID_TOKEN;
        json_scanf(payload, len, "{id: %T}", &msg_id);

        struct json_token sender = JSON_INVALID_TOKEN;
        int sender_status = json_scanf(payload, len, "{sender: %T}", &sender);

        snprintf(send_buf, buf_len, info_1_start, msg_count,
                 msg_id.len, msg_id.ptr,
                 "EzloPi",
                 CONFIG_IDF_TARGET,
                 VERSION_STR,
                 IDF_VER,
                 "ESP32 DEV KIT V1",
                 factory->id,
                 factory->controller_uuid,
                 27.661294f, 85.334448f, "UTC+5:45", "custom",
                 "2022-07-01T12:32:30+0545", "krishna.kumar@ezlo.com", "developer", "feature_wss",
                 100, 0xFFFFFFFF, "AC powered", 100,
                 "5d 22h 18m 11s",
                 "2022-07-01T12:35:40+0545", sender_status ? sender.len : 2, sender_status ? sender.ptr : "{}");

        TRACE_B(">>>>>>>>>>> WS Tx - '%.*s' [%d]\n\r%s", method->len, method->ptr, strlen(send_buf), send_buf);
    }

    return send_buf;
}
#endif