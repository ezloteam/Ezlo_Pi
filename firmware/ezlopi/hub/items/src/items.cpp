#include <string>
#include <cstring>

#include "items.h"
#include "debug.h"
#include "frozen.h"
#include "devices_common.h"
#include "interface_common.h"

#define TEST 0

using namespace std;
items *items::items_ = nullptr;
static void parse_item_id(char *data, uint32_t len, char *item_id);

items *items::get_instance(void)
{
    if (nullptr == items_)
    {
        items_ = new items();
    }

    return items_;
}

static const char *items_list_start = "{\"method\":\"hub.items.list\",\"msg_id\":%d,\"result\":{\"items\":[";
static const char *items_list_item = "{\"_id\":\"a%.*s\","
                                     "\"deviceId\":\"d%.*s\","
                                     "\"deviceName\":\"%.*s\","
                                     "\"deviceArmed\":true,"
                                     "\"hasGetter\":%s,"
                                     "\"hasSetter\":%s,"
                                     "\"name\":\"%.*s\","
                                     "\"show\":true,"
                                     "\"valueType\":\"%s\","
                                     "\"value\":%s,"
                                     "\"valueFormatted\":\"false\","
                                     "\"status\":\"synced\"}";
static const char *items_list_end = "]},\"error\":null,\"id\":\"%.*s\",\"sender\":%.*s}";

string items::list(const char *payload, uint32_t len, struct json_token *method, uint32_t msg_count)
{
    int len_b = 0;
    string ret = "";
    char send_buf[4096] = {'\0'};
    devices_common *devices_ctx = devices_common::get_instance();
    s_device_properties_t *devices = devices_ctx->device_list();

    struct json_token msg_id = JSON_INVALID_TOKEN;
    json_scanf(payload, len, "{id: %T}", &msg_id);

    struct json_token sender = JSON_INVALID_TOKEN;
    int snd_state = json_scanf(payload, len, "{sender: %T}", &sender);

#if (0 == TEST)
    snprintf(send_buf, sizeof(send_buf), items_list_start);
    snprintf(send_buf, sizeof(send_buf), items_list_start, msg_count);

    if (devices[0].name[0])
    {
        for (int i = 0; i < MAX_DEV; i++)
        {
            interface_common *gpio_interface = interface_common::get_instance();
            uint32_t current_state = gpio_interface->gpio_state_get(devices[i].out_gpio);

            char dev_value[40];
            switch (devices[i].dev_type)
            {
            case TAMPER:
            {
                snprintf(dev_value, sizeof(dev_value), "%f,\"scale\":\"celsius\"", current_state + 25.25f);
                break;
            }
            case LED:
            case SWITCH:
            case PLUG:
            {
                snprintf(dev_value, sizeof(dev_value), "%s", current_state ? "true" : "false");
                break;
            }

            default:
            {
                break;
            }
            }

            len_b = strlen(send_buf);
            snprintf(&send_buf[len_b], sizeof(send_buf) - len_b, items_list_item,
                     sizeof(devices[i].item_id), devices[i].item_id,
                     sizeof(devices[i].device_id), devices[i].device_id,
                     sizeof(devices[i].name), devices[i].name,
                     devices[i].has_getter ? "true" : "fasle",
                     devices[i].has_setter ? "true" : "false",
                     sizeof(devices[i].item_name), devices[i].item_name, // sizeof(devices->item_name), devices->item_name,
                     devices[i].value_type,
                     dev_value);
            //  current_state ? "true" : "false"); // get latest value from gpio(common interface)
            // "142F6C69", "588b7eb528b12d03be86f36f", "SWITCH1",       "switch");
            //  item_id     device_id                   device_name      item_name
            // snprintf(&send_buf[len_b], sizeof(send_buf) - len_b, items_list_item, devices[i].device_id, devices[i].name);
            if (i < (MAX_DEV - 1))
            {
                if (devices[i + 1].name[0])
                {
                    len_b = strlen(send_buf);
                    send_buf[len_b] = ',';
                    send_buf[len_b + 1] = 0;
                }
                else
                {
                    break;
                }
            }
        }
    }

    len_b = strlen(send_buf);
    snprintf(&send_buf[len_b], sizeof(send_buf) - len_b, items_list_end, msg_id.len, msg_id.ptr,
             snd_state ? sender.len : 2, snd_state ? sender.ptr : "{}");
#endif

#if (TEST == 1)
    static const char *test =
        "{\"method\":\"hub.items.list\","
        "\"msg_id\":3364512107,"
        "\"result\":{\"items\":[{\"_id\":\"142F6C69\","
        "\"deviceId\":\"588b7eb528b12d03be86f36f\","
        "\"deviceName\": \"SWITCH1\","
        "\"deviceArmed\":true,"
        "\"hasGetter\":true,"
        "\"hasSetter\":true,"
        "\"name\":\"switch\","
        "\"show\":true,"
        "\"valueType\":\"bool\","
        "\"value\":false, \"valueFormatted\":\"false\","
        "\"status\":\"synced\"}]},"
        "\"error\":null,"
        "\"id\":\"%.*s\",\"sender\":%.*s}";

    ret = "";
    snprintf(send_buf, sizeof(send_buf), test, msg_id.len, msg_id.ptr, snd_state ? sender.len : 2, snd_state ? sender.ptr : "{}");
#endif

    ret = send_buf;

    // TRACE_B(">>>>>>>>>>> WS Tx - '%.*s' [%d]\r\n%s", method->len, method->ptr, strlen(send_buf), send_buf);
    TRACE_B(">> WS Tx - '%.*s' [%d]\r\n%s", method->len, method->ptr, ret.length(), ret.c_str());
    return ret;
}

