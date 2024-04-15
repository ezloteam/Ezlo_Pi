#ifndef __HUB_ITEMS_LIST_H__
#define __HUB_ITEMS_LIST_H__

#include <string.h>

#include "cjext.h"
#include "ezlopi_core_devices_list.h"

#ifdef __cplusplus
extern "C"
{
#endif
    void items_list_v3(cJSON* cj_request, cJSON* cj_response);
    void items_set_value_v3(cJSON* cj_request, cJSON* cj_response);
    void items_update_v3(cJSON* cj_request, cJSON* cj_response);

    void items_list(cJSON* cj_request, cJSON* cj_response);
    void items_update(cJSON* cj_request, cJSON* cj_response);
    void items_set_value(cJSON* cj_request, cJSON* cj_response);
    // char *items_update_with_device_index(const char *payload, uint32_t len, struct json_token *method, uint32_t msg_count, int device_index);
    // char *items_update_from_sensor(int device_index, char *updated_value);
#if 0 // v2.x
    cJSON* ezlopi_cloud_items_updated_from_devices(l_ezlopi_configured_devices_t* registered_device);
#endif

    cJSON* ezlopi_cloud_items_updated_from_devices_v3(l_ezlopi_device_t* device, l_ezlopi_item_t* item);

#ifdef __cplusplus
}
#endif

#endif // __HUB_ITEMS_LIST_H__