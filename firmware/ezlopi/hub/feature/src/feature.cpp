#include <string>
#include <cstring>

#include "feature.h"
#include "debug.h"
#include "frozen.h"
#include "devices_common.h"

using namespace std;

const char *feature_1_start = "{\"method\":\"hub.features.list\",\"api\":\"1.0\",\"error\":null,\"id\":\"%.*s\",\"result\":{\"feature\":{\"devices\":[";
const char *feature_1_list = "{\"_id\":\"%s\"}";
const char *feature_1_end = "]}}}";

feature *feature::feature_ = nullptr;

feature *feature::get_instance(void)
{
    if (nullptr == feature_)
    {
        feature_ = new feature();
    }

    return feature_;
}

string feature::list(const char *payload, uint32_t len, struct json_token *method)
{
    string ret = "";
    char send_buf[2048] = {'\0'};
    // factory_info *factory = factory_info::get_instance();
    // devices_common *devices_ctx = devices_common::get_instance();
    // s_device_properties_t *devices = devices_ctx->device_list();

    struct json_token msg_id = JSON_INVALID_TOKEN;
    json_scanf(payload, len, "{id: %T}", &msg_id);

    snprintf(send_buf, sizeof(send_buf), feature_1_start, msg_id.len, msg_id.ptr);
    int len_b = strlen(send_buf);
    snprintf(&send_buf[len_b], sizeof(send_buf) - len_b, "%s", feature_1_end);
    len_b = strlen(send_buf);
    send_buf[len_b] = 0;

    ret = send_buf;

    TRACE_B(">> WS Tx - '%.*s' [%d]\n\r%s", method->len, method->ptr, ret.length(), ret.c_str());

    return ret;
}
