// Copyright 2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include <stdlib.h>
#include <sys/cdefs.h>
#include "esp_log.h"
#include "ir_tools.h"
#include "ir_timings.h"
#include "driver/rmt.h"
#include "ir_config.h"

/**
 * @brief RMT WORDS IN NEC DATA FRAME
 *
 */
#if CONFIG_EXAMPLE_IR_PROTOCOL_AIWA
#define NEC_DATA_FRAME_RMT_WORDS (44)
#else
#define NEC_DATA_FRAME_RMT_WORDS (34)
#endif
#define NEC_REPEAT_FRAME_RMT_WORDS (2)

/** ===============================================================================
 * @brief NEC BUILDER FUNCTIONS
 * ================================================================================
 */
static esp_err_t nec_build_frame(ir_builder_t *builder, uint32_t address, uint32_t command)
{
    esp_err_t ret = ESP_OK;
    ir_protocol_builder_t *ir_protocol_builder = __containerof(builder, ir_protocol_builder_t, parent);
    
// #if CONFIG_EXAMPLE_IR_PROTOCOL_AIWA
//     builder->make_head(builder);
//     // LSB -> MSB
//     for (int i = 0; i < 26; i++) {
//         if (address & (1 << i)) {
//             builder->make_logic1(builder);
//         } else {
//             builder->make_logic0(builder);
//         }
//     }
//     for (int i = 0; i < 16; i++) {
//         if (command & (1 << i)) {
//             builder->make_logic1(builder);
//         } else {
//             builder->make_logic0(builder);
//         }
//     }
//     builder->make_end(builder);
//     return ESP_OK;
// #else
    // if (!ir_protocol_builder->flags & IR_TOOLS_FLAGS_PROTO_EXT) {
    //     uint8_t low_byte = address & 0xFF;
    //     uint8_t high_byte = (address >> 8) & 0xFF;
    //     IR_CHECK(low_byte == (~high_byte & 0xFF), "address not match standard NEC protocol", err, ESP_ERR_INVALID_ARG);
    //     low_byte = command & 0xFF;
    //     high_byte = (command >> 8) & 0xFF;
    //     IR_CHECK(low_byte == (~high_byte & 0xFF), "command not match standard NEC protocol", err, ESP_ERR_INVALID_ARG);
    // }
    builder->make_head(builder);
    // LSB -> MSB
    for (int i = 0; i < 16; i++) {
        if (address & (1 << i)) {
            builder->make_logic1(builder);
        } else {
            builder->make_logic0(builder);
        }
    }
    for (int i = 0; i < 16; i++) {
        if (command & (1 << i)) {
            builder->make_logic1(builder);
        } else {
            builder->make_logic0(builder);
        }
    }
    builder->make_end(builder);
    ret = ESP_OK;
    return ret;
//#endif
}

