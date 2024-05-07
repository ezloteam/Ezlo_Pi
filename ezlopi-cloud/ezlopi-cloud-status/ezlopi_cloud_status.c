
#include "esp_system.h"

#include "ezlopi_util_trace.h"

#include "ezlopi_cloud_constants.h"
#include "ezlopi_cloud_status.h"
#include "ezlopi_core_processes.h"


void EZPI_CLOUD_status_get(cJSON* cj_request, cJSON* cj_response)
{
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_id_str, cJSON_GetObjectItem(cj_request, ezlopi_id_str));
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_method_str, cJSON_GetObjectItem(cj_request, ezlopi_method_str));
    cJSON* cjson_result = cJSON_AddObjectToObject(cj_response, ezlopi_result_str);
    if (cjson_result)
    {
        cJSON* processes_array = cJSON_AddArrayToObject(cjson_result, "processes");
        if (processes_array)
        {
            ezlopi_core_get_processes_details(processes_array);
        }
        cJSON* cj_ram = cJSON_AddObjectToObject(cjson_result, "ram");
        if (cj_ram)
        {
            double free_heap_size = (esp_get_free_heap_size() / 1024.0);
            cJSON* cj_available = cJSON_AddObjectToObject(cj_ram, "available");
            if (cj_available)
            {
                cJSON_AddStringToObject(cj_available, "scale", "Kb");
                cJSON_AddNumberToObject(cj_available, "value", free_heap_size);
            }
            cJSON* cj_total_available_ram = cJSON_AddObjectToObject(cj_ram, "total");
            if (cj_total_available_ram)
            {
                cJSON_AddStringToObject(cj_total_available_ram, "scale", "Kb");
                cJSON_AddNumberToObject(cj_total_available_ram, "value", 400);
            }
            float available_pct = (free_heap_size / 400.0) * 100.0;
            cJSON_AddNumberToObject(cj_ram, "availablePct", available_pct);
        }
    }
}


