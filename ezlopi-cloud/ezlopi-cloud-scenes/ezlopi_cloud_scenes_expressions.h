#ifndef __SCENES_EXPRESSIONS_H__
#define __SCENES_EXPRESSIONS_H__

#ifdef CONFIG_EZPI_SERV_ENABLE_MESHBOTS

#include "cjext.h"

#include "ezlopi_core_scenes_expressions.h"

void scenes_expressions_list(cJSON* cj_request, cJSON* cj_response);
void scenes_expressions_set(cJSON* cj_request, cJSON* cj_response);
void scenes_expressions_delete(cJSON* cj_request, cJSON* cj_response);

///////////// updaters
void scenes_expressions_added_changed(cJSON* cj_request, cJSON* cj_response);
// void scene_expressions_deleted(cJSON* cj_request, cJSON* cj_response);

#endif  // CONFIG_EZPI_SERV_ENABLE_MESHBOTS

#endif // __SCENES_EXPRESSIONS_H__