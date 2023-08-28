

#ifndef _029_IR_BLASTER_REMOTE_H_
#define _029_IR_BLASTER_REMOTE_H_

#include "ezlopi_devices.h"
#include "ezlopi_actions.h"
#include "cJSON.h"
// #include "ezlopi_devices_list.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "ezlopi_cloud_category_str.h"
#include "ezlopi_cloud_subcategory_str.h"
#include "ezlopi_item_name_str.h"
#include "sdkconfig.h"
#include "driver/rmt.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "ir_init.h"
#include "ezlopi_device_value_updated.h"
#include "trace.h"

int IR_blaster_remote_v3(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg);


#endif //_029_IR_BLASTER_REMOTE_H_
