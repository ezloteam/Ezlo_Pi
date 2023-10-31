#ifndef __HUB_FAVORITE_LIST_H__
#define __HUB_FAVORITE_LIST_H__

#include <string.h>
#include "frozen.h"
#include "cJSON.h"

#ifdef __cplusplus
extern "C"
{
#endif

    void favorite_list_v3(cJSON *cj_request, cJSON *cj_response);
#if 0 // v2.x
    void favorite_list(cJSON *cj_request, cJSON *cj_response);
#endif

#ifdef __cplusplus
}
#endif

#endif // __HUB_FAVORITE_LIST_H__