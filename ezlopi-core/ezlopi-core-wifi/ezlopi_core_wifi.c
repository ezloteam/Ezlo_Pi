/* WiFi station Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_event_base.h"
#include "esp_log.h"
#include "cJSON.h"

#include "lwip/err.h"
#include "lwip/sys.h"

#include "ezlopi_util_trace.h"

#include "ezlopi_core_nvs.h"
#include "ezlopi_core_wifi.h"
#include "ezlopi_core_factory_info.h"
#include "ezlopi_core_wifi_err_reason.h"
#include "ezlopi_core_event_group.h"
#include "ezlopi_core_device_value_updated.h"

#include "ezlopi_service_uart.h"

#define EXAMPLE_ESP_MAXIMUM_RETRY 5

static esp_netif_t* wifi_sta_netif = NULL;
static esp_netif_ip_info_t my_ip;
// static uint32_t new_wifi = 0;
static int s_retry_num = 0;
// static char wifi_ssid_pass_global_buffer[EZLOPI_FINFO_LEN_WIFI_SSID + EZLOPI_FINFO_LEN_WIFI_PASS];
static int station_got_ip = 0;
static const char* const wifi_no_error_str = "NO_ERROR";
static const char* last_disconnect_reason = wifi_no_error_str;
static ll_ezlopi_wifi_event_upcall_t* __event_upcall_head = NULL;
static volatile bool ezlopi_flag_wifi_status = false;

// This task name is used to get the task handle while deleting the scanner task. Also, the task name should be < 16 to get handle using the freeRTOS API.
static const char* wifi_scanner_task_name = "scanner_task";
static uint16_t total_wifi_APs_available = 0;
static wifi_ap_record_t* ap_record = NULL;

static ll_ezlopi_wifi_event_upcall_t* ezlopi_wifi_event_upcall_create(f_ezlopi_wifi_event_upcall upcall, void* arg);
static void ezlopi_wifi_scanner_task(void* params);

void ezlopi_wifi_event_add(f_ezlopi_wifi_event_upcall upcall, void* arg)
{
    if (__event_upcall_head)
    {
        ll_ezlopi_wifi_event_upcall_t* curr_upcall_head = __event_upcall_head;
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

const char* ezlopi_wifi_get_last_disconnect_reason(void)
{
    return last_disconnect_reason;
}

esp_netif_ip_info_t* ezlopi_wifi_get_ip_infos(void)
{
    return &my_ip;
}

int ezlopi_wifi_got_ip(void)
{
    return station_got_ip;
}

// void ezlopi_wifi_set_new_wifi_flag(void)
// {
//     new_wifi = 1;
// }

ezlopi_wifi_status_t* ezlopi_wifi_status(void)
{

    ezlopi_wifi_status_t* wifi_stat = (ezlopi_wifi_status_t*)malloc(sizeof(ezlopi_wifi_status_t));

    if (ezlopi_flag_wifi_status)
    {

        wifi_stat->wifi_connection = true;
        wifi_stat->wifi_mode = WIFI_MODE_STA;
        wifi_stat->ip_info = ezlopi_wifi_get_ip_infos();
    }
    else
    {
        wifi_stat->wifi_mode = WIFI_MODE_STA;
        wifi_stat->ip_info = ezlopi_wifi_get_ip_infos();
        wifi_stat->wifi_connection = false;
    }

    return wifi_stat;
}

static esp_err_t set_wifi_station_host_name(void)
{
    static char station_host_name[32];
    snprintf(station_host_name, sizeof(station_host_name), "EZLOPI-%llu", ezlopi_factory_info_v3_get_id());
    esp_err_t err = tcpip_adapter_set_hostname(TCPIP_ADAPTER_IF_STA, station_host_name);
    return err;
}

static int get_auth_mode_str(char auth_str[50], wifi_auth_mode_t mode)
{
    int ret = 0;
    memset(auth_str, 0, 50);
    char* auth_mode_str = "";
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
        auth_mode_str = "wpa-psk";
        break;
    }
    case WIFI_AUTH_WPA2_PSK:
    {
        auth_mode_str = "wpa2-psk";
        break;
    }
    case WIFI_AUTH_WPA_WPA2_PSK:
    {
        auth_mode_str = "wpa-wpa2-psk";
        break;
    }
    case WIFI_AUTH_WPA2_ENTERPRISE:
    {
        auth_mode_str = "wpa2-enterprise";
        break;
    }
    case WIFI_AUTH_WPA3_PSK:
    {
        auth_mode_str = "wpa2-psk";
        break;
    }
    case WIFI_AUTH_WPA2_WPA3_PSK:
    {
        auth_mode_str = "wpa2-wpa3-psk";
        break;
    }
    case WIFI_AUTH_WAPI_PSK:
    {
        auth_mode_str = "wapi-psk";
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

static void __event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        esp_wifi_connect();
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        ezlopi_event_group_clear_event(EZLOPI_EVENT_WIFI_CONNECTED);

        // event_data; //
        wifi_event_sta_disconnected_t* disconnected = (wifi_event_sta_disconnected_t*)event_data;
        TRACE_E("Disconnect reason[%d]: %s", disconnected->reason, ezlopi_wifi_err_reason_str(disconnected->reason));
        last_disconnect_reason = ezlopi_wifi_err_reason_str(disconnected->reason);
        ezlopi_flag_wifi_status = false;

        station_got_ip = 0;
        esp_wifi_connect();

        if (s_retry_num < EXAMPLE_ESP_MAXIMUM_RETRY)
        {
            s_retry_num++;
            TRACE_S("retry to connect to the AP");
        }
        else
        {
            ezlopi_event_group_set_event(EZLOPI_EVENT_WIFI_FAIL);
            s_retry_num = 0;
        }
        TRACE_W("connect to the AP fail");
        memset(&my_ip, 0, sizeof(esp_netif_ip_info_t));
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_SCAN_DONE)
    {
        wifi_event_sta_scan_done_t* scan_event_param = (wifi_event_sta_scan_done_t*)event_data;
        TRACE_I("status: %d, event data: %d", scan_event_param->status, scan_event_param->number);
        if (scan_event_param->status == 0)
        {
            total_wifi_APs_available = scan_event_param->number;
            ap_record = (wifi_ap_record_t*)malloc(total_wifi_APs_available * sizeof(wifi_ap_record_t));
            ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&total_wifi_APs_available, ap_record));
            cJSON* network_array = cJSON_CreateArray();
            if (network_array)
            {
                char temporary[50];
                for (int i = 0; i < total_wifi_APs_available; i++)
                {
                    cJSON* network_data = cJSON_CreateObject();
                    if (network_data)
                    {
                        memset(temporary, 0, 50);
                        memcpy(temporary, ap_record[i].ssid, 33);
                        cJSON_AddStringToObject(network_data, "ssid", temporary);
                        memset(temporary, 0, 50);
                        snprintf(temporary, 50, "%02x:%02x:%02x:%02x:%02x:%02x", ap_record[i].bssid[0], ap_record[i].bssid[1], ap_record[i].bssid[2],
                            ap_record[i].bssid[3], ap_record[i].bssid[0], ap_record[i].bssid[5]);
                        cJSON_AddStringToObject(network_data, "bssid", temporary);
                        cJSON_AddNumberToObject(network_data, "rssi", ap_record[i].rssi);
                        get_auth_mode_str(temporary, ap_record[i].authmode);
                        cJSON_AddStringToObject(network_data, "security", temporary);
                        cJSON_AddItemToArray(network_array, network_data);
                    }
                }
                ezlopi_network_update_wifi_scan_process(network_array);
            }
            free(ap_record);
            ap_record = NULL;
        }
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        last_disconnect_reason = wifi_no_error_str;
        ip_event_got_ip_t* event = (ip_event_got_ip_t*)event_data;
        TRACE_I("got - ip:      " IPSTR, IP2STR(&event->ip_info.ip));
        TRACE_I("      netmask: " IPSTR, IP2STR(&event->ip_info.netmask));
        TRACE_I("      gw:      " IPSTR, IP2STR(&event->ip_info.gw));

        station_got_ip = 1;
        s_retry_num = 0;

        memcpy(&my_ip, &event->ip_info, sizeof(esp_netif_ip_info_t));
        ezlopi_event_group_set_event(EZLOPI_EVENT_WIFI_CONNECTED);
        ezlopi_flag_wifi_status = true;
    }
    else
    {
        TRACE_E("unknown event:: event_base: %u, event_id: %d", (uint32_t)event_base, event_id);
    }

    ll_ezlopi_wifi_event_upcall_t* curr_upcall = __event_upcall_head;
    while (curr_upcall)
    {
        if (curr_upcall->upcall)
        {
            curr_upcall->upcall(event_base, event_id, curr_upcall->arg);
        }
        curr_upcall = curr_upcall->next;
    }
}

void ezlopi_wifi_initialize(void)
{
    memset(&my_ip, 0, sizeof(my_ip));
    wifi_sta_netif = esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;

    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        WIFI_EVENT, ESP_EVENT_ANY_ID, &__event_handler, NULL, &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        IP_EVENT, IP_EVENT_STA_GOT_IP, &__event_handler, NULL, &instance_got_ip));


}

void ezlopi_wifi_connect_from_id_bin(void)
{
    char* wifi_ssid = ezlopi_factory_info_v3_get_ssid();
    char* wifi_password = ezlopi_factory_info_v3_get_password();

    if ((NULL != wifi_ssid) && ('\0' != wifi_ssid[0]) &&
        (NULL != wifi_password) && ('\0' != wifi_password[0]))
    {
        esp_err_t wifi_error = ezlopi_wifi_connect(wifi_ssid, wifi_password);
        TRACE_W("wifi_error: %u", wifi_error);
    }
}

esp_err_t ezlopi_wifi_connect(const char* ssid, const char* pass)
{
    esp_err_t err = ESP_OK;

    if ((NULL != ssid) && (NULL != pass))
    {
        TRACE_D("SSID: %s, Password: %s,\r\n", ssid, pass);

        wifi_config_t wifi_config = {
            .sta = {
                .pmf_cfg = {.capable = true, .required = false},
            },
        };

        strncpy((char*)wifi_config.sta.ssid, ssid, sizeof(wifi_config.sta.ssid));
        strncpy((char*)wifi_config.sta.password, pass, sizeof(wifi_config.sta.password));
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
                    if (_err != ESP_OK) {
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

int ezlopi_wait_for_wifi_to_connect(uint32_t wait_time_ms)
{
    uint32_t ret = 0;
    while (-1 == ezlopi_event_group_wait_for_event(EZLOPI_EVENT_WIFI_CONNECTED, wait_time_ms, false))
    {
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }

    ret = ezlopi_event_group_wait_for_event(EZLOPI_EVENT_WIFI_CONNECTED, wait_time_ms, false);
    return ret;
}

static ll_ezlopi_wifi_event_upcall_t* ezlopi_wifi_event_upcall_create(f_ezlopi_wifi_event_upcall upcall, void* arg)
{
    ll_ezlopi_wifi_event_upcall_t* _upcall = malloc(sizeof(ll_ezlopi_wifi_event_upcall_t));
    if (_upcall)
    {
        _upcall->arg = arg;
        _upcall->upcall = upcall;
        _upcall->next = NULL;
    }

    return _upcall;
}

static void ezlopi_wifi_scanner_task(void* params)
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
    vTaskDelete(NULL);
}

void ezlopi_wifi_scan_stop()
{
    TRACE_E("Stopping the scanning process.");
    esp_wifi_scan_stop();
    TaskHandle_t scan_handle = xTaskGetHandle(wifi_scanner_task_name);
    if (scan_handle)
    {
        TRACE_E("Resetting WiFi scanner task.(handle: %p)", scan_handle);
        vTaskDelete(scan_handle);
    }
    TRACE_E("Deleting previous record.");
    if (ap_record)
    {
        free(ap_record);
        ap_record = NULL;
    }
}

void ezlopi_wifi_scan_start()
{
    ezlopi_wifi_scan_stop();
    xTaskCreate(ezlopi_wifi_scanner_task, wifi_scanner_task_name, 2 * 2048, NULL, 3, NULL);
}

uint8_t ezlopi_wifi_get_wifi_mac(uint8_t mac[6])
{
    int ret = 0;
    if (ESP_OK == esp_wifi_get_mac(WIFI_IF_STA, mac))
    {
        ret = 1;
    }
    return ret;
}
