#ifndef _EZLOPI_ADC_H_
#define _EZLOPI_ADC_H_

#include "driver/adc.h"
#include "esp_adc_cal.h"

typedef struct s_ezlopi_analog_data{
    uint32_t value;
    uint32_t voltage;
}s_ezlopi_analog_data_t;

typedef struct s_ezlopi_analog_object* ezlopi_analog_object_handle_t;

typedef void (*__ezlopi_adc_upcall)(s_ezlopi_analog_data_t*, uint8_t);

// ezlopi_analog_object_handle_t ezlopi_adc_init(adc_unit_t unit, adc_channel_t adc_channel, adc_bits_width_t width,
//                                             adc_atten_t attenuation, uint32_t vRef, __ezlopi_adc_upcall upcall);
int ezlopi_adc_init(uint8_t gpio_num, uint8_t width, __ezlopi_adc_upcall upcall);
adc_channel_t ezlopi_adc_get_channel_number(ezlopi_analog_object_handle_t ezlopi_analog_object_handle);

#endif //_EZLOPI_ADC_H_
