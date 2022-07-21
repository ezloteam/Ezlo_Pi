#include <string>
#include <cstring>

#include "favorite.h"
#include "debug.h"
#include "frozen.h"
#include "devices_common.h"

using namespace std;
favorite *favorite::favorite_ = nullptr;

const char *favorite_1_start = "{\"method\":\"hub.favorite.list\",\"msg_id\":%d,\"api\":\"1.0\",\"error\":null,\"id\":\"%.*s\",\"result\":{\"favorites\":{\"devices\":[";
const char *favorite_1_devs = "{\"_id\":\"d%.*s\"}";
const char *favorite_1_end = "]}},\"sender\":%.*s}";


favorite *favorite::get_instance(void)
{
    if (nullptr == favorite_)
    {
        favorite_ = new favorite();
    }

    return favorite_;
}

string favorite::list(const char *payload, uint32_t len, struct json_token *method, uint32_t msg_count)
{
    string ret = "";
    char send_buf[2048] = {'\0'};
    devices_common *devices_ctx = devices_common::get_instance();
    s_device_properties_t *devices = devices_ctx->device_list();

    struct json_token msg_id = JSON_INVALID_TOKEN;
    json_scanf(payload, len, "{id: %T}", &msg_id);

    struct json_token sender = JSON_INVALID_TOKEN;
    int sender_status = json_scanf(payload, len, "{sender: %T}", &sender);

    snprintf(send_buf, sizeof(send_buf), favorite_1_start, msg_count, msg_id.len, msg_id.ptr);

    if (devices[0].name[0])
    {
        for (int i = 0; i < MAX_DEV; i++)
        {
            int len_b = strlen(send_buf);
            snprintf(&send_buf[len_b], sizeof(send_buf) - len_b, favorite_1_devs,
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
    snprintf(&send_buf[len_b], sizeof(send_buf) - len_b, favorite_1_end, sender_status ? sender.len : 2, sender_status ? sender.ptr : "{}");
    len_b = strlen(send_buf);
    send_buf[len_b] = 0;

    ret = send_buf;

    TRACE_B(">>>>>>>>>>> WS Tx - '%.*s' [%d]\n\r%s", method->len, method->ptr, ret.length(), ret.c_str());

    return ret;
}
