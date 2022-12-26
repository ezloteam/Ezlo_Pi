#ifndef __WEB_PROVISIONING_H__
#define __WEB_PROVISIONING_H__

#include <string.h>
#include "frozen.h"
#include "cJSON.h"

#ifdef __cplusplus
extern "C"
{
#endif

    void web_provisioning_init(void);
    uint32_t web_provisioning_get_message_count(void);
    int web_provisioning_send_to_nma_websocket(cJSON *cjson_data);
    char *parse_device_from_web_payload(void *ws_ctx, char *web_payload);

#ifdef __cplusplus
}
#endif

#endif // __WEB_PROVISIONING_H__