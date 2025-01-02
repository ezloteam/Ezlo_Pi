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
/**
 * @file    ezlopi_core_wifi.c
 * @brief   perform some function on wifi-operations
 * @author  xx
 * @version 0.1
 * @date    12th DEC 2024
 */

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/

#include <string.h>

#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_system.h"
#include "esp_event_base.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "lwip/err.h"
#include "lwip/sys.h"

#include "cjext.h"
#include "ezlopi_util_trace.h"

#include "ezlopi_core_nvs.h"
#include "ezlopi_core_ping.h"
#include "ezlopi_core_sntp.h"
#include "ezlopi_core_wifi.h"
#include "ezlopi_core_ping.h"
#include "ezlopi_core_errors.h"
#include "ezlopi_core_processes.h"
#include "ezlopi_core_broadcast.h"
#include "ezlopi_core_event_group.h"
#include "ezlopi_core_factory_info.h"
#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_core_wifi_err_reason.h"
#include "ezlopi_core_device_value_updated.h"
// #include "ezlopi_core_errors.h"

#include "ezlopi_cloud_constants.h"

#include "ezlopi_service_uart.h"
#include "EZLOPI_USER_CONFIG.h"

/*******************************************************************************
 *                          Extern Data Declarations
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Declarations
 *******************************************************************************/

/*******************************************************************************
 *                          Type & Macro Definitions
 *******************************************************************************/
#define EXAMPLE_ESP_MAXIMUM_RETRY 5

/*******************************************************************************
 *                          Static Function Prototypes
 *******************************************************************************/
static void __event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);
static void __ezlopi_wifi_broadcast_scan_report(bool state);
static void __ezlopi_wifi_scanner_task(void *params);
static void __event_ip_got_ip(void *event_data);
static void __event_wifi_scan_done(void *event_data);
static void __event_wifi_disconnected(void *event_data);
static ll_ezlopi_wifi_event_upcall_t *ezlopi_wifi_event_upcall_create(f_ezlopi_wifi_event_upcall upcall, void *arg);
static esp_err_t set_wifi_station_host_name(void);
static int __ezlopi_wifi_wait_for_wifi_and_registration();

/*******************************************************************************
 *                          Static Data Definitions
 *******************************************************************************/
static esp_netif_ip_info_t sg_my_ip;
static esp_netif_t *sg_wifi_sta_netif = NULL;

static int sg_retry_num = 0;
static volatile int sg_station_got_ip = 0;
static const char *const scg_wifi_no_error_str = "NO_ERROR";
static const char *wifi_scanner_task_name = "WiFiScanTask";
static const char *wifi_try_connect_task_name = "WiFitrycnttask";
static const char *sg_last_disconnect_reason = scg_wifi_no_error_str;

static ll_ezlopi_wifi_event_upcall_t *__event_upcall_head = NULL;
// This task name is used to get the task handle while deleting the scanner task. Also, the task name should be < 16 to get handle using the freeRTOS API.
static TaskHandle_t sg_scan_handle = NULL;
static wifi_ap_record_t *ap_record = NULL;
static uint16_t total_wifi_APs_available = 0;

/*******************************************************************************
 *                          Extern Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Definitions
 *******************************************************************************/
void EZPI_core_wifi_event_add(f_ezlopi_wifi_event_upcall upcall, void *arg)
{
    if (__event_upcall_head)
    {
        ll_ezlopi_wifi_event_upcall_t *curr_upcall_head = __event_upcall_head;
        while (curr_upcall_head->next)
        {
            curr_upcall_head = curr_upcall_head->next;
        }

        curr_upcall_head->next = ezlopi_wifi_event_upcall_create(upcall, arg);
    }
    else
    {
        __event_upcall_head = ezlopi_wifi_event_upcall_create(upcall, arg);
    }
}

const char *EZPI_core_wifi_get_last_disconnect_reason(void)
{
    return sg_last_disconnect_reason;
}

esp_netif_ip_info_t *EZPI_core_wifi_get_ip_infos(void)
{
    return &sg_my_ip;
}

int EZPI_core_wifi_got_ip(void)
{
    return sg_station_got_ip;
}

