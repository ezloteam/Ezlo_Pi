#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "ezlopi_core_devices_list.h"
#include "ezlopi_cloud_constants.h"


void EZPI_CLOUD_offline_login(cJSON* cj_request, cJSON* cj_response)
{
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_id_str, cJSON_GetObjectItem(cj_request, ezlopi_id_str));
    // cJSON_AddItemReferenceToObject(cj_response, ezlopi_method_str, cJSON_GetObjectItem(cj_request, ezlopi_method_str));
    cJSON_AddObjectToObject(cj_response, ezlopi_result_str);
}