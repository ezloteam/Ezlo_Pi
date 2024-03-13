
#include "cJSON.h"

#include "ezlopi_util_trace.h"

#include "ezlopi_core_nvs.h"

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
            #warning "Nabin: what is 'cJSON_GetObjectItem(cj_params, "latitude")' returns NULL";

            latitude = cJSON_GetObjectItem(cj_params, "latitude")->valuedouble;
            longitude = cJSON_GetObjectItem(cj_params, "longitude")->valuedouble;
            char* lat_long_str = cJSON_Print(cj_params);
            if (lat_long_str)
            {
                cJSON_Minify(lat_long_str);
                ezlopi_nvs_write_latitude_longitude(lat_long_str);
                free(lat_long_str);
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

