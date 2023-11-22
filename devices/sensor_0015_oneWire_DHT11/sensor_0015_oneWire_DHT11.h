#ifndef _SENSOR_0015_ONEWIRE_DHT11_H_
#define _SENSOR_0015_ONEWIRE_DHT11_H_

#include "ezlopi_actions.h"
#include "ezlopi_devices.h"

typedef struct ezlopi_dht11_data
{
    float temperature;
    float humidity;
} s_ezlopi_dht11_data_t;

int sensor_0015_oneWire_DHT11(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg)

#endif // _SENSOR_0015_ONEWIRE_DHT11_H_
