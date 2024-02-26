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

#include "ezlopi_util_trace.h"

#include "ezlopi_core_nvs.h"
#include "ezlopi_core_wifi.h"
#include "ezlopi_core_reset.h"
#include "ezlopi_core_net.h"
#include "ezlopi_core_factory_info.h"

#include "ezlopi_hal_system_info.h"

#include "ezlopi_cloud_info.h"
#include "ezlopi_cloud_constants.h"

#include "ezlopi_service_uart.h"

static const int RX_BUF_SIZE = 3096;

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

static void serial_init(void)
{
    const uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
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

static int ezlopi_service_uart_execute_command_0(uint8_t sub_cmd)
{
    int ret = 0;
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
    return ret;
}

static int qt_serial_parse_rx_data(const char* data)
{
    cJSON* root = cJSON_Parse(data);

    if (root)
    {
        if (cJSON_GetObjectItem(root, ezlopi_cmd_str))
        {
            uint8_t cmd_temp = cJSON_GetObjectItem(root, ezlopi_cmd_str)->valueint;

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
                ezlopi_service_uart_execute_command_0(cJSON_GetObjectItem(root, ezlopi_sub_cmd_str)->valueint);
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

static void ezlopi_service_uart_get_info()
{
    cJSON* get_info = cJSON_CreateObject();

    if (get_info)
    {
        unsigned long long serial_id = ezlopi_factory_info_v3_get_id();
        char* controller_uuid = ezlopi_factory_info_v3_get_device_uuid();
        char* provisioning_uuid = ezlopi_factory_info_v3_get_provisioning_uuid();
        char* device_model = ezlopi_factory_info_v3_get_model();
        char* device_brand = ezlopi_factory_info_v3_get_brand();
        char* device_manufacturer = ezlopi_factory_info_v3_get_manufacturer();
        char* device_name = ezlopi_factory_info_v3_get_name();
        char* device_type = ezlopi_factory_info_v3_get_device_type();
        char* device_mac = ezlopi_factory_info_v3_get_ezlopi_mac();
        char* wifi_ssid = ezlopi_factory_info_v3_get_ssid();

        cJSON_AddNumberToObject(get_info, "cmd", 1);
        cJSON_AddStringToObject(get_info, "firmware_version", VERSION_STR);
        cJSON_AddNumberToObject(get_info, "build", BUILD);

        cJSON* json_chip_info = cJSON_CreateObject();
        if (json_chip_info)
        {
            esp_chip_info_t chip_info;
            char chip_revision[10];
            esp_chip_info(&chip_info);
            sprintf(chip_revision, "%.2f", (float)(chip_info.full_revision / 100.0));
            cJSON_AddStringToObject(json_chip_info, "chip_type", ezlopi_chip_type_str(chip_info.model));
            cJSON_AddStringToObject(json_chip_info, "chip_version", chip_revision);
            cJSON_AddStringToObject(json_chip_info, "firmware_SDK_name", "ESP-IDF");
            cJSON_AddStringToObject(json_chip_info, "firmware_SDK_version", esp_get_idf_version());
            cJSON_AddItemToObject(get_info, "chip_info", json_chip_info);
        }
        cJSON_AddStringToObject(get_info, ezlopi_uptime_str, ezlopi_tick_to_time((uint32_t)(xTaskGetTickCount() / portTICK_PERIOD_MS)));
        cJSON_AddNumberToObject(get_info, ezlopi_build_date_str, BUILD_DATE);
        cJSON_AddNumberToObject(get_info, "boot_count", ezlopi_system_info_get_boot_count());
        cJSON_AddStringToObject(get_info, "boot_reason", ezlopi_esp_reset_reason_str(esp_reset_reason()));

        cJSON_AddStringToObject(get_info, ezlopi_mac_str, device_mac ? device_mac : "");
        cJSON_AddStringToObject(get_info, ezlopi_uuid_str, controller_uuid ? controller_uuid : "");
        cJSON_AddStringToObject(get_info, ezlopi_uuid_prov_str, provisioning_uuid ? provisioning_uuid : "");
        cJSON_AddNumberToObject(get_info, ezlopi_serial_str, serial_id);

        cJSON_AddStringToObject(get_info, ezlopi_ssid_str, wifi_ssid ? wifi_ssid : "");
        cJSON_AddStringToObject(get_info, ezlopi_device_name_str, device_name ? device_name : "");
        cJSON_AddStringToObject(get_info, ezlopi_ezlopi_device_type_str, device_type ? device_type : "");
        cJSON_AddStringToObject(get_info, ezlopi_dev_flash_str, CONFIG_ESPTOOLPY_FLASHSIZE);
        // cJSON_AddStringToObject(get_info, "device_free_flash", ""); // TODO
        cJSON_AddStringToObject(get_info, ezlopi_brand_str, device_brand ? device_brand : "");
        cJSON_AddStringToObject(get_info, ezlopi_manf_name_str, device_manufacturer ? device_manufacturer : "");
        cJSON_AddStringToObject(get_info, ezlopi_model_num_str, device_model ? device_model : "");

        cJSON* json_net_info = cJSON_CreateObject();
        if (json_net_info)
        {

            s_ezlopi_net_status_t* net_stat = ezlopi_get_net_status();
            cJSON_AddStringToObject(json_net_info, "wifi_mode", "STA");
            if (net_stat)
            {
                if (net_stat->wifi_status->wifi_connection)
                {
                    cJSON_AddTrueToObject(json_net_info, "wifi_connection_status");

                    char* ip_addr = (char*)malloc(sizeof(char) * 30);

                    ip_addr = esp_ip4addr_ntoa(&net_stat->wifi_status->ip_info->ip, ip_addr, 30);
                    cJSON_AddStringToObject(json_net_info, "ip_sta", ip_addr);

                    ip_addr = esp_ip4addr_ntoa(&net_stat->wifi_status->ip_info->netmask, ip_addr, 30);
                    cJSON_AddStringToObject(json_net_info, "ip_nmask", ip_addr);

                    ip_addr = esp_ip4addr_ntoa(&net_stat->wifi_status->ip_info->gw, ip_addr, 30);
                    cJSON_AddStringToObject(json_net_info, "ip_gw", ip_addr);
                    free(ip_addr);

                    if (net_stat->internet_status == EZLOPI_PING_STATUS_LIVE)
                    {
                        cJSON_AddStringToObject(json_net_info, "internet_status", "connected");
                    }
                    else if (net_stat->internet_status == EZLOPI_PING_STATUS_DISCONNECTED)
                    {
                        cJSON_AddStringToObject(json_net_info, "internet_status", "disconnected");
                    }
                    else
                    {
                        cJSON_AddStringToObject(json_net_info, "internet_status", "unknown");
                    }

                    if (net_stat->nma_cloud_connection_status)
                    {
                        cJSON_AddTrueToObject(json_net_info, "cloud_connection_status");
                    }
                    else
                    {
                        cJSON_AddFalseToObject(json_net_info, "cloud_connection_status");
                    }
                }
                else
                {
                    cJSON_AddFalseToObject(json_net_info, "wifi_connection_status");
                }
            }
            cJSON_AddItemToObject(get_info, "net_info", json_net_info);
        }

        ezlopi_factory_info_v3_free((void*)controller_uuid);
        ezlopi_factory_info_v3_free((void*)provisioning_uuid);
        ezlopi_factory_info_v3_free((void*)device_model);
        ezlopi_factory_info_v3_free((void*)device_brand);
        ezlopi_factory_info_v3_free((void*)device_manufacturer);
        ezlopi_factory_info_v3_free((void*)device_name);
        ezlopi_factory_info_v3_free((void*)device_mac);
        ezlopi_factory_info_v3_free((void*)wifi_ssid);

        char* serial_data_json_string = cJSON_Print(get_info);
        if (serial_data_json_string)
        {
            cJSON_Minify(serial_data_json_string);
            EZPI_SERVICE_uart_tx_data(strlen(serial_data_json_string), (uint8_t*)serial_data_json_string);
            cJSON_free(serial_data_json_string);
        }

        cJSON_Delete(get_info); // free Json object
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
    xTaskCreate(ezlopi_service_uart_rx_task, "ezlopi_service_uart_rx_task", 1024 * 10, NULL, configMAX_PRIORITIES, NULL);
}
