#ifndef SENSOR_0033_ADC_TURBIDITY_H
#define SENSOR_0033_ADC_TURBIDITY_H

#include <stdio.h>
#include "../../build/config/sdkconfig.h"
// #include "driver/gpio.h"
// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"

#include "ezlopi_core_actions.h"
#include "ezlopi_core_devices.h"

typedef enum
{
    TURBIDITY_WATER_FILTER_OK,
    TURBIDITY_REPLACE_WATER_FILTER,
    TURBIDITY__UNKNOWN,
    TURBIDITY__MAX
} e_water_filter_replacement_alarm_states_t;

int sensor_0033_ADC_turbidity(e_ezlopi_actions_t action, l_ezlopi_item_t* item, void* arg, void* user_arg);

#endif // SENSOR_0033_ADC_TURBIDITY_H
