/* UART asynchronous example, that uses separate RX and TX tasks

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "freertos/FreeRTOSConfig.h"
#include "cJSON.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include "esp_idf_version.h"
#include "ezlopi_util_version.h"
#include "sdkconfig.h"
#include "esp_netif_ip_addr.h"

#include "ezlopi_util_trace.h"

#include "ezlopi_core_nvs.h"
#include "ezlopi_core_wifi.h"
#include "ezlopi_core_reset.h"
#include "ezlopi_core_net.h"
#include "ezlopi_core_factory_info.h"
#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_core_event_group.h"

#include "ezlopi_hal_system_info.h"

#include "ezlopi_cloud_info.h"
#include "ezlopi_cloud_constants.h"

#include "ezlopi_service_ble.h"

#include "ezlopi_service_uart.h"

static const int RX_BUF_SIZE = 3096;
static const uint8_t EZPI_UART_FLW_CTRL_STR_MAX = 10;

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

// cJson Types

static void ezlopi_service_uart_get_info();
static void ezlopi_service_uart_set_wifi(const char* data);
static void ezlopi_service_uart_response(uint8_t cmd, uint8_t status_write, uint8_t status_connect);
static void ezlopi_service_uart_save_config(const char* data);
static void ezlopi_service_uart_read_config(void);

int EZPI_SERVICE_uart_tx_data(int len, uint8_t* data)
{
    int ret = 0;
    // char start_bytes[] = {0x80, '\r', '\n'};
    // ret += uart_write_bytes(UART_NUM_0, start_bytes, sizeof(start_bytes));
    ret = uart_write_bytes(UART_NUM_0, data, len);
    ret += uart_write_bytes(UART_NUM_0, "\r\n", 2);

    return ret;
}

static uart_parity_t set_parity(const char* parity)
{
    uart_parity_t parity_val = UART_PARITY_DISABLE;

    if (strncmp("N", parity, 1) == 0)
    {
        parity_val = UART_PARITY_DISABLE;
    }
    else if (strncmp("P", parity, 1) == 0)
    {
        parity_val = UART_PARITY_ODD;
    }
    else if (strncmp("E", parity, 1) == 0)
    {
        parity_val = UART_PARITY_EVEN;
    }
    return parity_val;
}

static char get_parity(const uart_parity_t parity_val)
{
    char parity = 'N';
    switch (parity_val)
    {
    case UART_PARITY_ODD:
    {
        parity = 'P';
        break;
    }
    case UART_PARITY_EVEN:
    {
        parity = 'E';
        break;
    }
    case UART_PARITY_DISABLE:
    default:
    {
        parity = 'N';
        break;
    }
    }
    return parity;
}

static void get_flow_ctrl_to_str(uart_hw_flowcontrol_t flw_ctrl, char* flw_ctrl_str)
{
    switch (flw_ctrl)
    {
    case UART_HW_FLOWCTRL_DISABLE:
        strncpy(flw_ctrl_str, "DISABLED", EZPI_UART_FLW_CTRL_STR_MAX);
        break;
    case UART_HW_FLOWCTRL_RTS:
        strncpy(flw_ctrl_str, "RTS", EZPI_UART_FLW_CTRL_STR_MAX);
        break;
    case UART_HW_FLOWCTRL_CTS:
        strncpy(flw_ctrl_str, "CTS", EZPI_UART_FLW_CTRL_STR_MAX);
        break;
    case UART_HW_FLOWCTRL_CTS_RTS:
        strncpy(flw_ctrl_str, "CTS_RTS", EZPI_UART_FLW_CTRL_STR_MAX);
        break;
    default:
        strncpy(flw_ctrl_str, "DISABLED", EZPI_UART_FLW_CTRL_STR_MAX);
        break;
    }
}

static uart_hw_flowcontrol_t get_flow_ctrl_frm_str(char* flw_ctrl_str)
{
    uart_hw_flowcontrol_t flw_ctrl = UART_HW_FLOWCTRL_DISABLE;
    if (strncmp(flw_ctrl_str, "DISABLED", EZPI_UART_FLW_CTRL_STR_MAX) == 0)
    {
        flw_ctrl = UART_HW_FLOWCTRL_DISABLE;
    }
    else if (strncmp(flw_ctrl_str, "RTS", EZPI_UART_FLW_CTRL_STR_MAX) == 0)
    {
        flw_ctrl = UART_HW_FLOWCTRL_RTS;
    }
    else if (strncmp(flw_ctrl_str, "CTS", EZPI_UART_FLW_CTRL_STR_MAX) == 0)
    {
        flw_ctrl = UART_HW_FLOWCTRL_CTS;
    }
    else if (strncmp(flw_ctrl_str, "CTS_RTS", EZPI_UART_FLW_CTRL_STR_MAX) == 0)
    {
        flw_ctrl = UART_HW_FLOWCTRL_CTS_RTS;
    }
    else
    {
        flw_ctrl = UART_HW_FLOWCTRL_DISABLE;
    }
    return flw_ctrl;
}

static uart_word_length_t get_frame_size(const uint8_t frame_size)
{
    uart_word_length_t frame_size_val = UART_DATA_8_BITS;
    if (5 == frame_size)
    {
        frame_size_val = UART_DATA_5_BITS;
    }
    else if (6 == frame_size)
    {
        frame_size_val = UART_DATA_6_BITS;
    }
    else if (7 == frame_size)
    {
        frame_size_val = UART_DATA_7_BITS;
    }
    return frame_size_val;
}

static void serial_init(void)
{
    uint32_t baud = 0;
    uint8_t start_bits = 0;
    uint8_t stop_bits = 0;
    uint8_t frame_size = 0;
    uint32_t flow_control = 0;
    uint32_t parity_val = (uint32_t)UART_PARITY_DISABLE;

    EZPI_CORE_nvs_read_baud(&baud);
    EZPI_CORE_nvs_read_parity((uint32_t*)&parity_val);
    EZPI_CORE_nvs_read_start_bits(&start_bits);
    EZPI_CORE_nvs_read_stop_bits(&stop_bits);
    EZPI_CORE_nvs_read_frame_size(&frame_size);
    EZPI_CORE_nvs_read_flow_control(&flow_control);

    uart_word_length_t frame_size_val = get_frame_size(frame_size);

    const uart_config_t uart_config = {
        .baud_rate = baud,
        .data_bits = frame_size_val,
        .parity = (uart_parity_t)parity_val,
        .stop_bits = stop_bits,
        .flow_ctrl = (uart_hw_flowcontrol_t)flow_control,
        .source_clk = UART_SCLK_APB,
    };

    // We won't use a buffer for sending data.
    uart_driver_install(UART_NUM_0, RX_BUF_SIZE * 2, 0, 0, NULL, 0);
    uart_param_config(UART_NUM_0, &uart_config);
    uart_set_pin(UART_NUM_0, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
}

static char* ezlopi_esp_reset_reason_str(esp_reset_reason_t reason)
{
    switch (reason)
    {
    case ESP_RST_UNKNOWN:
        return "unknown";
        break;
    case ESP_RST_POWERON:
        return "Power ON";
        break;
    case ESP_RST_EXT:
        return "External pin";
        break;
    case ESP_RST_SW:
        return "Software Reset via esp_restart";
        break;
    case ESP_RST_PANIC:
        return "Software reset due to exception/panic";
        break;
    case ESP_RST_INT_WDT:
        return "Interrupt watchdog";
        break;
    case ESP_RST_TASK_WDT:
        return "Task watchdog";
        break;
    case ESP_RST_WDT:
        return "Other watchdogs";
        break;
    case ESP_RST_DEEPSLEEP:
        return "Reset after exiting deep sleep mode";
        break;
    case ESP_RST_BROWNOUT:
        return "Brownout reset";
        break;
    case ESP_RST_SDIO:
        return "Reset over SDIO";
        break;
    default:
        return "unknown";
        break;
    }
}

static char* ezlopi_chip_type_str(int chip_type)
{
    switch (chip_type)
    {
    case CHIP_ESP32:
        return "ESP32";
        break;
    case CHIP_ESP32S2:
        return "ESP32-S2";
        break;
    case CHIP_ESP32S3:
        return "ESP32-S3";
        break;
    case CHIP_ESP32C3:
        return "ESP32-C3";
        break;
    case CHIP_ESP32H2:
        return "ESP32-H2";
        break;
    default:
        return "UNKNOWN";
        break;
    }
}

static int ezlopi_service_uart_execute_command_0(cJSON* root)
{
    int ret = 0;
    cJSON* cj_sub_cmd = cJSON_GetObjectItem(root, ezlopi_sub_cmd_str);
    if (cj_sub_cmd)
    { 
        uint8_t sub_cmd = cj_sub_cmd->valuedouble;
        switch (sub_cmd)
        {
        case 0:
        {
            TRACE_E("Factory restore command");
            const static char* reboot_response = "{\"cmd\":0, \"sub_cmd\":0,\"status\":1}";
            EZPI_SERVICE_uart_tx_data(strlen(reboot_response), (uint8_t*)reboot_response);
            EZPI_CORE_factory_restore();
            break;
        }
        case 1:
        {
            TRACE_E("Reboot only command");
            const static char* reboot_response = "{\"cmd\":0, \"sub_cmd\":1, \"status\":1}";
            EZPI_SERVICE_uart_tx_data(strlen(reboot_response), (uint8_t*)reboot_response);
            EZPI_CORE_reboot();
            break;
        }
        default:
        {
            break;
        }
        }
        cJSON_Delete(cj_sub_cmd);
    }

    return ret;
}

static int ezlopi_service_uart_read_uart_params(const cJSON* root)
{
    int ret = 0;

    uint32_t baud = 0;
    char* parity = NULL;
    uint8_t start_bits = 0;
    uint8_t stop_bits = 0;
    uint16_t frame_size = 0;
    char* flow_control = NULL;

    CJSON_GET_VALUE_DOUBLE(root, ezlopi_baud_str, baud);
    CJSON_GET_VALUE_STRING(root, ezlopi_parity_str, parity);
    CJSON_GET_VALUE_DOUBLE(root, ezlopi_start_bits_str, start_bits);
    CJSON_GET_VALUE_DOUBLE(root, ezlopi_stop_bits_str, stop_bits);
    CJSON_GET_VALUE_DOUBLE(root, ezlopi_frame_size_str, frame_size);
    CJSON_GET_VALUE_STRING(root, ezlopi_flow_control_str, flow_control);

    uart_parity_t parity_val = set_parity(parity);

    EZPI_CORE_nvs_write_baud(baud);
    EZPI_CORE_nvs_write_parity((uint8_t)parity_val);
    EZPI_CORE_nvs_write_start_bits(start_bits);
    EZPI_CORE_nvs_write_stop_bits(stop_bits);
    EZPI_CORE_nvs_write_frame_size((uint8_t)frame_size);
    EZPI_CORE_nvs_write_flow_control(get_flow_ctrl_frm_str(flow_control));

    const static char* reboot_response = "{\"cmd\":5, \"status\":1}";
    EZPI_SERVICE_uart_tx_data(strlen(reboot_response), (uint8_t*)reboot_response);

    return ret;
}

static int qt_serial_parse_rx_data(const char* data)
{
    cJSON* root = cJSON_Parse(data);

    if (root)
    {
        cJSON* cj_cmd = cJSON_GetObjectItem(root, ezlopi_cmd_str);
        if (cj_cmd)
        {
            uint8_t cmd_temp = cj_cmd->valuedouble;

            switch (cmd_temp)
            {
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
                ezlopi_service_uart_save_config(data);
                break;
            }
            case 4:
            {
                ezlopi_service_uart_read_config();
                break;
            }
            case 0:
            {
                ezlopi_service_uart_execute_command_0(root);
                break;
            }
            case 5:
            {
                ezlopi_service_uart_read_uart_params(root);
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

        cJSON_Delete(root);
    }
    else
    {
        TRACE_E("Failed to parse json!");
    }

    return 1;
}

static void ezlopi_service_uart_rx_task(void* arg)
{
    static const char* RX_TASK_TAG = "RX_TASK";
    esp_log_level_set(RX_TASK_TAG, ESP_LOG_INFO);
    uint8_t* data = (uint8_t*)malloc(RX_BUF_SIZE + 1);
    memset(data, 0, RX_BUF_SIZE + 1);

    while (1)
    {
        int rxBytes = uart_read_bytes(UART_NUM_0, data, RX_BUF_SIZE, 1000 / portTICK_RATE_MS);

        if (rxBytes > 0)
        {
            data[rxBytes] = 0;
            TRACE_I("%s", data);
            qt_serial_parse_rx_data((const char*)data);
        }
    }

    free(data);
    vTaskDelete(NULL);
}

static int get_ezlopi_firmware_info(cJSON* parent)
{
    int ret = 0;
    cJSON* cj_firmware_info = cJSON_AddObjectToObject(parent, "ezlopi_firmware");
    if (cj_firmware_info)
    {
        cJSON_AddStringToObject(cj_firmware_info, ezlopi_version_str, VERSION_STR);
        cJSON_AddNumberToObject(cj_firmware_info, ezlopi_build_str, BUILD);
        cJSON_AddNumberToObject(cj_firmware_info, ezlopi_build_date_str, BUILD_DATE);
        ret = 1;
    }
    return ret;
}

static int get_ezlopi_chip_info(cJSON* parent)
{
    int ret = 0;
    cJSON* cj_chip = cJSON_AddObjectToObject(parent, "chip");
    if (cj_chip)
    {
        esp_chip_info_t chip_info;
        char chip_revision[10];
        esp_chip_info(&chip_info);
        sprintf(chip_revision, "%.2f", (float)(chip_info.full_revision / 100.0));
        cJSON_AddStringToObject(cj_chip, "type", ezlopi_chip_type_str(chip_info.model));
        cJSON_AddStringToObject(cj_chip, "version", chip_revision);
        ret = 1;
    }
    return ret;
}

static int get_firmware_sdk_info(cJSON* parent)
{
    int ret = 0;
    cJSON* cj_firmware_sdk = cJSON_AddObjectToObject(parent, "firmware_sdk");
    if (cj_firmware_sdk)
    {
        cJSON_AddStringToObject(cj_firmware_sdk, "name", "ESP-IDF");
        cJSON_AddStringToObject(cj_firmware_sdk, "version", esp_get_idf_version());
        ret = 1;
    }
    return ret;
}

static int get_device_status(cJSON* parent)
{
    int ret = 0;
    cJSON* cj_device_state = cJSON_AddObjectToObject(parent, "device_state");
    if (cj_device_state)
    {

        char time_string[50];
        uint32_t tick_count_ms = xTaskGetTickCount() / portTICK_PERIOD_MS;
        ezlopi_tick_to_time(time_string, sizeof(time_string), tick_count_ms);

        cJSON_AddStringToObject(cj_device_state, ezlopi_uptime_str, time_string);
        cJSON_AddNumberToObject(cj_device_state, "boot_count", ezlopi_system_info_get_boot_count());
        cJSON_AddStringToObject(cj_device_state, "boot_reason", ezlopi_esp_reset_reason_str(esp_reset_reason()));

        cJSON_AddStringToObject(cj_device_state, ezlopi_flash_size_str, CONFIG_ESPTOOLPY_FLASHSIZE);

        uint8_t mac[6];
        ezlopi_wifi_get_wifi_mac(mac);
        char mac_str[20];
        memset(mac_str, 0, sizeof(mac_str));
        snprintf(mac_str, 20, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
        cJSON_AddStringToObject(cj_device_state, "wifi_mac", mac_str);

        memset(mac, 0, sizeof(mac));
        ezlopi_ble_service_get_ble_mac(mac);
        memset(mac_str, 0, sizeof(mac_str));
        snprintf(mac_str, 20, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
        cJSON_AddStringToObject(cj_device_state, "ble_mac", mac_str);
        ret = 1;
    }
    return ret;
}

static int get_device_serial_protocol_info(cJSON* parent)
{
    int ret = 0;
    cJSON* cj_serial_config = cJSON_AddObjectToObject(parent, "serial_config");
    if (cj_serial_config)
    {
        uint32_t baud = 0;
        uint8_t start_bits = 0;
        uint8_t stop_bits = 0;
        uint8_t frame_size = 0;
        uint32_t flow_control = 0;
        uart_parity_t parity_val = UART_PARITY_DISABLE;
        char flw_ctrl_bffr[EZPI_UART_FLW_CTRL_STR_MAX + 1];
        flw_ctrl_bffr[EZPI_UART_FLW_CTRL_STR_MAX] = 0;

        EZPI_CORE_nvs_read_baud(&baud);
        cJSON_AddNumberToObject(cj_serial_config, ezlopi_baud_str, baud);

        EZPI_CORE_nvs_read_parity((uint8_t*)&parity_val);
        char parity = get_parity(parity_val);
        cJSON_AddStringToObject(cj_serial_config, "parity", &parity);

        EZPI_CORE_nvs_read_start_bits(&start_bits);
        cJSON_AddNumberToObject(cj_serial_config, "start_bits", start_bits);

        EZPI_CORE_nvs_read_stop_bits(&stop_bits);
        cJSON_AddNumberToObject(cj_serial_config, "stop_bits", stop_bits);

        EZPI_CORE_nvs_read_frame_size(&frame_size);
        cJSON_AddNumberToObject(cj_serial_config, "data_bits", frame_size);

        EZPI_CORE_nvs_read_flow_control(&flow_control);
        get_flow_ctrl_to_str(flow_control, flw_ctrl_bffr);
        cJSON_AddStringToObject(cj_serial_config, "flow_control", flw_ctrl_bffr);


        ret = 1;
    }
    return ret;
}

static int get_ezlopi_device_ezlopi_info(cJSON* parent)
{
    int ret = 0;
    cJSON* cj_ezlopi = cJSON_AddObjectToObject(parent, "ezlopi_cloud");
    if (cj_ezlopi)
    {
        char* device_mac = ezlopi_factory_info_v3_get_ezlopi_mac();
        char* controller_uuid = ezlopi_factory_info_v3_get_device_uuid();
        char* provisioning_uuid = ezlopi_factory_info_v3_get_provisioning_uuid();
        unsigned long long serial_id = ezlopi_factory_info_v3_get_id();
        const char* device_type = ezlopi_factory_info_v3_get_device_type();

        cJSON_AddStringToObject(cj_ezlopi, ezlopi_uuid_str, controller_uuid ? controller_uuid : "");
        cJSON_AddStringToObject(cj_ezlopi, ezlopi_uuid_prov_str, provisioning_uuid ? provisioning_uuid : "");
        cJSON_AddStringToObject(cj_ezlopi, ezlopi_type_str, device_type ? device_type : "");
        cJSON_AddNumberToObject(cj_ezlopi, ezlopi_serial_str, serial_id);
        cJSON_AddStringToObject(cj_ezlopi, ezlopi_mac_str, device_mac ? device_mac : "");


        ezlopi_factory_info_v3_free(device_mac);
        ezlopi_factory_info_v3_free(controller_uuid);
        ezlopi_factory_info_v3_free(provisioning_uuid);
        ret = 1;
    }
    return ret;
}

static int get_ezlopi_device_oem_info(cJSON* parent)
{
    int ret = 0;
    cJSON* cj_oem = cJSON_AddObjectToObject(parent, "oem");
    if (cj_oem)
    {
        char* device_model = ezlopi_factory_info_v3_get_model();
        char* device_brand = ezlopi_factory_info_v3_get_brand();
        char* device_manufacturer = ezlopi_factory_info_v3_get_manufacturer();

        cJSON_AddStringToObject(cj_oem, ezlopi_brand_str, device_brand ? device_brand : "");
        cJSON_AddStringToObject(cj_oem, ezlopi_manufacturer_str, device_manufacturer ? device_manufacturer : "");
        cJSON_AddStringToObject(cj_oem, ezlopi_model_str, device_model ? device_model : "");

        ezlopi_factory_info_v3_free(device_model);
        ezlopi_factory_info_v3_free(device_manufacturer);
        ezlopi_factory_info_v3_free(device_brand);
        ret = 0;
    }
    return ret;
}

static char* get_wifi_mode(const wifi_mode_t mode_val)
{
    char* mode = NULL;
    switch (mode_val)
    {
    case WIFI_MODE_STA:
    {
        mode = "STA";
        break;
    }
    case WIFI_MODE_AP:
    {
        mode = "AP";
        break;
    }
    case WIFI_MODE_APSTA:
    {
        mode = "APSTA";
        break;
    }
    default:
    {
        break;
    }
    }
    return mode;
}

static int get_ezlopi_device_newtwork_info(cJSON* parent)
{
    int ret = 0;
    cJSON* cj_network = cJSON_AddObjectToObject(parent, "network");
    if (cj_network)
    {
        char* wifi_ssid = ezlopi_factory_info_v3_get_ssid();
        cJSON_AddStringToObject(cj_network, ezlopi_ssid_str, wifi_ssid ? wifi_ssid : "");
        ezlopi_factory_info_v3_free(wifi_ssid);

        ezlopi_wifi_status_t* wifi_status = ezlopi_wifi_status();
        if (wifi_status)
        {
            char* wifi_mode = get_wifi_mode(wifi_status->wifi_mode);
            cJSON_AddStringToObject(cj_network, "wifi_mode", wifi_mode ? wifi_mode : "");

            char ip_str[20];
            memset(ip_str, 0, 20);
            snprintf(ip_str, 20, IPSTR, IP2STR(&wifi_status->ip_info->ip));
            cJSON_AddStringToObject(cj_network, "ip_sta", ip_str);

            memset(ip_str, 0, 20);
            snprintf(ip_str, 20, IPSTR, IP2STR(&wifi_status->ip_info->netmask));
            cJSON_AddStringToObject(cj_network, "ip_nmask", ip_str);

            memset(ip_str, 0, 20);
            snprintf(ip_str, 20, IPSTR, IP2STR(&wifi_status->ip_info->gw));
            cJSON_AddStringToObject(cj_network, "ip_gw", ip_str);

            cJSON_AddBoolToObject(cj_network, "wifi", wifi_status->wifi_connection);

            e_ping_status_t ping_status = ezlopi_ping_get_internet_status();
            cJSON_AddBoolToObject(cj_network, "internet", ping_status == EZLOPI_PING_STATUS_LIVE);

            e_ezlopi_event_t events = ezlopi_get_event_bit_status();
            bool cloud_connection_status = (EZLOPI_EVENT_NMA_REG & events) == EZLOPI_EVENT_NMA_REG;
            cJSON_AddBoolToObject(cj_network, "cloud", cloud_connection_status);

            free(wifi_status);
        }
        ret = 1;
    }
    return ret;
}

static void ezlopi_service_uart_get_info()
{
    cJSON* cj_get_info = cJSON_CreateObject();
    if (cj_get_info)
    {
        cJSON_AddNumberToObject(cj_get_info, "cmd", 1);
        cJSON_AddStringToObject(cj_get_info, "api", EZLOPI_SERIAL_API_VERSION);
        cJSON* cj_info = cJSON_CreateObject();
        if (cj_info)
        {
            get_ezlopi_firmware_info(cj_info);
            get_ezlopi_chip_info(cj_info);
            get_firmware_sdk_info(cj_info);
            get_device_status(cj_info);
            get_device_serial_protocol_info(cj_info);
            get_ezlopi_device_ezlopi_info(cj_info);
            get_ezlopi_device_oem_info(cj_info);
            get_ezlopi_device_newtwork_info(cj_info);

            cJSON_AddItemToObject(cj_get_info, ezlopi_info_str, cj_info);
            char* serial_data_json_string = cJSON_Print(cj_get_info);
            if (serial_data_json_string)
            {
                cJSON_Minify(serial_data_json_string);
                EZPI_SERVICE_uart_tx_data(strlen(serial_data_json_string), (uint8_t*)serial_data_json_string);
                free(serial_data_json_string);
            }

            cJSON_Delete(cj_get_info);
        }

    }
}

static void ezlopi_service_uart_set_wifi(const char* data)
{
    uint32_t status = 0;
    cJSON* root = cJSON_Parse(data);
    uint8_t status_write = 0;

    if (root)
    {
        cJSON* cj_wifi_ssid = cJSON_GetObjectItem(root, ezlopi_ssid_str);
        cJSON* cj_wifi_pass = cJSON_GetObjectItem(root, ezlopi_pass_str);
        if (cj_wifi_ssid && cj_wifi_pass && cj_wifi_ssid->valuestring && cj_wifi_pass->valuestring)
        {
            char* ssid = cj_wifi_ssid->valuestring;
            char* pass = cj_wifi_pass->valuestring;

            if (strlen(pass) >= EZLOPI_WIFI_MIN_PASS_CHAR)
            {
                // TRACE_S("SSID: %s\tPass : %s\r\n", ssid, pass);
                if (ezlopi_factory_info_v3_set_wifi(ssid, pass))
                {
                    status_write = 1;
                }
                // ezlopi_wifi_set_new_wifi_flag();
                uint8_t attempt = 1;
                while (attempt <= EZLOPI_WIFI_CONN_RETRY_ATTEMPT)
                {
                    ezlopi_wifi_connect((const char*)ssid, (const char*)pass);
                    ezlopi_wait_for_wifi_to_connect((uint32_t)EZLOPI_WIFI_CONN_ATTEMPT_INTERVAL);
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

        cJSON_Delete(root); // free Json string
    }
}

static void ezlopi_service_uart_response(uint8_t cmd, uint8_t status_write, uint8_t status_connect)
{
    cJSON* response = NULL;
    response = cJSON_CreateObject();

    if (response)
    {
        cJSON_AddNumberToObject(response, ezlopi_cmd_str, cmd);
        cJSON_AddNumberToObject(response, ezlopi_status_write_str, status_write);
        cJSON_AddNumberToObject(response, "status_connect", status_connect);

        char* my_json_string = cJSON_Print(response);
        TRACE_D("length of 'my_json_string': %d", strlen(my_json_string));

        cJSON_Delete(response); // free Json string

        if (my_json_string)
        {
            cJSON_Minify(my_json_string);
            EZPI_SERVICE_uart_tx_data(strlen(my_json_string), (uint8_t*)my_json_string);
            cJSON_free(my_json_string);
        }
    }
}

static void ezlopi_service_uart_save_config(const char* data)
{

    uint8_t ret = ezlopi_nvs_write_config_data_str((char*)data);
    TRACE_I("ezlopi_factory_info_set_ezlopi_config: %d", ret);
    if (ret)
    {
        TRACE_I("Successfully wrote config data..");
    }

    ezlopi_service_uart_response(3, ret, 5);
}

static void ezlopi_service_uart_read_config(void)
{
    cJSON* root = NULL;
    char* buf = ezlopi_factory_info_v3_get_ezlopi_config();

    if (buf)
    {
        TRACE_D("buf[len: %d]: %s", strlen(buf), buf);
        root = cJSON_Parse(buf);

        if (root)
        {
            cJSON_DeleteItemFromObject(root, "cmd");
            cJSON_AddNumberToObject(root, "cmd", 4);
        }
        else
        {
            TRACE_E("'root' is null!");
        }
    }
    else
    {
        TRACE_E("'buf' is null!");
    }

    if (NULL == root)
    {
        TRACE_E("Reading config failed!");
        root = cJSON_CreateObject();
        if (root)
        {
            cJSON_AddNumberToObject(root, "cmd", 4);
            TRACE_D("'root'");
        }
        else
        {
            TRACE_E("Failed to create 'root'!");
        }
    }

    if (root)
    {
        char* my_json_string = cJSON_Print(root);
        TRACE_D("length of 'my_json_string': %d", strlen(my_json_string));

        if (my_json_string)
        {
            cJSON_Minify(my_json_string);
            cJSON_Delete(root); // free Json string
            const int len = strlen(my_json_string);
            EZPI_SERVICE_uart_tx_data(len, (uint8_t*)my_json_string); // Send the data over uart
            // TRACE_D("Sending: %s", my_json_string);
            cJSON_free(my_json_string);
        }
    }
}

void EZPI_SERVICE_uart_init(void)
{
    serial_init();
    xTaskCreate(ezlopi_service_uart_rx_task, "ezlopi_service_uart_rx_task", 1024 * 3, NULL, configMAX_PRIORITIES, NULL);
}
