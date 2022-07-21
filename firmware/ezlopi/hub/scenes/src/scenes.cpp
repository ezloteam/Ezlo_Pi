#include <string>
#include <cstring>

#include "scenes.h"
#include "debug.h"
#include "frozen.h"
#include "devices_common.h"

using namespace std;
scenes *scenes::scenes_ = nullptr;
static const char *json_scenes_list = "{\"method\":\"%.*s\",\"msg_id\":%d,\"result\":{\"scenes\":[]},\"error\":null,\"id\":\"%.*s\",\"sender\":%.*s}";

scenes *scenes::get_instance(void)
{
    if (nullptr == scenes_)
    {
        scenes_ = new scenes();
    }

    return scenes_;
}

string scenes::list(const char *payload, uint32_t len, struct json_token *method, uint32_t msg_count)
{
    string ret = "";
    char send_buf[2048] = {'\0'};

    struct json_token msg_id;
    json_scanf(payload, len, "{id: %T}", &msg_id);

    struct json_token sender;
    int sender_status = json_scanf(payload, len, "{sender: %T}", &sender);

    snprintf(send_buf, sizeof(send_buf), json_scenes_list, method->len, method->ptr, msg_count, msg_id.len, msg_id.ptr, sender_status ? sender.len : 2, sender_status ? sender.ptr : "{}");
    ret = send_buf;
    TRACE_B(">> WS Tx - '%.*s' [%d]\r\n%s", method->len, method->ptr, ret.length(), ret.c_str());

    return ret;
}
