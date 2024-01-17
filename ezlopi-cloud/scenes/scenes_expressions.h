#ifndef __SCENES_EXPRESSIONS_H__
#define __SCENES_EXPRESSIONS_H__
#include <cJSON.h>

#include "ezlopi_scenes_expressions.h"

void scenes_expressions_list(cJSON *cj_request, cJSON *cj_response);
void scenes_expressions_added(cJSON *cj_request, cJSON *cj_response);
void scenes_expressions_set(cJSON *cj_request, cJSON *cj_response);

#endif // __SCENES_EXPRESSIONS_H__