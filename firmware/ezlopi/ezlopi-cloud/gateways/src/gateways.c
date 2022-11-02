#include <string.h>

#include "gateways.h"
#include "trace.h"
#include "frozen.h"

static const char *gateways_1_start = "{\"method\":\"hub.gateways.list\",\"msg_id\":%d,\"api\":\"1.0\",\"error\":null,\"id\":\"%.*s\",\"result\":{\"gateways\":[";
// static const char *gateways_1_list =
//     "{\"_id\" : \"%s\","
//     "\"addItemDictionaryValueCommand\": \"%s\"," //\"HUB:test_plugin/scripts/add_item_dictionary_value\","
//     "\"checkDeviceCommand\" : \"\","
//     "\"clearItemDictionaryCommand\" : \"\","
//     "\"forceRemoveDeviceCommand\" : \"\","
//     "\"label\" : \"test gateway\","
//     "\"manualDeviceAdding\" : \"wizard\","
//     "\"name\" : \"test_plugin\","
//     "\"pluginId\" : \"test_plugin\","
//     "\"ready\" : true,"
//     "\"removeItemDictionaryValueCommand\" : \"\"," //\"HUB:test_plugin/scripts/remove_item_dictionary_value\","
//     "\"resetSettingCommand\" : \"\","
//     "\"setGatewaySettingValueCommand\" : \"\"," // \"HUB:test_plugin/scripts/set_gateway_setting_value\","
//     "\"setItemDictionaryValueCommand\" : \"\"," // \"HUB:test_plugin/scripts/set_item_dictionary_value\","
//     "\"setItemValueCommand\" : \"\","           // \"HUB:test_plugin/scripts/set_item_value\","
//     "\"setSettingDictionaryValueCommand\" : \"\","
//     "\"setSettingValueCommand\" : \"\"," // \"HUB:test_plugin/scripts/set_setting_value\","
//     "\"settings\" : 3,"
//     "\"status\" : \"ready\","
//     "\"updateDeviceFirmwareCommand\" : \"\"}";
const char *gateways_1_end = "]},\"sender\":%.*s}";

char *gateways_list(const char *payload, uint32_t len, struct json_token *method, uint32_t msg_count)
{
    uint32_t buf_len = 2048;
    char *send_buf = (char *)malloc(buf_len);

    if (send_buf)
    {
        memset(send_buf, 0, buf_len);
        struct json_token sender = JSON_INVALID_TOKEN;
        struct json_token msg_id = JSON_INVALID_TOKEN;

        json_scanf(payload, len, "{id: %T}", &msg_id);
        int sender_stat = json_scanf(payload, len, "{sender: %T}", &sender);

        snprintf(send_buf, buf_len, gateways_1_start, msg_count, msg_id.len, msg_id.ptr);
        int len_b = strlen(send_buf);
        snprintf(&send_buf[len_b], buf_len - len_b, gateways_1_end, sender_stat ? sender.len : 2, sender_stat ? sender.ptr : "{}");
        len_b = strlen(send_buf);
        send_buf[len_b] = 0;

        TRACE_B(">> WS Tx - '%.*s' [%d]\n\r%s", method->len, method->ptr, strlen(send_buf), send_buf);
    }

    return send_buf;
}
