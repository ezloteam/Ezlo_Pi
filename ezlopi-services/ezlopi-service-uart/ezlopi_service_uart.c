#include "../../build/config/sdkconfig.h"

#ifdef CONFIG_EZPI_ENABLE_UART_PROVISIONING

#include "freertos/FreeRTOSConfig.h"

#include "cjext.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include "esp_idf_version.h"
#include "esp_netif_ip_addr.h"

#include "ezlopi_util_trace.h"
#include "ezlopi_util_version.h"

#include "ezlopi_core_nvs.h"
#include "ezlopi_core_net.h"
#include "ezlopi_core_wifi.h"
#include "ezlopi_core_info.h"
#include "ezlopi_core_reset.h"
#include "ezlopi_core_buffer.h"
#include "ezlopi_core_event_group.h"
#include "ezlopi_core_factory_info.h"
#include "ezlopi_core_cjson_macros.h"

#include "ezlopi_core_event_group.h"
#include "ezlopi_core_sntp.h"
#include "ezlopi_core_info.h"
#include "ezlopi_core_processes.h"
#include "ezlopi_core_ping.h"
#include "ezlopi_core_log.h"

#include "ezlopi_hal_uart.h"
#include "ezlopi_hal_system_info.h"

#include "ezlopi_cloud_info.h"
#include "ezlopi_cloud_constants.h"

#include "ezlopi_service_ble.h"
#include "ezlopi_service_uart.h"
#include "ezlopi_service_loop.h"
#include "EZLOPI_USER_CONFIG.h"


#if defined(CONFIG_IDF_TARGET_ESP32)
#define TXD_PIN (GPIO_NUM_1)
#define RXD_PIN (GPIO_NUM_3)
#elif defined(CONFIG_IDF_TARGET_ESP32S2)
#elif defined(CONFIG_IDF_TARGET_ESP32C3)
#define TXD_PIN (GPIO_NUM_21)
#define RXD_PIN (GPIO_NUM_20)
#elif defined(CONFIG_IDF_TARGET_ESP32S3)
#define TXD_PIN (GPIO_NUM_43)
#define RXD_PIN (GPIO_NUM_44)
#endif

static uint8_t __uart_data[EZPI_SERV_UART_RX_BUFFER_SIZE];

static void ezlopi_service_uart_get_info();
static void ezlopi_service_uart_set_wifi(const char* data);
static void ezlopi_service_uart_response(uint8_t cmd, uint8_t status_write, uint8_t status_connect);
static void ezlopi_service_uart_set_config(const char* data);
static void ezlopi_service_uart_get_config(void);

static int ezlopi_service_uart_reset(cJSON* root)
{
    int ret = 0;
    cJSON* cj_sub_cmd = cJSON_GetObjectItem(__FUNCTION__, root, ezlopi_sub_cmd_str);
    if (cj_sub_cmd)
    {
        uint8_t sub_cmd = cj_sub_cmd->valuedouble;
        switch (sub_cmd)
        {
        case 0:
        {
            TRACE_E("Factory restore command");
            const static char* reboot_response = "{\"cmd\":0, \"sub_cmd\":0,\"status\":1}";
            EZPI_SERV_uart_tx_data(strlen(reboot_response), (uint8_t*)reboot_response);
            EZPI_CORE_reset_factory_restore();
            break;
        }
        case 1:
        {
            TRACE_E("Reboot only command");
            const static char* reboot_response = "{\"cmd\":0, \"sub_cmd\":1, \"status\":1}";
            EZPI_SERV_uart_tx_data(strlen(reboot_response), (uint8_t*)reboot_response);
            EZPI_CORE_reset_reboot();
            break;
        }
        default:
        {
            break;
        }
        }
        cJSON_Delete(__FUNCTION__, cj_sub_cmd);
    }

    return ret;
}

