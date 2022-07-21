#include <string>
#include <cstring>

#include "frozen.h"
#include "settings.h"

settings *settings::settings_ = nullptr;
settings *settings::get_instance(void)
{
    if (nullptr == settings_)
    {
        settings_ = new settings();
    }
    return settings_;
}

string settings::list(const char *payload, uint32_t len, struct json_token *method, uint32_t msg_count)
{
    string ret = "";
    char send_buf[256];
    json_token msg_id = JSON_INVALID_TOKEN;
    json_token sender = JSON_INVALID_TOKEN;

    json_scanf(payload, len, "{id:%T}", &msg_id);
    json_scanf(payload, len, "{sender:%T}", &sender);

    static const char *list_frmt = "{\"id\":\"%.*s\",\"method\":\"%.*s\",\"msg_id\":%u,\"result\":{\"settings\":[]},\"error\":null,\"sender\":%.*s}";
    snprintf(send_buf, sizeof(send_buf), list_frmt,
             msg_id.len, msg_id.ptr,
             method->len, method->ptr,
             msg_count,
             sender.len ? sender.len : 2, sender.len ? sender.ptr : "{}");

    ret = send_buf;
    return ret;
}