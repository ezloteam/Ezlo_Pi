#ifndef __HUB_SCENES_LIST_H__
#define __HUB_SCENES_LIST_H__

#include <string.h>
#include "frozen.h"
#include "cJSON.h"

#ifdef __cplusplus
extern "C"
{
#endif

    void scenes_list(cJSON *cj_request, cJSON *cj_response);
    void scenes_get(cJSON *cj_request, cJSON *cj_response);

#ifdef __cplusplus
}
#endif

#endif // __HUB_SCENES_LIST_H__