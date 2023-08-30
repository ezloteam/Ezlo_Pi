#ifndef _TCS230_H_
#define _TCS230_H_

#include "sensor_0040_other_TCS230.h"
#include "driver/gpio.h"

static void Extract_TCS230_Pulse_Period_func(gpio_num_t gpio_pulse_output, int32_t *Time_period);
static int MAP_color_value(int x, int fromLow, int fromHigh, int toLow, int toHigh);
static void Get_mapped_color_value(uint32_t *color_value, gpio_num_t gpio_pulse_output, int32_t *period, int32_t min_time_limit, int32_t max_time_limit);

bool TCS230_set_filter_color(s_ezlopi_device_properties_t *properties, TCS230_color_enum_t color);
bool TCS230_set_frequency_scaling(s_ezlopi_device_properties_t *properties, TCS230_freq_scaling_enum_t scale);

void Calculate_max_min_color_values(gpio_num_t gpio_output_en, gpio_num_t gpio_pulse_output, int32_t *least_color_timeP, int32_t *most_color_timeP);
bool get_tcs230_sensor_value(s_ezlopi_device_properties_t *properties);

#endif