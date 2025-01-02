#ifndef __HUB_ROOM_H__
#define __HUB_ROOM_H__
#include <string.h>

#include "cjext.h"

#ifdef __cplusplus
extern "C"
{
#endif
    void room_get(cJSON* cj_request, cJSON* cj_response);
    void room_list(cJSON* cj_request, cJSON* cj_response);
    void room_create(cJSON* cj_request, cJSON* cj_response);
    void room_name_set(cJSON* cj_request, cJSON* cj_response);
    void room_order_set(cJSON* cj_request, cJSON* cj_response);
    void room_delete(cJSON* cj_request, cJSON* cj_response);
    void room_all_delete(cJSON* cj_request, cJSON* cj_response);

    void room_created(cJSON* cj_request, cJSON* cj_response);
    void room_edited(cJSON* cj_request, cJSON* cj_response);
    void room_deleted(cJSON* cj_request, cJSON* cj_response);
    void room_reordered(cJSON* cj_request, cJSON* cj_response);

#ifdef __cplusplus
}
#endif

#endif // __HUB_ROOM_H__