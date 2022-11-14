#include <string.h>
#include "stdint.h"
#include "frozen.h"
#include "settings.h"
#include "trace.h"

char *settings_list(const char *payload, uint32_t len, struct json_token *method, uint32_t msg_count)
{

    uint32_t buf_len = 256;
    char *send_buf = (char *)malloc(buf_len);

    if (send_buf)
    {
        memset(send_buf, 0, buf_len);
        struct json_token msg_id = JSON_INVALID_TOKEN;
        struct json_token sender = JSON_INVALID_TOKEN;

        json_scanf(payload, len, "{id:%T}", &msg_id);
        json_scanf(payload, len, "{sender:%T}", &sender);

        static const char *list_frmt = "{\"id\":\"%.*s\",\"method\":\"%.*s\",\"msg_id\":%u,\"result\":{\"settings\":[]},\"error\":null,\"sender\":%.*s}";
        snprintf(send_buf, buf_len, list_frmt,
                 msg_id.len, msg_id.ptr,
                 method->len, method->ptr,
                 msg_count,
                 sender.len ? sender.len : 2, sender.len ? sender.ptr : "{}");

        TRACE_B("%s", send_buf);
    }

    return send_buf;
}