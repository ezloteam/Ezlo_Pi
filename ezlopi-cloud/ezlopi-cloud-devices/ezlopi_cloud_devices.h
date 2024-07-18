#ifndef __HUB_DEVICE_LIST_H__
#define __HUB_DEVICE_LIST_H__

#include <string.h>

#include "cjext.h"

#ifdef __cplusplus
extern "C"
{
#endif

    void devices_list_v3(cJSON* cj_request, cJSON* cj_response);
    void device_name_set(cJSON* cj_request, cJSON* cj_response);
    void device_room_set(cJSON *cj_request, cJSON *cj_response);
    void device_armed_set(cJSON* cj_request, cJSON* cj_response);
    void device_updated(cJSON* cj_request, cJSON* cj_response);

    // char *devices_settings_list(const char *payload, uint32_t len, struct json_token *method, uint32_t msg_count);
    // char *devices_name_set(const char *payload, uint32_t len, struct json_token *method, uint32_t msg_count);

#ifdef __cplusplus
}
#endif

#endif // __HUB_DEVICE_LIST_H__