string items::set_value(const char *payload, uint32_t len, struct json_token *method, uint32_t msg_count)
{
    string ret = "";
    char send_buf[400];
    static const char *response = "{\"api\":\"1.0\",\"msg_id\":%d,\"method\":\"%.*s\",\"error\":null,\"id\":\"%.*s\",\"result\":{},\"sender\":%.*s}";

    int sender_found = 0;
    // bool value = false;
    struct json_token value = JSON_INVALID_TOKEN;
    struct json_token msg_id = JSON_INVALID_TOKEN;
    struct json_token sender = JSON_INVALID_TOKEN;
    struct json_token params = JSON_INVALID_TOKEN;
    devices_common *device_ctx = devices_common::get_instance();

    json_scanf(payload, len, "{id:%T}", &msg_id);
    json_scanf(payload, len, "{params:%T}", &params);
    json_scanf(params.ptr, params.len, "{value:%T}", &value);
    sender_found = json_scanf(payload, len, "{sender:%T}", &sender);

    snprintf(send_buf, sizeof(send_buf), response, msg_count,
             method->len, method->ptr,
             msg_id.len, msg_id.ptr,
             sender_found ? sender.len : 2, sender_found ? sender.ptr : "{}");

    char item_id[20] = {'\0', '\0'};
    parse_item_id((char *)params.ptr, params.len, item_id);

    int device_idx = device_ctx->get_device_by_item_id(&item_id[1]);

    if (device_idx < MAX_DEV)
    {
        s_device_properties_t *dev_list = device_ctx->device_list();

        interface_common *gpio_interface = interface_common::get_instance();
        uint32_t state = value.len ? (strncmp(value.ptr, "true", 4) ? false : true) : false;
        TRACE_D("Current state: %d", state);
        if (dev_list[device_idx].out_inv)
        {
            state = state ? 0 : 1;
            gpio_interface->gpio_state_set(dev_list[device_idx].out_gpio, state);
        }
        else
        {
            gpio_interface->gpio_state_set(dev_list[device_idx].out_gpio, state);
        }
    }

    ret = send_buf;
    TRACE_B(">> WS Tx - '%.*s' [%d]\r\n%s", method->len, method->ptr, ret.length(), ret.c_str());

    return ret;
}

string items::update(const char *payload, uint32_t len, struct json_token *method, uint32_t msg_count)
{
    return update(payload, len, method, msg_count, UINT32_MAX);
}

