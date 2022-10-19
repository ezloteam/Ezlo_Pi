#include <string.h>

#include "favorite.h"
#include "trace.h"
#include "frozen.h"
#include "devices_common.h"

const char *favorite_1_start = "{\"method\":\"hub.favorite.list\",\"msg_id\":%d,\"api\":\"1.0\",\"error\":null,\"id\":\"%.*s\",\"result\":{\"favorites\":{\"devices\":[";
const char *favorite_1_devs = "{\"_id\":\"%.*s\"}";
const char *favorite_1_end = "]}},\"sender\":%.*s}";

char *favorite_list(const char *payload, uint32_t len, struct json_token *method, uint32_t msg_count)
{
    uint32_t buf_len = 2048;
    char *send_buf = (char *)malloc(buf_len);

    if (send_buf)
    {
        memset(send_buf, 0, buf_len);
        s_device_properties_t *devices = devices_common_device_list();

        struct json_token msg_id = JSON_INVALID_TOKEN;
        json_scanf(payload, len, "{id: %T}", &msg_id);

        struct json_token sender = JSON_INVALID_TOKEN;
        int sender_status = json_scanf(payload, len, "{sender: %T}", &sender);

        snprintf(send_buf, buf_len, favorite_1_start, msg_count, msg_id.len, msg_id.ptr);

        if (devices[0].name[0])
        {
            for (int i = 0; i < MAX_DEV; i++)
            {
                int len_b = strlen(send_buf);
                snprintf(&send_buf[len_b], buf_len - len_b, favorite_1_devs,
                         sizeof(devices[i].device_id), devices[i].device_id);
                if (i < MAX_DEV - 1)
                {
                    if (devices[i + 1].name[0])
                    {
                        len_b = strlen(send_buf);
                        send_buf[len_b] = ',';
                        send_buf[len_b + 1] = 0;
                    }
                    else
                    {
                        break;
                    }
                }
            }
        }

        int len_b = strlen(send_buf);
        snprintf(&send_buf[len_b], buf_len - len_b, favorite_1_end, sender_status ? sender.len : 2, sender_status ? sender.ptr : "{}");

        TRACE_B(">>>>>>>>>>> WS Tx - '%.*s' [%d]\n\r%s", method->len, method->ptr, strlen(send_buf), send_buf);
    }

    return send_buf;
}
