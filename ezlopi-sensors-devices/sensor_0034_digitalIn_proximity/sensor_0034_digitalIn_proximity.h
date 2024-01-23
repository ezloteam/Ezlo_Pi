#ifndef _SENSOR_0034_DIGITALIN_PROXIMITY_H_
#define _SENSOR_0034_DIGITALIN_PROXIMITY_H_

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

int sensor_0034_digitalIn_proximity(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *args, void *user_arg);

#endif // _SENSOR_0034_DIGITALIN_PROXIMITY_H_