static int ezlopi_service_uart_set_uart_config(const cJSON* root)
{
    int ret = 0;

    char parity[16];
    char flow_control[16];

    uint32_t baud = 0;
    uint32_t parity_val = EZPI_SERV_UART_PARITY_DEFAULT;
    uint32_t start_bits = EZPI_SERV_UART_START_BIT_DEFAULT;
    uint32_t stop_bits = EZPI_SERV_UART_STOP_BIT_DEFAULT;
    uint32_t frame_size = 0;
    uint32_t flow_control_val = EZPI_SERV_UART_FLOW_CTRL_DEFAULT;

    uint32_t baud_current = EZPI_SERV_UART_BAUD_DEFAULT;
    uint32_t parity_val_current = EZPI_SERV_UART_PARITY_DEFAULT;
    uint32_t start_bits_current = EZPI_SERV_UART_START_BIT_DEFAULT;
    uint32_t stop_bits_current = EZPI_SERV_UART_STOP_BIT_DEFAULT;
    uint32_t frame_size_current = EZPI_SERV_UART_FRAME_SIZE_DEFAULT;
    uint32_t flow_control_current = EZPI_SERV_UART_FLOW_CTRL_DEFAULT;

    bool flag_new_config = false;

    CJSON_GET_VALUE_DOUBLE(root, ezlopi_baud_str, baud);
    CJSON_GET_VALUE_STRING_BY_COPY(root, ezlopi_parity_str, parity);
    CJSON_GET_VALUE_DOUBLE(root, ezlopi_start_bits_str, start_bits);
    CJSON_GET_VALUE_DOUBLE(root, ezlopi_stop_bits_str, stop_bits);
    CJSON_GET_VALUE_DOUBLE(root, ezlopi_frame_size_str, frame_size);
    CJSON_GET_VALUE_STRING_BY_COPY(root, ezlopi_flow_control_str, flow_control);

    EZPI_CORE_nvs_read_baud(&baud_current);
    EZPI_CORE_nvs_read_parity(&parity_val_current);
    EZPI_CORE_nvs_read_start_bits(&start_bits_current);
    EZPI_CORE_nvs_read_stop_bits(&stop_bits_current);
    EZPI_CORE_nvs_read_frame_size(&frame_size_current);
    EZPI_CORE_nvs_read_flow_control(&flow_control_current);

#if 0
    TRACE_D("Obtained baud: %d", baud);
    TRACE_D("Obtained parity: %d", (uint32_t)EZPI_CORE_info_name_to_parity(parity));
    TRACE_D("Obtained start bits: %d", start_bits);
    TRACE_D("Obtained stop bits: %d", stop_bits);
    TRACE_D("Obtained frame size: %d", frame_size);
    TRACE_D("Obtained flow control: %d", (uint32_t)EZPI_CORE_info_get_flw_ctrl_from_name(flow_control));

    TRACE_D("Current baud: %d", baud_current);
    TRACE_D("Current parity: %d", parity_val_current);
    TRACE_D("Current start bits: %d", start_bits_current);
    TRACE_D("Current stop bits: %d", stop_bits_current);
    TRACE_D("Current frame size: %d", frame_size_current);
    TRACE_D("Current flow control: %d", flow_control_current);
#endif 

    if (
        (baud_current != baud) ||
        (parity_val_current != (uint32_t)EZPI_CORE_info_name_to_parity(parity)) ||
        (start_bits_current != start_bits) ||
        (stop_bits != stop_bits_current) ||
        (frame_size != frame_size_current) ||
        (flow_control_current != (uint32_t)EZPI_CORE_info_get_flw_ctrl_from_name(flow_control))
        )
    {
        flag_new_config = true;
    }

    if (flag_new_config)
    {
        if ('\0' != parity[0])
        {
            parity_val = (uint32_t)EZPI_CORE_info_name_to_parity(parity);
        }

        EZPI_CORE_nvs_write_parity(parity_val);

        if (baud)
        {
            EZPI_CORE_nvs_write_baud(baud);
        }
        else
        {
            baud = EZPI_SERV_UART_BAUD_DEFAULT;
            EZPI_CORE_nvs_write_baud(baud);
        }

        EZPI_CORE_nvs_write_start_bits(start_bits);
        EZPI_CORE_nvs_write_stop_bits(stop_bits);

        if (!frame_size)
        {
            frame_size = EZPI_SERV_UART_FRAME_SIZE_DEFAULT;
        }
        EZPI_CORE_nvs_write_frame_size(frame_size);


        if ('\0' != flow_control[0])
        {
            flow_control_val = (uint32_t)EZPI_CORE_info_get_flw_ctrl_from_name(flow_control);
            TRACE_W("New Flow control: %d", flow_control_val);
        }

        EZPI_CORE_nvs_write_flow_control(flow_control_val);


        const static char* reboot_response = "{\"cmd\":5, \"status\":1}";
        EZPI_SERV_uart_tx_data(strlen(reboot_response), (uint8_t*)reboot_response);

        TRACE_W("New config has been applied, device rebooting");
        vTaskDelay(10);
        EZPI_CORE_reset_reboot();
    }
    else
    {
        const static char* reboot_response = "{\"cmd\":5, \"status\":0}";
        EZPI_SERV_uart_tx_data(strlen(reboot_response), (uint8_t*)reboot_response);

        TRACE_W("Configuration unchanged !");
        vTaskDelay(10);
    }

    return ret;
}

