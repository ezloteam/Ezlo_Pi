
#include "ezlopi_util_trace.h"

#include "ezlopi_cloud_constants.h"
#include "ezlopi_cloud_hub_status_get.h"
#include "ezlopi_core_processes.h"


void hub_status_get(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_id_str, cJSON_GetObjectItem(cj_request, ezlopi_id_str));
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_method_str, cJSON_GetObjectItem(cj_request, ezlopi_method_str));
    cJSON* cjson_result = cJSON_AddObjectToObject(cj_response, ezlopi_result_str);
    if (cjson_result)
    {
        cJSON *processes_array = cJSON_AddArrayToObject(cjson_result, "processes");
        if(processes_array)
        {
            ezopi_core_get_processes_details(processes_array);
        }
    }
}

