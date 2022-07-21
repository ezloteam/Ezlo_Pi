#include <string>
#include <cstring>

#include "data.h"
#include "debug.h"
#include "frozen.h"
#include "devices_common.h"

using namespace std;
data *data::data_ = nullptr;

static const char *data_list_start = "{\"method\":\"hub.data.list\",\"msg_id\":%d,\"api\":\"1.0\",\"error\":null,\"method\":\"hub.data.list\",\"id\":\"%.*s\",\"result\":{\"settings\":{";
static const char *data_list_cont = "\"first_start\":{\"value\": 0}";
static const char *data_list_end = "}},\"sender\":%.*s}";

data *data::get_instance(void)
{
    if (nullptr == data_)
    {
        data_ = new data();
    }

    return data_;
}

string data::list(const char *data, uint32_t len, struct json_token *method, uint32_t msg_count)
{
    string ret = "";
    char send_buf[4096] = {'\0'};
    devices_common *devices_ctx = devices_common::get_instance();
    s_device_properties_t *devices = devices_ctx->device_list();

    struct json_token msg_id = JSON_INVALID_TOKEN;
    json_scanf(data, len, "{id: %T}", &msg_id);

    struct json_token sender = JSON_INVALID_TOKEN;
    int sender_status = json_scanf(data, len, "{sender: %T}", &sender);

    snprintf(send_buf, sizeof(send_buf), data_list_start, msg_count, msg_id.len, msg_id.ptr);

    if (devices[0].name[0])
    {
        for (int i = 0; i < MAX_DEV; i++)
        {
            int len_b = strlen(send_buf);
            snprintf(&send_buf[len_b], sizeof(send_buf) - len_b, "%s", data_list_cont); //, devices[i].device_id, devices[i].name);
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
    snprintf(&send_buf[len_b], sizeof(send_buf) - len_b, data_list_end, sender_status ? sender.len : 2, sender_status ? sender.ptr : "{}");
    len_b = strlen(send_buf);
    send_buf[len_b] = 0;
    ret = send_buf;
    TRACE_B(">>>>>>>>>>> WS Tx - '%.*s' [%d]\n\r%s", method->len, method->ptr, ret.length(), ret.c_str());

    return ret;
}
