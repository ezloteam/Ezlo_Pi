#ifndef __HUB_SCENES_LIST_H__
#define __HUB_SCENES_LIST_H__

#ifdef CONFIG_EZPI_SERV_ENABLE_MESHBOTS

#include <string.h>
#include "cjext.h"

#ifdef __cplusplus
extern "C"
{
#endif

    void scenes_list(cJSON *cj_request, cJSON *cj_response);
    void scenes_create(cJSON *cj_request, cJSON *cj_response);
    void scenes_get(cJSON *cj_request, cJSON *cj_response);
    void scenes_edit(cJSON *cj_request, cJSON *cj_response);
    void scenes_delete(cJSON *cj_request, cJSON *cj_response);
    void scenes_status_get(cJSON *cj_request, cJSON *cj_response);
    void scenes_blocks_list(cJSON *cj_request, cJSON *cj_response);
    void scenes_block_data_list(cJSON *cj_request, cJSON *cj_response);
    void scenes_run(cJSON *cj_request, cJSON *cj_response);
    void scenes_enable_set(cJSON *cj_request, cJSON *cj_response);
    void scenes_notification_add(cJSON *cj_request, cJSON *cj_response);
    void scenes_notification_remove(cJSON *cj_request, cJSON *cj_response);
    void scenes_room_set(cJSON *cj_request, cJSON *cj_response);
    void scenes_time_list(cJSON *cj_request, cJSON *cj_response);
    void scenes_trigger_device_list(cJSON *cj_request, cJSON *cj_response);
    void scenes_block_enabled_set(cJSON *cj_request, cJSON *cj_response);
    void scenes_house_modes_set(cJSON *cj_request, cJSON *cj_response);
    void scenes_action_block_test(cJSON *cj_request, cJSON *cj_response);
    void scenes_block_status_reset(cJSON *cj_request, cJSON *cj_response);
    void scenes_meta_set(cJSON *cj_request, cJSON *cj_response);
    void scenes_blockmeta_set(cJSON *cj_request, cJSON *cj_response);
    void scenes_stop(cJSON *cj_request, cJSON *cj_response);
    void scenes_clone(cJSON *cj_request, cJSON *cj_response);

    ///////////// updaters
    void scene_changed(cJSON *cj_request, cJSON *cj_response);
    void scene_added(cJSON *cj_request, cJSON *cj_response);
    void scene_deleted(cJSON *cj_request, cJSON *cj_response);

#ifdef __cplusplus
}
#endif

#endif // CONFIG_EZPI_SERV_ENABLE_MESHBOTS

#endif // __HUB_SCENES_LIST_H__