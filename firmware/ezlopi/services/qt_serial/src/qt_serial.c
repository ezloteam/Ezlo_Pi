/* UART asynchronous example, that uses separate RX and TX tasks

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include "freertos/FreeRTOSConfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "string.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "driver/gpio.h"
#include "cJSON.h"
#include "debug.h"
#include "nvs_storage.h"
#include "qt_serial.h"
#include "wifi_interface.h"
#include "factory_info.h"
#include "version.h"

static const int RX_BUF_SIZE = 3096;

#define TXD_PIN (GPIO_NUM_1)
#define RXD_PIN (GPIO_NUM_3)
#define STORAGE_NAMESPACE "storage"
// cJson Types

void QT_GET_INFO();
void QT_SET_WIFI(const char *data);
void QT_RESPONE(uint8_t cmd, uint8_t status_write, uint8_t status_connect);
void QT_SET_DATA(const char *data);
void QT_READ_DATA(void);

#if 0
char *JSON_Types(int type)
{
    if (type == cJSON_Invalid)
        return ("cJSON_Invalid");
    if (type == cJSON_False)
        return ("cJSON_False");
    if (type == cJSON_True)
        return ("cJSON_True");
    if (type == cJSON_NULL)
        return ("cJSON_NULL");
    if (type == cJSON_Number)
        return ("cJSON_Number");
    if (type == cJSON_String)
        return ("cJSON_String");
    if (type == cJSON_Array)
        return ("cJSON_Array");
    if (type == cJSON_Object)
        return ("cJSON_Object");
    if (type == cJSON_Raw)
        return ("cJSON_Raw");
    return NULL;
}

void JSON_Analyze(const cJSON *const root)
{
    ////TRACE_I( "root->type=%s", JSON_Types(root->type));
    cJSON *current_element = NULL;
    ////TRACE_I( "roo->child=%p", root->child);
    ////TRACE_I( "roo->next =%p", root->next);
    cJSON_ArrayForEach(current_element, root)
    {
        ////TRACE_I( "type=%s", JSON_Types(current_element->type));
        ////TRACE_I( "current_element->string=%p", current_element->string);
        if (current_element->string)
        {
            const char *string = current_element->string;
            // TRACE_I( "[%s]", string);
        }
        if (cJSON_IsInvalid(current_element))
        {
            // TRACE_I( "Invalid");
        }
        else if (cJSON_IsFalse(current_element))
        {
            // TRACE_I( "False");
        }
        else if (cJSON_IsTrue(current_element))
        {
            // TRACE_I( "True");
        }
        else if (cJSON_IsNull(current_element))
        {
            // TRACE_I( "Null");
        }
        else if (cJSON_IsNumber(current_element))
        {
            int valueint = current_element->valueint;
            double valuedouble = current_element->valuedouble;
            // TRACE_I( "int=%d double=%f", valueint, valuedouble);
        }
        else if (cJSON_IsString(current_element))
        {
            const char *valuestring = current_element->valuestring;
            // TRACE_I( "%s", valuestring);
        }
        else if (cJSON_IsArray(current_element))
        {
            ////TRACE_I( "Array");
            JSON_Analyze(current_element);
        }
        else if (cJSON_IsObject(current_element))
        {
            ////TRACE_I( "Object");
            JSON_Analyze(current_element);
        }
        else if (cJSON_IsRaw(current_element))
        {
            // TRACE_I( "Raw(Not support)");
        }
    }
}
#endif

void UART_INIT(void)
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

int sendData(const char *logName, const char *data)
{
    cJSON *root = cJSON_Parse(data);

    if (root)
    {
        if (cJSON_GetObjectItem(root, "cmd"))
        {
            uint8_t cmd_temp = cJSON_GetObjectItem(root, "cmd")->valueint;

            switch (cmd_temp)
            {
            case 1:
                QT_GET_INFO();
                break;
            case 2:
                QT_SET_WIFI(data);
                break;
            case 3:
                QT_SET_DATA(data);
                break;
            case 4:
                QT_READ_DATA();
                break;

            default:
                TRACE_E("InValid command");
                break;
            }
        }

        cJSON_Delete(root);
    }
    return 1;
}

static void tx_task(const char *data)
{
    static const char *TX_TASK_TAG = "TX_TASK";
    esp_log_level_set(TX_TASK_TAG, ESP_LOG_INFO);
    sendData(TX_TASK_TAG, data);
}

static void rx_task(void *arg)
{
    static const char *RX_TASK_TAG = "RX_TASK";
    esp_log_level_set(RX_TASK_TAG, ESP_LOG_INFO);
    uint8_t *data = (uint8_t *)malloc(RX_BUF_SIZE + 1);

    while (1)
    {
        int rxBytes = uart_read_bytes(UART_NUM_0, data, RX_BUF_SIZE, 1000 / portTICK_RATE_MS);

        if (rxBytes > 0)
        {
            data[rxBytes] = 0;
            TRACE_I("%s", data);
            tx_task((const char *)data);
        }
    }

    free(data);
    vTaskDelete(NULL);
}

//---------------------------------QT-Funtions

void QT_GET_INFO()
{
    cJSON *get_info = cJSON_CreateObject();

    if (get_info)
    {
        char wifi_info[64];
        esp_chip_info_t chip_info;
        esp_chip_info(&chip_info);
        s_factory_info_t *factory = factory_info_get_info();
        nvs_storage_read_wifi(wifi_info, 64);

        if (factory)
        {
            cJSON_AddNumberToObject(get_info, "cmd", 1);
            cJSON_AddNumberToObject(get_info, "status", 1);
            cJSON_AddNumberToObject(get_info, "v_sw", (MAJOR << 16) | (MINOR << 8) | BATCH);
            cJSON_AddNumberToObject(get_info, "v_type", 1);
            cJSON_AddNumberToObject(get_info, "build", BUILD);
            cJSON_AddNumberToObject(get_info, "v_idf", (4 << 16) | (4 << 8) | 1);
            cJSON_AddNumberToObject(get_info, "uptime", 1234);
            cJSON_AddNumberToObject(get_info, "build_date", 1657623331);
            cJSON_AddNumberToObject(get_info, "boot_count", 15);
            cJSON_AddNumberToObject(get_info, "boot_reason", 2);
            cJSON_AddNumberToObject(get_info, "mac", 45647894);
            cJSON_AddStringToObject(get_info, "uuid", factory->controller_uuid);
            cJSON_AddNumberToObject(get_info, "serial", factory->id);
            cJSON_AddStringToObject(get_info, "ssid", &wifi_info[0]);

            cJSON_AddNumberToObject(get_info, "dev_type", 1);
            cJSON_AddNumberToObject(get_info, "dev_flash", 64256);
            cJSON_AddNumberToObject(get_info, "dev_free_flash", 300);
            cJSON_AddStringToObject(get_info, "dev_name", factory->name);
        }

        char *my_json_string = cJSON_Print(get_info);
        cJSON_Delete(get_info); // free Json object

        if (my_json_string)
        {
            cJSON_Minify(my_json_string);
            qt_serial_respond_to_qt(strlen(my_json_string), (uint8_t *)my_json_string);
            cJSON_free(my_json_string);
        }
    }
}

void QT_SET_WIFI(const char *data)
{
    cJSON *root = cJSON_Parse(data);

    if (root)
    {
        if (cJSON_GetObjectItem(root, "cmd"))
        {
            uint8_t cmd_temp = cJSON_GetObjectItem(root, "cmd")->valueint;
        }
        if (cJSON_GetObjectItem(root, "pass"))
        {
            char *ssid = cJSON_GetObjectItem(root, "ssid")->valuestring;
            char *pass = cJSON_GetObjectItem(root, "pass")->valuestring;

            if (ssid && pass && (strlen(pass) >= 8))
            {
                set_new_wifi_flag();
                wifi_connect((const char *)ssid, (const char *)pass);
            }

            // uint8_t status_connect = 1; // WIFI_CONNET;
            // QT_RESPONE(2, 1, status_connect);
        }

        cJSON_Delete(root); // free Json string
    }
}

void QT_RESPONE(uint8_t cmd, uint8_t status_write, uint8_t status_connect)
{
    cJSON *response = NULL;
    response = cJSON_CreateObject();

    if (response)
    {
        cJSON_AddNumberToObject(response, "cmd", cmd);
        cJSON_AddNumberToObject(response, "status_write", status_write);

        if (status_connect != 5) // Unknown
        {
            cJSON_AddNumberToObject(response, "status_connect", status_connect);
        }

        char *my_json_string = cJSON_Print(response);
        cJSON_Delete(response); // free Json string

        if (my_json_string)
        {
            cJSON_Minify(my_json_string);

            qt_serial_respond_to_qt(strlen(my_json_string), (uint8_t *)my_json_string);
            // const int len = strlen(my_json_string);
            // const int txBytes = uart_write_bytes(UART_NUM_0, my_json_string, len); // Send the data over uart

            cJSON_free(my_json_string);
        }
    }
}

void QT_SET_DATA(const char *data)
{
#if 0
    nvs_handle_t confi_data;
    esp_err_t err;
    err = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &confi_data);
    if (err != ESP_OK)
    {
        QT_RESPONE(3, 0, 5);
        return;
    }

    err = nvs_set_str(confi_data, "confi_data", data);
    if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND)
    {
        QT_RESPONE(3, 0, 5);
        return;
    }

    err = nvs_commit(confi_data);
    if (err != ESP_OK)
    {
        QT_RESPONE(3, 0, 5);
        return;
    }
    nvs_close(confi_data);
    QT_RESPONE(3, 1, 5);
#endif

    uint8_t ret = nvs_storage_write_config_data_str(data);
    if (ret)
    {
        TRACE_B("Successfully wrote config data..");
    }

    QT_RESPONE(3, ret, 5);

    return;
}

void QT_READ_DATA(void)
{
    char *buf = NULL;
    nvs_storage_read_config_data_str(&buf);

    if (buf)
    {
        cJSON *root = cJSON_Parse(buf);
        free(buf);

        if (root)
        {
            cJSON_DeleteItemFromObject(root, "cmd");
            cJSON_AddNumberToObject(root, "cmd", 4);

            char *my_json_string = cJSON_Print(root);

            if (my_json_string)
            {
                cJSON_Minify(my_json_string);
                cJSON_Delete(root); // free Json string

                const int len = strlen(my_json_string);
                const int txBytes = qt_serial_respond_to_qt(len, (uint8_t *)my_json_string); // Send the data over uart

                cJSON_free(my_json_string);
            }
        }
    }
}

int qt_serial_respond_to_qt(int len, uint8_t *data)
{
    int ret = 0;
    char start_bytes[] = {0x80, '\r', '\n'};
    ret += uart_write_bytes(UART_NUM_0, start_bytes, sizeof(start_bytes));
    ret = uart_write_bytes(UART_NUM_0, data, len);
    // ret += uart_write_bytes(UART_NUM_0, "\r\n", 2);

    return ret;
}

void qt_serial_init(void)
{
    UART_INIT();
    xTaskCreate(rx_task, "uart_rx_task", 1024 * 10, NULL, configMAX_PRIORITIES, NULL);
}
