#ifndef __HUB_ITEMS_LIST_H__
#define __HUB_ITEMS_LIST_H__

#include "string.h"
#include "frozen.h"
#include "cJSON.h"
// #include "ezlopi_devices.h"
#include "ezlopi_devices_list.h"

#ifdef __cplusplus
extern "C"
{
#endif
    cJSON *items_list(const char *payload, uint32_t len, struct json_token *method, uint32_t msg_count);
    char *items_update(const char *payload, uint32_t len, struct json_token *method, uint32_t msg_count);
    cJSON *items_set_value(const char *payload, uint32_t len, struct json_token *method, uint32_t msg_count);
    // char *items_update_with_device_index(const char *payload, uint32_t len, struct json_token *method, uint32_t msg_count, int device_index);
    // char *items_update_from_sensor(int device_index, char *updated_value);
    cJSON *ezlopi_cloud_items_updated_from_devices(l_ezlopi_configured_devices_t *registered_device);

#ifdef __cplusplus
}
#endif

#endif // __HUB_ITEMS_LIST_H__