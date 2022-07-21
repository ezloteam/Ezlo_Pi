#include <string>
#include <cstring>

#include "sdkconfig.h"
#include "factory_info.h"
#include "info.h"
#include "debug.h"
#include "frozen.h"
#include "devices_common.h"
#include "version.h"

using namespace std;

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

info *info::info_ = nullptr;

info *info::get_instance(void)
{
    if (nullptr == info_)
    {
        info_ = new info();
    }

    return info_;
}

string info::get(const char *payload, uint32_t len, struct json_token *method, uint32_t msg_count)
{
    string ret = "";
    char send_buf[1024] = {'\0'};
    factory_info *factory = factory_info::get_instance();
    // devices_common *devices_ctx = devices_common::get_instance();
    // s_device_properties_t *devices = devices_ctx->device_list();

    struct json_token msg_id = JSON_INVALID_TOKEN;
    json_scanf(payload, len, "{id: %T}", &msg_id);

    struct json_token sender = JSON_INVALID_TOKEN;
    int sender_status = json_scanf(payload, len, "{sender: %T}", &sender);

    snprintf(send_buf, sizeof(send_buf), info_1_start, msg_count,
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

    int len_b = strlen(send_buf);
    send_buf[len_b] = 0;

    ret = send_buf;

    TRACE_B(">>>>>>>>>>> WS Tx - '%.*s' [%d]\n\r%s", method->len, method->ptr, ret.length(), ret.c_str());

    return ret;
}
