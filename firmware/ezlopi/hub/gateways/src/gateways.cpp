#include <string>
#include <cstring>

#include "gateways.h"
#include "debug.h"
#include "frozen.h"
#include "devices_common.h"

using namespace std;

static const char *gateways_1_start = "{\"method\":\"hub.gateways.list\",\"msg_id\":%d,\"api\":\"1.0\",\"error\":null,\"id\":\"%.*s\",\"result\":{\"gateways\":[";
static const char *gateways_1_list =
    "{\"_id\" : \"%s\","
    "\"addItemDictionaryValueCommand\": \"%s\"," //\"HUB:test_plugin/scripts/add_item_dictionary_value\","
    "\"checkDeviceCommand\" : \"\","
    "\"clearItemDictionaryCommand\" : \"\","
    "\"forceRemoveDeviceCommand\" : \"\","
    "\"label\" : \"test gateway\","
    "\"manualDeviceAdding\" : \"wizard\","
    "\"name\" : \"test_plugin\","
    "\"pluginId\" : \"test_plugin\","
    "\"ready\" : true,"
    "\"removeItemDictionaryValueCommand\" : \"\"," //\"HUB:test_plugin/scripts/remove_item_dictionary_value\","
    "\"resetSettingCommand\" : \"\","
    "\"setGatewaySettingValueCommand\" : \"\"," // \"HUB:test_plugin/scripts/set_gateway_setting_value\","
    "\"setItemDictionaryValueCommand\" : \"\"," // \"HUB:test_plugin/scripts/set_item_dictionary_value\","
    "\"setItemValueCommand\" : \"\","           // \"HUB:test_plugin/scripts/set_item_value\","
    "\"setSettingDictionaryValueCommand\" : \"\","
    "\"setSettingValueCommand\" : \"\"," // \"HUB:test_plugin/scripts/set_setting_value\","
    "\"settings\" : 3,"
    "\"status\" : \"ready\","
    "\"updateDeviceFirmwareCommand\" : \"\"}";
const char *gateways_1_end = "]},\"sender\":%.*s}";

gateways *gateways::gateways_ = nullptr;
gateways *gateways::get_instance(void)
{
    if (nullptr == gateways_)
    {
        gateways_ = new gateways();
    }
    return gateways_;
}

string gateways::list(const char *payload, uint32_t len, struct json_token *method, uint32_t msg_count)
{
    string ret = "";
    char send_buf[2048] = {'\0'};
    struct json_token sender = JSON_INVALID_TOKEN;
    struct json_token msg_id = JSON_INVALID_TOKEN;

    devices_common *devices_ctx = devices_common::get_instance();
    s_device_properties_t *devices = devices_ctx->device_list();

    json_scanf(payload, len, "{id: %T}", &msg_id);
    int sender_stat = json_scanf(payload, len, "{sender: %T}", &sender);

    snprintf(send_buf, sizeof(send_buf), gateways_1_start, msg_count, msg_id.len, msg_id.ptr);
    int len_b = strlen(send_buf);
    snprintf(&send_buf[len_b], sizeof(send_buf) - len_b, gateways_1_end, sender_stat ? sender.len : 2, sender_stat ? sender.ptr : "{}");
    len_b = strlen(send_buf);
    send_buf[len_b] = 0;

    ret = send_buf;

    TRACE_B(">>>>>>>>>>> WS Tx - '%.*s' [%d]\n\r%s", method->len, method->ptr, ret.length(), ret.c_str());

    return ret;
}