static int ezlopi_service_uart_process_log_severity(const cJSON* root)
{
    int ret = 0;

    int target = 0;
    int severity = 0;

    CJSON_GET_VALUE_DOUBLE(root, "target", target);
    CJSON_GET_VALUE_DOUBLE(root, ezlopi_severity_str, severity);

#ifdef CONFIG_EZPI_UTIL_TRACE_EN
    if (0 == target)
    {
        // Call cloud log severity setter
        ret = ezlopi_core_cloud_log_severity_process_id(severity);
    }
    else if (1 == target)
    {
        // Call serial log severity setter
        ret = ezlopi_core_serial_log_severity_process_id(severity);
    }
#endif // CONFIG_EZPI_UTIL_TRACE_EN

    cJSON* cj_uart_response = cJSON_CreateObject(__func__);
    if (cj_uart_response)
    {
        cJSON_AddNumberToObject(__func__, cj_uart_response, ezlopi_cmd_str, 6);
        if (0 == ret)
        {
            cJSON_AddNumberToObject(__func__, cj_uart_response, ezlopi_status_str, 0);
        }
        else
        {
            cJSON_AddNumberToObject(__func__, cj_uart_response, ezlopi_status_str, 1);
        }

        const char* str_uart_response = cJSON_Print(__func__, cj_uart_response);
        if (str_uart_response)
        {
            cJSON_Minify((char *)str_uart_response);
            EZPI_SERV_uart_tx_data(strlen(str_uart_response), (uint8_t *)str_uart_response);
            free((char *)str_uart_response);
        }
        cJSON_Delete(__func__, cj_uart_response);

    }

    return ret;
}

static int ezlopi_service_uart_parser(const char* data)
{
    cJSON* root = cJSON_ParseWithRef(__FUNCTION__, data);

    if (root)
    {
        cJSON* cj_cmd = cJSON_GetObjectItem(__FUNCTION__, root, ezlopi_cmd_str);
        if (cj_cmd)
        {
            uint8_t cmd_temp = cj_cmd->valuedouble;

            switch (cmd_temp)
            {
            case 0:
            {
                ezlopi_service_uart_reset(root);
                break;
            }
            case 1:
            {
                ezlopi_service_uart_get_info();
                break;
            }
            case 2:
            {
                ezlopi_service_uart_set_wifi(data);
                break;
            }
            case 3:
            {
                ezlopi_service_uart_set_config(data);
                break;
            }
            case 4:
            {
                ezlopi_service_uart_get_config();
                break;
            }
            case 5:
            {
                ezlopi_service_uart_set_uart_config(root);
                break;
            }
            case 6:
            {
                ezlopi_service_uart_process_log_severity(root);
                break;
            }
            default:
            {
                TRACE_E("Invalid command!");
                break;
            }
            }
        }
        else
        {
            TRACE_E("'cmd' not found!");
        }

        cJSON_Delete(__FUNCTION__, root);
    }
    else
    {
        TRACE_E("Failed to parse json!");
    }

    return 1;
}

static void __uart_loop(void *arg)
{
    uint32_t cur_len = 0;
    static uint32_t __rx_len;

    uart_get_buffered_data_len(EZPI_SERV_UART_NUM_DEFAULT, &cur_len);

    if (cur_len && (cur_len > __rx_len))
    {
        memset(__uart_data, 0, EZPI_SERV_UART_RX_BUFFER_SIZE);

        int rxBytes = uart_read_bytes(EZPI_SERV_UART_NUM_DEFAULT, __uart_data, (EZPI_SERV_UART_RX_BUFFER_SIZE - 1), 1000 / portTICK_RATE_MS);

        if (rxBytes > (strlen(ezlopi_cmd_str) + 6))
        {
            __uart_data[rxBytes] = 0;
            TRACE_I("%s", __uart_data);
            ezlopi_service_uart_parser((const char*)__uart_data);
        }
    }
}

