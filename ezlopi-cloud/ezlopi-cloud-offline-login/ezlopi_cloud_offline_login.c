#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "ezlopi_core_devices_list.h"
#include "ezlopi_cloud_constants.h"

bool flag_offline_login = false;

void EZPI_CLOUD_offline_login(cJSON* cj_request, cJSON* cj_response)
{
    cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
}