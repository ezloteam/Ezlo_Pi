#include <string.h>

#include "modes.h"
#include "debug.h"
#include "frozen.h"
#include "devices_common.h"

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

char *modes_get(const char *payload, uint32_t len, struct json_token *method, uint32_t msg_count)
{
    uint32_t buf_len = 2014;
    char *send_buf = (char *)malloc(buf_len);

    if (send_buf)
    {

        struct json_token msg_id = JSON_INVALID_TOKEN;
        json_scanf(payload, len, "{id: %T}", &msg_id);

        struct json_token sender = JSON_INVALID_TOKEN;
        int sender_status = json_scanf(payload, len, "{sender: %T}", &sender);

        snprintf(send_buf, buf_len, modes_1_start, msg_count, msg_id.len, msg_id.ptr, "");
        int len_b = strlen(send_buf);
        snprintf(&send_buf[len_b], buf_len - len_b, modes_1_end, sender_status ? sender.len : 2, sender_status ? sender.ptr : "{}");

        TRACE_B(">>>>>>>>>>> WS Tx - '%.*s' [%d]\n\r%s", method->len, method->ptr, strlen(send_buf), send_buf);
    }

    return send_buf;
}
