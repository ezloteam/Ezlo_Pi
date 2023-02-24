#pragma once

#ifdef __cplusplus
extern "C" {
#endif


#include "driver/rmt.h"
#include "ir_tools.h"
#include "ir_remote.h"
#include "trace.h"


#define RMT_RX_GPIO 8
#define RMT_TX_GPIO 18
#define RMT_RX_CHANNEL 4
#define RMT_TX_CHANNEL 0

/**
 * @brief RMT IR receiver Initialization
 *
 */
ir_parser_config_t rmt_rx_init(); 


/**
 * @brief RMT IR receiver Initialization
 *
 */
ir_builder_config_t rmt_tx_init(); 


/**
 * @brief IR Protocol Selection
 *
 */
ir_protocol_type_t ir_protocol_selection(ir_remote_info_t*  ir_remote_info_handler);


/**
 * @brief IR Protocol Initialization
 *
 */
esp_err_t ir_protocol_init(ir_protocol_init_t *ir_protocol_init_props, ir_remote_info_t *ir_remote_info_handler, ir_parser_config_t* ir_parser_config, ir_builder_config_t* ir_builder_config);

#ifdef __cplusplus
}
#endif