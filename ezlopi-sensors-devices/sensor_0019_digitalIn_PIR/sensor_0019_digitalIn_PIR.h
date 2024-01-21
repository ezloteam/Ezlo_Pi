

#ifndef SENSOR_0019_DIGITALIN_PIR_H
#define SENSOR_0019_DIGITALIN_PIR_H

#include "esp_err.h"
#include "driver/gpio.h"
#include "ezlopi_cloud_items.h"
#include "trace.h"
#include "cJSON.h"

#include "ezlopi_core_actions.h"
#include "ezlopi_core_devices.h"
#include "ezlopi_core_devices_list.h"
#include "ezlopi_core_device_value_updated.h"

#include "ezlopi_cloud_category_str.h"
#include "ezlopi_cloud_subcategory_str.h"
#include "ezlopi_cloud_item_name_str.h"
#include "ezlopi_cloud_device_types_str.h"
#include "ezlopi_cloud_value_type_str.h"

int sensor_0019_digitalIn_PIR(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *args, void *user_arg);

#endif // SENSOR_0019_DIGITALIN_PIR_H