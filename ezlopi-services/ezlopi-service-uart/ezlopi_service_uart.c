

/**
 * @file    ezlopi_service_uart.c
 * @brief
 * @author
 * @version
 * @date
 */
/* ===========================================================================
** Copyright (C) 2024 Ezlo Innovation Inc
**
** Under EZLO AVAILABLE SOURCE LICENSE (EASL) AGREEMENT
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met:
**
** 1. Redistributions of source code must retain the above copyright notice,
**    this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. Neither the name of the copyright holder nor the names of its
**    contributors may be used to endorse or promote products derived from
**    this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
** AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
** ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
** LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
** CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
** SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
** INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
** CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
** ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
** POSSIBILITY OF SUCH DAMAGE.
** ===========================================================================
*/

#include "../../build/config/sdkconfig.h"

#include "freertos/FreeRTOSConfig.h"

#if !defined(CONFIG_IDF_TARGET_ESP32) && !defined(CONFIG_IDF_TARGET_ESP32C3)
#include "tinyusb.h"
#include "tusb_cdc_acm.h"
#endif // NOT defined CONFIG_IDF_TARGET_ESP32 or CONFIG_IDF_TARGET_ESP32C3

#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include "esp_idf_version.h"
#include "esp_netif_ip_addr.h"

#include "cjext.h"
#include "ezlopi_util_trace.h"
#include "ezlopi_util_version.h"

#include "ezlopi_hal_uart.h"
#include "ezlopi_hal_system_info.h"

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
#include "ezlopi_core_errors.h"

#include "ezlopi_cloud_info.h"
#include "ezlopi_cloud_constants.h"

#include "ezlopi_service_ble.h"
#include "ezlopi_service_uart.h"
#include "ezlopi_service_loop.h"
#include "ezlopi_service_webprov.h"

#include "EZLOPI_USER_CONFIG.h"

#if defined(CONFIG_IDF_TARGET_ESP32)
/**
 * @brief UART TX pin number
 *
 */
#define TXD_PIN (GPIO_NUM_1)
/**
 * @brief UART RX pin number
 *
 */
#define RXD_PIN (GPIO_NUM_3)
#elif defined(CONFIG_IDF_TARGET_ESP32S2)
#elif defined(CONFIG_IDF_TARGET_ESP32C3)
/**
 * @brief UART TX pin number
 *
 */
#define TXD_PIN (GPIO_NUM_21)
/**
 * @brief UART RX pin number
 *
 */
#define RXD_PIN (GPIO_NUM_20)
#elif defined(CONFIG_IDF_TARGET_ESP32S3)
/**
 * @brief UART TX pin number
 *
 */
#define TXD_PIN (GPIO_NUM_43)
/**
 * @brief UART RX pin number
 *
 */
#define RXD_PIN (GPIO_NUM_44)
#endif

/**
 * @brief Function to process reset command
 *
 * @param root Pointer to the root JSON coming from UART
 * @return int
 */
static int ezpi_service_uart_reset(cJSON *root);
/**
 * @brief Function to process uart config command
 *
 * @param root Pointer to the root JSON coming from UART
 * @return int
 */
static int ezpi_service_uart_set_uart_config(const cJSON *root);
/**
 * @brief Function to process log secerity command
 *
 * @param root Pointer to the root JSON coming from UART
 * @return int
 */
static int ezpi_service_uart_process_log_severity(const cJSON *root);
/**
 * @brief Function to process provisioning command
 *
 * @param root Pointer to the root JSON coming from UART
 * @return int
 */
static ezlopi_error_t ezpi_service_uart_process_provisioning_api(const cJSON *root);
/**
 * @brief Function to process UART JSON
 *
 * @param data Pointer to the data coming from UART
 * @return int
 */
static int ezpi_service_uart_parser(const char *data);
/**
 * @brief Function to add firmware information to the parent JSON
 *
 * @param parent Pointer to the parent JSON
 * @return int
 */
static int ezpi_service_uart_firmware_info(cJSON *parent);
/**
 * @brief Function to add chip information to the parent JSON
 *
 * @param parent Pointer to the parent JSON
 * @return int
 */
static int ezpi_service_uart_chip_info(cJSON *parent);
/**
 * @brief Function to add firmware SDK information to the parent JSON
 *
 * @param parent Pointer to the parent JSON
 * @return int
 */
static int ezpi_service_uart_firmware_sdk_info(cJSON *parent);
/**
 * @brief Function to add device status information to the parent JSON
 *
 * @param parent Pointer to the parent JSON
 * @return int
 */
static int ezpi_service_uart_device_status_info(cJSON *parent);
/**
 * @brief Function to add UART config information to the parent JSON
 *
 * @param parent Pointer to the parent JSON
 * @return int
 */
static int ezpi_service_uart_config_info(cJSON *parent);
/**
 * @brief Function to add ezlopi information to the parent JSON
 *
 * @param parent Pointer to the parent JSON
 * @return int
 */
static int ezpi_service_uart_ezlopi_info(cJSON *parent);
/**
 * @brief Function to add OEM information to the parent JSON
 *
 * @param parent Pointer to the parent JSON
 * @return int
 */
static int ezpi_service_uart_oem_info(cJSON *parent);
/**
 * @brief Function to add network information to the parent JSON
 *
 * @param parent Pointer to the parent JSON
 * @return int
 */
static int ezpi_service_uart_newtwork_info(cJSON *parent);
/**
 * @brief Function to send device information through data
 *
 */
static void ezpi_service_uart_get_info();
/**
 * @brief Function to set Wifi detials coming from UART
 *
 * @param data Pointer to the JSON data
 */
static void ezpi_service_uart_set_wifi(const char *data);
/**
 * @brief Function to send UART command response
 *
 * @param cmd Command triggering UART write
 * @param status_write Status to send
 * @param status_connect Optional param for WiFi command
 */
static void ezpi_service_uart_response(uint8_t cmd, uint8_t status_write, uint8_t status_connect);
/**
 * @brief Function to set configurations
 *
 * @param data Pointer to the JSON data
 */
static void ezpi_service_uart_set_config(const char *data);
/**
 * @brief Function to get configurations
 *
 */
