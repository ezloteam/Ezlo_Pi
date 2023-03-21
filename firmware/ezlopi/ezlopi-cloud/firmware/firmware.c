#include <string.h>

#include "cJSON.h"
#include "data.h"
#include "trace.h"
#include "frozen.h"
#include "ezlopi_devices_list.h"
#include "ezlopi_cloud_keywords.h"
#include "ezlopi_cloud_methods_str.h"

#include "ezlopi_ota.h"

void firmware_update(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_id_str, cJSON_GetObjectItem(cj_request, ezlopi_id_str));
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_key_method_str, cJSON_GetObjectItem(cj_request, ezlopi_key_method_str));
    cJSON_AddNullToObject(cj_response, "error");
    cJSON_AddObjectToObject(cj_response, "result");

    cJSON *version = NULL;
    cJSON *source_url = NULL;
    cJSON *params = cJSON_GetObjectItem(cj_request, "params");
    if (params)
    {
        version = cJSON_GetObjectItem(params, "version");
        source_url = cJSON_GetObjectItem(params, "source");
        TRACE_D("OTA - version: %s", (version && version->valuestring) ? version->valuestring : "null");
        TRACE_D("OTA - source: %s", (source_url && source_url->valuestring) ? source_url->valuestring : "null");
    }

    ezlopi_ota_start(source_url);
}
