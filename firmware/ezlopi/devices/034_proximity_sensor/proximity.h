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

int proximity_sensor(e_ezlopi_actions_t action, s_ezlopi_device_properties_t *properties, void *args, void *user_arg);
static int proximity_sensor_prepare_and_add(void *args);
static int proximity_sensor_init(s_ezlopi_device_properties_t *properties);
static s_ezlopi_device_properties_t *proximity_sensor_prepare(cJSON *cjson_device);
static void proximity_sensor_value_updated_from_device(s_ezlopi_device_properties_t *properties);
static int proximity_sensor_get_value_cjson(s_ezlopi_device_properties_t *properties, void *args);



#endif //_PROXIMITY_H_