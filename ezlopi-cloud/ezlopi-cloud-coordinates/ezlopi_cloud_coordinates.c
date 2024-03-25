
#include "cJSON.h"

#include "ezlopi_util_trace.h"

#include "ezlopi_core_nvs.h"
#include "ezlopi_core_cjson_macros.h"

#include "ezlopi_cloud_constants.h"
#include "ezlopi_cloud_coordinates.h"

static double latitude;
static double longitude;

void hub_coordinates_set(cJSON* cj_request, cJSON* cj_response)
{
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_id_str, cJSON_GetObjectItem(cj_request, ezlopi_id_str));
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_method_str, cJSON_GetObjectItem(cj_request, ezlopi_method_str));
    cJSON_AddObjectToObject(cj_response, ezlopi_result_str);
    if (cj_request)
    {
        cJSON* cj_params = cJSON_GetObjectItem(cj_request, ezlopi_params_str);
        if (cj_params)
        {
            CJSON_GET_VALUE_DOUBLE(cj_params, ezlopi_latitude_str, latitude);
            CJSON_GET_VALUE_DOUBLE(cj_params, ezlopi_longitude_str, longitude);

            char lat_long_str[256];
            if (cJSON_PrintPreallocated(cj_params, lat_long_str, sizeof(lat_long_str), false))
            {
                ezlopi_nvs_write_latitude_longitude(lat_long_str);
            }
        }
    }
}

void hub_coordinates_get(cJSON* cj_request, cJSON* cj_response)
{
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_id_str, cJSON_GetObjectItem(cj_request, ezlopi_id_str));
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_method_str, cJSON_GetObjectItem(cj_request, ezlopi_method_str));
    char* lat_long_vals = ezlopi_nvs_read_latidtude_longitude();
    cJSON* cj_lat_long_val = cJSON_Parse(lat_long_vals);
    cJSON_AddItemToObject(cj_response, ezlopi_result_str, cj_lat_long_val);
    free(lat_long_vals);
}

double ezlopi_cloud_get_latitude()
{
    return latitude;
}

double ezlopi_cloud_get_longitude()
{
    return longitude;
}

