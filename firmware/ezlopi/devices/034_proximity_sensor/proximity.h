#ifndef _PROXIMITY_H_
#define _PROXIMITY_H_

#include "esp_err.h"
#include "driver/gpio.h"
#include "items.h"
#include "trace.h"
#include "cJSON.h"

#include "ezlopi_actions.h"
#include "ezlopi_devices.h"
#include "ezlopi_devices_list.h"
#include "ezlopi_device_value_updated.h"

#include "ezlopi_cloud_category_str.h"
#include "ezlopi_cloud_subcategory_str.h"
#include "ezlopi_item_name_str.h"
#include "ezlopi_cloud_device_types_str.h"

int proximity_sensor(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *args, void *user_arg);

#endif //_PROXIMITY_H_