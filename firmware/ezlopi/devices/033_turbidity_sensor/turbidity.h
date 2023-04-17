#ifndef _TURBIDITY_H_
#define _TURBIDITY_H_

#include <stdio.h>
#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FREERTOS.h"
#include "freertos/task.h"
#include "ezlopi_devices_list.h"
#include "ezlopi_actions.h"
#include "ezlopi_cloud_category_str.h"
#include "ezlopi_cloud_subcategory_str.h"
#include "ezlopi_cloud_device_types_str.h"
#include "ezlopi_cloud_devices_str.h"
#include "ezlopi_item_name_str.h"
#include "sdkconfig.h"
#include "cJSON.h"

static const char *TURBIDITY_TAG = "Ezlopi_turbidity sensor";
static int ezlopi_turbidity_sensor_prepare_and_add(void *args);
static s_ezlopi_device_properties_t *ezlopi_turbidity_sensor_prepare(cJSON *cjson_device);
static int ezlopi_turbidity_sensor_init(s_ezlopi_device_properties_t *properties);
static int ezlopi_turbidity_sensor_update_value(s_ezlopi_device_properties_t *properties, void *arg);
static int ezlopi_turbidity_sensor_get_value_cjson(s_ezlopi_device_properties_t *properties, void *args);

int turbidity_sensor(e_ezlopi_actions_t action, s_ezlopi_device_properties_t *properties, void *arg, void *user_arg);

#endif // _TURBIDITY_H_