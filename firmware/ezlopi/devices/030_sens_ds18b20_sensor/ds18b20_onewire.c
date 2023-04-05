

#include "ds18b20_onewire.h"
#include "esp_err.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/uart.h"
#include "driver/i2c.h"

static esp_err_t one_wire_write_bit_to_line(uint8_t bit, uint32_t gpio_pin);
static esp_err_t one_wire_read_bit_from_line(uint8_t* bit, uint32_t gpio_pin);


esp_err_t one_wire_write_byte_to_line(uint8_t* data, uint32_t gpio_pin)
{
    esp_err_t error = ESP_OK;
    uint8_t count = 0;
    uint8_t bit = 0;
    for(; count < 8; count++)
    {
        bit = (*data >> count) & 0x01;
        error = one_wire_write_bit_to_line(bit, gpio_pin);
    }
    return error;
}

esp_err_t one_wire_read_byte_from_line(uint8_t* data, uint32_t gpio_pin)
{
    esp_err_t error = ESP_OK;
    uint8_t count = 0;
    uint8_t bit = 0;
    for(; count < 8; count++)
    {
        error = one_wire_read_bit_from_line(&bit, gpio_pin);
        // ESP_LOGI(ONEWIRE_TAG, "(%d, func: %s) REceived bit is %d", ONEWIRE_GET_LINE, __func__, bit);
        *data |= (bit << count);
    }
    return error;
}

static esp_err_t one_wire_write_bit_to_line(uint8_t bit, uint32_t gpio_pin)
{
    esp_err_t error = ESP_OK;
    if(1 == bit)
    {
        gpio_set_direction(gpio_pin, GPIO_MODE_OUTPUT);
        gpio_set_pull_mode(gpio_pin, GPIO_FLOATING);

        onewireENTER_CRITICAL_REGION();
        gpio_set_level(gpio_pin, 0);
        ets_delay_us(ONE_WIRE_WRITE_1_LINE_PULL_DOWN_HOLD_US);
        gpio_set_direction(gpio_pin, GPIO_MODE_INPUT);
        ets_delay_us(ONE_WIRE_WRITE_1_LINE_RELEASE_HOLD_US);
        onewireEXIT_CRITICAL_REGION();
        // ESP_LOGI(ONEWIRE_TAG, "(%d, func: %s) 1", ONEWIRE_GET_LINE, __func__);
    }
    else 
    {
        gpio_set_direction(gpio_pin, GPIO_MODE_OUTPUT);
        gpio_set_pull_mode(gpio_pin, GPIO_FLOATING);

        onewireENTER_CRITICAL_REGION();
        gpio_set_level(gpio_pin, 0);
        ets_delay_us(ONE_WIRE_WRITE_0_LINE_PULL_DOWN_HOLD_US);
        gpio_set_direction(gpio_pin, GPIO_MODE_INPUT);
        ets_delay_us(ONE_WIRE_WRITE_0_LINE_RELEASE_HOLD_US);
        onewireEXIT_CRITICAL_REGION();
        // ESP_LOGI(ONEWIRE_TAG, "(%d, func: %s) 0", ONEWIRE_GET_LINE, __func__);
    }
    return error;
}


static esp_err_t one_wire_read_bit_from_line(uint8_t* bit, uint32_t gpio_pin)
{
    esp_err_t error = ESP_OK;
    gpio_set_direction(gpio_pin, GPIO_MODE_OUTPUT);
    gpio_set_pull_mode(gpio_pin, GPIO_FLOATING);

    onewireENTER_CRITICAL_REGION();
    gpio_set_level(gpio_pin, 0);
    ets_delay_us(ONE_WIRE_READ_LINE_PULL_DOWN_HOLD_US);
    gpio_set_direction(gpio_pin, GPIO_MODE_INPUT);
    ets_delay_us(ONE_WIRE_READ_LINE_RELEASE_HOLD_US);
    *bit = gpio_get_level(gpio_pin);
    ets_delay_us(ONE_WIRE_READ_LINE_SAMPLING_US);
    onewireEXIT_CRITICAL_REGION();
    
    return error;
}


bool one_wire_reset_line(uint32_t gpio_pin)
{
    uint8_t presence = 0;

    gpio_set_direction(gpio_pin, GPIO_MODE_OUTPUT);
    gpio_set_pull_mode(gpio_pin, GPIO_FLOATING);

    onewireENTER_CRITICAL_REGION();
    gpio_set_level(gpio_pin, 0);
    ets_delay_us(ONE_WIRE_RESET_LINE_PULL_DOWN_HOLD_US);
    gpio_set_level(gpio_pin, 1);
    gpio_set_direction(gpio_pin, GPIO_MODE_INPUT);
    ets_delay_us(ONE_WIRE_RESET_LINE_RELEASE_HOLD_US);
    presence = gpio_get_level(gpio_pin); 
    ets_delay_us(ONE_WIRE_RESET_LINE_SAMPLING_US);
    onewireEXIT_CRITICAL_REGION();

    return (presence == 0) ?  true: false;
}



