#include <string>
#include <cstring>

#include "room.h"
#include "debug.h"
#include "frozen.h"
#include "devices_common.h"

using namespace std;
room *room::room_ = nullptr;
room *room::get_instance(void)
{
    if (nullptr == room_)
    {
        room_ = new room();
    }

    return room_;
}

static const char *room_list_start = "{\"method\":\"hub.room.list\",\"msg_id\":%d,\"error\":null,\"id\":\"%.*s\",\"result\":[";
static const char *room_list_room = "{\"_id\":\"b%.*s\", \"name\":\"%.*s\"}";
static const char *room_list_end = "],\"sender\":%.*s}";

string room::list(const char *payload, uint32_t len, struct json_token *method, uint32_t msg_count)
{
    string ret = "";
    int len_s = 0;
    char send_buf[2048] = {"\0"};
    int remove_comma = 0;

    devices_common *devices_ctx = devices_common::get_instance();
    s_device_properties_t *devices_list = devices_ctx->device_list();

    struct json_token msg_id = JSON_INVALID_TOKEN;
    struct json_token sender = JSON_INVALID_TOKEN;

    json_scanf(payload, len, "{id:%T}", &msg_id);
    int found_sender = json_scanf(payload, len, "{sender:%T}", &sender);
    snprintf(send_buf, sizeof(send_buf), room_list_start, msg_count, msg_id.len, msg_id.ptr);

    for (int idx = 0; idx < MAX_DEV; idx++)
    {
        if (devices_list[idx].name[0])
        {
            remove_comma = 1;
            len_s = strlen(send_buf);
            snprintf(&send_buf[len_s], sizeof(send_buf) - len_s, room_list_room,
                     sizeof(devices_list[idx].roomId), devices_list[idx].roomId,
                     sizeof(devices_list[idx].roomName), devices_list[idx].roomName);
            len_s = strlen(send_buf);
            send_buf[len_s] = ',';
        }
    }

    if (remove_comma)
    {
        len_s = strlen(send_buf);
        send_buf[len_s - 1] = '\0';
    }

    len_s = strlen(send_buf);
    snprintf(&send_buf[len_s], sizeof(send_buf) - len_s, room_list_end, found_sender ? sender.len : 2, found_sender ? sender.ptr : "{}");

    ret = send_buf;
    TRACE_B(">> WS Tx - '%.*s' [%d]\r\n%s", method->len, method->ptr, ret.length(), ret.c_str());

    return ret;
}
