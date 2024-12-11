
#include "esp_system.h"

#include "ezlopi_util_trace.h"

#include "ezlopi_cloud_constants.h"
#include "ezlopi_cloud_status.h"
#include "ezlopi_core_processes.h"


void EZPI_CLOUD_status_get(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddItemReferenceToObject(__FUNCTION__, cj_response, ezlopi_id_str, cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_id_str));
    cJSON_AddItemReferenceToObject(__FUNCTION__, cj_response, ezlopi_method_str, cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_method_str));
    cJSON *cjson_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    if (cjson_result)
    {
        cJSON *processes_array = cJSON_AddArrayToObject(__FUNCTION__, cjson_result, "processes");
        if (processes_array)
        {
            ezlopi_core_get_processes_details(processes_array);
        }
        cJSON *cj_ram = cJSON_AddObjectToObject(__FUNCTION__, cjson_result, "ram");
        if (cj_ram)
        {
            double free_heap_size = (esp_get_free_heap_size() / 1024.0);
            cJSON *cj_available = cJSON_AddObjectToObject(__FUNCTION__, cj_ram, "available");
            if (cj_available)
            {
                cJSON_AddStringToObject(__FUNCTION__, cj_available, ezlopi_scale_str, "Kb");
                cJSON_AddNumberToObject(__FUNCTION__, cj_available, ezlopi_value_str, free_heap_size);
            }
            cJSON *cj_total_available_ram = cJSON_AddObjectToObject(__FUNCTION__, cj_ram, "total");
            if (cj_total_available_ram)
            {
                cJSON_AddStringToObject(__FUNCTION__, cj_total_available_ram, ezlopi_scale_str, "Kb");
                cJSON_AddNumberToObject(__FUNCTION__, cj_total_available_ram, ezlopi_value_str, 400);
            }
            float available_pct = (free_heap_size / 400.0) * 100.0;
            cJSON_AddNumberToObject(__FUNCTION__, cj_ram, "availablePct", available_pct);
        }
    }
}


