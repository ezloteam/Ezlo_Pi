

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

static int IR_Blaster_prepare(void *arg); // don't declare static function in header file
static int add_device_to_list(s_ezlopi_prep_arg_t *prep_arg, s_ezlopi_device_properties_t *properties, void *user_arg);
static s_ezlopi_device_properties_t *IR_Blaster_Remote_prepare(uint32_t dev_id, const char *category, const char *sub_category, const char *item_name, const char *value_type, cJSON *cjson_device);
static int IR_BLaster_Remote_init(s_ezlopi_device_properties_t *properties); //
static int IR_BLaster_Remote_set_value(s_ezlopi_device_properties_t *properties, void *arg);
static int IR_BLaster_Remote_get_value_cjson(s_ezlopi_device_properties_t *properties, void *args);
esp_err_t blaster_mode_get_value_cjson(cJSON *params);
static esp_err_t learner_mode_get_value_cjson(cJSON *params);
esp_err_t ir_remote_learner();
int IR_blaster_remote(e_ezlopi_actions_t action, s_ezlopi_device_properties_t *properties, void *arg, void *user_arg);
esp_err_t ir_remote_blaster_learned_code(char *Hex_string_data);
esp_err_t build(void *result, const ir_builder_config_t *config, uint32_t *buffer1, uint32_t timing_array_len);
int capture(); // 
void store(rmt_item32_t *items, uint32_t len);

#endif //_029_IR_BLASTER_REMOTE_H_
