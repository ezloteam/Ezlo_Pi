#ifndef __HUB_SCENES_LIST_H__
#define __HUB_SCENES_LIST_H__

#include <string.h>
#include "cJSON.h"

#ifdef __cplusplus
extern "C"
{
#endif

    void scenes_list(cJSON *cj_request, cJSON *cj_response);
    void scenes_create(cJSON *cj_request, cJSON *cj_response);
    void scenes_get(cJSON *cj_request, cJSON *cj_response);
    // void scenes_edit(cJSON *cj_request, cJSON *cj_response);
    void scenes_delete(cJSON *cj_request, cJSON *cj_response);
    // void scenes_status_get(cJSON *cj_request, cJSON *cj_response);
    void scenes_blocks_list(cJSON *cj_request, cJSON *cj_response);
    void scenes_block_data_list(cJSON *cj_request, cJSON *cj_response);
    void scenes_run(cJSON *cj_request, cJSON *cj_response);

#ifdef __cplusplus
}
#endif

#endif // __HUB_SCENES_LIST_H__