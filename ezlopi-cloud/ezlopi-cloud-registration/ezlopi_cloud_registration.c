#include <string.h>

#include "ezlopi_util_trace.h"
#include "ezlopi_util_version.h"

#include "ezlopi_cloud_constants.h"
#include "ezlopi_cloud_registration.h"

#include "ezlopi_core_broadcast.h"
#include "ezlopi_core_event_group.h"
#include "ezlopi_core_factory_info.h"

#include "ezlopi_service_loop.h"
#include "ezlopi_service_webprov.h"

static void __reg_loop(void *arg);

static cJSON * cj_reg_data = NULL;
static const char * __reg_loop_str = "reg-loop";

void registration_init(void)
{
    ezlopi_service_loop_add(__reg_loop_str, __reg_loop, 5000, NULL);
}

void register_repeat(cJSON* cj_request, cJSON* cj_response)
{
    ezlopi_service_loop_add(__reg_loop_str, __reg_loop, 5000, NULL);
}

void registered(cJSON* cj_request, cJSON* cj_response)
{
    if (cj_reg_data)
    {
        cJSON_Delete(__FUNCTION__, cj_reg_data);
        cj_reg_data = NULL;
    }
    ezlopi_service_loop_remove(__reg_loop);
}

static void __create_reg_packet(void)
{
    if (NULL == cj_reg_data)
    {
        cj_reg_data = cJSON_CreateObject(__FUNCTION__);

        if (cj_reg_data)
        {
            char mac_str[18];
            uint8_t mac_addr[6];

            esp_read_mac(mac_addr, ESP_MAC_WIFI_STA);
            snprintf(mac_str, sizeof(mac_str), "%02X:%02X:%02X:%02X:%02X:%02X",
                mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);

            cJSON_AddStringToObject(__FUNCTION__, cj_reg_data, "id", "__ID__");
            cJSON_AddStringToObject(__FUNCTION__, cj_reg_data, ezlopi_method_str, "register");

            cJSON* cj_params = cJSON_AddObjectToObject(__FUNCTION__, cj_reg_data, ezlopi_params_str);

            if (cj_params)
            {
                char __id_str[32];

                unsigned long long __id = ezlopi_factory_info_v3_get_id();
                snprintf(__id_str, sizeof(__id_str), "%llu", __id);
                cJSON_AddStringToObject(__FUNCTION__, cj_params, "serial", __id_str);

                cJSON_AddStringToObject(__FUNCTION__, cj_params, ezlopi_firmware_str, VERSION_STR);
                cJSON_AddNumberToObject(__FUNCTION__, cj_params, "timeOffset", 20700);
                cJSON_AddStringToObject(__FUNCTION__, cj_params, "media", "radio");
                cJSON_AddStringToObject(__FUNCTION__, cj_params, "hubType", "32.1");
                // cJSON_AddStringToObject(__FUNCTION__, cj_params, "mac_address", "11:22:33:44:55:66");

                char * __device_uuid = ezlopi_factory_info_v3_get_device_uuid();
                if (__device_uuid) {
                    cJSON_AddStringToObject(__FUNCTION__, cj_params, "controller_uuid", __device_uuid);
                    ezlopi_free(__FUNCTION__, __device_uuid);
                }

                cJSON_AddStringToObject(__FUNCTION__, cj_params, "mac_address", mac_str);
                cJSON_AddNumberToObject(__FUNCTION__, cj_params, "maxFrameSize", (20 * 1024));
            }
        }
    }
}

static void __reg_loop(void *arg)
{
    TRACE_D("reg-loop");
    int reg_event = ezlopi_event_group_wait_for_event(EZLOPI_EVENT_NMA_REG, 0, false);
    TRACE_D("reg-event: %d", reg_event);

    if (reg_event <= 0)
    {
        __create_reg_packet();

        cJSON* cj_register_dup = cJSON_CreateObjectReference(__FUNCTION__, cj_reg_data->child);
        if (cj_register_dup)
        {
            if (!ezlopi_core_broadcast_add_to_queue(cj_register_dup))
            {
                TRACE_E("Error adding to broadcast queue!");
                cJSON_Delete(__FUNCTION__, cj_register_dup);
            }
        }
    }
}

void ezpi_cloud_dummy()
{
    TRACE_S("I am just a dummy");
}