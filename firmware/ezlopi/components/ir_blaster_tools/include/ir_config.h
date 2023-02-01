#pragma once

#ifdef __cplusplus
extern "C" {
#endif


#include <sys/cdefs.h>
#include "esp_log.h"
#include "ir_tools.h"
#include "ir_timings.h"
#include "driver/rmt.h"

static const char *TAG = "Example";
//static rmt_channel_t example_tx_channel = CONFIG_EXAMPLE_RMT_TX_CHANNEL;
//static rmt_channel_t example_rx_channel = CONFIG_EXAMPLE_RMT_RX_CHANNEL;

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
 * @brief IR Builder timings and functions
 *
 */
typedef struct {
    ir_builder_t parent;
    uint32_t buffer_size;
    uint32_t cursor;
    uint32_t flags;
    uint32_t leading_code_high_ticks;
    uint32_t leading_code_low_ticks;
    uint32_t message_gap_high_ticks;
    uint32_t message_gap_low_ticks;
    uint32_t repeat_code_high_ticks;
    uint32_t repeat_code_low_ticks;
    uint32_t payload_logic0_high_ticks;
    uint32_t payload_logic0_low_ticks;
    uint32_t payload_logic1_high_ticks;
    uint32_t payload_logic1_low_ticks;
    uint32_t ending_code_high_ticks;
    uint32_t ending_code_low_ticks;
    bool inverse;
    rmt_item32_t buffer[0];
} ir_protocol_builder_t;                          // IF ERROR IN NAME CONFLICTION

/**
 * @brief IR Parser timings and functions
 *
 */
typedef struct {
    ir_parser_t parent;
    uint32_t flags;
    uint32_t leading_code_high_ticks;
    uint32_t leading_code_low_ticks;
    uint32_t message_gap_high_ticks;
    uint32_t message_gap_low_ticks;
    uint32_t new_message_low_ticks;
    uint32_t repeat_code_high_ticks;
    uint32_t repeat_code_low_ticks;
    uint32_t payload_logic0_high_ticks;
    uint32_t payload_logic0_low_ticks;
    uint32_t payload_logic1_high_ticks;
    uint32_t payload_logic1_low_ticks;
    uint32_t margin_ticks;
    rmt_item32_t *buffer;
    uint32_t cursor;
    uint32_t last_address;
    uint32_t last_command;
    bool repeat;
    bool inverse;
} ir_protocol_parser_t;


/** =================================================================================
 * 
 * @brief IR BUILDER FUNCTIONS
 *
 * =================================================================================
 */

static esp_err_t ir_protocol_builder_make_head(ir_builder_t *builder)
{
    ir_protocol_builder_t *ir_protocol_builder = __containerof(builder, ir_protocol_builder_t, parent);
    ir_protocol_builder->cursor = 0;
    ir_protocol_builder->buffer[ir_protocol_builder->cursor].level0 = !ir_protocol_builder->inverse;
    ir_protocol_builder->buffer[ir_protocol_builder->cursor].duration0 = ir_protocol_builder->leading_code_high_ticks;
    ir_protocol_builder->buffer[ir_protocol_builder->cursor].level1 = ir_protocol_builder->inverse;
    ir_protocol_builder->buffer[ir_protocol_builder->cursor].duration1 = ir_protocol_builder->leading_code_low_ticks;
    ir_protocol_builder->cursor += 1;
    return ESP_OK;
}

static esp_err_t ir_protocol_builder_make_logic0(ir_builder_t *builder)
{
    ir_protocol_builder_t *ir_protocol_builder = __containerof(builder, ir_protocol_builder_t, parent);
    ir_protocol_builder->buffer[ir_protocol_builder->cursor].level0 = !ir_protocol_builder->inverse;
    ir_protocol_builder->buffer[ir_protocol_builder->cursor].duration0 = ir_protocol_builder->payload_logic0_high_ticks;
    ir_protocol_builder->buffer[ir_protocol_builder->cursor].level1 = ir_protocol_builder->inverse;
    ir_protocol_builder->buffer[ir_protocol_builder->cursor].duration1 = ir_protocol_builder->payload_logic0_low_ticks;
    ir_protocol_builder->cursor += 1;
    return ESP_OK;
}

static esp_err_t ir_protocol_builder_make_logic1(ir_builder_t *builder)
{
    ir_protocol_builder_t *ir_protocol_builder = __containerof(builder, ir_protocol_builder_t, parent);
    ir_protocol_builder->buffer[ir_protocol_builder->cursor].level0 = !ir_protocol_builder->inverse;
    ir_protocol_builder->buffer[ir_protocol_builder->cursor].duration0 = ir_protocol_builder->payload_logic1_high_ticks;
    ir_protocol_builder->buffer[ir_protocol_builder->cursor].level1 = ir_protocol_builder->inverse;
    ir_protocol_builder->buffer[ir_protocol_builder->cursor].duration1 = ir_protocol_builder->payload_logic1_low_ticks;
    ir_protocol_builder->cursor += 1;
    return ESP_OK;
}

static esp_err_t ir_protocol_builder_make_end(ir_builder_t *builder)
{
    ir_protocol_builder_t *ir_protocol_builder = __containerof(builder, ir_protocol_builder_t, parent);
    ir_protocol_builder->buffer[ir_protocol_builder->cursor].level0 = !ir_protocol_builder->inverse;
    ir_protocol_builder->buffer[ir_protocol_builder->cursor].duration0 = ir_protocol_builder->ending_code_high_ticks;
    ir_protocol_builder->buffer[ir_protocol_builder->cursor].level1 = ir_protocol_builder->inverse;
    ir_protocol_builder->buffer[ir_protocol_builder->cursor].duration1 = ir_protocol_builder->ending_code_low_ticks;
    ir_protocol_builder->cursor += 1;
    ir_protocol_builder->buffer[ir_protocol_builder->cursor].val = 0;
    ir_protocol_builder->cursor += 1;
    return ESP_OK;
}

/******** To Make Footer BITS 010 *****************/
static esp_err_t ir_protocol_builder_make_footer(ir_builder_t *builder)
{
    //ESP_LOGI("INFO", "Footer function called");
    //gree_builder_t *gree_builder = __containerof(builder, gree_builder_t, parent);
    builder->make_logic0(builder);
    builder->make_logic1(builder);
    builder->make_logic0(builder);
    //gree_builder->cursor += 1;
    return ESP_OK;
}

static esp_err_t ir_protocol_builder_make_message_space(ir_builder_t *builder)
{
    //ESP_LOGI("INFO", "Message SPACE function called");
    ir_protocol_builder_t *ir_protocol_builder = __containerof(builder, ir_protocol_builder_t, parent);
    ir_protocol_builder->buffer[ir_protocol_builder->cursor].level0 = !ir_protocol_builder->inverse;
    ir_protocol_builder->buffer[ir_protocol_builder->cursor].duration0 = ir_protocol_builder->message_gap_high_ticks;
    ir_protocol_builder->buffer[ir_protocol_builder->cursor].level1 = ir_protocol_builder->inverse;
    ir_protocol_builder->buffer[ir_protocol_builder->cursor].duration1 = ir_protocol_builder->message_gap_low_ticks;
    ir_protocol_builder->cursor += 1;
    //ESP_LOGI("INFO", "Message SPACE2 function called");
    return ESP_OK;
}

static esp_err_t ir_protocol_build_repeat_frame(ir_builder_t *builder)
{
    ir_protocol_builder_t *ir_protocol_builder = __containerof(builder, ir_protocol_builder_t, parent);
    ir_protocol_builder->cursor = 0;
    ir_protocol_builder->buffer[ir_protocol_builder->cursor].level0 = !ir_protocol_builder->inverse;
    ir_protocol_builder->buffer[ir_protocol_builder->cursor].duration0 = ir_protocol_builder->repeat_code_high_ticks;
    ir_protocol_builder->buffer[ir_protocol_builder->cursor].level1 = ir_protocol_builder->inverse;
    ir_protocol_builder->buffer[ir_protocol_builder->cursor].duration1 = ir_protocol_builder->repeat_code_low_ticks;
    ir_protocol_builder->cursor += 1;
    ir_protocol_builder_make_end(builder);
    return ESP_OK;
}

static esp_err_t ir_protocol_builder_get_result(ir_builder_t *builder, void *result, size_t *length)
{
    esp_err_t ret = ESP_OK;
    ir_protocol_builder_t *ir_protocol_builder = __containerof(builder, ir_protocol_builder_t, parent);
    IR_CHECK(result && length, "result and length can't be null", err, ESP_ERR_INVALID_ARG);
    *(rmt_item32_t **)result = ir_protocol_builder->buffer;
    *length = ir_protocol_builder->cursor;
    return ESP_OK;
err:
    return ret;
}

static esp_err_t ir_protocol_builder_del(ir_builder_t *builder)
{
    ir_protocol_builder_t *ir_protocol_builder = __containerof(builder, ir_protocol_builder_t, parent);
    free(ir_protocol_builder);
    return ESP_OK;
}

// =====================================================================================================
/**
 * @brief IR BUILDER FUNCTIONS
 *
 * =====================================================================================================
 */
static inline bool ir_protocol_check_in_range(uint32_t raw_ticks, uint32_t target_ticks, uint32_t margin_ticks)
{
    return (raw_ticks < (target_ticks + margin_ticks)) && (raw_ticks > (target_ticks - margin_ticks));
}

static bool ir_protocol_parse_head(ir_protocol_parser_t *ir_protocol_parser)
{
    ir_protocol_parser->cursor = 0;
    rmt_item32_t item = ir_protocol_parser->buffer[ir_protocol_parser->cursor];
    bool ret = (item.level0 == ir_protocol_parser->inverse) && (item.level1 != ir_protocol_parser->inverse) &&
               ir_protocol_check_in_range(item.duration0, ir_protocol_parser->leading_code_high_ticks, ir_protocol_parser->margin_ticks) &&
               ir_protocol_check_in_range(item.duration1, ir_protocol_parser->leading_code_low_ticks, ir_protocol_parser->margin_ticks);
    ir_protocol_parser->cursor += 1;
    return ret;
}

static bool ir_protocol_parse_logic0(ir_protocol_parser_t *ir_protocol_parser)
{
    rmt_item32_t item = ir_protocol_parser->buffer[ir_protocol_parser->cursor];
    bool ret = (item.level0 == ir_protocol_parser->inverse) && (item.level1 != ir_protocol_parser->inverse) &&
               ir_protocol_check_in_range(item.duration0, ir_protocol_parser->payload_logic0_high_ticks, ir_protocol_parser->margin_ticks) &&
               ir_protocol_check_in_range(item.duration1, ir_protocol_parser->payload_logic0_low_ticks, ir_protocol_parser->margin_ticks);
    return ret;
}

static bool ir_protocol_parse_logic1(ir_protocol_parser_t *ir_protocol_parser)
{
    rmt_item32_t item = ir_protocol_parser->buffer[ir_protocol_parser->cursor];
    bool ret = (item.level0 == ir_protocol_parser->inverse) && (item.level1 != ir_protocol_parser->inverse) &&
               ir_protocol_check_in_range(item.duration0, ir_protocol_parser->payload_logic1_high_ticks, ir_protocol_parser->margin_ticks) &&
               ir_protocol_check_in_range(item.duration1, ir_protocol_parser->payload_logic1_low_ticks, ir_protocol_parser->margin_ticks);
    return ret;
}

static bool ir_protocol_parse_message(ir_protocol_parser_t *ir_protocol_parser)
{
    rmt_item32_t item = ir_protocol_parser->buffer[ir_protocol_parser->cursor];
    bool ret = (item.level0 == ir_protocol_parser->inverse) && (item.level1 != ir_protocol_parser->inverse) &&
               ir_protocol_check_in_range(item.duration0, ir_protocol_parser->message_gap_high_ticks, ir_protocol_parser->margin_ticks) &&
               ir_protocol_check_in_range(item.duration1, ir_protocol_parser->message_gap_low_ticks, ir_protocol_parser->margin_ticks);
    ir_protocol_parser->cursor += 1;
   // ESP_LOGI("INFO", "Message function called");
    return ret;
}

static esp_err_t ir_protocol_parse_logic(ir_parser_t *parser, bool *logic)
{
    esp_err_t ret = ESP_FAIL;
    bool logic_value = false;
    ir_protocol_parser_t *ir_protocol_parser = __containerof(parser, ir_protocol_parser_t, parent);
    if (ir_protocol_parse_logic0(ir_protocol_parser)) {
        logic_value = false;
        ret = ESP_OK;
    } else if (ir_protocol_parse_logic1(ir_protocol_parser)) {
        logic_value = true;
        ret = ESP_OK;
    }
    if (ret == ESP_OK) {
        *logic = logic_value;
    }
    ir_protocol_parser->cursor += 1;
    return ret;
}

static bool ir_protocol_parse_repeat_frame(ir_protocol_parser_t *ir_protocol_parser)
{
    ir_protocol_parser->cursor = 0;
    rmt_item32_t item = ir_protocol_parser->buffer[ir_protocol_parser->cursor];
    bool ret = (item.level0 == ir_protocol_parser->inverse) && (item.level1 != ir_protocol_parser->inverse) &&
               ir_protocol_check_in_range(item.duration0, ir_protocol_parser->repeat_code_high_ticks, ir_protocol_parser->margin_ticks) &&
               ir_protocol_check_in_range(item.duration1, ir_protocol_parser->repeat_code_low_ticks, ir_protocol_parser->margin_ticks);
    ir_protocol_parser->cursor += 1;
    return ret;
}

static esp_err_t ir_protocol_parser_del(ir_parser_t *parser)
{
    ir_protocol_parser_t *ir_protocol_parser = __containerof(parser, ir_protocol_parser_t, parent);
    free(ir_protocol_parser);
    return ESP_OK;
}

#ifdef __cplusplus
}
#endif