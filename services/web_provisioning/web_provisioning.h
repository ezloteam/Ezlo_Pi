#ifndef __WEB_PROVISIONING_H__
#define __WEB_PROVISIONING_H__

#include <string.h>
#include "frozen.h"
#include "cJSON.h"

#ifdef __cplusplus
extern "C"
{
#endif

    typedef enum e_trace_type
    {
        TRACE_TYPE_NONE = 0,
        TRACE_TYPE_B,
        TRACE_TYPE_I,
        TRACE_TYPE_E
    } e_trace_type_t;

    void web_provisioning_init(void);
    uint32_t web_provisioning_get_message_count(void);
    char *parse_device_from_web_payload(void *ws_ctx, char *web_payload);
    int web_provisioning_send_to_nma_websocket(cJSON *cjson_data, e_trace_type_t print_type);

#ifdef __cplusplus
}
#endif

#endif // __WEB_PROVISIONING_H__