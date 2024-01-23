#ifndef __SCENES_SCRIPTS_H__
#define __SCENES_SCRIPTS_H__

#include <cJSON.h>

void scenes_scripts_add(cJSON *cj_request, cJSON *cj_response);
void scenes_scripts_get(cJSON *cj_request, cJSON *cj_response);
void scenes_scripts_list(cJSON *cj_request, cJSON *cj_response);
void scenes_scripts_delete(cJSON *cj_request, cJSON *cj_response);
void scenes_scripts_set(cJSON *cj_request, cJSON *cj_response);
void scenes_scripts_run(cJSON *cj_request, cJSON *cj_response);

#endif // __SCENES_SCRIPTS_H__
