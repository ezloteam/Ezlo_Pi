#include <string>
#include <cstring>

#include "modes.h"
#include "debug.h"
#include "frozen.h"
#include "devices_common.h"

using namespace std;

const char *modes_1_start = "{\"method\":\"hub.modes.get\",\"msg_id\":%d,\"api\":\"1.0\",\"error\":null,\"id\":\"%.*s\",\"result\":{\"current\":\"%s\",\"switchTo\":\"\",\"timeIsLeftToSwitch\":0,\"switchToDelay\":0,\"alarmDelay\":0,\"modes\":[";
const char *modes_1_modes_start = "{\"_id\":\"%s\",\"name\":\"%s\",\"description\":\"%s\"";
const char *modes_1_notifications = "\"notifications\":[\"%s\"";
const char *modes_1_disarmedDefault = "\"disarmedDefault\":[\"%s\"";
const char *modes_1_disarmedDevices = "\"disarmedDevices\":[\"%s\"";
const char *modes_1_alarmsOffDevices = "\"alarmsOffDevices\":[\"%s\"";
const char *modes_1_camerasOffDevices = "\"camerasOffDevices\":[\"%s\"";
const char *modes_1_protect = "\"protect\":[\"%s\"";
const char *modes_1_item_end = "]";
const char *modes_1_end = "]},\"sender\":%.*s}";

modes *modes::modes_ = nullptr;

modes *modes::get_instance(void)
{
    if (nullptr == modes_)
    {
        modes_ = new modes();
    }

    return modes_;
}

string modes::get(const char *payload, uint32_t len, struct json_token *method, uint32_t msg_count)
{
    string ret = "";
    char send_buf[2048] = {'\0'};
    devices_common *devices_ctx = devices_common::get_instance();
    s_device_properties_t *devices = devices_ctx->device_list();

    struct json_token msg_id = JSON_INVALID_TOKEN;
    json_scanf(payload, len, "{id: %T}", &msg_id);

    struct json_token sender = JSON_INVALID_TOKEN;
    int sender_status = json_scanf(payload, len, "{sender: %T}", &sender);

    snprintf(send_buf, sizeof(send_buf), modes_1_start, msg_count, msg_id.len, msg_id.ptr, "");
    int len_b = strlen(send_buf);
    snprintf(&send_buf[len_b], sizeof(send_buf) - len_b, modes_1_end, sender_status ? sender.len : 2, sender_status ? sender.ptr : "{}");
    len_b = strlen(send_buf);
    send_buf[len_b] = 0;

    ret = send_buf;

    TRACE_B(">>>>>>>>>>> WS Tx - '%.*s' [%d]\n\r%s", method->len, method->ptr, ret.length(), ret.c_str());

    return ret;
}