ezlopi_wifi_status_t *EZPI_core_wifi_status(void)
{

    ezlopi_wifi_status_t *wifi_stat = (ezlopi_wifi_status_t *)ezlopi_malloc(__FUNCTION__, sizeof(ezlopi_wifi_status_t));

    if (sg_station_got_ip)
    {
        wifi_stat->wifi_connection = true;
        wifi_stat->wifi_mode = WIFI_MODE_STA;
        wifi_stat->ip_info = EZPI_core_wifi_get_ip_infos();
    }
    else
    {
        wifi_stat->wifi_connection = false;
        wifi_stat->wifi_mode = WIFI_MODE_STA;
        wifi_stat->ip_info = EZPI_core_wifi_get_ip_infos();
    }

    return wifi_stat;
}

int EZPI_core_wifi_get_auth_mode_str(char auth_str[50], wifi_auth_mode_t mode)
{
    int ret = 0;
    memset(auth_str, 0, 50);
    char *auth_mode_str = "";
    switch (mode)
    {
    case WIFI_AUTH_OPEN:
    {
        auth_mode_str = "open";
        break;
    }
    case WIFI_AUTH_WEP:
    {
        auth_mode_str = "wep";
        break;
    }
    case WIFI_AUTH_WPA_PSK:
    {
        auth_mode_str = "psk";
        break;
    }
    case WIFI_AUTH_WPA2_PSK:
    {
        auth_mode_str = "psk2";
        break;
    }
    case WIFI_AUTH_WPA_WPA2_PSK:
    {
        auth_mode_str = "psk2";
        break;
    }
    case WIFI_AUTH_WPA2_ENTERPRISE:
    {
        auth_mode_str = "enterprise";
        break;
    }
    case WIFI_AUTH_WPA3_PSK:
    {
        auth_mode_str = "psk3";
        break;
    }
    case WIFI_AUTH_WPA2_WPA3_PSK:
    {
        auth_mode_str = "psk3";
        break;
    }
    case WIFI_AUTH_WAPI_PSK:
    {
        auth_mode_str = "psk3";
        break;
    }
    case WIFI_AUTH_MAX:
    default:
    {
        break;
    }
    }
    snprintf(auth_str, 50, "%s", auth_mode_str);
    return ret;
}

void EZPI_wifi_initialize(void)
{
    memset(&sg_my_ip, 0, sizeof(sg_my_ip));
    sg_wifi_sta_netif = esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_set_ps(WIFI_PS_MIN_MODEM);
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;

    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        WIFI_EVENT, ESP_EVENT_ANY_ID, &__event_handler, NULL, &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        IP_EVENT, IP_EVENT_STA_GOT_IP, &__event_handler, NULL, &instance_got_ip));
}

void EZPI_core_wifi_connect_from_id_bin(void)
{
    char *wifi_ssid = EZPI_core_factory_info_v3_get_ssid();
    char *wifi_password = EZPI_core_factory_info_v3_get_password();

    if ((NULL != wifi_ssid) && ('\0' != wifi_ssid[0]) &&
        (NULL != wifi_password) && ('\0' != wifi_password[0]))
    {
#ifndef CONFIG_EZPI_UTIL_TRACE_EN
        EZPI_core_wifi_connect(wifi_ssid, wifi_password);
#else
        esp_err_t wifi_error = EZPI_core_wifi_connect(wifi_ssid, wifi_password);
        TRACE_W("wifi_error: %u", wifi_error);
#endif
    }

    if (wifi_ssid)
        ezlopi_free(__FUNCTION__, wifi_ssid);
    if (wifi_password)
        ezlopi_free(__FUNCTION__, wifi_password);
}

