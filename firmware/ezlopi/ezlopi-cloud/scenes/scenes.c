#include <string.h>

#include "scenes.h"
#include "trace.h"
#include "frozen.h"
#include "cJSON.h"
#include "ezlopi_cloud_constants.h"

void scenes_list(cJSON *cj_request, cJSON *cj_response)
{
    cJSON *cjson_result = cJSON_AddObjectToObject(cj_response, "result");
    if (cjson_result)
    {
        cJSON *cjson_devices_array = cJSON_AddArrayToObject(cjson_result, "scenes");
    }
}