string items::update(const char *payload, uint32_t len, struct json_token *method, uint32_t msg_count, int device_index)
{
    string ret = "";
    int device_idx = 0;
    char send_buf[512] = {'\0'};
    char item_id[20] = {'\0', '\0'};
    struct json_token value = JSON_INVALID_TOKEN;
    struct json_token params = JSON_INVALID_TOKEN;
    devices_common *device_ctx = devices_common::get_instance();

    if (payload && len > 10)
    {
        json_scanf(payload, len, "{params:%T}", &params);
        json_scanf(params.ptr, params.len, "{value:%T}", &value);

        TRACE_W("params: %.*s", params.len, params.ptr);
        TRACE_W("value: %.*s", value.len, value.ptr);

        parse_item_id((char *)params.ptr, params.len, item_id);
        device_idx = device_ctx->get_device_by_item_id(&item_id[1]);
    }
    else
    {
        device_idx = device_index;
    }

    static const char *update_frmt = "{\"id\":\"ui_broadcast\",\"msg_id\":%d,\"msg_subclass\":\"hub.item.updated\",\"result\":{\"_id\":\"a%.*s\",\"deviceId\":\"d%.*s\","
                                     "\"deviceName\":\"%.*s\",\"deviceCategory\":\"switch\",\"deviceSubcategory\":\"in_wall\",\"roomName\":\"%.*s\","
                                     "\"serviceNotification\":false,\"userNotification\":false,\"notifications\":null,\"name\":\"%.*s\",\"valueType\":\"%s\","
                                     "\"value\":%s}}";

    TRACE_D("device idx: %d", device_idx);

    if (device_idx < MAX_DEV)
    {
        s_device_properties_t *dev_list = device_ctx->device_list();
        interface_common *gpio_interface = interface_common::get_instance();
        uint32_t current_state = gpio_interface->gpio_state_get(dev_list[device_idx].out_gpio);

        snprintf(send_buf, sizeof(send_buf), update_frmt, msg_count,
                 sizeof(dev_list[device_idx].item_id), dev_list[device_idx].item_id,
                 sizeof(dev_list[device_idx].device_id), dev_list[device_idx].device_id,
                 sizeof(dev_list[device_idx].name), dev_list[device_idx].name,
                 sizeof(dev_list[device_idx].roomName), dev_list[device_idx].roomName,
                 sizeof(dev_list[device_idx].item_name), dev_list[device_idx].item_name,
                 "bool", current_state ? "true" : "false");
        //  "142F6C69", "588b7eb528b12d03be86f36f", "SWITCH1",   "dining room", "switch",   "bool",       value ? "true" : "false");
        //   item_id,    device_id,                  device_name  room_name      item_name   valute_type  value
    }
    else
    {
        if ('\0' == item_id[0])
        {
            TRACE_E("Missing Item_id");
            strcpy(send_buf, "{\"id\":\"_ID_\",\"error\":{\"code\":-32600,\"data\":\"rpc.params.empty._id\",\"description\":\"Missing item_id\"},\"result\":{}}");
        }
        else
        {
            TRACE_E("Device Not found: device_idx: %d", device_idx);
        }
    }

    ret = send_buf;
    TRACE_B(">> WS Tx - 'hub.item.updated' [%d]\r\n%s", ret.length(), ret.c_str());

    return ret;
}

static void parse_item_id(char *data, uint32_t len, char *item_id)
{
    char *param_ptr = data;
    char param_c = param_ptr[len];
    param_ptr[len] = '\0';
    const char *needle = "\"_id\":\"";
    char *_id_start = strstr(param_ptr, needle);
    _id_start += strlen(needle);
    char *_id_end = strchr(_id_start, '"');
    param_ptr[len] = param_c;

    if (_id_start && _id_end && (_id_end - _id_start))
    {
        snprintf(item_id, _id_end - _id_start + 2, "%.*s", _id_end - _id_start, _id_start);
        TRACE_D("## item_id: %s", item_id);
    }
    else
    {
        TRACE_E("item_id not found!");
    }
}