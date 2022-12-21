#include <string.h>

#include "gateways.h"
#include "trace.h"
#include "frozen.h"
#include "ezlopi_cloud_constants.h"

void gateways_list(cJSON *cj_request, cJSON *cj_response)
{
    cJSON *cjson_result = cJSON_AddObjectToObject(cj_response, "result");
    if (cjson_result)
    {
        cJSON_AddArrayToObject(cjson_result, "gateways");
    }
}
