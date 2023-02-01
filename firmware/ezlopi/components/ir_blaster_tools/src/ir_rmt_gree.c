
#include <stdlib.h>
#include <sys/cdefs.h>
#include "esp_log.h"
#include "ir_tools.h"
#include "ir_timings.h"
#include "driver/rmt.h"
#include "ir_config.h"

/**
 * @brief RMT WORDS IN GREE DATA FRAME
 *
 */

#define GREE_DATA_FRAME_RMT_WORDS (70)

/** ===============================================================================
 * @brief GREE BUILDER FUNCTIONS
 * ================================================================================
 */


static esp_err_t gree_build_frame(ir_builder_t *builder, uint32_t address, uint32_t command)
{
    esp_err_t ret = ESP_OK;
    ir_protocol_builder_t *ir_protocol_builder = __containerof(builder, ir_protocol_builder_t, parent);

    builder->make_head(builder);
    // MSB -> LSB
     for (int i = 0; i < 32; i++) {
        if (address & (1 << i)) {
            builder->make_logic1(builder);
        } else {
            builder->make_logic0(builder);
        }
       // ESP_LOGI("BUFFER INFO", "%d", i);
    }
    
    builder->make_footer(builder);
    builder->make_message_space(builder);
    //ESP_LOGI("INFO", "Data1 left to encode");
    //data1
    for (int i = 0; i < 32; i++) {
        if (command & (1 << i)) {
            builder->make_logic1(builder);
        } else {
            builder->make_logic0(builder);
        }
    }
    builder->make_end(builder);
    return ESP_OK;
// err:
//    return ret;
}

ir_builder_t *ir_builder_rmt_new_gree(const ir_builder_config_t *config)
{
    ir_builder_t *ret = NULL;
    IR_CHECK(config, "gree configuration can't be null", err, NULL);
    IR_CHECK(config->buffer_size, "buffer size can't be zero", err, NULL);

    uint32_t builder_size = sizeof(ir_protocol_builder_t) + config->buffer_size * sizeof(rmt_item32_t);
    ir_protocol_builder_t *ir_protocol_builder = calloc(1, builder_size);
    IR_CHECK(ir_protocol_builder, "request memory for gree_builder failed", err, NULL);

    ir_protocol_builder->buffer_size = config->buffer_size;
    ir_protocol_builder->flags = config->flags;
    if (config->flags & IR_TOOLS_FLAGS_INVERSE) {
        ir_protocol_builder->inverse = true;
    }

    uint32_t counter_clk_hz = 0;
    IR_CHECK(rmt_get_counter_clock((rmt_channel_t)config->dev_hdl, &counter_clk_hz) == ESP_OK,
              "get rmt counter clock failed", err, NULL);
    float ratio = (float)counter_clk_hz / 1e6;
    ir_protocol_builder->leading_code_high_ticks = (uint32_t)(ratio * GREE_LEADING_CODE_HIGH_US);
    ir_protocol_builder->leading_code_low_ticks = (uint32_t)(ratio * GREE_LEADING_CODE_LOW_US);
    ir_protocol_builder->message_gap_high_ticks = (uint32_t)(ratio * GREE_MESSAGE_SPACE_HIGH_US);
    ir_protocol_builder->message_gap_low_ticks = (uint32_t)(ratio * GREE_MESSAGE_SPACE_LOW_US);
    ir_protocol_builder->payload_logic0_high_ticks = (uint32_t)(ratio * GREE_PAYLOAD_ZERO_HIGH_US);
    ir_protocol_builder->payload_logic0_low_ticks = (uint32_t)(ratio * GREE_PAYLOAD_ZERO_LOW_US);
    ir_protocol_builder->payload_logic1_high_ticks = (uint32_t)(ratio * GREE_PAYLOAD_ONE_HIGH_US);
    ir_protocol_builder->payload_logic1_low_ticks = (uint32_t)(ratio * GREE_PAYLOAD_ONE_LOW_US);
    ir_protocol_builder->ending_code_high_ticks = (uint32_t)(ratio * GREE_ENDING_CODE_HIGH_US);
    ir_protocol_builder->ending_code_low_ticks = 0x7FFF;
    ir_protocol_builder->parent.make_head = ir_protocol_builder_make_head;
    ir_protocol_builder->parent.make_logic0 = ir_protocol_builder_make_logic0;
    ir_protocol_builder->parent.make_logic1 = ir_protocol_builder_make_logic1;
    ir_protocol_builder->parent.make_end = ir_protocol_builder_make_end;
    ir_protocol_builder->parent.frame_builder_t.build_frame = gree_build_frame;
    ir_protocol_builder->parent.make_footer = ir_protocol_builder_make_footer;
    ir_protocol_builder->parent.make_message_space = ir_protocol_builder_make_message_space;
    //ir_protocol_builder->parent.build_repeat_frame = ir_protocol_build_repeat_frame;
    ir_protocol_builder->parent.get_result = ir_protocol_builder_get_result;
    ir_protocol_builder->parent.del = ir_protocol_builder_del;
    //ir_protocol_builder->parent.repeat_period_ms = 110;
    return &ir_protocol_builder->parent;
err:
    return ret;
}

/** ===============================================================================
 * @brief GREE PARSER FUNCTIONS
 * ================================================================================
 */
