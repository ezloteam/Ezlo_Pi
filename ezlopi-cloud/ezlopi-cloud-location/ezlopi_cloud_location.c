#include <string.h>
#include <time.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "cjext.h"
#include "zones.h"

#include "ezlopi_cloud_constants.h"
#include "ezlopi_util_trace.h"
#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_core_sntp.h"

void EZPI_CLOUD_location_list(cJSON* cj_request, cJSON* cj_response)
{
    cJSON* cjson_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    if (cjson_result)
    {
        cJSON* json_array_tzs = cJSON_CreateArray(__FUNCTION__);

        if (NULL != json_array_tzs)
        {
            const micro_tz_db_pair* tz_list = get_tz_db();
            uint32_t i = 0;
            // while (tz_list[i].name != NULL)
            while (i <= 50)
            {
                cJSON* json_location_name = cJSON_CreateString(__FUNCTION__, tz_list[i].name);

                if (NULL != json_location_name)
                {
                    cJSON_AddItemToArray(json_array_tzs, json_location_name);
                }
#ifdef CONFIG_EZPI_UTIL_TRACE_EN
                TRACE_E("Here !!! Looping %d : %s", i + 1, tz_list[i].name);
#endif
                vTaskDelay(1);
                i++;
            }
            cJSON_AddItemToObject(__FUNCTION__, cjson_result, ezlopi_locations_str, json_array_tzs);
        }
    }
}

void EZPI_CLOUD_location_get(cJSON* cj_request, cJSON* cj_response)
{
    cJSON* cjson_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    if (cjson_result)
    {
        char* location = EZPI_CORE_sntp_get_location();
        if (location)
        {
            cJSON_AddStringToObject(__FUNCTION__, cjson_result, ezlopi_location_str, location);
            ezlopi_free(__FUNCTION__, location);
        }
        else
        {
            cJSON_AddStringToObject(__FUNCTION__, cjson_result, ezlopi_location_str, ezlopi__str);
        }
    }
}

void EZPI_CLOUD_location_set(cJSON* cj_request, cJSON* cj_response)
{
    cJSON* cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
    if (cj_params)
    {
        char location_str[128];
        memset(location_str, 0, sizeof(location_str));
        CJSON_GET_VALUE_STRING_BY_COPY(cj_params, ezlopi_location_str, location_str);

        if ('\0' != location_str[0])
        {
            TRACE_I("Location: %s", location_str);
            EZPI_CORE_sntp_set_location(location_str);
            cJSON* cjson_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
            if (cjson_result)
            {
                cJSON_AddStringToObject(__FUNCTION__, cjson_result, ezlopi_location_str, location_str);
            }
        }
    }
}