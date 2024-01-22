/* WiFi station Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include "string.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_event_base.h"
#include "esp_log.h"

#include "lwip/err.h"
#include "lwip/sys.h"
#include "esp_netif_types.h"
// #include "esp_netif_lwip_internal.h"

#include "trace.h"
#include "ezlopi_service_uart.h"

#include "ezlopi_core_nvs.h"
#include "ezlopi_core_wifi.h"
#include "ezlopi_core_factory_info.h"
#include "ezlopi_core_wifi_err_reason.h"
#include "ezlopi_core_event_group.h"

#define EXAMPLE_ESP_MAXIMUM_RETRY 5

static esp_netif_t *wifi_sta_netif = NULL;
static esp_netif_ip_info_t my_ip;
static uint32_t new_wifi = 0;
static int s_retry_num = 0;
static char wifi_ssid_pass_global_buffer[64];
static int station_got_ip = 0;
static const char *const wifi_no_error_str = "NO_ERROR";
static const char *last_disconnect_reason = wifi_no_error_str;
static ll_ezlopi_wifi_event_upcall_t *__event_upcall_head = NULL;
static volatile bool ezlopi_flag_wifi_status = false;

static ll_ezlopi_wifi_event_upcall_t *ezlopi_wifi_event_upcall_create(f_ezlopi_wifi_event_upcall *upcall, void *arg);

void ezlopi_wifi_event_add(f_ezlopi_wifi_event_upcall *upcall, void *arg)
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

const char *ezlopi_wifi_get_last_disconnect_reason(void)
{
    return last_disconnect_reason;
}

esp_netif_ip_info_t *ezlopi_wifi_get_ip_infos(void)
{
    return &my_ip;
}

// static void alert_qt_wifi_fail(void)
// {
//     char *qt_resp = "{\"cmd\":2,\"status_write\":0,\"status_connect\":0}";
//     qt_serial_tx_data(strlen(qt_resp), (uint8_t *)qt_resp);
// }

int ezlopi_wifi_got_ip(void)
{
    return station_got_ip;
}

void ezlopi_wifi_set_new_wifi_flag(void)
{
    new_wifi = 1;
}

ezlopi_wifi_status_t *ezlopi_wifi_status(void)
{

    ezlopi_wifi_status_t *wifi_stat = (ezlopi_wifi_status_t *)malloc(sizeof(ezlopi_wifi_status_t));

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

static void __event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        esp_wifi_connect();
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        // event_data; //
        wifi_event_sta_disconnected_t *disconnected = (wifi_event_sta_disconnected_t *)event_data;
        TRACE_E("Disconnect reason[%d]: %s", disconnected->reason, ezlopi_wifi_err_reason_str(disconnected->reason));
        last_disconnect_reason = ezlopi_wifi_err_reason_str(disconnected->reason);
        ezlopi_flag_wifi_status = false;

        station_got_ip = 0;
        esp_wifi_connect();

        if (s_retry_num < EXAMPLE_ESP_MAXIMUM_RETRY)
        {
            s_retry_num++;
            TRACE_I("retry to connect to the AP");
        }
        else
        {
            ezlopi_event_group_set_event(EZLOPI_EVENT_WIFI_FAIL);
            s_retry_num = 0;
        }
        TRACE_W("connect to the AP fail");
        memset(&my_ip, 0, sizeof(esp_netif_ip_info_t));
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        last_disconnect_reason = wifi_no_error_str;
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        TRACE_I("got - ip:      " IPSTR, IP2STR(&event->ip_info.ip));
        TRACE_I("      netmask: " IPSTR, IP2STR(&event->ip_info.netmask));
        TRACE_I("      gw:      " IPSTR, IP2STR(&event->ip_info.gw));

        station_got_ip = 1;
        s_retry_num = 0;

        memcpy(&my_ip, &event->ip_info, sizeof(esp_netif_ip_info_t));
        ezlopi_event_group_set_event(EZLOPI_EVENT_WIFI_CONNECTED);
        ezlopi_flag_wifi_status = true;

        // alert_qt_wifi_got_ip();
    }

    ll_ezlopi_wifi_event_upcall_t *curr_upcall = __event_upcall_head;
    while (curr_upcall)
    {
        if (curr_upcall->upcall)
        {
            curr_upcall->upcall(event_base, curr_upcall->arg);
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
    char *wifi_ssid = ezlopi_factory_info_v3_get_ssid();
    char *wifi_password = ezlopi_factory_info_v3_get_password();
    ezlopi_wifi_set_new_wifi_flag();

    if ((NULL != wifi_ssid) && ('\0' != wifi_ssid[0]) &&
        (NULL != wifi_password) && ('\0' != wifi_password[0]))
    {
        memset(wifi_ssid_pass_global_buffer, 0, sizeof(wifi_ssid_pass_global_buffer));
        snprintf(&wifi_ssid_pass_global_buffer[00], 31, "%s", wifi_ssid);
        snprintf(&wifi_ssid_pass_global_buffer[32], 31, "%s", wifi_password);
    }
    else
    {
        strcpy(&wifi_ssid_pass_global_buffer[00], "ezlopitest");
        strcpy(&wifi_ssid_pass_global_buffer[32], "ezlopitest");
    }

    ezlopi_factory_info_v3_free(wifi_ssid);
    ezlopi_factory_info_v3_free(wifi_password);

    esp_err_t wifi_error = ezlopi_wifi_connect(&wifi_ssid_pass_global_buffer[0], &wifi_ssid_pass_global_buffer[32]);
    TRACE_W("wifi_error: %u", wifi_error);
}

void ezlopi_wifi_connect_from_nvs(void)
{
    memset(wifi_ssid_pass_global_buffer, 0, sizeof(wifi_ssid_pass_global_buffer));
    ezlopi_nvs_read_wifi(wifi_ssid_pass_global_buffer, sizeof(wifi_ssid_pass_global_buffer));

    if (wifi_ssid_pass_global_buffer[0] == 0)
    {
        strcpy(&wifi_ssid_pass_global_buffer[00], "ezlopitest");
        strcpy(&wifi_ssid_pass_global_buffer[32], "ezlopitest");
        ezlopi_wifi_set_new_wifi_flag();
    }

    esp_err_t wifi_error = ezlopi_wifi_connect(&wifi_ssid_pass_global_buffer[0], &wifi_ssid_pass_global_buffer[32]);
    TRACE_W("wifi_error: %u", wifi_error);
}

esp_err_t ezlopi_wifi_connect(const char *ssid, const char *pass)
{
    esp_err_t err = ESP_OK;

    if ((NULL != ssid) && (NULL != pass))
    {
        if ((0 != strncmp(ssid, &wifi_ssid_pass_global_buffer[0], 32)) || ((0 != strncmp(pass, &wifi_ssid_pass_global_buffer[32], 32))))
        {
            ezlopi_wifi_set_new_wifi_flag();
            strncpy((char *)&wifi_ssid_pass_global_buffer[0], ssid, 32);
            strncpy((char *)&wifi_ssid_pass_global_buffer[32], pass, 32);
        }

        TRACE_D("SSID: %s, Password: %s,\r\n", ssid, pass);

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
                        TRACE_W("'tcpip_adapter_set_hostname' ERROR: %s", esp_err_to_name(err));
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
    while (-1 == ezlopi_event_group_wait_for_event(EZLOPI_EVENT_WIFI_CONNECTED, wait_time_ms, 0))
    {
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }

    ret = ezlopi_event_group_wait_for_event(EZLOPI_EVENT_WIFI_CONNECTED, wait_time_ms, 0);
    return ret;
}

static ll_ezlopi_wifi_event_upcall_t *ezlopi_wifi_event_upcall_create(f_ezlopi_wifi_event_upcall *upcall, void *arg)
{
    ll_ezlopi_wifi_event_upcall_t *_upcall = malloc(sizeof(ll_ezlopi_wifi_event_upcall_t));
    if (_upcall)
    {
        _upcall->arg = arg;
        _upcall->upcall = upcall;
        _upcall->next = NULL;
    }

    return _upcall;
}