static esp_err_t gree_parser_input(ir_parser_t *parser, void *raw_data, uint32_t length)
{
    esp_err_t ret = ESP_OK;
    ir_protocol_parser_t *ir_protocol_parser = __containerof(parser, ir_protocol_parser_t, parent);
    IR_CHECK(raw_data, "input data can't be null", err, ESP_ERR_INVALID_ARG);
    ir_protocol_parser->buffer = raw_data;
    // Data Frame costs 34 items and Repeat Frame costs 2 items
    if (length == GREE_DATA_FRAME_RMT_WORDS) {
        ir_protocol_parser->repeat = false;
    }// else if (length == GREE_REPEAT_FRAME_RMT_WORDS) {
      //  ir_protocol_parser->repeat = true;
    //}
    else {
        ret = ESP_FAIL;
    }
    return ret;
err:
    return ret;
}

static esp_err_t gree_parser_get_scan_code_gree(ir_parser_t *parser, uint32_t *address, uint32_t *footer, uint32_t *command, bool *repeat)
{
    esp_err_t ret = ESP_FAIL;
    uint32_t addr = 0;
    uint32_t cmd = 0;
    uint32_t ftr = 0;
    bool logic_value = false;
    ir_protocol_parser_t *ir_protocol_parser = __containerof(parser, ir_protocol_parser_t, parent);
    IR_CHECK(address && command && repeat, "address, command and repeat can't be null", out, ESP_ERR_INVALID_ARG);
    if (ir_protocol_parse_head(ir_protocol_parser)) 
    {
        for (int i = 0; i < 32; i++) {
            if (ir_protocol_parse_logic(parser, &logic_value) == ESP_OK) {
                    addr |= (logic_value << i);
            }
        }
        for (int i = 0; i < 3; i++) {
            if (ir_protocol_parse_logic(parser, &logic_value) == ESP_OK) {
                ftr |= (logic_value << i);
            }
        }
        if (ir_protocol_parse_message(ir_protocol_parser)){
            ESP_LOGI("INFO", "Message space good");
        }
        //ESP_LOGI("INFO", "Message out");
        for (int i = 0; i < 32; i++) {
            if (ir_protocol_parse_logic(parser, &logic_value) == ESP_OK) {
                cmd |= (logic_value << i);
               // ESP_LOGI("INFO", "data1 decoding %d", i);
            }
        }
        *address = addr;
        *command = cmd;
        *footer = ftr;
        *repeat = false;
        // keep it as potential repeat code
        //ir_protocol_parser->last_address = addr;
        //ir_protocol_parser->last_command = cmd;
        ret = ESP_OK;
    }
    else{
        ESP_LOGI("INFO", "NO HEADER MATCH");
        //ESP_LOGI("TIME INFO",)
    }
out:
    return ret;
}

ir_parser_t *ir_parser_rmt_new_gree(const ir_parser_config_t *config)
{
    ir_parser_t *ret = NULL;
    IR_CHECK(config, "gree configuration can't be null", err, NULL);

    ir_protocol_parser_t *ir_protocol_parser = calloc(1, sizeof(ir_protocol_parser_t));
    IR_CHECK(ir_protocol_parser, "request memory for gree_parser failed", err, NULL);

    ir_protocol_parser->flags = config->flags;
    if (config->flags & IR_TOOLS_FLAGS_INVERSE) {
        ir_protocol_parser->inverse = true;
    }

    uint32_t counter_clk_hz = 0;
    IR_CHECK(rmt_get_counter_clock((rmt_channel_t)config->dev_hdl, &counter_clk_hz) == ESP_OK,
              "get rmt counter clock failed", err, NULL);
    float ratio = (float)counter_clk_hz / 1e6;
    ir_protocol_parser->leading_code_high_ticks = (uint32_t)(ratio * GREE_LEADING_CODE_HIGH_US);
    ir_protocol_parser->leading_code_low_ticks = (uint32_t)(ratio * GREE_LEADING_CODE_LOW_US);
    ir_protocol_parser->message_gap_high_ticks = (uint32_t)(ratio * GREE_MESSAGE_SPACE_HIGH_US);
    ir_protocol_parser->message_gap_low_ticks = (uint32_t)(ratio * GREE_MESSAGE_SPACE_LOW_US);
    ir_protocol_parser->payload_logic0_high_ticks = (uint32_t)(ratio * GREE_PAYLOAD_ZERO_HIGH_US);
    ir_protocol_parser->payload_logic0_low_ticks = (uint32_t)(ratio * GREE_PAYLOAD_ZERO_LOW_US);
    ir_protocol_parser->payload_logic1_high_ticks = (uint32_t)(ratio * GREE_PAYLOAD_ONE_HIGH_US);
    ir_protocol_parser->payload_logic1_low_ticks = (uint32_t)(ratio * GREE_PAYLOAD_ONE_LOW_US);
    ir_protocol_parser->margin_ticks = (uint32_t)(ratio * config->margin_us);
    ir_protocol_parser->parent.input = gree_parser_input;
    ir_protocol_parser->parent.scan_code_t.get_scan_code_gree = gree_parser_get_scan_code_gree;
    ir_protocol_parser->parent.del = ir_protocol_parser_del;
    return &ir_protocol_parser->parent;
err:
    return ret;
}