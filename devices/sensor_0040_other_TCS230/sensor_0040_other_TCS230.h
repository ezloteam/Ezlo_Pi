#ifndef _0040_SENSOR_OTHER_TCS230_H_
#define _0040_SENSOR_OTHER_TCS230_H_

#include "ezlopi_actions.h"
#include "ezlopi_devices.h"
#include "driver/gpio.h"

typedef enum TCS230_queue_enum
{
    TCS230_QUEUE_RESET = 0,
    TCS230_QUEUE_AVAILABLE,
    TCS230_QUEUE_FULL,
} TCS230_queue_enum_t;

typedef enum TCS230_color_enum
{
    COLOR_SENSOR_COLOR_RED = 0,
    COLOR_SENSOR_COLOR_BLUE,
    COLOR_SENSOR_COLOR_CLEAR,
    COLOR_SENSOR_COLOR_GREEN,
    COLOR_SENSOR_COLOR_MAX
} TCS230_color_enum_t;

typedef enum TCS230_freq_scaling_enum
{
    COLOR_SENSOR_FREQ_SCALING_POWER_DOWN = 0,
    COLOR_SENSOR_FREQ_SCALING_2_PERCENT,
    COLOR_SENSOR_FREQ_SCALING_20_PERCENT,
    COLOR_SENSOR_FREQ_SCALING_100_PERCENT,
    COLOR_SENSOR_FREQ_SCALING_MAX
} TCS230_freq_scaling_enum_t;

typedef struct
{
    // contains larger time_periods
    int32_t least_red_timeP;
    int32_t least_green_timeP;
    int32_t least_blue_timeP;

    // contains smaller time_periods
    int32_t most_red_timeP;
    int32_t most_green_timeP;
    int32_t most_blue_timeP;
} TCS230_calib_data_t;

typedef struct
{
    uint32_t gpio_s0;
    uint32_t gpio_s1;
    uint32_t gpio_s2;
    uint32_t gpio_s3;
    uint32_t gpio_output_en;    // 'gpio_output_en' pin acts as a gate to start/stop output from 'gpio_pulse_output' pin
    uint32_t gpio_pulse_output; // 'gpio_pulse_output' pin produces pulses, whose periods determine the corresponding color.
} TCS230_pin_t;

typedef struct
{
    TCS230_calib_data_t calib_data;
    uint32_t red_mapped;
    uint32_t green_mapped;
    uint32_t blue_mapped;
    TCS230_pin_t TCS230_pin;
} TCS230_data_t;

//------------------------------------------------------------------------
//              Action
//------------------------------------------------------------------------
int sensor_0040_other_TCS230(e_ezlopi_actions_t action, s_ezlopi_device_properties_t *properties, void *arg, void *user_arg);

#endif // _0040_SENSOR_OTHER_TCS230_H_