esp_err_t EZPI_core_wifi_connect(const char *ssid, const char *pass)
{
    esp_err_t err = ESP_OK;

    if ((NULL != ssid) && (NULL != pass))
    {
        wifi_config_t wifi_config = {
            .sta = {
                .pmf_cfg = {.capable = true, .required = false},
            },
        };

        strncpy((char *)wifi_config.sta.ssid, ssid, sizeof(wifi_config.sta.ssid));
        strncpy((char *)wifi_config.sta.password, pass, sizeof(wifi_config.sta.password));
        wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;

        esp_wifi_stop();
        esp_err_t _err = esp_wifi_set_mode(WIFI_MODE_STA);
        // TRACE_W("WiFi Err : %s", esp_err_to_name(_err));
        if (_err == ESP_OK)
        {
            _err = esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
            // TRACE_W("WiFi Err : %s", esp_err_to_name(_err));
            if (_err == ESP_OK)
            {
                _err = esp_wifi_start();
                // TRACE_W("WiFi Err : %s", esp_err_to_name(_err));
                if (_err == ESP_OK)
                {
                    _err = set_wifi_station_host_name();
                    // TRACE_W("WiFi Err : %s", esp_err_to_name(_err));
                    if (_err != ESP_OK)
                    {
                        TRACE_W("'tcpip_adapter_set_hostname' ERROR: %s", esp_err_to_name(err));
                    }
                }
            }
        }
        err = _err;
    }
    TRACE_W("WiFi Err : %s", esp_err_to_name(err));
    return err;
}

ezlopi_error_t EZPI_core_wait_for_wifi_to_connect(uint32_t wait_time_ms)
{
    ezlopi_error_t ret = EZPI_FAILED;
    while (EZPI_SUCCESS != EZPI_core_event_group_wait_for_event(EZLOPI_EVENT_WIFI_CONNECTED, wait_time_ms, false))
    {
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }

    ret = EZPI_core_event_group_wait_for_event(EZLOPI_EVENT_WIFI_CONNECTED, wait_time_ms, false);
    return ret;
}

void ezlopi_wifi_try_connect_task(void *params)
{
    cJSON *cj_network = (cJSON *)params;
    if (cj_network)
    {
        while (1)
        {
            char ssid[32];
            char pass[32];
            CJSON_GET_VALUE_STRING_BY_COPY(cj_network, ezlopi_ssid_str, ssid);
            CJSON_GET_VALUE_STRING_BY_COPY(cj_network, ezlopi_key_str, pass);
            if (('\0' != ssid[0]) && ('\0' != pass))
            {
                TRACE_D("Trying to connect to %s with password %s", ssid, pass);
                esp_err_t error = EZPI_core_wifi_connect(ssid, pass);
                if (ESP_OK == error)
                {
                    int ret = __ezlopi_wifi_wait_for_wifi_and_registration();
                    TRACE_Dw("Ret: %d", ret);
                    if (-1 == ret)
                    {
                        TRACE_E("Error connecting to wifi with new WiFi SSID");
                        EZPI_core_wifi_connect_from_id_bin();
                        __ezlopi_wifi_wait_for_wifi_and_registration();
                    }
                }
                else
                {
                    TRACE_E("Error connecting to wifi with new WiFi SSID");
                    EZPI_core_wifi_connect_from_id_bin();
                    __ezlopi_wifi_wait_for_wifi_and_registration();
                }
            }
            break;
        }
    }
#if defined(CONFIG_FREERTOS_USE_TRACE_FACILITY)
    EZPI_core_process_set_is_deleted(ENUM_EZLOPI_CORE_WIFI_TRY_CONNECT_TASK);
#endif
    vTaskDelete(NULL);
}

int EZPI_core_wifi_try_new_connect(cJSON *cj_network)
{
    int ret = 0;
    cJSON *cj_network_copy = cJSON_Duplicate(__FUNCTION__, cj_network, true);
    static TaskHandle_t try_connect_task_handle = NULL;
    xTaskCreate(ezlopi_wifi_try_connect_task, wifi_try_connect_task_name, EZLOPI_CORE_WIFI_TRY_CONNECT_TASK_DEPTH, cj_network_copy, 3, &try_connect_task_handle);
#if defined(CONFIG_FREERTOS_USE_TRACE_FACILITY)
    EZPI_core_process_set_process_info(ENUM_EZLOPI_CORE_WIFI_TRY_CONNECT_TASK, &try_connect_task_handle, EZLOPI_CORE_WIFI_TRY_CONNECT_TASK_DEPTH);
#endif
    return ret;
}

