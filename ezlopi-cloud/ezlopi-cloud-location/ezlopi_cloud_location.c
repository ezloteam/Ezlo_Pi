#include <string.h>
#include <time.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "cJSON.h"
#include "zones.h"

#include "ezlopi_cloud_constants.h"
#include "ezlopi_util_trace.h"
#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_core_sntp.h"

void EZPI_CLOUD_location_list(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_id_str, cJSON_GetObjectItem(cj_request, ezlopi_id_str));
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_key_method_str, cJSON_GetObjectItem(cj_request, ezlopi_key_method_str));
    cJSON *cjson_result = cJSON_AddObjectToObject(cj_response, ezlopi_result_str);
    if (cjson_result)
    {
        cJSON *json_array_tzs = cJSON_CreateArray();

        if (NULL != json_array_tzs)
        {
            const micro_tz_db_pair *tz_list = get_tz_db();
            uint32_t i = 0;
            // while (tz_list[i].name != NULL)
            while (i <= 50)
            {
                cJSON *json_location_name = cJSON_CreateString(tz_list[i].name);

                if (NULL != json_location_name)
                {
                    cJSON_AddItemToArray(json_array_tzs, json_location_name);
                }
                // TRACE_E("Here !!! Looping %d : %s", i+1, tz_list[i].name);
                vTaskDelay(1);
                i++;
            }
            cJSON_AddItemToObject(cjson_result, ezlopi_locations_str, json_array_tzs);
        }
    }
}

void EZPI_CLOUD_location_get(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_id_str, cJSON_GetObjectItem(cj_request, ezlopi_id_str));
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_key_method_str, cJSON_GetObjectItem(cj_request, ezlopi_key_method_str));
    cJSON *cjson_result = cJSON_AddObjectToObject(cj_response, ezlopi_result_str);
    if (cjson_result)
    {
        char *location = EZPI_CORE_sntp_get_location();
        if (location)
        {
            cJSON_AddStringToObject(cjson_result, ezlopi_location_str, location);
            free(location);
        }
        else
        {
            cJSON_AddStringToObject(cjson_result, ezlopi_location_str, ezlopi__str);
        }
    }
}

void EZPI_CLOUD_location_set(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_id_str, cJSON_GetObjectItem(cj_request, ezlopi_id_str));
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_key_method_str, cJSON_GetObjectItem(cj_request, ezlopi_key_method_str));

    cJSON *cj_params = cJSON_GetObjectItem(cj_request, ezlopi_params_str);
    if (cj_params)
    {
        char *location_str = NULL;
        CJSON_GET_VALUE_STRING(cj_params, ezlopi_location_str, location_str);
        if (location_str)
        {
            TRACE_I("Location: %s", location_str);
            EZPI_CORE_sntp_set_location(location_str);
            cJSON *cjson_result = cJSON_AddObjectToObject(cj_response, ezlopi_result_str);
            if (cjson_result)
            {
                cJSON_AddStringToObject(cjson_result, ezlopi_location_str, location_str);
            }
        }
    }
}