#include <string.h>

#include "scenes.h"
#include "trace.h"
#include "frozen.h"
#include "cJSON.h"

static const char *json_scenes_list = "{\"method\":\"%.*s\",\"msg_id\":%d,\"result\":{\"scenes\":[]},\"error\":null,\"id\":\"%.*s\",\"sender\":%.*s}";

cJSON *scenes_list(const char *payload, uint32_t len, struct json_token *method, uint32_t msg_count)
{
    uint32_t buf_len = 2048;
    char *send_buf = (char *)malloc(buf_len);
    cJSON *cjson_resp = NULL;

    if (send_buf)
    {
        struct json_token msg_id;
        json_scanf(payload, len, "{id: %T}", &msg_id);

        struct json_token sender;
        int sender_status = json_scanf(payload, len, "{sender: %T}", &sender);

        snprintf(send_buf, buf_len, json_scenes_list, method->len, method->ptr, msg_count, msg_id.len, msg_id.ptr, sender_status ? sender.len : 2, sender_status ? sender.ptr : "{}");

        TRACE_B(">> WS Tx - '%.*s' [%d]\r\n%s", method->len, method->ptr, strlen(send_buf), send_buf);
        cjson_resp = cJSON_Parse(send_buf);
    }

    return cjson_resp;
}