void EZPI_core_wifi_scan_stop()
{
    TRACE_E("Stopping the scanning process.");
    esp_wifi_scan_stop();

    if (sg_scan_handle)
    {
        TRACE_E("Resetting WiFi scanner task.(handle: %p)", sg_scan_handle);
#if defined(CONFIG_FREERTOS_USE_TRACE_FACILITY)
        EZPI_core_process_set_is_deleted(ENUM_EZLOPI_CORE_WIFI_SCANNER_TASK);
#endif
        vTaskDelete(sg_scan_handle);
        sg_scan_handle = NULL;
    }

    TRACE_E("Deleting previous record.");

    if (ap_record)
    {
        ezlopi_free(__FUNCTION__, ap_record);
        ap_record = NULL;
    }
}

void EZPI_core_wifi_scan_start()
{
    EZPI_core_wifi_scan_stop();
    if (NULL == sg_scan_handle)
    {
        xTaskCreate(__ezlopi_wifi_scanner_task, wifi_scanner_task_name, EZLOPI_CORE_WIFI_SCANNER_TASK_DEPTH, NULL, 3, &sg_scan_handle);
#if defined(CONFIG_FREERTOS_USE_TRACE_FACILITY)
        EZPI_core_process_set_process_info(ENUM_EZLOPI_CORE_WIFI_SCANNER_TASK, &sg_scan_handle, EZLOPI_CORE_WIFI_SCANNER_TASK_DEPTH);
#endif
    }
}

int EZPI_core_wifi_get_wifi_mac(uint8_t mac[6])
{
    int ret = 0;
    if (ESP_OK == esp_wifi_get_mac(WIFI_IF_STA, mac))
    {
        ret = 1;
    }
    return ret;
}

/*******************************************************************************
 *                         Static Function Definitions
 *******************************************************************************/

static esp_err_t set_wifi_station_host_name(void)
{
    static char station_host_name[32];
    snprintf(station_host_name, sizeof(station_host_name), "EZLOPI-%llu", EZPI_core_factory_info_v3_get_id());
    esp_err_t err = tcpip_adapter_set_hostname(TCPIP_ADAPTER_IF_STA, station_host_name);
    return err;
}

static void __event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    if (WIFI_EVENT == event_base)
    {
        switch (event_id)
        {
        case WIFI_EVENT_STA_START:
        {
            esp_wifi_connect();
            break;
        }
        case WIFI_EVENT_STA_DISCONNECTED:
        {
            __event_wifi_disconnected(event_data);
            break;
        }
        case WIFI_EVENT_SCAN_DONE:
        {
            __event_wifi_scan_done(event_data);
            break;
        }
        case WIFI_EVENT_STA_STOP:
        {
            TRACE_D("wifi-stopped!");
            break;
        }
        case WIFI_EVENT_STA_CONNECTED:
        {
            TRACE_D("wifi-connected!");
            break;
        }
        default:
        {
            TRACE_W("un-known wifi-event received: %d", event_id);
            break;
        }
        }
    }
    else if (IP_EVENT == event_base)
    {
        switch (event_id)
        {
        case IP_EVENT_STA_GOT_IP:
        {
            __event_ip_got_ip(event_data);
            break;
        }
        default:
        {
            TRACE_W("un-known ip-event received: %d", event_id);
            break;
        }
        }
    }
    else
    {
        TRACE_W("unkown event received, event-base: %d, event-id: %d", (uint32_t)event_base, event_id);
    }

    ll_ezlopi_wifi_event_upcall_t *curr_upcall = __event_upcall_head;
    while (curr_upcall)
    {
        if (curr_upcall->upcall)
        {
            curr_upcall->upcall(event_base, event_id, curr_upcall->arg);
        }
        curr_upcall = curr_upcall->next;
    }
}

