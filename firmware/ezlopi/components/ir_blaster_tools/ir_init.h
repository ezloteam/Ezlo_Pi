#pragma once

#ifdef __cplusplus
extern "C" {
#endif


#include "driver/rmt.h"
// #include "ir_tools.h"
// #include "ir_remote.h"
#include "trace.h"


#define RMT_RX_GPIO 8
#define RMT_TX_GPIO 18
#define RMT_RX_CHANNEL 4
#define RMT_TX_CHANNEL 0

#define IR_TOOLS_FLAGS_PROTO_EXT (1 << 0) /*!< Enable Extended IR protocol */
#define IR_TOOLS_FLAGS_INVERSE (1 << 1)   /*!< Inverse the IR signal, i.e. take high level as low, and vice versa */

/**
* @brief IR device type
*
*/
typedef void *ir_dev_t;

/**
 * @brief IR Builder structure
 *
 */
typedef struct {
    // ir_builder_t parent;
    uint32_t buffer_size;
    uint32_t cursor;
    uint32_t flags;
    bool inverse;
    rmt_item32_t buffer[0];
} ir_protocol_builder_t;                          // IF ERROR IN NAME CONFLICTION

/**
 * @brief IR Parser structure
 *
 */
typedef struct {
    // ir_parser_t parent;
    uint32_t flags;
    uint32_t margin_ticks;
    rmt_item32_t *buffer;
    uint32_t cursor;
    // bool repeat;
    bool inverse;
} ir_protocol_parser_t;

/**
* @brief Configuration type of IR builder
*
*/
typedef struct {
    uint32_t buffer_size; /*!< Size of the internal buffer used by IR builder */
    ir_dev_t dev_hdl;     /*!< IR device handle */
    uint32_t flags;       /*!< Flags for IR builder, different flags will enable different features */
} ir_builder_config_t;

/**
* @brief Configuration type of IR parser
*
*/
typedef struct {
    ir_dev_t dev_hdl;   /*!< IR device handle */
    uint32_t flags;     /*!< Flags for IR parser, different flags will enable different features */
    uint32_t margin_us; /*!< Timing parameter, indicating the tolerance to environment noise */
} ir_parser_config_t;

/**
 * @brief Default configuration for IR builder
 *
 */
#define IR_BUILDER_DEFAULT_CONFIG(dev) \
    {                                  \
        .buffer_size = 500,             \
        .dev_hdl = dev,                \
        .flags = 0,                    \
    }

/**
 * @brief Default configuration for IR parser
 *
 */
#define IR_PARSER_DEFAULT_CONFIG(dev) \
    {                                 \
        .dev_hdl = dev,               \
        .flags = 0,                   \
        .margin_us = 100,             \
    }

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

static const char *TAG = "Example";

#define IR_CHECK(a, str, goto_tag, ret_value, ...)                               \
    do                                                                            \
    {                                                                             \
        if (!(a))                                                                 \
        {                                                                         \
            ESP_LOGE(TAG, "%s(%d): " str, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
            ret = ret_value;                                                      \
            goto goto_tag;                                                        \
        }                                                                         \
    } while (0)

/**
 * @brief IR Protocol Selection
 *
 */
// ir_protocol_type_t ir_protocol_selection(ir_remote_info_t*  ir_remote_info_handler);


/**
 * @brief IR Protocol Initialization
 *
 */
// esp_err_t ir_protocol_init(ir_protocol_init_t *ir_protocol_init_props, ir_remote_info_t *ir_remote_info_handler, ir_parser_config_t* ir_parser_config, ir_builder_config_t* ir_builder_config);

#ifdef __cplusplus
}
#endif