#include <string.h>

#include "cJSON.h"
#include "data.h"
#include "trace.h"
#include "frozen.h"
#include "ezlopi_devices_list.h"

static const char *data_list_start = "{\"method\":\"hub.data.list\",\"msg_id\":%d,\"api\":\"1.0\",\"error\":null,\"id\":\"%.*s\",\"result\":{\"settings\":{";
static const char *data_list_cont = "\"first_start\":{\"value\": 0}";
static const char *data_list_end = "}},\"sender\":%.*s}";

char *data_list(const char *data, uint32_t len, struct json_token *method, uint32_t msg_count)
{
    uint32_t buf_len = 4096;
    char *send_buf = (char *)malloc(buf_len);

    if (send_buf)
    {
        int sender_status = 0;
        struct json_token msg_id = JSON_INVALID_TOKEN;
        struct json_token sender = JSON_INVALID_TOKEN;
        s_ezlopi_device_t *devices_list = ezlopi_devices_list_get_list();

        if (devices_list)
        {
            json_scanf(data, len, "{id: %T}", &msg_id);
            sender_status = json_scanf(data, len, "{sender: %T}", &sender);

            snprintf(send_buf, buf_len, data_list_start, msg_count, msg_id.len, msg_id.ptr);

            int device_idx = 0;
            while (NULL != devices_list[device_idx].func)
            {
                if (true == devices_list[device_idx].is_configured)
                {
                    int len_b = strlen(send_buf);
                    snprintf(&send_buf[len_b], buf_len - len_b, "%s", data_list_cont); //, devices[i].device_id, devices[i].name);
#warning "WARNING: Remove break from here!"
                    break;
                }

                device_idx++;
            }
        }

        snprintf(&send_buf[strlen(send_buf)], buf_len - strlen(send_buf), data_list_end, sender_status ? sender.len : 2, sender_status ? sender.ptr : "{}");
        TRACE_B(">> WS Tx - '%.*s' [%d]\n\r%s", method->len, method->ptr, strlen(send_buf), send_buf);
    }

    return send_buf;
}

static cJSON *ezlopi_cloud_data_create_device_list(void)
{
    cJSON *cjson_device_list = cJSON_CreateObject();

    if (cjson_device_list)
    {
        cJSON_AddNumberToObject(cjson_device_list, "ids", 1234);
    }

    return cjson_device_list;
}
