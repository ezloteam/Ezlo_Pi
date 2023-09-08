#ifndef _TURBIDITY_H_
#define _TURBIDITY_H_

#include <stdio.h>
#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
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


int turbidity_sensor_v3(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg);

#endif // _TURBIDITY_H_