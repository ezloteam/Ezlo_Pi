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
    void device_armed_set(cJSON* cj_request, cJSON* cj_response);
    void device_updated(cJSON* cj_request, cJSON* cj_response);

    // char *devices_settings_list(const char *payload, uint32_t len, struct json_token *method, uint32_t msg_count);
    // char *devices_name_set(const char *payload, uint32_t len, struct json_token *method, uint32_t msg_count);



    //--------- device-groups-------
    void device_group_create(cJSON* cj_request, cJSON* cj_response);
    void device_group_get(cJSON* cj_request, cJSON* cj_response);
    void device_group_delete(cJSON* cj_request, cJSON* cj_response);
    void device_group_update(cJSON* cj_request, cJSON* cj_response);
    void device_groups_list(cJSON* cj_request, cJSON* cj_response);
    void device_group_find(cJSON* cj_request, cJSON* cj_response);
    void device_group_devitem_expand(cJSON* cj_request, cJSON* cj_response);

    ///////////// device-grp updaters
    void device_group_created(cJSON * cj_request, cJSON * cj_response);
    void device_group_deleted(cJSON * cj_request, cJSON * cj_response);
    void device_group_updated(cJSON * cj_request, cJSON * cj_response);


    //--------- item-groups-------
    void item_group_create(cJSON* cj_request, cJSON* cj_response);
    void item_group_get(cJSON* cj_request, cJSON* cj_response);
    void item_group_delete(cJSON* cj_request, cJSON* cj_response);
    void item_group_update(cJSON* cj_request, cJSON* cj_response);
    void item_groups_list(cJSON* cj_request, cJSON* cj_response);
    ///////////// item-grp updaters
    void item_group_created(cJSON * cj_request, cJSON * cj_response);
    void item_group_deleted(cJSON * cj_request, cJSON * cj_response);
    void item_group_updated(cJSON * cj_request, cJSON * cj_response);



#ifdef __cplusplus
}
#endif

#endif // __HUB_DEVICE_LIST_H__