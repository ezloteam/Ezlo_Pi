
#include <string.h>
#include <stdlib.h>

#include "esp_check.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "ezlopi_util_trace.h"

#include "ezlopi_core_errors.h"

#include "ezlopi_hal_adc.h"
#include "EZLOPI_USER_CONFIG.h"

typedef struct s_ezlopi_analog_object
{
    uint32_t vRef;
    adc_unit_t unit;
    adc_bits_width_t width;
    adc_atten_t attenuation;
    adc_channel_t adc_channel;
    esp_adc_cal_characteristics_t adc_characteristics;

} ezlopi_analog_object_handle_t;

static void __check_eFuse_support(void);
static int __get_adc_channel(uint8_t gpio_num);
static void __fill_adc_characteristics(esp_adc_cal_characteristics_t* chars, adc_unit_t unit, adc_atten_t attenuation, adc_bits_width_t width, uint32_t vRef);

// object handle array to check if a channel is already configured.
static ezlopi_analog_object_handle_t* ezlopi_analog_object_array[ADC1_CHANNEL_MAX] = { NULL };

#if CONFIG_IDF_TARGET_ESP32
static e_ezlopi_gpio_channel_t ezlopi_channel_to_gpio_map[ADC1_CHANNEL_MAX] = {
    EZLOPI_GPIO_CHANNEL_0, EZLOPI_GPIO_CHANNEL_1, EZLOPI_GPIO_CHANNEL_2, EZLOPI_GPIO_CHANNEL_3,
    EZLOPI_GPIO_CHANNEL_4, EZLOPI_GPIO_CHANNEL_5, EZLOPI_GPIO_CHANNEL_6, EZLOPI_GPIO_CHANNEL_7
};
#elif CONFIG_IDF_TARGET_ESP32S3 || CONFIG_IDF_TARGET_ESP32S2
static e_ezlopi_gpio_channel_t ezlopi_channel_to_gpio_map[ADC1_CHANNEL_MAX] = {
    EZLOPI_GPIO_CHANNEL_0, EZLOPI_GPIO_CHANNEL_1, EZLOPI_GPIO_CHANNEL_2, EZLOPI_GPIO_CHANNEL_3,
    EZLOPI_GPIO_CHANNEL_4, EZLOPI_GPIO_CHANNEL_5, EZLOPI_GPIO_CHANNEL_6, EZLOPI_GPIO_CHANNEL_7,
    EZLOPI_GPIO_CHANNEL_8, EZLOPI_GPIO_CHANNEL_9
};
#elif CONFIG_IDF_TARGET_ESP32C3
static e_ezlopi_gpio_channel_t ezlopi_channel_to_gpio_map[ADC1_CHANNEL_MAX] = {
    EZLOPI_GPIO_CHANNEL_0, EZLOPI_GPIO_CHANNEL_1, EZLOPI_GPIO_CHANNEL_2, EZLOPI_GPIO_CHANNEL_3,
    EZLOPI_GPIO_CHANNEL_4
};
#endif

ezlopi_error_t ezlopi_adc_init(uint8_t gpio_num, uint8_t width)
{
    ezlopi_analog_object_handle_t* ezlopi_analog_object_handle = (struct s_ezlopi_analog_object*)ezlopi_malloc(__FUNCTION__, sizeof(struct s_ezlopi_analog_object));
    memset(ezlopi_analog_object_handle, 0, sizeof(struct s_ezlopi_analog_object));
    ezlopi_error_t ret = EZPI_ERR_HAL_INIT_FAILED;
    int channel = ezlopi_adc_get_channel_number(gpio_num);
    if (-1 == channel)
    {
        TRACE_E("gpio_num %d is invalid for ADC.", gpio_num);
    }
    else if (ADC_WIDTH_MAX <= width)
    {
        TRACE_E("Invalid width(%d) for ADC; must be less than %d", width, ADC_WIDTH_MAX);
    }
    else if (NULL != ezlopi_analog_object_array[channel])
    {
        TRACE_E("Invalid gpio_num(%d) for ADC; it is already in use.", gpio_num);
    }
    else
    {
        ezlopi_analog_object_handle->adc_channel = channel;
        ezlopi_analog_object_handle->unit = ADC_UNIT_1;
        ezlopi_analog_object_handle->attenuation = ADC_ATTEN_11db;
#if CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S2
        ezlopi_analog_object_handle->width = width;
#elif CONFIG_IDF_TARGET_ESP32S3 || CONFIG_IDF_TARGET_ESP32C3
        ezlopi_analog_object_handle->width = ADC_WIDTH_BIT_12;
#endif
        ezlopi_analog_object_handle->vRef = 1100;
        __fill_adc_characteristics(&ezlopi_analog_object_handle->adc_characteristics, ezlopi_analog_object_handle->unit,
            ezlopi_analog_object_handle->attenuation, ezlopi_analog_object_handle->width, ezlopi_analog_object_handle->vRef);
        TRACE_D("Checking for eFuse support.");
        __check_eFuse_support();

        TRACE_D("Configuring ADC.");
        adc1_config_width(ezlopi_analog_object_handle->width);
        adc1_config_channel_atten((adc1_channel_t)ezlopi_analog_object_handle->adc_channel, ezlopi_analog_object_handle->attenuation);
        TRACE_D("ADC was configured successfully.");

        ezlopi_analog_object_array[ezlopi_analog_object_handle->adc_channel] = ezlopi_analog_object_handle;

        ret = EZPI_SUCCESS;
    }

    return ret;
}

