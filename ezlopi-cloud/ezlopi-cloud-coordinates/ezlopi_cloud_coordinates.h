
#ifndef _EZLOPI_CLOUD_COORDINSTES_H_
#define _EZLOPI_CLOUD_COORDINSTES_H_

#include "cJSON.h"

void hub_coordinates_set(cJSON* cjson_request, cJSON* cjson_response);
void hub_coordinates_get(cJSON* cj_request, cJSON* cj_response);
double ezlopi_cloud_get_longitude();
double ezlopi_cloud_get_latitude();

#endif // _EZLOPI_CLOUD_COORDINSTES_H_