static void ezpi_service_uart_get_config(void);
/**
 * @brief Function task that handles UART communication
 *
 * @param arg Task arguments
 */
static void ezpi_service_uart_task(void *arg);
#if !defined(CONFIG_IDF_TARGET_ESP32) && !defined(CONFIG_IDF_TARGET_ESP32C3)
/**
 * @brief Function that will be called on incoming data at CDC
 *
 * @param itf Interface
 * @param event CDC event
 */
static void ezpi_tinyusb_cdc_rx_callback(int itf, cdcacm_event_t *event);
#endif // NOT defined CONFIG_IDF_TARGET_ESP32 or CONFIG_IDF_TARGET_ESP32C3

#if !defined(CONFIG_IDF_TARGET_ESP32) && !defined(CONFIG_IDF_TARGET_ESP32C3)
static uint8_t usb_rx_buffer[CONFIG_TINYUSB_CDC_RX_BUFSIZE - 1];
static size_t rx_buffer_pointer = 0;
static int cdc_port = TINYUSB_CDC_ACM_0;
static SemaphoreHandle_t usb_semaphore_handle = NULL;
#endif // NOT defined CONFIG_IDF_TARGET_ESP32 or CONFIG_IDF_TARGET_ESP32C3

#if !defined(CONFIG_IDF_TARGET_ESP32) && !defined(CONFIG_IDF_TARGET_ESP32C3)
void EZPI_SERV_cdc_init()
{
    usb_semaphore_handle = xSemaphoreCreateBinary();
    if (usb_semaphore_handle)
    {
        xSemaphoreGive(usb_semaphore_handle);
        tinyusb_config_t ezlopi_usb_device_configuration = {
            .descriptor = NULL,
            .string_descriptor = NULL,
            .external_phy = false,
        };

        tinyusb_config_cdcacm_t ezlopi_usb_cdc_configuration = {
            .usb_dev = TINYUSB_USBDEV_0,
            .cdc_port = cdc_port,
            .rx_unread_buf_sz = CONFIG_TINYUSB_CDC_RX_BUFSIZE,
            .callback_rx = &ezpi_tinyusb_cdc_rx_callback,
            .callback_rx_wanted_char = NULL,
            .callback_line_state_changed = &ezpi_tinyusb_cdc_rx_callback,
            .callback_line_coding_changed = NULL,
        };

        ESP_ERROR_CHECK(tinyusb_driver_install(&ezlopi_usb_device_configuration));
        ESP_ERROR_CHECK(tusb_cdc_acm_init(&ezlopi_usb_cdc_configuration));
        TRACE_I("USB CDC initialization completed successfully.");
    }
}
#endif // NOT defined CONFIG_IDF_TARGET_ESP32 or CONFIG_IDF_TARGET_ESP32C3

void EZPI_SERV_uart_init(void)
{
    // EZPI_service_loop_add("uart-loop", __uart_loop, 1, NULL);
#if 1
    TaskHandle_t __uart_loop_handle = NULL;
    xTaskCreate(ezpi_service_uart_task, "serv_uart_task", EZLOPI_SERVICE_UART_TASK_DEPTH, NULL, configMAX_PRIORITIES - 4, &__uart_loop_handle);

#if defined(CONFIG_FREERTOS_USE_TRACE_FACILITY)
    EZPI_core_process_set_process_info(ENUM_EZLOPI_SERVICE_UART_TASK, &__uart_loop_handle, EZLOPI_SERVICE_UART_TASK_DEPTH);
#endif // CONFIG_FREERTOS_USE_TRACE_FACILITY

#endif
}

