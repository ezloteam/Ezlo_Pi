#ifndef __HUB_ITEMS_LIST_H__
#define __HUB_ITEMS_LIST_H__

#include "string.h"
#include "frozen.h"

#ifdef __cplusplus
extern "C"
{
#endif
    char *items_list(const char *payload, uint32_t len, struct json_token *method, uint32_t msg_count);
    char *items_update(const char *payload, uint32_t len, struct json_token *method, uint32_t msg_count);
    char *items_set_value(const char *payload, uint32_t len, struct json_token *method, uint32_t msg_count);
    char *items_update_with_device_index(const char *payload, uint32_t len, struct json_token *method, uint32_t msg_count, int device_index);

#ifdef __cplusplus
}
#endif

#endif // __HUB_ITEMS_LIST_H__