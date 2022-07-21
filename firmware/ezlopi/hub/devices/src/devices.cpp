#include <string>
#include <cstring>

#include "devices.h"
#include "debug.h"
#include "frozen.h"
#include "devices_common.h"

#define TEST 0

using namespace std;
devices *devices::devices_ = nullptr;

const char *devices_1_start = "{\"method\":\"hub.devices.list\",\"msg_id\":%d,\"id\":\"%.*s\",\"result\":{\"devices\":[";
const char *devices_1_devs =
    "{\"_id\":\"d%.*s\","        // 588b7eb528b12d03be86f36f
    "\"deviceTypeId\":\"%.*s\"," // plughub
    "\"parentDeviceId\":\"\","
    "\"category\":\"%.*s\","    // switch -> Category -> https://api.ezlo.com/devices/categories/index.html
    "\"subcategory\":\"%.*s\"," // in_wall -> sub-category -> https://api.ezlo.com/devices/categories/index.html
    "\"batteryPowered\":false,"
    "\"name\":\"%.*s\"," // less than 16 bytes, user enters from QT-app
    "\"type\":\"%.*s\"," // switch.inwall -> deviceType -> https://api.ezlo.com/devices/devices/index.html#devices-new-types
    "\"reachable\":true,"
    "\"persistent\":true,"
    "\"serviceNotification\":false,"
    "\"armed\":false,"
    "\"roomId\":\"b%.*s\"," // 1234567
    "\"security\":\"\","
    "\"ready\":true,"
    "\"status\":\"synced\","
    "\"info\":{}"
    "}";
const char *devices_1_end = "]},\"error\":null,\"sender\":%.*s}";

devices *devices::get_instance(void)
{
    if (nullptr == devices_)
    {
        devices_ = new devices();
    }

    return devices_;
}

string devices::list(const char *payload, uint32_t len, struct json_token *method, uint32_t msg_count)
{
    int len_b = 0;
    string ret = "";
    char send_buf[4096] = {'\0'};

    struct json_token msg_id = JSON_INVALID_TOKEN;
    json_scanf(payload, len, "{id: %T}", &msg_id);
    struct json_token sender = JSON_INVALID_TOKEN;
    int sender_status = json_scanf(payload, len, "{sender: %T}", &sender);

#if (0 == TEST)
    devices_common *devices_ctx = devices_common::get_instance();
    s_device_properties_t *devices_list = devices_ctx->device_list();
    snprintf(send_buf, sizeof(send_buf), devices_1_start, msg_count, msg_id.len, msg_id.ptr);

    if (devices_list[0].name[0])
    {
        for (int i = 0; i < MAX_DEV; i++)
        {
            len_b = strlen(send_buf);
            snprintf(&send_buf[len_b], sizeof(send_buf) - len_b, devices_1_devs,
                     sizeof(devices_list[i].device_id), devices_list[i].device_id,
                     //  7, "plughub", // manufacturer deviceTypeId
                     //  6, "ezlopi", // manufacturer deviceTypeId
                     strlen(devices_ctx->get_device_type_id()), devices_ctx->get_device_type_id(),

                     //  6, "switch", // category -> https://api.ezlo.com/devices/categories/index.html
                     sizeof(devices_list[i].category), devices_list[i].category,

                     //  5, "relay",  // "in_wall", // subCategory -> https://api.ezlo.com/devices/categories/index.html
                     sizeof(devices_list[i].subcategory), devices_list[i].subcategory,

                     sizeof(devices_list[i].name), devices_list[i].name,

                     // 13, "switch.inwall", // deviceTypes ->  https://api.ezlo.com/devices/devices/index.html#devices-new-types
                     // 13, "switch.outlet",
                     sizeof(devices_list[i].devicType), devices_list[i].devicType,

                     sizeof(devices_list[i].roomId), devices_list[i].roomId);
            //  "588b7eb528b12d03be86f36f", "plughub",    "switch", "in_wall",   "SWITCH1",   "switch.inwall", "1234567");
            //   device_name                 deviceTypeId  category  subcategory  device_name  device_type      room_id

            if (i < MAX_DEV - 1)
            {
                if (devices_list[i + 1].name[0])
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
    snprintf(&send_buf[len_b], sizeof(send_buf) - len_b, devices_1_end, sender_status ? sender.len : 2, sender_status ? sender.ptr : "{}");
    len_b = strlen(send_buf);
    // send_buf[len_b] = 0;

    ret = send_buf;

    TRACE_B(">> WS Tx - '%.*s' [%d]\r\n%s", method->len, method->ptr, ret.length(), ret.c_str());
#endif

#if (1 == TEST)
    static const char *test =
        "{\"method\" : \"hub.devices.list\","
        "\"msg_id\" : 3364512106,"
        "\"result\" : {"
        "\"devices\" : ["
        "{"
        "\"_id\" : \"588b7eb528b12d03be86f36f\","
        "\"deviceTypeId\" : \"plughub\","
        "\"parentDeviceId\" : \"\","
        "\"category\" : \"switch\","
        "\"subcategory\" : \"in_wall\","
        "\"name\" : \"SWITCH1\","
        "\"type\" : \"switch.inwall\","
        "\"batteryPowered\" : false,"
        "\"reachable\" : true,"
        "\"persistent\" : true,"
        "\"serviceNotification\" : false,"
        "\"armed\" : false,"
        "\"roomId\" : \"\","
        "\"security\" : \"\","
        "\"ready\" : true,"
        "\"status\":\"synced\","
        "\"info\":{}"
        "}"
        "]"
        "},"
        "\"error\":null,"
        "\"id\":\"%.*s\",\"sender\":%.*s}";

    snprintf(send_buf, sizeof(send_buf), test, msg_id.len, msg_id.ptr, sender_status ? sender.len : 2, sender_status ? sender.ptr : "{}");
    ret = send_buf;

    TRACE_B(">>>>>>>>>>> WS Tx - '%.*s' [%d]\r\n%s", method->len, method->ptr, ret.length(), ret.c_str());
#endif

    return ret;
}

string devices::settings_list(const char *payload, uint32_t len, struct json_token *method, uint32_t msg_count)
{
    string ret = "";
    char send_buf[256] = {'\0'};
    json_token msg_id = JSON_INVALID_TOKEN;
    json_token sender = JSON_INVALID_TOKEN;

    json_scanf(payload, len, "{id:%T}", &msg_id);
    json_scanf(payload, len, "{sender:%T}", &sender);

    static const char *settings_list_frmt = "{\"id\":\"%.*s\",\"msg_id\":%u,\"method\":\"%.*s\",\"result\":{\"settings\":[]},\"sender\":%.*s,\"error\":null}";

    snprintf(send_buf, sizeof(send_buf), settings_list_frmt,
             msg_id.len, msg_id.ptr,
             msg_count,
             method->len, method->ptr,
             sender.len ? sender.len : 2, sender.len ? sender.ptr : "{}");

    ret = send_buf;
    return ret;
}