ir_builder_t *ir_builder_rmt_new_nec(const ir_builder_config_t *config)
{
    ir_builder_t *ret = NULL;
    IR_CHECK(config, "nec configuration can't be null", err, NULL);
    IR_CHECK(config->buffer_size, "buffer size can't be zero", err, NULL);

    uint32_t builder_size = sizeof(ir_protocol_builder_t) + config->buffer_size * sizeof(rmt_item32_t);
    ir_protocol_builder_t *ir_protocol_builder = calloc(1, builder_size);
    IR_CHECK(ir_protocol_builder, "request memory for nec_builder failed", err, NULL);

    ir_protocol_builder->buffer_size = config->buffer_size;
    ir_protocol_builder->flags = config->flags;
    if (config->flags & IR_TOOLS_FLAGS_INVERSE) {
        ir_protocol_builder->inverse = true;
    }

    uint32_t counter_clk_hz = 0;
    IR_CHECK(rmt_get_counter_clock((rmt_channel_t)config->dev_hdl, &counter_clk_hz) == ESP_OK,
              "get rmt counter clock failed", err, NULL);
    float ratio = (float)counter_clk_hz / 1e6;
    ir_protocol_builder->leading_code_high_ticks = (uint32_t)(ratio * NEC_LEADING_CODE_HIGH_US);
    ir_protocol_builder->leading_code_low_ticks = (uint32_t)(ratio * NEC_LEADING_CODE_LOW_US);
    ir_protocol_builder->repeat_code_high_ticks = (uint32_t)(ratio * NEC_REPEAT_CODE_HIGH_US);
    ir_protocol_builder->repeat_code_low_ticks = (uint32_t)(ratio * NEC_REPEAT_CODE_LOW_US);
    ir_protocol_builder->payload_logic0_high_ticks = (uint32_t)(ratio * NEC_PAYLOAD_ZERO_HIGH_US);
    ir_protocol_builder->payload_logic0_low_ticks = (uint32_t)(ratio * NEC_PAYLOAD_ZERO_LOW_US);
    ir_protocol_builder->payload_logic1_high_ticks = (uint32_t)(ratio * NEC_PAYLOAD_ONE_HIGH_US);
    ir_protocol_builder->payload_logic1_low_ticks = (uint32_t)(ratio * NEC_PAYLOAD_ONE_LOW_US);
    ir_protocol_builder->ending_code_high_ticks = (uint32_t)(ratio * NEC_ENDING_CODE_HIGH_US);
    ir_protocol_builder->ending_code_low_ticks = 0x7FFF;
    ir_protocol_builder->parent.make_head = ir_protocol_builder_make_head;
    ir_protocol_builder->parent.make_logic0 = ir_protocol_builder_make_logic0;
    ir_protocol_builder->parent.make_logic1 = ir_protocol_builder_make_logic1;
    ir_protocol_builder->parent.make_end = ir_protocol_builder_make_end;
    ir_protocol_builder->parent.frame_builder_t.build_frame = nec_build_frame;
    ir_protocol_builder->parent.build_repeat_frame = ir_protocol_build_repeat_frame;
    ir_protocol_builder->parent.get_result = ir_protocol_builder_get_result;
    ir_protocol_builder->parent.del = ir_protocol_builder_del;
    ir_protocol_builder->parent.repeat_period_ms = 110;
    return &ir_protocol_builder->parent;
err:
    return ret;
}

/** ===============================================================================
 * @brief NEC PARSER FUNCTIONS
 * ================================================================================
 */
static esp_err_t nec_parser_input(ir_parser_t *parser, void *raw_data, uint32_t length)
{
    esp_err_t ret = ESP_OK;
    ir_protocol_parser_t *ir_protocol_parser = __containerof(parser, ir_protocol_parser_t, parent);
    IR_CHECK(raw_data, "input data can't be null", err, ESP_ERR_INVALID_ARG);
    ir_protocol_parser->buffer = raw_data;
    // Data Frame costs 34 items and Repeat Frame costs 2 items
    if (length == NEC_DATA_FRAME_RMT_WORDS) {
        ir_protocol_parser->repeat = false;
    } else if (length == NEC_REPEAT_FRAME_RMT_WORDS) {
        ir_protocol_parser->repeat = true;
    } else {
        ret = ESP_FAIL;
    }
    return ret;
err:
    return ret;
}

