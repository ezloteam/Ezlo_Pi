#ifndef _EZLOPI_SERVICE_WEBPROV_H_
#define _EZLOPI_SERVICE_WEBPROV_H_

#include <string.h>

#include "cJSON.h"

#ifdef __cplusplus
extern "C"
{
#endif

    typedef enum e_trace_type
    {
        TRACE_TYPE_NONE = 0,
        TRACE_TYPE_W, // Warning (Orange)
        TRACE_TYPE_B,
        TRACE_TYPE_D, // debug (White)
        TRACE_TYPE_I, // Info (Blue)
        TRACE_TYPE_E, // Error (Red)

    } e_trace_type_t;

    void ezlopi_service_web_provisioning_init(void);
    void ezlopi_service_web_provisioning_deinit(void);
    uint32_t ezlopi_service_web_provisioning_get_message_count(void);
    int ezlopi_service_web_provisioning_send_to_nma_websocket(cJSON* cjson_data, e_trace_type_t print_type);
    int ezlopi_service_web_provisioning_send_str_data_to_nma_websocket(char* str_data, e_trace_type_t print_type);

#ifdef __cplusplus
}
#endif

#endif // _EZLOPI_SERVICE_WEBPROV_H_