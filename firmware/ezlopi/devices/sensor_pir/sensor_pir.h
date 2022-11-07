

#ifndef SENSOR_PIR_H
#define SENSOR_PIR_H

#include "esp_err.h"
#include "driver/gpio.h"
#include "ezlopi_actions.h"
#include "ezlopi_timer.h"
#include "items.h"
#include "frozen.h"
#include "trace.h"
#include "cJSON.h"
#include "ezlopi_devices.h"
#include "ezlopi_devices_list.h"
#include "ezlopi_cloud_category_str.h"
#include "ezlopi_cloud_subcategory_str.h"
#include "ezlopi_item_name_str.h"
#include "ezlopi_cloud_device_types_str.h"
#include "ezlopi_cloud_value_type_str.h"
#include "ezlopi_device_value_updated.h"


int ezlopi_pir_begin(e_ezlopi_actions_t action, s_ezlopi_device_properties_t *properties, void *arg);


#endif //PIR_SENSOR_H



