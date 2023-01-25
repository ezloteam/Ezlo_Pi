


#ifndef _029_IR_BLASTER_REMOTE_H_
#define _029_IR_BLASTER_REMOTE_H_

#include "ezlopi_devices.h"
#include "ezlopi_actions.h"
#include "029_IR_blaster_remote_config.h"
#include "029_IR_blaster_remote_timings.h"
#include "029_IR_blaster_remote_tools.h"


int IR_blaster_remote(e_ezlopi_actions_t action, s_ezlopi_device_properties_t *properties, void *arg, void *user_arg);
ir_parser_config_t rmt_rx_init(uint32_t ezlopi_ir_blaster_rx_gpio_num, rmt_channel_t ezlopi_ir_blaster_rx_channel);
ir_builder_config_t rmt_tx_init(uint32_t ezlopi_ir_blaster_tx_gpio_num, rmt_channel_t ezlopi_ir_blaster_tx_channel);


#endif //_029_IR_BLASTER_REMOTE_H_