int ezlopi_adc_get_channel_number(uint8_t gpio_num)
{
    int channel = __get_adc_channel(gpio_num);
    if (-1 == channel)
    {
        TRACE_E("gpio_num %d is invalid for ADC.", gpio_num);
        channel = -1;
    }
    return channel;
}

int ezlopi_adc_get_adc_data(uint8_t gpio_num, s_ezlopi_analog_data_t* ezlopi_analog_data)
{
    int channel = __get_adc_channel(gpio_num);
    // TRACE_E("Channel is %d and gpio-num is %d", channel, gpio_num);
    if (-1 == channel)
    {
        TRACE_E("Invalid gpio_num(%d)", gpio_num);
        channel = -1;
    }
    else if (NULL != ezlopi_analog_object_array[channel])
    {
        ezlopi_analog_data->value = adc1_get_raw((adc1_channel_t)ezlopi_analog_object_array[channel]->adc_channel);
        ezlopi_analog_data->voltage = esp_adc_cal_raw_to_voltage(ezlopi_analog_data->value, &ezlopi_analog_object_array[channel]->adc_characteristics);
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
    return channel;
}

static int __get_adc_channel(uint8_t gpio_num)
{
    int ret = -1;
    for (uint8_t channel = 0; channel < EZLOPI_GPIO_CHANNEL_MAX; channel++)
    {
        if (gpio_num == ezlopi_channel_to_gpio_map[channel])
        {
            ret = channel;
            break;
        }
    }

    return ret;
}

static void __check_eFuse_support(void)
{
#if CONFIG_IDF_TARGET_ESP32
    if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_TP) == ESP_OK)
    {
        TRACE_S("eFuse Two Point support available.");
    }
    else
    {
        TRACE_E("eFuse Two Point support not available.");
    }
    if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_VREF) == ESP_OK)
    {
        TRACE_S("eFuse Vref support available");
    }
    else
    {
        TRACE_E("eFuse Vref support not available");
    }
#elif CONFIG_IDF_TARGET_ESP32S3
    if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_TP))
    {
        TRACE_S("eFuse Two Point support available.");
    }
    else
    {
        TRACE_E("eFuse Two Point support not available.");
    }
#endif
}

static void __fill_adc_characteristics(esp_adc_cal_characteristics_t* chars, adc_unit_t unit, adc_atten_t attenuation, adc_bits_width_t width, uint32_t vRef)
{
    if (chars)
    {
        memset(chars, 0, sizeof(esp_adc_cal_characteristics_t));
        esp_adc_cal_value_t value = esp_adc_cal_characterize(unit, attenuation, width, vRef, chars);
        if (value == ESP_ADC_CAL_VAL_EFUSE_TP)
        {
            TRACE_S("Characterized using Two Point Value");
        }
        else if (value == ESP_ADC_CAL_VAL_EFUSE_VREF)
        {
            TRACE_S("Characterized using eFuse Vref");
        }
        else
        {
            TRACE_S("Characterized using Default Vref");
        }
    }
}
