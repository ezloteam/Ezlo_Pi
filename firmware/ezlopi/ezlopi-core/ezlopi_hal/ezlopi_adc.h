#ifndef _EZLOPI_ADC_H_
#define _EZLOPI_ADC_H_

#include "driver/adc.h"
#include "esp_adc_cal.h"

typedef struct s_ezlopi_analog_data{
    uint32_t value;
    uint32_t voltage;
}s_ezlopi_analog_data_t;

typedef struct s_ezlopi_analog_object* ezlopi_analog_object_handle_t;

int ezlopi_adc_init(uint8_t gpio_num, uint8_t width);
int ezlopi_adc_get_adc_data(uint8_t gpio_num, s_ezlopi_analog_data_t* ezlopi_analog_data);
int ezlopi_adc_get_channel_number(uint8_t gpio_num);

#endif //_EZLOPI_ADC_H_