#if 0
static void ezlopi_service_uart_task(void* arg)
{
    static const char* RX_TASK_TAG = "RX_TASK";
    esp_log_level_set(RX_TASK_TAG, ESP_LOG_INFO);

    uint8_t* data = (uint8_t*)ezlopi_malloc(__FUNCTION__, EZPI_SERV_UART_RX_BUFFER_SIZE);
    memset(data, 0, EZPI_SERV_UART_RX_BUFFER_SIZE);

    while (1)
    {
        int rxBytes = uart_read_bytes(EZPI_SERV_UART_NUM_DEFAULT, data, (EZPI_SERV_UART_RX_BUFFER_SIZE - 1), 1000 / portTICK_RATE_MS);

        if (rxBytes > 0)
        {
            data[rxBytes] = 0;
            TRACE_I("%s", data);
            ezlopi_service_uart_parser((const char*)data);
        }
    }

    ezlopi_free(__FUNCTION__, data);
    ezlopi_core_process_set_is_deleted(ENUM___uart_loop);
    vTaskDelete(NULL);
}
#endif

static int ezlopi_service_uart_firmware_info(cJSON* parent)
{
    int ret = 0;
    static const char * _ezlopi_firmware_str = "ezlopi_firmware";
    cJSON* cj_firmware_info = cJSON_AddObjectToObjectWithRef(__FUNCTION__, parent, _ezlopi_firmware_str);

    if (cj_firmware_info)
    {
        char build_time[64];
        cJSON_AddStringToObjectWithRef(__FUNCTION__, cj_firmware_info, ezlopi_version_str, VERSION_STR);
        cJSON_AddNumberToObjectWithRef(__FUNCTION__, cj_firmware_info, ezlopi_build_str, BUILD);
        EZPI_CORE_sntp_epoch_to_iso8601(build_time, sizeof(build_time), (time_t)BUILD_DATE);
        cJSON_AddStringToObject(__FUNCTION__, cj_firmware_info, ezlopi_build_date_str, build_time);

        ret = 1;
    }
    return ret;
}

static int ezlopi_service_uart_chip_info(cJSON* parent)
{
    int ret = 0;
    cJSON* cj_chip = cJSON_AddObjectToObject(__FUNCTION__, parent, "chip");
    if (cj_chip)
    {
        esp_chip_info_t chip_info;
        char chip_revision[10];
        esp_chip_info(&chip_info);
        sprintf(chip_revision, "%.2f", (float)(chip_info.full_revision / 100.0));
        cJSON_AddStringToObject(__FUNCTION__, cj_chip, "type", EZPI_CORE_info_get_chip_type_to_name(chip_info.model));
        cJSON_AddStringToObject(__FUNCTION__, cj_chip, "version", chip_revision);
        ret = 1;
    }
    return ret;
}

static int ezlopi_service_uart_firmware_sdk_info(cJSON* parent)
{
    int ret = 0;
    cJSON* cj_firmware_sdk = cJSON_AddObjectToObject(__FUNCTION__, parent, "firmware_sdk");
    if (cj_firmware_sdk)
    {
        cJSON_AddStringToObject(__FUNCTION__, cj_firmware_sdk, "name", "ESP-IDF");
        cJSON_AddStringToObject(__FUNCTION__, cj_firmware_sdk, "version", esp_get_idf_version());
        ret = 1;
    }
    return ret;
}

