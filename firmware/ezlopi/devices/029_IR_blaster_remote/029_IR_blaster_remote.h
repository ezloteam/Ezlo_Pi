


#ifndef _029_IR_BLASTER_REMOTE_H_
#define _029_IR_BLASTER_REMOTE_H_

#include "ezlopi_devices.h"
#include "ezlopi_actions.h"
#include "cJSON.h"
// #include "ezlopi_devices_list.h"
#include "string.h"
#include "stdlib.h"
#include "ezlopi_cloud_category_str.h"
#include "ezlopi_cloud_subcategory_str.h"
#include "ezlopi_item_name_str.h"
// #include "ezlopi_cloud_device_types_str.h"
// #include "ezlopi_cloud_value_type_str.h"
#include "sdkconfig.h"
#include "driver/rmt.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "ir_tools.h"
#include "ir_timings.h"
#include "ir_config.h"
//#include "ir_init.h"
#include "ezlopi_device_value_updated.h"
#include "trace.h"

// ir_parser_config_t ir_parser_config;
// ir_builder_config_t ir_builder_config;
// static ir_protocol_init_t ir_protocol_init_props;

static int IR_Blaster_Remote_prepare_and_add(void* args);
static s_ezlopi_device_properties_t* IR_Blaster_Remote_prepare(cJSON *cjson_device);
static int IR_BLaster_Remote_init(s_ezlopi_device_properties_t* properties);
static int IR_BLaster_Remote_set_value(s_ezlopi_device_properties_t *properties, void *arg);
static int IR_BLaster_Remote_get_value_cjson(s_ezlopi_device_properties_t *properties, void *args);

int IR_blaster_remote(e_ezlopi_actions_t action, s_ezlopi_device_properties_t *properties, void *arg, void *user_arg);
/**
 * @brief RMT Transmit Task
 *
 */
void ezlopi_ir_tx(uint32_t address, uint32_t command);

static void ezlopi_ir_rx_task(void *arg);

#endif //_029_IR_BLASTER_REMOTE_H_

