#ifndef _EZLOPI_ADC_H_
#define _EZLOPI_ADC_H_

#include "driver/adc.h"
#include "esp_adc_cal.h"


#if CONFIG_IDF_TARGET_ESP32
typedef enum e_ezlopi_gpio_channel{
    EZLOPI_GPIO_CHANNEL_0       = 36,
    EZLOPI_GPIO_CHANNEL_1       = 37,
    EZLOPI_GPIO_CHANNEL_2       = 38,
    EZLOPI_GPIO_CHANNEL_3       = 39,
    EZLOPI_GPIO_CHANNEL_4       = 32,
    EZLOPI_GPIO_CHANNEL_5       = 33,
    EZLOPI_GPIO_CHANNEL_6       = 34,
    EZLOPI_GPIO_CHANNEL_7       = 35,
    EZLOPI_GPIO_CHANNEL_MAX
}e_ezlopi_gpio_channel_t;
#elif CONFIG_IDF_TARGET_ESP32S3 ||  CONFIG_IDF_TARGET_ESP32S2
typedef enum e_ezlopi_gpio_channel{
    EZLOPI_GPIO_CHANNEL_0       = 1,
    EZLOPI_GPIO_CHANNEL_1       = 2,
    EZLOPI_GPIO_CHANNEL_2       = 3,
    EZLOPI_GPIO_CHANNEL_3       = 4,
    EZLOPI_GPIO_CHANNEL_4       = 5,
    EZLOPI_GPIO_CHANNEL_5       = 6,
    EZLOPI_GPIO_CHANNEL_6       = 7,
    EZLOPI_GPIO_CHANNEL_7       = 8,
    EZLOPI_GPIO_CHANNEL_8       = 9,
    EZLOPI_GPIO_CHANNEL_9       = 10,
    EZLOPI_GPIO_CHANNEL_MAX
}e_ezlopi_gpio_channel_t;
#elif CONFIG_IDF_TARGET_ESP32C3
typedef enum e_ezlopi_gpio_channel{
    EZLOPI_GPIO_CHANNEL_0       = 0,
    EZLOPI_GPIO_CHANNEL_1       = 1,
    EZLOPI_GPIO_CHANNEL_2       = 2,
    EZLOPI_GPIO_CHANNEL_3       = 3,
    EZLOPI_GPIO_CHANNEL_4       = 4,
    EZLOPI_GPIO_CHANNEL_MAX
}e_ezlopi_gpio_channel_t;
#endif


typedef struct s_ezlopi_analog_data{
    uint32_t value;
    uint32_t voltage;
}s_ezlopi_analog_data_t;

typedef struct s_ezlopi_adc{
    uint32_t gpio_num;
    uint8_t resln_bit;
}s_ezlopi_adc_t;


int ezlopi_adc_init(uint8_t gpio_num, uint8_t width);
int ezlopi_adc_get_adc_data(uint8_t gpio_num, s_ezlopi_analog_data_t* ezlopi_analog_data);
int ezlopi_adc_get_channel_number(uint8_t gpio_num);

#endif //_EZLOPI_ADC_H_