static int __ezlopi_wifi_wait_for_wifi_and_registration()
{
    int ret = 0;
    int count = 0;
    ret = EZPI_core_wait_for_wifi_to_connect(15000 / portTICK_PERIOD_MS);
    if (-1 == ret)
    {
        TRACE_E("Failed to connect to the wifi");
    }
    else
    {
        TRACE_I("WiFi connected successully");
        do
        {
            count++;
            TRACE_D("Waiting for device internet connection to complete...(%d)", count);
            e_ezlopi_event_t event = EZPI_core_event_group_get_eventbit_status();
            if (EZLOPI_EVENT_NMA_REG == (event & EZLOPI_EVENT_NMA_REG))
            {
                TRACE_Dw("Device registered successfully");
                ret = 0;
                break;
            }
            else if (count == 15)
            {
                TRACE_Dw("Couldn't register the device with new wifi");
                ret = -1;
                break;
            }
            vTaskDelay(1500 / portTICK_PERIOD_MS);
        } while (1);
    }
    return ret;
}

static ll_ezlopi_wifi_event_upcall_t *ezlopi_wifi_event_upcall_create(f_ezlopi_wifi_event_upcall upcall, void *arg)
{
    ll_ezlopi_wifi_event_upcall_t *_upcall = ezlopi_malloc(__FUNCTION__, sizeof(ll_ezlopi_wifi_event_upcall_t));
    if (_upcall)
    {
        _upcall->arg = arg;
        _upcall->upcall = upcall;
        _upcall->next = NULL;
    }
    else
    {
        TRACE_E("malloc failed!");
    }

    return _upcall;
}

// 0: start, 1: stop
static void __ezlopi_wifi_broadcast_scan_report(bool state)
{
    cJSON *cj_scan_report = cJSON_CreateObject(__FUNCTION__);
    if (cj_scan_report)
    {
        // cJSON_AddNumberToObject(__FUNCTION__, cj_scan_report, ezlopi_startTime_str, EZPI_core_sntp_get_current_time_sec());

        cJSON_AddStringToObject(__FUNCTION__, cj_scan_report, ezlopi_id_str, ezlopi_ui_broadcast_str);
        cJSON_AddStringToObject(__FUNCTION__, cj_scan_report, ezlopi_msg_subclass_str, method_hub_network_wifi_scan_progress);

        cJSON *cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_scan_report, ezlopi_result_str);
        if (cj_result)
        {
            cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_interfaceId_str, ezlopi_wlan0_str);
            if (0 == state)
            {
                cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_status_str, scene_status_started_str);
            }
            else
            {
                cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_status_str, "finished");
            }
        }

        if (EZPI_SUCCESS != EZPI_core_broadcast_add_to_queue(cj_scan_report, EZPI_core_sntp_get_current_time_sec()))
        {
            cJSON_Delete(__FUNCTION__, cj_scan_report);
        }
    }
}