static int ezpi_service_uart_reset(cJSON *root)
{
    int ret = 0;
    cJSON *cj_sub_cmd = cJSON_GetObjectItem(__FUNCTION__, root, ezlopi_sub_cmd_str);
    if (cj_sub_cmd)
    {
        uint8_t sub_cmd = cj_sub_cmd->valuedouble;
        switch (sub_cmd)
        {
        case 0:
        {
            TRACE_E("Factory restore command");
            TRACE_OTEL(ENUM_EZLOPI_TRACE_SEVERITY_INFO, "UART: Factory restore command");

            const static char *reboot_response = "{\"cmd\":0, \"sub_cmd\":0,\"status\":1}";
            EZPI_SERV_uart_tx_data(strlen(reboot_response), (uint8_t *)reboot_response);
            EZPI_core_reset_factory_restore();
            break;
        }
        case 1:
        {
            TRACE_E("Reboot only command");
            TRACE_OTEL(ENUM_EZLOPI_TRACE_SEVERITY_INFO, "UART: Reboot only command");

            const static char *reboot_response = "{\"cmd\":0, \"sub_cmd\":1, \"status\":1}";
            EZPI_SERV_uart_tx_data(strlen(reboot_response), (uint8_t *)reboot_response);
            EZPI_core_reset_reboot();
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

static int ezpi_service_uart_set_uart_config(const cJSON *root)
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

    EZPI_core_nvs_read_baud(&baud_current);
    EZPI_core_nvs_read_parity(&parity_val_current);
    EZPI_core_nvs_read_start_bits(&start_bits_current);
    EZPI_core_nvs_read_stop_bits(&stop_bits_current);
    EZPI_core_nvs_read_frame_size(&frame_size_current);
    EZPI_core_nvs_read_flow_control(&flow_control_current);

    if (
        (baud_current != baud) ||
        (parity_val_current != (uint32_t)EZPI_core_info_name_to_parity(parity)) ||
        (start_bits_current != start_bits) ||
        (stop_bits != stop_bits_current) ||
        (frame_size != frame_size_current) ||
        (flow_control_current != (uint32_t)EZPI_core_info_get_flw_ctrl_from_name(flow_control)))
    {
        flag_new_config = true;
    }

    if (flag_new_config)
    {
        if ('\0' != parity[0])
        {
            parity_val = (uint32_t)EZPI_core_info_name_to_parity(parity);
        }

        EZPI_core_nvs_write_parity(parity_val);

        if (baud)
        {
            EZPI_core_nvs_write_baud(baud);
        }
        else
        {
            baud = EZPI_SERV_UART_BAUD_DEFAULT;
            EZPI_core_nvs_write_baud(baud);
        }

        EZPI_core_nvs_write_start_bits(start_bits);
        EZPI_core_nvs_write_stop_bits(stop_bits);

        if (!frame_size)
        {
            frame_size = EZPI_SERV_UART_FRAME_SIZE_DEFAULT;
        }
        EZPI_core_nvs_write_frame_size(frame_size);

        if ('\0' != flow_control[0])
        {
            flow_control_val = (uint32_t)EZPI_core_info_get_flw_ctrl_from_name(flow_control);
            TRACE_W("New Flow control: %d", flow_control_val);
        }

        EZPI_core_nvs_write_flow_control(flow_control_val);

        const static char *reboot_response = "{\"cmd\":5, \"status\":1}";
        EZPI_SERV_uart_tx_data(strlen(reboot_response), (uint8_t *)reboot_response);

        TRACE_OTEL(ENUM_EZLOPI_TRACE_SEVERITY_INFO, "UART: New config applied.");
        TRACE_W("New config has been applied, device rebooting");

        vTaskDelay(100);
        EZPI_core_reset_reboot();
    }
    else
    {
        const static char *reboot_response = "{\"cmd\":5, \"status\":0}";
        EZPI_SERV_uart_tx_data(strlen(reboot_response), (uint8_t *)reboot_response);

        TRACE_OTEL(ENUM_EZLOPI_TRACE_SEVERITY_INFO, "UART: Configuration unchanged!");
        TRACE_W("Configuration unchanged !");
        vTaskDelay(10);
    }

    return ret;
}

static int ezpi_service_uart_process_log_severity(const cJSON *root)
{
    int ret = 0;

#ifdef CONFIG_EZPI_UTIL_TRACE_EN
    int target = 0;
    int severity = 0;

    CJSON_GET_VALUE_DOUBLE(root, "target", target);
    CJSON_GET_VALUE_DOUBLE(root, ezlopi_severity_str, severity);

    if (0 == target)
    {
        // Call cloud log severity setter
        ret = EZPI_core_cloud_log_severity_process_id(severity);
    }
    else if (1 == target)
    {
        // Call serial log severity setter
        ret = EZPI_core_serial_log_severity_process_id(severity);
    }
#endif // CONFIG_EZPI_UTIL_TRACE_EN

    cJSON *cj_uart_response = cJSON_CreateObject(__func__);
    if (cj_uart_response)
    {
        cJSON_AddNumberToObject(__func__, cj_uart_response, ezlopi_cmd_str, EZPI_UART_CMD_LOG_CONFIG);
        if (EZPI_SUCCESS != ret)
        {
            cJSON_AddNumberToObject(__func__, cj_uart_response, ezlopi_status_str, EZPI_UART_CMD_STATUS_FAIL);
        }
        else
        {
            cJSON_AddNumberToObject(__func__, cj_uart_response, ezlopi_status_str, EZPI_UART_CMD_STATUS_SUCCESS);
        }

        const char *str_uart_response = cJSON_Print(__func__, cj_uart_response);
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

static ezlopi_error_t ezpi_service_uart_process_provisioning_api(const cJSON *root)
{
    ezlopi_error_t ret = EZPI_FAILED;
    int uart_response = 0;

    // if (EZPI_core_factory_info_v3_get_provisioning_status())
    if (0)
    {
        uart_response = 0;
    }
    else
    {
        cJSON *cj_data = cJSON_GetObjectItem(__FUNCTION__, root, "data");
        if (cj_data)
        {
            s_basic_factory_info_t *ezlopi_config_basic = ezlopi_malloc(__FUNCTION__, sizeof(s_basic_factory_info_t));
            if (ezlopi_config_basic)
            {
                char device_name[EZLOPI_FINFO_LEN_DEVICE_NAME];
                char manufacturer[EZLOPI_FINFO_LEN_MANUF_NAME];
                char brand[EZLOPI_FINFO_LEN_BRAND_NAME];
                char model_number[EZLOPI_FINFO_LEN_MODEL_NAME];
                char device_uuid[EZLOPI_FINFO_LEN_DEVICE_UUID];
                char prov_uuid[EZLOPI_FINFO_LEN_PROV_UUID];
                char device_mac[EZLOPI_FINFO_LEN_DEVICE_MAC];
                char provision_server[EZLOPI_FINFO_LEN_PROVISIONING_SERVER_URL];
                char cloud_server[EZLOPI_FINFO_LEN_CLOUD_SERVER_URL];
                char provision_token[EZLOPI_FINFO_LEN_PROVISIONING_TOKEN];
                char local_key[EZLOPI_FINFO_LEN_LOCAL_KEY];

                memset(device_name, 0, EZLOPI_FINFO_LEN_DEVICE_NAME);
                memset(manufacturer, 0, EZLOPI_FINFO_LEN_MANUF_NAME);
                memset(brand, 0, EZLOPI_FINFO_LEN_BRAND_NAME);
                memset(model_number, 0, EZLOPI_FINFO_LEN_MODEL_NAME);
                memset(device_uuid, 0, EZLOPI_FINFO_LEN_DEVICE_UUID);
                memset(prov_uuid, 0, EZLOPI_FINFO_LEN_PROV_UUID);
                memset(device_mac, 0, EZLOPI_FINFO_LEN_DEVICE_MAC);
                memset(provision_server, 0, EZLOPI_FINFO_LEN_PROVISIONING_SERVER_URL);
                memset(cloud_server, 0, EZLOPI_FINFO_LEN_CLOUD_SERVER_URL);
                memset(provision_token, 0, EZLOPI_FINFO_LEN_PROVISIONING_TOKEN);
                memset(local_key, 0, EZLOPI_FINFO_LEN_LOCAL_KEY);

                CJSON_GET_VALUE_DOUBLE(cj_data, ezlopi_serial_str, ezlopi_config_basic->id); // id => OK
                CJSON_GET_VALUE_DOUBLE(cj_data, ezlopi_version_str, ezlopi_config_basic->config_version);

                CJSON_GET_VALUE_STRING_BY_COPY(cj_data, ezlopi_device_name_str, device_name);
                CJSON_GET_VALUE_STRING_BY_COPY(cj_data, ezlopi_manufacturer_name_str, manufacturer);
                CJSON_GET_VALUE_STRING_BY_COPY(cj_data, ezlopi_brand_str, brand);
                CJSON_GET_VALUE_STRING_BY_COPY(cj_data, ezlopi_model_number_str, model_number);
                CJSON_GET_VALUE_STRING_BY_COPY(cj_data, ezlopi_uuid_str, device_uuid);
                CJSON_GET_VALUE_STRING_BY_COPY(cj_data, ezlopi_mac_str, device_mac);
                CJSON_GET_VALUE_STRING_BY_COPY(cj_data, "provisioning_uuid", prov_uuid);
                CJSON_GET_VALUE_STRING_BY_COPY(cj_data, ezlopi_provision_server_str, provision_server);
                CJSON_GET_VALUE_STRING_BY_COPY(cj_data, ezlopi_cloud_server_str, cloud_server);
                CJSON_GET_VALUE_STRING_BY_COPY(cj_data, ezlopi_provision_token_str, provision_token);
                CJSON_GET_VALUE_STRING_BY_COPY(cj_data, "local_key", local_key);

                ezlopi_config_basic->device_name = device_name;
                ezlopi_config_basic->manufacturer = manufacturer;
                ezlopi_config_basic->brand = brand;
                ezlopi_config_basic->model_number = model_number;
                ezlopi_config_basic->device_uuid = device_uuid;
                ezlopi_config_basic->prov_uuid = prov_uuid;
                ezlopi_config_basic->device_mac = device_mac;
                ezlopi_config_basic->provision_server = provision_server;
                ezlopi_config_basic->cloud_server = cloud_server;
                ezlopi_config_basic->provision_token = provision_token;
                ezlopi_config_basic->device_type = NULL;
                ezlopi_config_basic->local_key = local_key;

                EZPI_core_factory_info_v3_set_basic(ezlopi_config_basic);
                EZPI_core_factory_info_v3_set_ca_cert(cJSON_GetObjectItem(__FUNCTION__, cj_data, ezlopi_signing_ca_certificate_str));
                EZPI_core_factory_info_v3_set_ssl_shared_key(cJSON_GetObjectItem(__FUNCTION__, cj_data, ezlopi_ssl_shared_key_str));
                EZPI_core_factory_info_v3_set_ssl_private_key(cJSON_GetObjectItem(__FUNCTION__, cj_data, ezlopi_ssl_private_key_str));

                uart_response = 1;
            }
        }
    }

    cJSON *cj_response = cJSON_CreateObject(__FUNCTION__);
    if (cj_response)
    {
        cJSON_AddNumberToObject(__FUNCTION__, cj_response, ezlopi_cmd_str, EZPI_UART_CMD_SET_PROV);
        cJSON_AddNumberToObject(__FUNCTION__, cj_response, ezlopi_status_str, uart_response);
        const char *str_response = cJSON_Print(__func__, cj_response);
        if (str_response)
        {
            cJSON_Minify((char *)str_response);
            EZPI_SERV_uart_tx_data(strlen(str_response), (uint8_t *)str_response);
            free((char *)str_response);
        }
        cJSON_Delete(__func__, cj_response);
    }

    return ret;
}

static int ezpi_service_uart_parser(const char *data)
{
    cJSON *root = cJSON_ParseWithRef(__FUNCTION__, data);

    if (root)
    {
        cJSON *cj_cmd = cJSON_GetObjectItem(__FUNCTION__, root, ezlopi_cmd_str);
        if (cj_cmd)
        {
            uint8_t cmd_temp = cj_cmd->valuedouble;

            switch (cmd_temp)
            {
            case EZPI_UART_CMD_RESET:
            {
                ezpi_service_uart_reset(root);
                break;
            }
            case EZPI_UART_CMD_INFO:
            {
                ezpi_service_uart_get_info();
                break;
            }
            case EZPI_UART_CMD_WIFI:
            {
                ezpi_service_uart_set_wifi(data);
                break;
            }
            case EZPI_UART_CMD_SET_CONFIG:
            {
                ezpi_service_uart_set_config(data);
                break;
            }
            case EZPI_UART_CMD_GET_CONFIG:
            {
                ezpi_service_uart_get_config();
                break;
            }
            case EZPI_UART_CMD_UART_CONFIG:
            {
                ezpi_service_uart_set_uart_config(root);
                break;
            }
            case EZPI_UART_CMD_LOG_CONFIG:
            {
                ezpi_service_uart_process_log_severity(root);
                break;
            }
            case EZPI_UART_CMD_SET_PROV:
            {
                ezpi_service_uart_process_provisioning_api(root);
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
            TRACE_OTEL(ENUM_EZLOPI_TRACE_SEVERITY_ERROR, "UART: 'cmd' not found!");
        }

        cJSON_Delete(__FUNCTION__, root);
    }
    else
    {
        TRACE_E("Failed to parse json!");
        TRACE_OTEL(ENUM_EZLOPI_TRACE_SEVERITY_ERROR, "UART: Failed to parse json!.");
    }

    return 1;
}

static void ezpi_service_uart_task(void *arg)
{
    static const char *RX_TASK_TAG = "RX_TASK";
    esp_log_level_set(RX_TASK_TAG, ESP_LOG_INFO);

    while (1)
    {
        uint32_t tmp_len = 0;
        uint32_t buffred_data_len = 0;
        uart_get_buffered_data_len(EZPI_SERV_UART_NUM_DEFAULT, &tmp_len);

        if (tmp_len > 0)
        {
            while (tmp_len != buffred_data_len)
            {
                buffred_data_len = tmp_len;
                vTaskDelay(10 / portTICK_RATE_MS);
                uart_get_buffered_data_len(EZPI_SERV_UART_NUM_DEFAULT, &tmp_len);
            }

            if (buffred_data_len)
            {
                uint8_t *uart_rx_data = (uint8_t *)ezlopi_malloc(__FUNCTION__, buffred_data_len + 1);

                if (uart_rx_data)
                {
                    memset(uart_rx_data, 0, buffred_data_len);

                    int rxBytes = uart_read_bytes(EZPI_SERV_UART_NUM_DEFAULT, uart_rx_data, buffred_data_len, 1000 / portTICK_RATE_MS);

                    if (rxBytes > 0)
                    {
                        uart_rx_data[rxBytes] = 0;
                        TRACE_I("%s", uart_rx_data);
                        ezpi_service_uart_parser((const char *)uart_rx_data);
                    }

                    ezlopi_free(__FUNCTION__, uart_rx_data);
                }
                else
                {
                    uart_flush_input(EZPI_SERV_UART_NUM_DEFAULT);
                }
            }
        }

        vTaskDelay(100 / portTICK_RATE_MS);
    }

#if defined(CONFIG_FREERTOS_USE_TRACE_FACILITY)
    EZPI_core_process_set_is_deleted(ENUM___uart_loop);
#endif
    vTaskDelete(NULL);
}

static int ezpi_service_uart_firmware_info(cJSON *parent)
{
    int ret = 0;
    static const char *_ezlopi_firmware_str = "ezlopi_firmware";
    cJSON *cj_firmware_info = cJSON_AddObjectToObjectWithRef(__FUNCTION__, parent, _ezlopi_firmware_str);

    if (cj_firmware_info)
    {
        char build_time[64];
        cJSON_AddStringToObjectWithRef(__FUNCTION__, cj_firmware_info, ezlopi_version_str, VERSION_STR);
        cJSON_AddNumberToObjectWithRef(__FUNCTION__, cj_firmware_info, ezlopi_build_str, BUILD);
        EZPI_core_sntp_epoch_to_iso8601(build_time, sizeof(build_time), (time_t)BUILD_DATE);
        cJSON_AddStringToObject(__FUNCTION__, cj_firmware_info, ezlopi_build_date_str, build_time);

        ret = 1;
    }
    return ret;
}

static int ezpi_service_uart_chip_info(cJSON *parent)
{
    int ret = 0;
    cJSON *cj_chip = cJSON_AddObjectToObject(__FUNCTION__, parent, ezlopi_chip_str);
    if (cj_chip)
    {
        esp_chip_info_t chip_info;
        char chip_revision[10];
        esp_chip_info(&chip_info);
        sprintf(chip_revision, "%.2f", (float)(chip_info.full_revision / 100.0));
        cJSON_AddStringToObject(__FUNCTION__, cj_chip, ezlopi_type_str, EZPI_core_info_get_chip_type_to_name(chip_info.model));
        cJSON_AddStringToObject(__FUNCTION__, cj_chip, ezlopi_version_str, chip_revision);
        ret = 1;
    }
    return ret;
}

static int ezpi_service_uart_firmware_sdk_info(cJSON *parent)
{
    int ret = 0;
    cJSON *cj_firmware_sdk = cJSON_AddObjectToObject(__FUNCTION__, parent, "firmware_sdk");
    if (cj_firmware_sdk)
    {
        cJSON_AddStringToObject(__FUNCTION__, cj_firmware_sdk, ezlopi_name_str, "ESP-IDF");
        cJSON_AddStringToObject(__FUNCTION__, cj_firmware_sdk, ezlopi_version_str, esp_get_idf_version());
        ret = 1;
    }
    return ret;
}

static int ezpi_service_uart_device_status_info(cJSON *parent)
{
    int ret = 0;
    cJSON *cj_device_state = cJSON_AddObjectToObject(__FUNCTION__, parent, "device_state");
    if (cj_device_state)
    {

        char time_string[50];
        uint32_t tick_count_ms = xTaskGetTickCount() / portTICK_PERIOD_MS;
        EZPI_core_info_get_tick_to_time_name(time_string, sizeof(time_string), tick_count_ms);

        cJSON_AddStringToObject(__FUNCTION__, cj_device_state, ezlopi_uptime_str, time_string);
        cJSON_AddNumberToObject(__FUNCTION__, cj_device_state, "boot_count", EZPI_hal_system_info_get_boot_count());
        cJSON_AddStringToObject(__FUNCTION__, cj_device_state, "boot_reason", EZPI_core_info_get_esp_reset_reason_to_name(esp_reset_reason()));

        cJSON_AddStringToObject(__FUNCTION__, cj_device_state, ezlopi_flash_size_str, CONFIG_ESPTOOLPY_FLASHSIZE);

        uint8_t mac[6];
        EZPI_core_wifi_get_wifi_mac(mac);
        char mac_str[20];
        memset(mac_str, 0, sizeof(mac_str));
        snprintf(mac_str, 20, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
        cJSON_AddStringToObject(__FUNCTION__, cj_device_state, "wifi_mac", mac_str);

        memset(mac, 0, sizeof(mac));
        EZPI_core_info_get_ble_mac(mac);
        memset(mac_str, 0, sizeof(mac_str));
        snprintf(mac_str, 20, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
        cJSON_AddStringToObject(__FUNCTION__, cj_device_state, "ble_mac", mac_str);
        ret = 1;
    }
    return ret;
}

static int ezpi_service_uart_config_info(cJSON *parent)
{
    int ret = 0;
    cJSON *cj_serial_config = cJSON_AddObjectToObject(__FUNCTION__, parent, "serial_config");
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

        EZPI_core_nvs_read_baud(&baud);
        cJSON_AddNumberToObject(__FUNCTION__, cj_serial_config, ezlopi_baud_str, baud);

        EZPI_core_nvs_read_parity((uint32_t *)&parity_val);
        parity[0] = EZPI_core_info_parity_to_name(parity_val);
        parity[1] = 0;
        cJSON_AddStringToObject(__FUNCTION__, cj_serial_config, ezlopi_parity_str, parity);

        EZPI_core_nvs_read_start_bits(&start_bits);
        cJSON_AddNumberToObject(__FUNCTION__, cj_serial_config, ezlopi_start_bits_str, start_bits);

        EZPI_core_nvs_read_stop_bits(&stop_bits);
        cJSON_AddNumberToObject(__FUNCTION__, cj_serial_config, ezlopi_stop_bits_str, stop_bits);

        EZPI_core_nvs_read_frame_size(&frame_size);
        cJSON_AddNumberToObject(__FUNCTION__, cj_serial_config, ezlopi_frame_size_str, frame_size);

        EZPI_core_nvs_read_flow_control(&flow_control);
        EZPI_core_info_get_flow_ctrl_to_name(flow_control, flw_ctrl_bffr);
        cJSON_AddStringToObject(__FUNCTION__, cj_serial_config, ezlopi_flow_control_str, flw_ctrl_bffr);

        ret = 1;
    }
    return ret;
}

static int ezpi_service_uart_ezlopi_info(cJSON *parent)
{
    int ret = 0;
    cJSON *cj_ezlopi = cJSON_AddObjectToObject(__FUNCTION__, parent, "ezlopi_cloud");
    if (cj_ezlopi)
    {
        char *device_mac = EZPI_core_factory_info_v3_get_ezlopi_mac();
        char *controller_uuid = EZPI_core_factory_info_v3_get_device_uuid();
        char *provisioning_uuid = EZPI_core_factory_info_v3_get_provisioning_uuid();
        unsigned long long serial_id = EZPI_core_factory_info_v3_get_id();
        const char *device_type = EZPI_core_factory_info_v3_get_device_type();

        cJSON_AddBoolToObject(__FUNCTION__, cj_ezlopi, ezlopi_provisioned_str, EZPI_core_factory_info_v3_get_provisioning_status());
        cJSON_AddStringToObject(__FUNCTION__, cj_ezlopi, ezlopi_uuid_str, controller_uuid ? controller_uuid : "");
        cJSON_AddStringToObject(__FUNCTION__, cj_ezlopi, ezlopi_uuid_prov_str, provisioning_uuid ? provisioning_uuid : "");
        cJSON_AddStringToObject(__FUNCTION__, cj_ezlopi, ezlopi_type_str, device_type ? device_type : "");
        cJSON_AddNumberToObject(__FUNCTION__, cj_ezlopi, ezlopi_serial_str, serial_id);
        cJSON_AddStringToObject(__FUNCTION__, cj_ezlopi, ezlopi_mac_str, device_mac ? device_mac : "");

        EZPI_core_factory_info_v3_free(device_mac);
        EZPI_core_factory_info_v3_free(controller_uuid);
        EZPI_core_factory_info_v3_free(provisioning_uuid);
        ret = 1;
    }
    return ret;
}

static int ezpi_service_uart_oem_info(cJSON *parent)
{
    int ret = 0;
    cJSON *cj_oem = cJSON_AddObjectToObject(__FUNCTION__, parent, "oem");
    if (cj_oem)
    {
        char *device_model = EZPI_core_factory_info_v3_get_model();
        char *device_brand = EZPI_core_factory_info_v3_get_brand();
        char *device_manufacturer = EZPI_core_factory_info_v3_get_manufacturer();

        cJSON_AddStringToObject(__FUNCTION__, cj_oem, ezlopi_brand_str, device_brand ? device_brand : "");
        cJSON_AddStringToObject(__FUNCTION__, cj_oem, ezlopi_manufacturer_str, device_manufacturer ? device_manufacturer : "");
        cJSON_AddStringToObject(__FUNCTION__, cj_oem, ezlopi_model_str, device_model ? device_model : "");

        EZPI_core_factory_info_v3_free(device_model);
        EZPI_core_factory_info_v3_free(device_manufacturer);
        EZPI_core_factory_info_v3_free(device_brand);
        ret = 0;
    }
    return ret;
}

static int ezpi_service_uart_newtwork_info(cJSON *parent)
{
    int ret = 0;
    cJSON *cj_network = cJSON_AddObjectToObject(__FUNCTION__, parent, "network");
    if (cj_network)
    {
        char *wifi_ssid = EZPI_core_factory_info_v3_get_ssid();
        cJSON_AddStringToObject(__FUNCTION__, cj_network, ezlopi_ssid_str, wifi_ssid ? wifi_ssid : "");
        EZPI_core_factory_info_v3_free(wifi_ssid);

        ezlopi_wifi_status_t *wifi_status = EZPI_core_wifi_status();
        if (wifi_status)
        {
            char *wifi_mode = EZPI_core_info_get_wifi_mode_to_name(wifi_status->wifi_mode);
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

            e_ezlopi_event_t events = EZPI_core_event_group_get_eventbit_status();
            bool cloud_connection_status = (EZLOPI_EVENT_NMA_REG & events) == EZLOPI_EVENT_NMA_REG;

#ifdef CONFIG_EZPI_ENABLE_PING
            e_ping_status_t ping_status = EZPI_core_ping_get_internet_status();
            if (ping_status == EZLOPI_PING_STATUS_LIVE)
            {
                cJSON_AddTrueToObject(__FUNCTION__, cj_network, ezlopi_internet_str);
            }
            else
            {
                cJSON_AddFalseToObject(__FUNCTION__, cj_network, ezlopi_internet_str);
            }
#else  // CONFIG_EZPI_ENABLE_PING
            cJSON_AddBoolToObject(__FUNCTION__, cj_network, ezlopi_internet_str, cloud_connection_status);
#endif // CONFIG_EZPI_ENABLE_PING

            cJSON_AddBoolToObject(__FUNCTION__, cj_network, "cloud", cloud_connection_status);

            ezlopi_free(__FUNCTION__, wifi_status);
        }
        ret = 1;
    }
    return ret;
}

static void ezpi_service_uart_get_info()
{
    cJSON *cj_get_info = cJSON_CreateObject(__FUNCTION__);
    if (cj_get_info)
    {
        cJSON_AddNumberToObject(__FUNCTION__, cj_get_info, "cmd", 1);
        cJSON_AddStringToObject(__FUNCTION__, cj_get_info, "api", EZPI_VERSION_API_SERIAL);
        cJSON *cj_info = cJSON_CreateObject(__FUNCTION__);
        if (cj_info)
        {
            ezpi_service_uart_firmware_info(cj_info);
            ezpi_service_uart_chip_info(cj_info);
            ezpi_service_uart_firmware_sdk_info(cj_info);
            ezpi_service_uart_device_status_info(cj_info);
            ezpi_service_uart_config_info(cj_info);
            ezpi_service_uart_ezlopi_info(cj_info);
            ezpi_service_uart_oem_info(cj_info);
            ezpi_service_uart_newtwork_info(cj_info);

            cJSON_AddItemToObject(__FUNCTION__, cj_get_info, ezlopi_info_str, cj_info);

            char *serial_data_json_string = cJSON_Print(__FUNCTION__, cj_get_info);
            if (serial_data_json_string)
            {
                cJSON_Minify(serial_data_json_string);
                EZPI_SERV_uart_tx_data(strlen(serial_data_json_string), (uint8_t *)serial_data_json_string);
                ezlopi_free(__FUNCTION__, serial_data_json_string);
            }

            cJSON_Delete(__FUNCTION__, cj_get_info);
        }
    }
}

static void ezpi_service_uart_set_wifi(const char *data)
{
    uint32_t status = 0;
    cJSON *root = cJSON_Parse(__FUNCTION__, data);
    uint8_t status_write = 0;

    if (root)
    {
        cJSON *cj_wifi_ssid = cJSON_GetObjectItem(__FUNCTION__, root, ezlopi_ssid_str);
        cJSON *cj_wifi_pass = cJSON_GetObjectItem(__FUNCTION__, root, ezlopi_pass_str);
        if (cj_wifi_ssid && cj_wifi_pass && cj_wifi_ssid->valuestring && cj_wifi_pass->valuestring)
        {
            char *ssid = cj_wifi_ssid->valuestring;
            char *pass = cj_wifi_pass->valuestring;

            if (strlen(pass) >= EZPI_CORE_WIFI_PASS_CHAR_MIN_LEN)
            {
                // TRACE_S("SSID: %s\tPass : %s", ssid, pass);
                if (EZPI_core_factory_info_v3_set_wifi(ssid, pass))
                {
                    status_write = 1;
                }
                // ezlopi_wifi_set_new_wifi_flag();
                uint8_t attempt = 1;
                while (attempt <= EZPI_CORE_WIFI_CONN_RETRY_ATTEMPT)
                {
                    EZPI_core_wifi_connect((const char *)ssid, (const char *)pass);
                    EZPI_core_wait_for_wifi_to_connect((uint32_t)EZPI_CORE_WIFI_CONN_ATTEMPT_INTERVAL);
                    s_ezlopi_net_status_t *net_stat = EZPI_core_net_get_net_status();
                    if (net_stat)
                    {
                        net_stat->nma_cloud_connection_status = EZPI_service_webprov_is_connected();

                        if (net_stat->wifi_status->wifi_connection)
                        {
                            status = 1;
                            break;
                        }
                        else
                        {
                            TRACE_E("WiFi Connection to AP: %s failed !", ssid);
                            status = 0;
                        }
                    }
                    TRACE_W("Trying to connect to AP : %s, attempt %d ....", ssid, attempt);
                    attempt++;
                }

                ezpi_service_uart_response(EZPI_UART_CMD_WIFI, status_write, status);
            }
            else
            {
                TRACE_E("Invalid WiFi SSID or Password, aborting!");
                ezpi_service_uart_response(EZPI_UART_CMD_WIFI, status_write, status);
            }
        }

        cJSON_Delete(__FUNCTION__, root); // free Json string
    }
}

static void ezpi_service_uart_response(uint8_t cmd, uint8_t status_write, uint8_t status_connect)
{
    cJSON *response = NULL;
    response = cJSON_CreateObject(__FUNCTION__);

    if (response)
    {
        cJSON_AddNumberToObject(__FUNCTION__, response, ezlopi_cmd_str, cmd);

        switch (cmd)
        {
        case EZPI_UART_CMD_RESET:
        {
            break;
        }
        case EZPI_UART_CMD_INFO:
        {

            break;
        }
        case EZPI_UART_CMD_WIFI:
        {
            cJSON_AddNumberToObject(__FUNCTION__, response, ezlopi_status_str, status_connect);
            cJSON_AddNumberToObject(__FUNCTION__, response, ezlopi_status_write_str, status_write);
            break;
        }
        case EZPI_UART_CMD_SET_CONFIG:
        {
            cJSON_AddNumberToObject(__FUNCTION__, response, ezlopi_status_write_str, status_write);
            break;
        }
        // case EZPI_UART_CMD_GET_CONFIG:
        // {
        //     break;
        // }
        // Config already in handler
        // case EZPI_UART_CMD_UART_CONFIG:
        // {
        //     // cJSON_AddNumberToObject(__FUNCTION__, response, ezlopi_status_write_str, status_write);
        //     break;
        // }
        // case EZPI_UART_CMD_LOG_CONFIG:
        // {
        //     cJSON_AddNumberToObject(__FUNCTION__, response, ezlopi_status_write_str, status_write);
        //     break;
        // }
        // case EZPI_UART_CMD_SET_PROV:
        // {
        //     cJSON_AddNumberToObject(__FUNCTION__, response, ezlopi_status_write_str, status_write);
        //     break;
        // }
        default:
        {
            TRACE_OTEL(ENUM_EZLOPI_TRACE_SEVERITY_ERROR, "UART: Invalid command!");
            TRACE_E("Invalid command!");
            break;
        }
        }

        char *my_json_string = cJSON_Print(__FUNCTION__, response);
        cJSON_Delete(__FUNCTION__, response); // free Json string

        if (my_json_string)
        {
            cJSON_Minify(my_json_string);
            EZPI_SERV_uart_tx_data(strlen(my_json_string), (uint8_t *)my_json_string);
            ezlopi_free(__FUNCTION__, my_json_string);
        }
    }
}

static void ezpi_service_uart_set_config(const char *data)
{

    cJSON *cjson_config = cJSON_Parse(__FUNCTION__, data);
    if (cjson_config)
    {
        uint8_t ret = EZPI_core_factory_info_v3_set_ezlopi_config(cjson_config);
        if (ret)
        {
            TRACE_I("Successfully wrote config data..");
            ezpi_service_uart_response(EZPI_UART_CMD_SET_CONFIG, EZPI_UART_CMD_STATUS_SUCCESS, 0);
        }
        else
        {
            ezpi_service_uart_response(EZPI_UART_CMD_SET_CONFIG, EZPI_UART_CMD_STATUS_FAIL, 0);
        }
        cJSON_Delete(__FUNCTION__, cjson_config);
    }
}

static void ezpi_service_uart_get_config(void)
{
    cJSON *cj_root = NULL;

    char *current_config = EZPI_core_factory_info_v3_get_ezlopi_config();

    if (current_config)
    {
        // TRACE_D("current_config[len: %d]: %s", strlen(current_config), current_config);
        cj_root = cJSON_Parse(__FUNCTION__, current_config);

        if (cj_root)
        {
            cJSON_DeleteItemFromObject(__FUNCTION__, cj_root, ezlopi_cmd_str);

            // cJSON* device_total = cJSON_GetObjectItem(__FUNCTION__, cj_root, "dev_total");
            // if (device_total)
            // {
            //     if (cJSON_IsNumber(device_total))
            //     {
            //         cJSON_DeleteItemFromObject(__FUNCTION__, cj_root, "dev_total");
            //     }
            //     cJSON_Delete(__FUNCTION__, device_total);
            // }

            cJSON_AddNumberToObject(__FUNCTION__, cj_root, ezlopi_cmd_str, EZPI_UART_CMD_GET_CONFIG);
            cJSON_AddNumberToObject(__FUNCTION__, cj_root, ezlopi_status_str, EZPI_UART_CMD_STATUS_SUCCESS);
        }
        ezlopi_free(__FUNCTION__, current_config);
    }
    else
    {

        TRACE_E("'current_config' is null!");
        TRACE_OTEL(ENUM_EZLOPI_TRACE_SEVERITY_ERROR, "UART: 'current-config' is null!");
    }

    if (NULL == cj_root)
    {
        TRACE_E("Reading config failed!");
        cj_root = cJSON_CreateObject(__FUNCTION__);
        if (cj_root)
        {
            cJSON_AddNumberToObject(__FUNCTION__, cj_root, ezlopi_cmd_str, EZPI_UART_CMD_GET_CONFIG);
            cJSON_AddNumberToObject(__FUNCTION__, cj_root, ezlopi_status_str, EZPI_UART_CMD_STATUS_FAIL);
        }
        else
        {
            TRACE_E("Failed to create 'cj_root'!");
            TRACE_OTEL(ENUM_EZLOPI_TRACE_SEVERITY_ERROR, "UART: config json parsing fialed!");
        }
    }

    if (cj_root)
    {
        char *my_json_string = cJSON_Print(__FUNCTION__, cj_root);
        cJSON_Delete(__FUNCTION__, cj_root); // free Json string

        TRACE_D("length of 'my_json_string': %d", strlen(my_json_string));

        if (my_json_string)
        {
            cJSON_Minify(my_json_string);
            const int len = strlen(my_json_string);
            EZPI_SERV_uart_tx_data(len, (uint8_t *)my_json_string); // Send the data over uart
            // TRACE_D("Sending: %s", my_json_string);
            ezlopi_free(__FUNCTION__, my_json_string);
        }
    }
}

int EZPI_SERV_uart_tx_data(int len, uint8_t *data)
{
    int ret = 0;
    ret = uart_write_bytes(EZPI_SERV_UART_NUM_DEFAULT, (void *)data, len);
    ret += uart_write_bytes(EZPI_SERV_UART_NUM_DEFAULT, "\r\n", 2);

#if !defined(CONFIG_IDF_TARGET_ESP32) && !defined(CONFIG_IDF_TARGET_ESP32C3)
    if (pdTRUE == xSemaphoreTake(usb_semaphore_handle, portMAX_DELAY))
    {
        tinyusb_cdcacm_write_queue(cdc_port, (uint8_t *)data, len);
        tinyusb_cdcacm_write_queue(cdc_port, (uint8_t *)"\r\n", 2);
        ret = tinyusb_cdcacm_write_flush(cdc_port, 0);
        xSemaphoreGive(usb_semaphore_handle);
    }
#endif // NOT defined CONFIG_IDF_TARGET_ESP32 or CONFIG_IDF_TARGET_ESP32C3

    return ret;
}

#if !defined(CONFIG_IDF_TARGET_ESP32) && !defined(CONFIG_IDF_TARGET_ESP32C3)
static void ezpi_tinyusb_cdc_rx_callback(int itf, cdcacm_event_t *event)
{
    if (CDC_EVENT_RX == event->type)
    {
        size_t rx_size = 0;
        uint8_t temporary_buffer[63];
        if (pdTRUE == xSemaphoreTake(usb_semaphore_handle, portMAX_DELAY))
        {
            esp_err_t error = tinyusb_cdcacm_read(itf, temporary_buffer, CONFIG_TINYUSB_CDC_RX_BUFSIZE, &rx_size);
            xSemaphoreGive(usb_semaphore_handle);
            if (ESP_OK != error)
            {
                TRACE_E("Error reading cdc data");
            }
            else if (rx_size > 9)
            {
                memcpy((void *)usb_rx_buffer + rx_buffer_pointer, temporary_buffer, rx_size);
                rx_buffer_pointer += rx_size;
                if (0x0d == temporary_buffer[rx_size - 2] && 0x0a == temporary_buffer[rx_size - 1])
                {
                    usb_rx_buffer[rx_buffer_pointer - 2] = '\0';
                    ezpi_service_uart_parser((const char *)usb_rx_buffer);
                    memset(usb_rx_buffer, 0, rx_buffer_pointer);
                    rx_buffer_pointer = 0;
                }
            }
        }
    }
    else if (CDC_EVENT_LINE_STATE_CHANGED == event->type)
    {
        int dtr = event->line_state_changed_data.dtr;
        int rts = event->line_state_changed_data.rts;
        TRACE_I("Line state changed on channel %d: DTR:%d, RTS:%d", itf, dtr, rts);
    }
    else
    {
        TRACE_E("Untracked CDC event(code: %d)", event->type);
    }
}
#endif // NOT defined CONFIG_IDF_TARGET_ESP32 or CONFIG_IDF_TARGET_ESP32C3

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