static esp_err_t nec_parser_get_scan_code(ir_parser_t *parser, uint32_t *address, uint32_t *command, bool *repeat)
{
    esp_err_t ret = ESP_FAIL;
    uint32_t addr = 0;
    uint32_t cmd = 0;
    bool logic_value = false;
    ir_protocol_parser_t *ir_protocol_parser = __containerof(parser, ir_protocol_parser_t, parent);
    IR_CHECK(address && command && repeat, "address, command and repeat can't be null", out, ESP_ERR_INVALID_ARG);
    if (ir_protocol_parser->repeat) {
        if (ir_protocol_parse_repeat_frame(ir_protocol_parser)) {
            *address = ir_protocol_parser->last_address;
            *command = ir_protocol_parser->last_command;
            *repeat = true;
            ret = ESP_OK;
        }
    } else {
        if (ir_protocol_parse_head(ir_protocol_parser)) {
// #if CONFIG_EXAMPLE_IR_PROTOCOL_AIWA
//             for (int i = 0; i < 26; i++) {
//                 if (ir_protocol_parse_logic(parser, &logic_value) == ESP_OK) {
//                     addr |= (logic_value << i);
//                 }
//             }
//             for (int i = 0; i < 16; i++) {
//                 if (ir_protocol_parse_logic(parser, &logic_value) == ESP_OK) {
//                     cmd |= (logic_value << i);
//                 }
//             }
// #else
            for (int i = 0; i < 16; i++) {
                if (ir_protocol_parse_logic(parser, &logic_value) == ESP_OK) {
                    addr |= (logic_value << i);
                }
            }
            for (int i = 0; i < 16; i++) {
                if (ir_protocol_parse_logic(parser, &logic_value) == ESP_OK) {
                    cmd |= (logic_value << i);
                }
            }
//#endif
            *address = addr;
            *command = cmd;
            *repeat = false;
            // keep it as potential repeat code
            ir_protocol_parser->last_address = addr;
            ir_protocol_parser->last_command = cmd;
            ret = ESP_OK;
        }
    }
out:
    return ret;
}

ir_parser_t *ir_parser_rmt_new_nec(const ir_parser_config_t *config)
{
    ir_parser_t *ret = NULL;
    IR_CHECK(config, "nec configuration can't be null", err, NULL);

    ir_protocol_parser_t *ir_protocol_parser = calloc(1, sizeof(ir_protocol_parser_t));
    IR_CHECK(ir_protocol_parser, "request memory for nec_parser failed", err, NULL);

    ir_protocol_parser->flags = config->flags;
    if (config->flags & IR_TOOLS_FLAGS_INVERSE) {
        ir_protocol_parser->inverse = true;
    }

    uint32_t counter_clk_hz = 0;
    IR_CHECK(rmt_get_counter_clock((rmt_channel_t)config->dev_hdl, &counter_clk_hz) == ESP_OK,
              "get rmt counter clock failed", err, NULL);
    float ratio = (float)counter_clk_hz / 1e6;
    ir_protocol_parser->leading_code_high_ticks = (uint32_t)(ratio * NEC_LEADING_CODE_HIGH_US);
    ir_protocol_parser->leading_code_low_ticks = (uint32_t)(ratio * NEC_LEADING_CODE_LOW_US);
    ir_protocol_parser->repeat_code_high_ticks = (uint32_t)(ratio * NEC_REPEAT_CODE_HIGH_US);
    ir_protocol_parser->repeat_code_low_ticks = (uint32_t)(ratio * NEC_REPEAT_CODE_LOW_US);
    ir_protocol_parser->payload_logic0_high_ticks = (uint32_t)(ratio * NEC_PAYLOAD_ZERO_HIGH_US);
    ir_protocol_parser->payload_logic0_low_ticks = (uint32_t)(ratio * NEC_PAYLOAD_ZERO_LOW_US);
    ir_protocol_parser->payload_logic1_high_ticks = (uint32_t)(ratio * NEC_PAYLOAD_ONE_HIGH_US);
    ir_protocol_parser->payload_logic1_low_ticks = (uint32_t)(ratio * NEC_PAYLOAD_ONE_LOW_US);
    ir_protocol_parser->margin_ticks = (uint32_t)(ratio * config->margin_us);
    ir_protocol_parser->parent.input = nec_parser_input;
    ir_protocol_parser->parent.scan_code_t.get_scan_code = nec_parser_get_scan_code;
    ir_protocol_parser->parent.del = ir_protocol_parser_del;
    return &ir_protocol_parser->parent;
err:
    return ret;
}