static int ezlopi_service_uart_device_status_info(cJSON* parent)
{
    int ret = 0;
    cJSON* cj_device_state = cJSON_AddObjectToObject(__FUNCTION__, parent, "device_state");
    if (cj_device_state)
    {

        char time_string[50];
        uint32_t tick_count_ms = xTaskGetTickCount() / portTICK_PERIOD_MS;
        EZPI_CORE_info_get_tick_to_time_name(time_string, sizeof(time_string), tick_count_ms);

        cJSON_AddStringToObject(__FUNCTION__, cj_device_state, ezlopi_uptime_str, time_string);
        cJSON_AddNumberToObject(__FUNCTION__, cj_device_state, "boot_count", ezlopi_system_info_get_boot_count());
        cJSON_AddStringToObject(__FUNCTION__, cj_device_state, "boot_reason", EZPI_CORE_info_get_esp_reset_reason_to_name(esp_reset_reason()));

        cJSON_AddStringToObject(__FUNCTION__, cj_device_state, ezlopi_flash_size_str, CONFIG_ESPTOOLPY_FLASHSIZE);

        uint8_t mac[6];
        ezlopi_wifi_get_wifi_mac(mac);
        char mac_str[20];
        memset(mac_str, 0, sizeof(mac_str));
        snprintf(mac_str, 20, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
        cJSON_AddStringToObject(__FUNCTION__, cj_device_state, "wifi_mac", mac_str);

        memset(mac, 0, sizeof(mac));
        EZPI_CORE_info_get_ble_mac(mac);
        memset(mac_str, 0, sizeof(mac_str));
        snprintf(mac_str, 20, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
        cJSON_AddStringToObject(__FUNCTION__, cj_device_state, "ble_mac", mac_str);
        ret = 1;
    }
    return ret;
}

static int ezlopi_service_uart_config_info(cJSON* parent)
{
    int ret = 0;
    cJSON* cj_serial_config = cJSON_AddObjectToObject(__FUNCTION__, parent, "serial_config");
    if (cj_serial_config)
    {
        uint32_t baud = EZPI_SERV_UART_BAUD_DEFAULT;
        uint32_t parity_val = EZPI_SERV_UART_PARITY_DEFAULT;
        uint32_t start_bits = EZPI_SERV_UART_START_BIT_DEFAULT;
        uint32_t stop_bits = EZPI_SERV_UART_STOP_BIT_DEFAULT;
        uint32_t frame_size = EZPI_SERV_UART_FRAME_SIZE_DEFAULT;
        uint32_t flow_control = EZPI_SERV_UART_FLOW_CTRL_DEFAULT;
        char parity[2];

        char flw_ctrl_bffr[EZPI_UART_SERV_FLW_CTRL_STR_SIZE + 1];
        flw_ctrl_bffr[EZPI_UART_SERV_FLW_CTRL_STR_SIZE] = 0;

        EZPI_CORE_nvs_read_baud(&baud);
        cJSON_AddNumberToObject(__FUNCTION__, cj_serial_config, ezlopi_baud_str, baud);

        EZPI_CORE_nvs_read_parity((uint32_t*)&parity_val);
        parity[0] = EZPI_CORE_info_parity_to_name(parity_val);
        parity[1] = 0;
        cJSON_AddStringToObject(__FUNCTION__, cj_serial_config, ezlopi_parity_str, parity);

        EZPI_CORE_nvs_read_start_bits(&start_bits);
        cJSON_AddNumberToObject(__FUNCTION__, cj_serial_config, ezlopi_start_bits_str, start_bits);

        EZPI_CORE_nvs_read_stop_bits(&stop_bits);
        cJSON_AddNumberToObject(__FUNCTION__, cj_serial_config, ezlopi_stop_bits_str, stop_bits);

        EZPI_CORE_nvs_read_frame_size(&frame_size);
        cJSON_AddNumberToObject(__FUNCTION__, cj_serial_config, ezlopi_frame_size_str, frame_size);

        EZPI_CORE_nvs_read_flow_control(&flow_control);
        EZPI_CORE_info_get_flow_ctrl_to_name(flow_control, flw_ctrl_bffr);
        cJSON_AddStringToObject(__FUNCTION__, cj_serial_config, ezlopi_flow_control_str, flw_ctrl_bffr);

        ret = 1;
    }
    return ret;
}

static int ezlopi_service_uart_ezlopi_info(cJSON* parent)
{
    int ret = 0;
    cJSON* cj_ezlopi = cJSON_AddObjectToObject(__FUNCTION__, parent, "ezlopi_cloud");
    if (cj_ezlopi)
    {
        char* device_mac = ezlopi_factory_info_v3_get_ezlopi_mac();
        char* controller_uuid = ezlopi_factory_info_v3_get_device_uuid();
        char* provisioning_uuid = ezlopi_factory_info_v3_get_provisioning_uuid();
        unsigned long long serial_id = ezlopi_factory_info_v3_get_id();
        const char* device_type = ezlopi_factory_info_v3_get_device_type();

        cJSON_AddBoolToObject(__FUNCTION__, cj_ezlopi, ezlopi_provisioned_str, ezlopi_factory_info_v3_get_provisioning_status());
        cJSON_AddStringToObject(__FUNCTION__, cj_ezlopi, ezlopi_uuid_str, controller_uuid ? controller_uuid : "");
        cJSON_AddStringToObject(__FUNCTION__, cj_ezlopi, ezlopi_uuid_prov_str, provisioning_uuid ? provisioning_uuid : "");
        cJSON_AddStringToObject(__FUNCTION__, cj_ezlopi, ezlopi_type_str, device_type ? device_type : "");
        cJSON_AddNumberToObject(__FUNCTION__, cj_ezlopi, ezlopi_serial_str, serial_id);
        cJSON_AddStringToObject(__FUNCTION__, cj_ezlopi, ezlopi_mac_str, device_mac ? device_mac : "");


        ezlopi_factory_info_v3_free(device_mac);
        ezlopi_factory_info_v3_free(controller_uuid);
        ezlopi_factory_info_v3_free(provisioning_uuid);
        ret = 1;
    }
    return ret;
}

static int ezlopi_service_uart_oem_info(cJSON* parent)
{
    int ret = 0;
    cJSON* cj_oem = cJSON_AddObjectToObject(__FUNCTION__, parent, "oem");
    if (cj_oem)
    {
        char* device_model = ezlopi_factory_info_v3_get_model();
        char* device_brand = ezlopi_factory_info_v3_get_brand();
        char* device_manufacturer = ezlopi_factory_info_v3_get_manufacturer();

        cJSON_AddStringToObject(__FUNCTION__, cj_oem, ezlopi_brand_str, device_brand ? device_brand : "");
        cJSON_AddStringToObject(__FUNCTION__, cj_oem, ezlopi_manufacturer_str, device_manufacturer ? device_manufacturer : "");
        cJSON_AddStringToObject(__FUNCTION__, cj_oem, ezlopi_model_str, device_model ? device_model : "");

        ezlopi_factory_info_v3_free(device_model);
        ezlopi_factory_info_v3_free(device_manufacturer);
        ezlopi_factory_info_v3_free(device_brand);
        ret = 0;
    }
    return ret;
}

static int ezlopi_service_uart_newtwork_info(cJSON* parent)
{
    int ret = 0;
    cJSON* cj_network = cJSON_AddObjectToObject(__FUNCTION__, parent, "network");
    if (cj_network)
    {
        char* wifi_ssid = ezlopi_factory_info_v3_get_ssid();
        cJSON_AddStringToObject(__FUNCTION__, cj_network, ezlopi_ssid_str, wifi_ssid ? wifi_ssid : "");
        ezlopi_factory_info_v3_free(wifi_ssid);

        ezlopi_wifi_status_t* wifi_status = ezlopi_wifi_status();
        if (wifi_status)
        {
            char* wifi_mode = EZPI_CORE_info_get_wifi_mode_to_name(wifi_status->wifi_mode);
            cJSON_AddStringToObject(__FUNCTION__, cj_network, "wifi_mode", wifi_mode ? wifi_mode : "");

            char ip_str[20];
            memset(ip_str, 0, 20);
            snprintf(ip_str, 20, IPSTR, IP2STR(&wifi_status->ip_info->ip));
            cJSON_AddStringToObject(__FUNCTION__, cj_network, "ip_sta", ip_str);

            memset(ip_str, 0, 20);
            snprintf(ip_str, 20, IPSTR, IP2STR(&wifi_status->ip_info->netmask));
            cJSON_AddStringToObject(__FUNCTION__, cj_network, "ip_nmask", ip_str);

            memset(ip_str, 0, 20);
            snprintf(ip_str, 20, IPSTR, IP2STR(&wifi_status->ip_info->gw));
            cJSON_AddStringToObject(__FUNCTION__, cj_network, "ip_gw", ip_str);

            cJSON_AddBoolToObject(__FUNCTION__, cj_network, "wifi", wifi_status->wifi_connection);

            e_ezlopi_event_t events = ezlopi_get_event_bit_status();
            bool cloud_connection_status = (EZLOPI_EVENT_NMA_REG & events) == EZLOPI_EVENT_NMA_REG;

#ifdef CONFIG_EZPI_ENABLE_PING
            e_ping_status_t ping_status = ezlopi_ping_get_internet_status();
            cJSON_AddBoolToObject(__FUNCTION__, cj_network, ezlopi_internet_str, ping_status == EZLOPI_PING_STATUS_LIVE);
#else // CONFIG_EZPI_ENABLE_PING
            cJSON_AddBoolToObject(__FUNCTION__, cj_network, ezlopi_internet_str, cloud_connection_status);
#endif // CONFIG_EZPI_ENABLE_PING

            cJSON_AddBoolToObject(__FUNCTION__, cj_network, "cloud", cloud_connection_status);

            ezlopi_free(__FUNCTION__, wifi_status);
        }
        ret = 1;
    }
    return ret;
}

static void ezlopi_service_uart_get_info()
{
    cJSON* cj_get_info = cJSON_CreateObject(__FUNCTION__);
    if (cj_get_info)
    {
        cJSON_AddNumberToObject(__FUNCTION__, cj_get_info, "cmd", 1);
        cJSON_AddStringToObject(__FUNCTION__, cj_get_info, "api", EZPI_VERSION_API_SERIAL);
        cJSON* cj_info = cJSON_CreateObject(__FUNCTION__);
        if (cj_info)
        {
            ezlopi_service_uart_firmware_info(cj_info);
            ezlopi_service_uart_chip_info(cj_info);
            ezlopi_service_uart_firmware_sdk_info(cj_info);
            ezlopi_service_uart_device_status_info(cj_info);
            ezlopi_service_uart_config_info(cj_info);
            ezlopi_service_uart_ezlopi_info(cj_info);
            ezlopi_service_uart_oem_info(cj_info);
            ezlopi_service_uart_newtwork_info(cj_info);

            cJSON_AddItemToObject(__FUNCTION__, cj_get_info, ezlopi_info_str, cj_info);

            char* serial_data_json_string = cJSON_Print(__FUNCTION__, cj_get_info);
            if (serial_data_json_string)
            {
                cJSON_Minify(serial_data_json_string);
                EZPI_SERV_uart_tx_data(strlen(serial_data_json_string), (uint8_t*)serial_data_json_string);
                ezlopi_free(__FUNCTION__, serial_data_json_string);
            }

            cJSON_Delete(__FUNCTION__, cj_get_info);
        }

    }
}

static void ezlopi_service_uart_set_wifi(const char* data)
{
    uint32_t status = 0;
    cJSON* root = cJSON_Parse(__FUNCTION__, data);
    uint8_t status_write = 0;

    if (root)
    {
        cJSON* cj_wifi_ssid = cJSON_GetObjectItem(__FUNCTION__, root, ezlopi_ssid_str);
        cJSON* cj_wifi_pass = cJSON_GetObjectItem(__FUNCTION__, root, ezlopi_pass_str);
        if (cj_wifi_ssid && cj_wifi_pass && cj_wifi_ssid->valuestring && cj_wifi_pass->valuestring)
        {
            char* ssid = cj_wifi_ssid->valuestring;
            char* pass = cj_wifi_pass->valuestring;

            if (strlen(pass) >= EZPI_CORE_WIFI_PASS_CHAR_MIN_LEN)
            {
                // TRACE_S("SSID: %s\tPass : %s\r\n", ssid, pass);
                if (ezlopi_factory_info_v3_set_wifi(ssid, pass))
                {
                    status_write = 1;
                }
                // ezlopi_wifi_set_new_wifi_flag();
                uint8_t attempt = 1;
                while (attempt <= EZPI_CORE_WIFI_CONN_RETRY_ATTEMPT)
                {
                    ezlopi_wifi_connect((const char*)ssid, (const char*)pass);
                    ezlopi_wait_for_wifi_to_connect((uint32_t)EZPI_CORE_WIFI_CONN_ATTEMPT_INTERVAL);
                    s_ezlopi_net_status_t* net_stat = ezlopi_get_net_status();
                    if (net_stat)
                    {
                        if (net_stat->wifi_status->wifi_connection)
                        {
                            status = 1;
                            break;
                        }
                        else
                        {
                            TRACE_E("WiFi Connection to AP: %s failed !", ssid);
                            // printf("WiFi Connection to AP: %s failed !\r\n", ssid);
                            status = 0;
                        }
                    }
                    TRACE_W("Trying to connect to AP : %s, attempt %d ....", ssid, attempt);
                    // printf("Trying to connect to AP : %s, attempt %d ....\r\n", ssid, attempt);
                    attempt++;
                    // vTaskDelay(EZLOPI_WIFI_CONNECT_ATTEMPT_INTERVAL / portTICK_PERIOD_MS);
                }

                ezlopi_service_uart_response(2, status_write, status);
            }
            else
            {
                TRACE_E("Invalid WiFi SSID or Password, aborting!");
                // printf("Invalid WiFi SSID or Password, aborting!\r\n");
                ezlopi_service_uart_response(2, 0, 0);
            }
        }

        cJSON_Delete(__FUNCTION__, root); // free Json string
    }
}

static void ezlopi_service_uart_response(uint8_t cmd, uint8_t status_write, uint8_t status_connect)
{
    cJSON* response = NULL;
    response = cJSON_CreateObject(__FUNCTION__);

    if (response)
    {
        cJSON_AddNumberToObject(__FUNCTION__, response, ezlopi_cmd_str, cmd);
        cJSON_AddNumberToObject(__FUNCTION__, response, ezlopi_status_write_str, status_write);
        cJSON_AddNumberToObject(__FUNCTION__, response, "status_connect", status_connect);

        char* my_json_string = cJSON_Print(__FUNCTION__, response);
        cJSON_Delete(__FUNCTION__, response); // free Json string

        if (my_json_string)
        {
            cJSON_Minify(my_json_string);
            EZPI_SERV_uart_tx_data(strlen(my_json_string), (uint8_t*)my_json_string);
            ezlopi_free(__FUNCTION__, my_json_string);
        }
    }
}

static void ezlopi_service_uart_set_config(const char* data)
{

    uint8_t ret = ezlopi_nvs_write_config_data_str((char*)data);
    TRACE_I("ezlopi_factory_info_set_ezlopi_config: %d", ret);
    if (ret)
    {
        TRACE_I("Successfully wrote config data..");
    }

    ezlopi_service_uart_response(3, ret, 5);
}

static void ezlopi_service_uart_get_config(void)
{
    cJSON* root = NULL;
    char* current_config = ezlopi_factory_info_v3_get_ezlopi_config();

    if (current_config)
    {
        TRACE_D("current_config[len: %d]: %s", strlen(current_config), current_config);
        root = cJSON_Parse(__FUNCTION__, current_config);

        if (root)
        {
            cJSON_DeleteItemFromObject(__FUNCTION__, root, "cmd");
            cJSON_AddNumberToObject(__FUNCTION__, root, "cmd", 4);
        }
        else
        {
            TRACE_E("'root' is null!");
        }
    }
    else
    {
        TRACE_E("'current_config' is null!");
    }

    if (NULL == root)
    {
        TRACE_E("Reading config failed!");
        root = cJSON_CreateObject(__FUNCTION__);
        if (root)
        {
            cJSON_AddNumberToObject(__FUNCTION__, root, "cmd", 4);
            TRACE_D("'root'");
        }
        else
        {
            TRACE_E("Failed to create 'root'!");
        }
    }

    if (root)
    {
        char* my_json_string = cJSON_Print(__FUNCTION__, root);
        TRACE_D("length of 'my_json_string': %d", strlen(my_json_string));

        if (my_json_string)
        {
            cJSON_Minify(my_json_string);
            cJSON_Delete(__FUNCTION__, root); // free Json string
            const int len = strlen(my_json_string);
            EZPI_SERV_uart_tx_data(len, (uint8_t*)my_json_string); // Send the data over uart
            // TRACE_D("Sending: %s", my_json_string);
            ezlopi_free(__FUNCTION__, my_json_string);
        }
    }
}

int EZPI_SERV_uart_tx_data(int len, uint8_t* data)
{
    int ret = 0;
    ret = uart_write_bytes(EZPI_SERV_UART_NUM_DEFAULT, (void *)data, len);
    ret += uart_write_bytes(EZPI_SERV_UART_NUM_DEFAULT, "\r\n", 2);
    return ret;
}

void EZPI_SERV_uart_init(void)
{
    ezlopi_service_loop_add("uart-loop", __uart_loop, 1, NULL);
#if 0
    TaskHandle_t __uart_loop_handle = NULL;
    xTaskCreate(__uart_loop, "serv_uart_task", __uart_loop_DEPTH, NULL, configMAX_PRIORITIES, &__uart_loop_handle);
    ezlopi_core_process_set_process_info(ENUM___uart_loop, &__uart_loop_handle, __uart_loop_DEPTH);
#endif
}

#endif // CONFIG_EZPI_ENABLE_UART_PROVISIONING