static void __ezlopi_wifi_scanner_task(void *params)
{
    TickType_t start_time = xTaskGetTickCount();
    TickType_t current_time, previous_scan_time = 0;

    wifi_scan_config_t scan_config = {
        .ssid = 0,
        .bssid = 0,
        .channel = 0,
        .show_hidden = false,
        .scan_type = WIFI_SCAN_TYPE_PASSIVE,
        .scan_time.active.min = 120,
        .scan_time.active.max = 150,
    };

    __ezlopi_wifi_broadcast_scan_report(0);

    while (1)
    {
        current_time = (xTaskGetTickCount() - start_time);
        if (current_time <= (5 * 60 * configTICK_RATE_HZ))
        {
            TRACE_I("Scanning time: %d", current_time);
            // Scanning for available AP for about 30s. This is because, the scanning tends to prevent data TX/RX over wifi.
            if (current_time - previous_scan_time >= 30000)
            {
                TRACE_E("Starting the scan, (%d)", current_time);
                ESP_ERROR_CHECK(esp_wifi_scan_start(&scan_config, true));
                TRACE_E("Scanning completed.");
                previous_scan_time = current_time;
            }
        }
        else
        {
            TRACE_I("5 minutes passed, stopping scanner.");
            break;
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    __ezlopi_wifi_broadcast_scan_report(1);
#if defined(CONFIG_FREERTOS_USE_TRACE_FACILITY)
    EZPI_core_process_set_is_deleted(ENUM_EZLOPI_CORE_WIFI_SCANNER_TASK);
#endif
    sg_scan_handle = NULL;
    vTaskDelete(NULL);
}

static void __event_wifi_disconnected(void *event_data)
{
    EZPI_core_event_group_clear_event(EZLOPI_EVENT_WIFI_CONNECTED);

    if (event_data)
    {
        // event_data; //
        wifi_event_sta_disconnected_t *disconnected = (wifi_event_sta_disconnected_t *)event_data;
        TRACE_E("Disconnect reason[%d]: %s", disconnected->reason, EZPI_core_wifi_err_reason_str(disconnected->reason));
        sg_last_disconnect_reason = EZPI_core_wifi_err_reason_str(disconnected->reason);

        sg_station_got_ip = 0;
        esp_wifi_connect();

        if (sg_retry_num < EXAMPLE_ESP_MAXIMUM_RETRY)
        {
            sg_retry_num++;
            TRACE_S("retry to connect to the AP");
        }
        else
        {
            EZPI_core_event_group_set_event(EZLOPI_EVENT_WIFI_FAIL);
            sg_retry_num = 0;
        }

        TRACE_W("connect to the AP fail");
        memset(&sg_my_ip, 0, sizeof(esp_netif_ip_info_t));
    }
}

static void __event_wifi_scan_done(void *event_data)
{
    if (event_data)
    {
        wifi_event_sta_scan_done_t *scan_event_param = (wifi_event_sta_scan_done_t *)event_data;
        TRACE_I("status: %d, event data: %d", scan_event_param->status, scan_event_param->number);
        if (scan_event_param->status == 0)
        {
            total_wifi_APs_available = scan_event_param->number;
            ap_record = (wifi_ap_record_t *)ezlopi_malloc(__FUNCTION__, total_wifi_APs_available * sizeof(wifi_ap_record_t));
            ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&total_wifi_APs_available, ap_record));
            cJSON *network_array = cJSON_CreateArray(__FUNCTION__);
            if (network_array)
            {
                char temporary[50];
                for (int i = 0; i < total_wifi_APs_available; i++)
                {
                    cJSON *network_data = cJSON_CreateObject(__FUNCTION__);
                    if (network_data)
                    {
                        memset(temporary, 0, 50);
                        memcpy(temporary, ap_record[i].ssid, 33);
                        cJSON_AddStringToObject(__FUNCTION__, network_data, "ssid", temporary);
                        memset(temporary, 0, 50);
                        snprintf(temporary, 50, "%02x:%02x:%02x:%02x:%02x:%02x", ap_record[i].bssid[0], ap_record[i].bssid[1], ap_record[i].bssid[2],
                                 ap_record[i].bssid[3], ap_record[i].bssid[0], ap_record[i].bssid[5]);
                        cJSON_AddStringToObject(__FUNCTION__, network_data, "bssid", temporary);
                        cJSON_AddNumberToObject(__FUNCTION__, network_data, "rssi", ap_record[i].rssi);
                        EZPI_core_wifi_get_auth_mode_str(temporary, ap_record[i].authmode);
                        cJSON_AddStringToObject(__FUNCTION__, network_data, "security", temporary);
                        cJSON_AddItemToArray(network_array, network_data);
                    }
                }

                EZPI_core_device_value_update_wifi_scan_broadcast(network_array);
            }

            ezlopi_free(__FUNCTION__, ap_record);
            ap_record = NULL;
        }
    }
}

static void __event_ip_got_ip(void *event_data)
{

    if (event_data)
    {
        sg_last_disconnect_reason = scg_wifi_no_error_str;
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        TRACE_I("got - ip:      " IPSTR, IP2STR(&event->ip_info.ip));
        TRACE_I("      netmask: " IPSTR, IP2STR(&event->ip_info.netmask));
        TRACE_I("      gw:      " IPSTR, IP2STR(&event->ip_info.gw));

        sg_station_got_ip = 1;
        sg_retry_num = 0;

        memcpy(&sg_my_ip, &event->ip_info, sizeof(esp_netif_ip_info_t));
    }
    vTaskDelay(1);
    EZPI_core_event_group_set_event(EZLOPI_EVENT_WIFI_CONNECTED);
}

/*******************************************************************************
 *                          End of File
 *******************************************************************************/