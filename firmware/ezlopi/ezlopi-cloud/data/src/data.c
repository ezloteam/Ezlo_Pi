#include <string.h>

#include "data.h"
#include "trace.h"
#include "frozen.h"
#include "devices_common.h"

static const char *data_list_start = "{\"method\":\"hub.data.list\",\"msg_id\":%d,\"api\":\"1.0\",\"error\":null,\"id\":\"%.*s\",\"result\":{\"settings\":{";
static const char *data_list_cont = "\"first_start\":{\"value\": 0}";
static const char *data_list_end = "}},\"sender\":%.*s}";

char *data_list(const char *data, uint32_t len, struct json_token *method, uint32_t msg_count)
{
    uint32_t buf_len = 4096;
    char *send_buf = (char *)malloc(buf_len);

    if (send_buf)
    {
        s_device_properties_t *devices = devices_common_device_list();

        struct json_token msg_id = JSON_INVALID_TOKEN;
        json_scanf(data, len, "{id: %T}", &msg_id);

        struct json_token sender = JSON_INVALID_TOKEN;
        int sender_status = json_scanf(data, len, "{sender: %T}", &sender);

        snprintf(send_buf, buf_len, data_list_start, msg_count, msg_id.len, msg_id.ptr);

        if (devices[0].name[0])
        {
            for (int i = 0; i < MAX_DEV; i++)
            {
                int len_b = strlen(send_buf);
                snprintf(&send_buf[len_b], buf_len - len_b, "%s", data_list_cont); //, devices[i].device_id, devices[i].name);
                if (i < MAX_DEV - 1)
                {
                    if (devices[i + 1].name[0])
                    {
#warning "WARNING: Remove break from here!"
                        break;

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

        snprintf(&send_buf[strlen(send_buf)], buf_len - strlen(send_buf), data_list_end, sender_status ? sender.len : 2, sender_status ? sender.ptr : "{}");
        TRACE_B(">> WS Tx - '%.*s' [%d]\n\r%s", method->len, method->ptr, strlen(send_buf), send_buf);
    }

    return send_buf;
}
