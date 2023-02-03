


#ifndef _029_IR_BLASTER_REMOTE_H_
#define _029_IR_BLASTER_REMOTE_H_

#include "ezlopi_devices.h"
#include "ezlopi_actions.h"
#include "ir_config.h"
#include "ir_timings.h"
#include "ir_tools.h"



int IR_blaster_remote(e_ezlopi_actions_t action, s_ezlopi_device_properties_t *properties, void *arg, void *user_arg);
/**
 * @brief RMT Transmit Task
 *
 */
void ezlopi_ir_tx(uint32_t address, uint32_t command);

static void ezlopi_ir_rx_task(void *arg);

#endif //_029_IR_BLASTER_REMOTE_H_

