#include <string.h>

#include "feature.h"
#include "trace.h"
#include "frozen.h"

const char *feature_1_start = "{\"method\":\"hub.features.list\",\"api\":\"1.0\",\"error\":null,\"id\":\"%.*s\",\"result\":{\"feature\":{\"devices\":[";
const char *feature_1_list = "{\"_id\":\"%s\"}";
const char *feature_1_end = "]}}}";

char *feature_list(const char *payload, uint32_t len, struct json_token *method)
{
    uint32_t buf_len = 2048;
    char *send_buf = (char *)malloc(buf_len);

    if (send_buf)
    {
        struct json_token msg_id = JSON_INVALID_TOKEN;
        json_scanf(payload, len, "{id: %T}", &msg_id);

        snprintf(send_buf, buf_len, feature_1_start, msg_id.len, msg_id.ptr);

        uint32_t len_b = strlen(send_buf);
        snprintf(&send_buf[len_b], buf_len - len_b, "%s", feature_1_end);

        TRACE_B(">> WS Tx - '%.*s' [%d]\n\r%s", method->len, method->ptr, strlen(send_buf), send_buf);
    }

    return send_buf;
}
