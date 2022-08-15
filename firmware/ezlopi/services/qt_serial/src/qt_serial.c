#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"

#include "debug.h"
#include "nvs_storage.h"
#include "interface_common.h"
#include "devices_common.h"
#include "qt_serial.h"
#include "wifi.h"

// Temp def
static int is_pars;
static int new_wifi;
static bool is_start;
static int device_count;
static int uart_buffer_size = 4096;
static TaskHandle_t read_process_handle;

static void __pars(int len, uint8_t *data);
static void __pars_command(int len, uint8_t *data);
static void __assign_to_device(int idx, uint8_t *data);
static void __assign_from_device(int idx, uint8_t *data);
static void __qt_serial_read_process(void *pv);

int qt_serial_respond_to_qt(int len, uint8_t *data)
{
    return uart_write_bytes(UART_NUM_0, (const void *)data, len);
}

int qt_serial_init(void)
{
    int ret = 0;
    uart_driver_delete(0);
    uart_config_t uart_config = {
        .baud_rate = 460800,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };

    uart_param_config(UART_NUM_0, &uart_config);
    uart_set_pin(UART_NUM_0, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(UART_NUM_0, uart_buffer_size + UART_FIFO_LEN, uart_buffer_size + UART_FIFO_LEN, 0, NULL, 0);

    xTaskCreatePinnedToCore(__qt_serial_read_process, "qt serial read process", 10 * 1024, NULL, 10, &read_process_handle, 0);

    return ret;
}

static void __qt_serial_read_process(void *pv)
{
    int read_len = 0;
    static int no_data = 0;
    uint8_t data_read[1024] = {0};

    is_pars = false;
    is_start = false;

    while (1)
    {
        read_len = uart_read_bytes(UART_NUM_0, data_read, sizeof(data_read), 0);
        if (read_len < 0)
        {
            break;
        }
        else if (read_len > 0)
        {
            if (!is_start)
            {
                qt_serial_respond_to_qt(read_len, data_read);
                if (data_read[0] == '$')
                {
                    is_start = true;
                }
            }
            else
            {
                if (!is_pars && data_read[0] == '$')
                {
                    qt_serial_respond_to_qt(read_len, data_read);
                }
                else
                {
                    __pars(read_len, data_read);
                    no_data = 0;
                }
            }
        }
        else // read_len == 0
        {
            no_data++;
            if (no_data > 25)
            {
                is_pars = false;
            }
        }

        vTaskDelay(20 / portTICK_RATE_MS);
    }

    read_process_handle = NULL;
    vTaskDelete(NULL);
}

static void __assign_to_device(int idx, uint8_t *data)
{
    s_device_properties_t *device_list = devices_common_device_list();

    int pos = 0;
    memcpy(device_list[idx].device_id, &data[pos], sizeof(device_list[idx].device_id));
    pos += sizeof(device_list[idx].device_id);

    device_list[idx].dev_type = data[pos];
    pos += 1;

    memcpy(device_list[idx].name, &data[pos], sizeof(device_list[idx].name));
    pos += sizeof(device_list[idx].name);

    memcpy(device_list[idx].roomId, &data[pos], sizeof(device_list[idx].roomId));
    pos += sizeof(device_list[idx].roomId);

    memcpy(device_list[idx].item_id, &data[pos], sizeof(device_list[idx].item_id));
    pos += sizeof(device_list[idx].item_id);

    device_list[idx].input_vol = data[pos];
    pos += 1;

    device_list[idx].out_vol = data[pos];
    pos += 1;

    device_list[idx].input_gpio = data[pos];
    pos += 1;

    device_list[idx].out_gpio = data[pos];
    pos += 1;

    device_list[idx].is_input = data[pos];
    pos += 1;

    device_list[idx].input_inv = data[pos];
    pos += 1;

    device_list[idx].out_inv = data[pos];
    pos += 1;

    device_list[idx].is_meter = data[pos];
}

static void __assign_from_device(int idx, uint8_t *data)
{
    s_device_properties_t *device_list = devices_common_device_list();

    int pos = 0;
    memcpy(&data[pos], device_list[idx].device_id, sizeof(device_list[idx].device_id));
    pos += sizeof(device_list[idx].device_id);

    data[pos] = device_list[idx].dev_type;
    pos += 1;

    memcpy(&data[pos], device_list[idx].name, sizeof(device_list[idx].name));
    pos += sizeof(device_list[idx].name);

    memcpy(&data[pos], device_list[idx].roomId, sizeof(device_list[idx].roomId));
    pos += sizeof(device_list[idx].roomId);

    memcpy(&data[pos], device_list[idx].item_id, sizeof(device_list[idx].item_id));
    pos += sizeof(device_list[idx].item_id);

    data[pos] = device_list[idx].input_vol;
    pos += 1;

    data[pos] = device_list[idx].out_vol;
    pos += 1;

    data[pos] = device_list[idx].input_gpio;
    pos += 1;

    data[pos] = device_list[idx].out_gpio;
    pos += 1;

    data[pos] = device_list[idx].is_input;
    pos += 1;

    data[pos] = device_list[idx].input_inv;
    pos += 1;

    data[pos] = device_list[idx].out_inv;
    pos += 1;

    data[pos] = device_list[idx].is_meter;
}

static void __pars(int len, uint8_t *data)
{
    static uint8_t pars_data[4096];
    static int len_pars = 0;
    static int cur_pars_cou = 0;

    for (int i = 0; i < len; i++)
    {
        if (!is_pars)
        {
            if (data[i] == 0x95)
            {
                is_pars = true;
                cur_pars_cou = 0;
                len_pars = 0;
            }
        }
        else
        {
            pars_data[cur_pars_cou] = data[i];
            if (cur_pars_cou == 0)
            {
                len_pars = pars_data[0];
            }
            cur_pars_cou++;
            if (len_pars == cur_pars_cou)
            {
                if (len_pars >= 2)
                {
                    __pars_command(len_pars, pars_data);
                }
                is_pars = false;
                cur_pars_cou = 0;
                len_pars = 0;
            }
        }
    }
}

static void __pars_command(int len, uint8_t *data)
{
    s_device_properties_t *device_list = devices_common_device_list();

    uint8_t buf[4096];
    int len_d = 2;
    switch (data[1])
    {
    case GET_CONFIG:
    {
        TRACE_D("GET_CONFIG\n\r");
        buf[0] = 0xC5;
        buf[2] = GET_CONFIG;
        for (int i = 0; i < 28; i++)
        {
            buf[3 + i] = interface_common_get_config(i);
        }
        len_d = len_d + 28;
        buf[1] = len_d & 0xff;

        qt_serial_respond_to_qt(len_d + 1, buf);

        TRACE_D("\n\n>>>>>>> qt_serial_respond_to_qt:\n");
        for (int i = 0; i < len_d; i++)
        {
            TRACE_D("%02x ", data[i]);
        }
        TRACE_D("\n\n\n");

        if (device_list[0].name[0])
        {
            len_d = 2 + sizeof(s_device_properties_t);
            buf[0] = 0xC5;
            buf[1] = len_d & 0xff;
            for (int i = 0; i < MAX_DEV; i++)
            {
                if (device_list[i].name[0] == 0)
                {
                    break;
                }

                if (i == 0)
                {
                    buf[2] = FIRST_DEV;
                }
                else
                {
                    buf[2] = SET_DEV;
                }

                __assign_from_device(i, &buf[3]);
                qt_serial_respond_to_qt(len_d + 1, buf);
            }
        }

        break;
    }
    case SET_CONFIG:
    {
        TRACE_D("SET_CONFIG\n\r");
        uint8_t gpio_conf1[28];
        memcpy((void *)gpio_conf1, (void *)&data[2], 28);

        uint8_t gpio_conf[28];

        for (uint32_t idx = 0; idx < 28; idx++)
        {
            if (interface_common_get_config(idx) != 0xff)
            {
                if (interface_common_get_config(idx) != gpio_conf1[idx])
                {
                    interface_common_set_config(gpio_conf1[idx], idx);
                    interface_common_gpio_config_sets(idx);
                }
            }
            gpio_conf[idx] = interface_common_get_config(idx);
        }

        nvs_storage_write_gpio_config(gpio_conf, 28);

        vTaskDelay(100 / portTICK_RATE_MS);
        buf[0] = 0xC5;
        buf[1] = 2;
        buf[2] = SET_CONFIG;
        qt_serial_respond_to_qt(3, buf);
        break;
    }
    case SET_WiFi:
    {
        new_wifi = 1;
        TRACE_D("SET_WIFI\n\r");
        TRACE_D("try wifi ssid-%s pass-%s\r\n", (const char *)&data[2], (const char *)&data[34]);
        set_new_wifi_flag();
        wifi_connect((const char *)&data[2], (const char *)&data[34]);
        break;
    }
    case FIRST_DEV:
    {
        TRACE_D("FIRST_DEV\n\r");
        device_count = 1;
        memset(device_list, 0, sizeof(s_device_properties_t) * MAX_DEV);

        __assign_to_device(0, &data[2]);

        TRACE_D("NAME %s\n\r", device_list[0].name);
        break;
    }
    case SET_DEV:
    {
        TRACE_D("SET_DEV\n\r");
        if (device_count >= MAX_DEV)
            break;
        __assign_to_device(device_count, &data[2]);
        TRACE_D("DEV[%d] NAME %s\n\r", device_count, device_list[device_count].name);
        device_count++;
        break;
    }
    case END_DEV:
    {
        TRACE_D("END_DEV\n\r");
        int required_size = sizeof(s_device_properties_t) * MAX_DEV;
        nvs_storage_write_device_config((void *)device_list, required_size);
    }
    }
}
