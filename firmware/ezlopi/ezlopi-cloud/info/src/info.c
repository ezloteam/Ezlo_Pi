#include <string.h>

#include "sdkconfig.h"
#include "ezlopi_factory_info.h"
#include "info.h"
#include "trace.h"
#include "frozen.h"
#include "devices_common.h"
#include "version.h"

const char *info_1_start = "{\"method\":\"hub.info.get\",\"msg_id\":%d,\"api\":\"1.0\",\"error\":null,\"id\":\"%.*s\",\"result\":{"
                           "\"model\":\"%s\","
                           "\"architecture\":\"%s\","
                           "\"firmware\":\"%s\","
                           "\"kernel\":\"%s\","
                           "\"hardware\":\"%s\","
                           "\"serial\":\"%llu\","
                           "\"uuid\":\"%s\","
                           "\"offlineAnonymousAccess\":true,"
                           "\"offlineInsecureAccess\":true,"
                           "\"location\":{\"latitude\": %f, \"longitude\": %f, \"timezone\":\"%s\", \"state\":\"%s\"},"
                           "\"build\":{\"time\": \"%s\", \"builder\":\"%s\", \"branch\":\"%s\", \"commit\":\"%s\"},"
                           "\"battery\":{\"stateOfCharge\":%d, \"remainingTime\": %d, \"status\":\"%s\", \"health\": %d},"
                           "\"uptime\":\"%s\","
                           "\"localtime\":\"%s\""
                           "},"
                           "\"sender\":%.*s"
                           "}";

char *info_get(const char *payload, uint32_t len, struct json_token *method, uint32_t msg_count)
{
    uint32_t buf_len = 1024;
    char *send_buf = (char *)malloc(buf_len);

    if (send_buf)
    {
        memset(send_buf, 0, buf_len);
        // factory_info *factory = factory_info::get_instance();
        s_ezlopi_factory_info_t *factory = ezlopi_factory_info_get_info();

        struct json_token msg_id = JSON_INVALID_TOKEN;
        json_scanf(payload, len, "{id: %T}", &msg_id);

        struct json_token sender = JSON_INVALID_TOKEN;
        int sender_status = json_scanf(payload, len, "{sender: %T}", &sender);

        snprintf(send_buf, buf_len, info_1_start, msg_count,
                 msg_id.len, msg_id.ptr,
                 "EzloPi",
                 CONFIG_IDF_TARGET,
                 VERSION_STR,
                 IDF_VER,
                 "ESP32 DEV KIT V1",
                 factory->id,
                 factory->controller_uuid,
                 27.661294f, 85.334448f, "UTC+5:45", "custom",
                 "2022-07-01T12:32:30+0545", "krishna.kumar@ezlo.com", "developer", "feature_wss",
                 100, 0xFFFFFFFF, "AC powered", 100,
                 "5d 22h 18m 11s",
                 "2022-07-01T12:35:40+0545", sender_status ? sender.len : 2, sender_status ? sender.ptr : "{}");

        TRACE_B(">>>>>>>>>>> WS Tx - '%.*s' [%d]\n\r%s", method->len, method->ptr, strlen(send_buf), send_buf);
    }

    return send_buf;
}
