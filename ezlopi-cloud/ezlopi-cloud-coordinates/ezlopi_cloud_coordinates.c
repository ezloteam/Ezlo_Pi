
#include "cJSON.h"

#include "ezlopi_util_trace.h"

#include "ezlopi_core_nvs.h"
#include "ezlopi_core_cjson_macros.h"

#include "ezlopi_cloud_constants.h"
#include "ezlopi_cloud_coordinates.h"

static double sg_latitude;
static double sg_longitude;

void hub_coordinates_set(cJSON* cj_request, cJSON* cj_response)
{
    cJSON_AddObjectToObject(cj_response, ezlopi_result_str);
    if (cj_request)
    {
        cJSON* cj_params = cJSON_GetObjectItem(cj_request, ezlopi_params_str);
        if (cj_params)
        {
            CJSON_GET_VALUE_DOUBLE(cj_params, ezlopi_latitude_str, sg_latitude);
            CJSON_GET_VALUE_DOUBLE(cj_params, ezlopi_longitude_str, sg_longitude);

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
    char* lat_long_vals = ezlopi_nvs_read_latidtude_longitude();
    if (lat_long_vals)
    {
        cJSON_AddRawToObject(cj_response, ezlopi_result_str, lat_long_vals);
        free(lat_long_vals);
    }
}

double ezlopi_cloud_get_latitude()
{
    return sg_latitude;
}

double ezlopi_cloud_get_longitude()
{
    return sg_longitude;
}

