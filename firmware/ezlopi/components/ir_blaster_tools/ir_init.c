#include "trace.h"
#include "ir_init.h"

/**
 * @brief RMT IR receiver Initialization
 *
 */
ir_parser_config_t rmt_rx_init() 
{
    // rmt_config_t rmt_rx_config = RMT_DEFAULT_CONFIG_RX(RMT_RX_GPIO, RMT_RX_CHANNEL);
    rmt_config_t rmt_rx_config = RMT_CONFIG_RX(RMT_RX_GPIO, RMT_RX_CHANNEL);
    rmt_config(&rmt_rx_config);
    rmt_driver_install(RMT_RX_CHANNEL, 4000, 0);
    ir_parser_config_t ir_parser_conf = IR_PARSER_DEFAULT_CONFIG((ir_dev_t)RMT_RX_CHANNEL);
    ir_parser_conf.flags |= IR_TOOLS_FLAGS_PROTO_EXT;
    TRACE_E("RMT RX threshold = %d", rmt_rx_config.rx_config.idle_threshold);
    return ir_parser_conf;
}

/**
 * @brief RMT IR receiver Initialization
 *
 */
ir_builder_config_t rmt_tx_init() 
{
    // rmt_config_t rmt_tx_config = RMT_DEFAULT_CONFIG_TX(RMT_TX_GPIO, RMT_TX_CHANNEL);
    rmt_config_t rmt_tx_config = RMT_CONFIG_TX(RMT_TX_GPIO, RMT_TX_CHANNEL);
    rmt_tx_config.tx_config.carrier_en = true;
    rmt_config(&rmt_tx_config);
    rmt_driver_install(RMT_TX_CHANNEL, 0, 0);
    ir_builder_config_t ir_builder_conf = IR_BUILDER_DEFAULT_CONFIG((ir_dev_t)RMT_TX_CHANNEL);
    ir_builder_conf.flags |= IR_TOOLS_FLAGS_PROTO_EXT; 
    return ir_builder_conf;
}
