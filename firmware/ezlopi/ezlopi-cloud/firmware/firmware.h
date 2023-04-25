#ifndef __HUB_FIRMWARE_H__
#define __HUB_FIRMWARE_H__

#include <string.h>
#include "frozen.h"
#include "cJSON.h"

#ifdef __cplusplus
extern "C"
{
#endif

    void firmware_info_get(cJSON *cj_request, cJSON *cj_response);
    void firmware_update_start(cJSON *cj_request, cJSON *cj_response);
    cJSON *firmware_send_firmware_query_to_nma_server(uint32_t message_count);

#ifdef __cplusplus
}
#endif
#endif // __HUB_FIRMWARE_H__