//=============================================================================
// TTTTTTTTTTT   OOO     SSSS  H  H  IIIII  BBB     AAA        AAA     CCCCC
//     TT      OO   OO  S      H  H    I    B  B   A   A      A   A    C   
//     TT     OO     OO  SSS   HHHH    I    BBB    AAAAA      AAAAA    C
//     TT      OO   OO      S  H  H    I    B  B   A   A      A   A    C   
//     TT        OOO    SSSS   H  H  IIIII  BBB    A   A      A   A    CCCCC
//============================================================================
#include <stdlib.h>
#include <sys/cdefs.h>
#include "esp_log.h"
#include "ir_tools.h"
#include "ir_timings.h"
#include "driver/rmt.h"
#include "ir_config.h"

/**
 * @brief RMT WORDS IN TOSHIBAAC DATA FRAME
 *
 */

#if CONFIG_EXAMPLE_IR_PROTOCOL_TOSHIBAAC72
#define TOSHIBAAC_DATA_FRAME_RMT_WORDS (148)
#else
#define TOSHIBAAC_DATA_FRAME_RMT_WORDS (100)
#endif
#define TOSHIBAAC_REPEAT_FRAME_RMT_WORDS (2)

/** ===============================================================================
 * @brief TOSHIBAAC BUILDER FUNCTIONS
 * ================================================================================
 */


static esp_err_t toshibaAC_build_frame(ir_builder_t *builder, uint32_t address, uint32_t command)
{
    esp_err_t ret = ESP_OK;
    ir_protocol_builder_t *ir_protocol_builder = __containerof(builder, ir_protocol_builder_t, parent);

    ir_protocol_builder->cursor = 0;
    builder->make_head(builder);
    // LSB -> MSB
    for (int i = 23; i >= 0; i--) {
        if (address & (1 << i)) {
            builder->make_logic1(builder);
        } else {
            builder->make_logic0(builder);
        }
    }
    for (int i = 23; i >= 0; i--) {
        if (command & (1 << i)) {
            builder->make_logic1(builder);
        } else {
            builder->make_logic0(builder);
        }
    }
    builder->make_message_space(builder);
    builder->make_head(builder);
    for (int i = 23; i >= 0; i--) {
        if (address & (1 << i)) {
            builder->make_logic1(builder);
        } else {
            builder->make_logic0(builder);
        }
    }
    for (int i = 23; i >= 0; i--) {
        if (command & (1 << i)) {
            builder->make_logic1(builder);
        } else {
            builder->make_logic0(builder);
        }
    }
    builder->make_end(builder);
    return ESP_OK;
}

static esp_err_t toshibaAC_build_frame_toshibaAC(ir_builder_t *builder, uint32_t address, uint32_t command, uint32_t checksum, uint32_t addr, uint32_t cmd, uint32_t cksum)
{
    //esp_err_t ret = ESP_OK;
    //uint32_t address1 = address;
   // uint32_t command1 = command;
    ir_protocol_builder_t *ir_protocol_builder = __containerof(builder, ir_protocol_builder_t, parent);

    ir_protocol_builder->cursor = 0;
    builder->make_head(builder);
    // LSB -> MSB
    for (int i = 31; i >= 0; i--) {
        if (address & (1 << i)) {
            builder->make_logic1(builder);
        } else {
            builder->make_logic0(builder);
        }
    }
   // builder->make_message_space(builder);
    for (int i = 31; i >= 0; i--) {
        if (command & (1 << i)) {
            builder->make_logic1(builder);
        } else {
            builder->make_logic0(builder);
        }
    }
    for (int i = 7; i >= 0; i--) {
        if (checksum & (1 << i)) {
            builder->make_logic1(builder);
        } else {
            builder->make_logic0(builder);
        }
    }
    builder->make_message_space(builder);
    builder->make_head(builder);
    for (int i = 31; i >= 0; i--) {
        if (addr & (1 << i)) {
            builder->make_logic1(builder);
        } else {
            builder->make_logic0(builder);
        }
    }
    for (int i = 31; i >= 0; i--) {
        if (cmd & (1 << i)) {
            builder->make_logic1(builder);
        } else {
            builder->make_logic0(builder);
        }
    }
    for (int i = 7; i >= 0; i--) {
        if (cksum & (1 << i)) {
            builder->make_logic1(builder);
        } else {
            builder->make_logic0(builder);
        }
    }
    builder->make_end(builder);
    return ESP_OK;
//err:
    //return ret;
}

ir_builder_t *ir_builder_rmt_new_toshibaAC(const ir_builder_config_t *config)
{
    ir_builder_t *ret = NULL;
    IR_CHECK(config, "toshibaAC configuration can't be null", err, NULL);
    IR_CHECK(config->buffer_size, "buffer size can't be zero", err, NULL);

    uint32_t builder_size = sizeof(ir_protocol_builder_t) + config->buffer_size * sizeof(rmt_item32_t);
    ir_protocol_builder_t *ir_protocol_builder = calloc(1, builder_size);
    IR_CHECK(ir_protocol_builder, "request memory for ir_protocol_builder failed", err, NULL);

    ir_protocol_builder->buffer_size = config->buffer_size;
    ir_protocol_builder->flags = config->flags;
    if (config->flags & IR_TOOLS_FLAGS_INVERSE) {
        ir_protocol_builder->inverse = true;
    }

    uint32_t counter_clk_hz = 0;
    IR_CHECK(rmt_get_counter_clock((rmt_channel_t)config->dev_hdl, &counter_clk_hz) == ESP_OK,
              "get rmt counter clock failed", err, NULL);
    float ratio = (float)counter_clk_hz / 1e6;

    #if CONFIG_EXAMPLE_IR_PROTOCOL_TOSHIBAAC72
    ir_protocol_builder->leading_code_high_ticks = (uint32_t)(ratio * TOSHIBAAC_LEADING_CODE_HIGH_US);
    ir_protocol_builder->leading_code_low_ticks = (uint32_t)(ratio * TOSHIBAAC_LEADING_CODE_LOW_US);
    ir_protocol_builder->message_gap_high_ticks = (uint32_t)(ratio * TOSHIBAAC_MESSAGE_SPACE_HIGH_US);
    ir_protocol_builder->message_gap_low_ticks = (uint32_t)(ratio * TOSHIBAAC_MESSAGE_SPACE_LOW_US);
    ir_protocol_builder->payload_logic0_high_ticks = (uint32_t)(ratio * TOSHIBAAC_PAYLOAD_ZERO_HIGH_US);
    ir_protocol_builder->payload_logic0_low_ticks = (uint32_t)(ratio * TOSHIBAAC_PAYLOAD_ZERO_LOW_US);
    ir_protocol_builder->payload_logic1_high_ticks = (uint32_t)(ratio * TOSHIBAAC_PAYLOAD_ONE_HIGH_US);
    ir_protocol_builder->payload_logic1_low_ticks = (uint32_t)(ratio * TOSHIBAAC_PAYLOAD_ONE_LOW_US);
    ir_protocol_builder->ending_code_high_ticks = (uint32_t)(ratio * TOSHIBAAC_ENDING_CODE_HIGH_US);
    
    #else
    ir_protocol_builder->leading_code_high_ticks = (uint32_t)(ratio * TOSHIBAAC50_LEADING_CODE_HIGH_US);
    ir_protocol_builder->leading_code_low_ticks = (uint32_t)(ratio * TOSHIBAAC50_LEADING_CODE_LOW_US);
    ir_protocol_builder->message_gap_high_ticks = (uint32_t)(ratio * TOSHIBAAC50_MESSAGE_SPACE_HIGH_US);
    ir_protocol_builder->message_gap_low_ticks = (uint32_t)(ratio * TOSHIBAAC50_MESSAGE_SPACE_LOW_US);
    //ir_protocol_builder->new_message_low_ticks = (uint32_t)(ratio * TOSHIBAAC50_NEW_MESSAGE_SPACE_LOW_US);
    ir_protocol_builder->payload_logic0_high_ticks = (uint32_t)(ratio * TOSHIBAAC50_PAYLOAD_ZERO_HIGH_US);
    ir_protocol_builder->payload_logic0_low_ticks = (uint32_t)(ratio * TOSHIBAAC50_PAYLOAD_ZERO_LOW_US);
    ir_protocol_builder->payload_logic1_high_ticks = (uint32_t)(ratio * TOSHIBAAC50_PAYLOAD_ONE_HIGH_US);
    ir_protocol_builder->payload_logic1_low_ticks = (uint32_t)(ratio * TOSHIBAAC50_PAYLOAD_ONE_LOW_US);
    ir_protocol_builder->ending_code_high_ticks = (uint32_t)(ratio * TOSHIBAAC50_ENDING_CODE_HIGH_US);
    #endif

    ir_protocol_builder->ending_code_low_ticks = 0x7FFF;
    ir_protocol_builder->parent.make_head = ir_protocol_builder_make_head;
    ir_protocol_builder->parent.make_logic0 = ir_protocol_builder_make_logic0;
    ir_protocol_builder->parent.make_logic1 = ir_protocol_builder_make_logic1;
    ir_protocol_builder->parent.make_end = ir_protocol_builder_make_end;
#if CONFIG_EXAMPLE_IR_PROTOCOL_TOSHIBAAC72
    ir_protocol_builder->parent.frame_builder_t.build_frame_toshibaAC = toshibaAC_build_frame_toshibaAC;
#else
    ir_protocol_builder->parent.frame_builder_t.build_frame = toshibaAC_build_frame;
#endif
    ir_protocol_builder->parent.make_message_space = ir_protocol_builder_make_message_space;
    ir_protocol_builder->parent.get_result = ir_protocol_builder_get_result;
    ir_protocol_builder->parent.del = ir_protocol_builder_del;
    return &ir_protocol_builder->parent;
err:
    return ret;
}

/** ===============================================================================
 * @brief TOSHIBAAC PARSER FUNCTIONS
 * ================================================================================
 */
static esp_err_t toshibaAC_parser_input(ir_parser_t *parser, void *raw_data, uint32_t length)
{
    esp_err_t ret = ESP_OK;
    ir_protocol_parser_t *ir_protocol_parser = __containerof(parser, ir_protocol_parser_t, parent);
    IR_CHECK(raw_data, "input data can't be null", err, ESP_ERR_INVALID_ARG);
    ir_protocol_parser->buffer = raw_data;
    // Data Frame costs 148 items and 50 costs 100 items
    if (length == TOSHIBAAC_DATA_FRAME_RMT_WORDS) {
        ir_protocol_parser->repeat = false;
    } else if (length == TOSHIBAAC_REPEAT_FRAME_RMT_WORDS) {
        ir_protocol_parser->repeat = true;
    }
    else {
        ret = ESP_FAIL;
    }
    return ret;
err:
    return ret;
}

static esp_err_t toshibaAC_parser_get_scan_code(ir_parser_t *parser, uint32_t *address, uint32_t *command, bool *repeat)
{
    esp_err_t ret = ESP_FAIL;
    uint32_t addr = 0;
    uint32_t cmd = 0;
    uint32_t addrR = 0;
    uint32_t cmdR = 0;
    bool logic_value = false;
    ir_protocol_parser_t *ir_protocol_parser = __containerof(parser, ir_protocol_parser_t, parent);
    IR_CHECK(address && command && repeat, "address, command and repeat can't be null", out, ESP_ERR_INVALID_ARG);
    if (ir_protocol_parse_head(ir_protocol_parser)) 
    {
        for (int i = 23; i >= 0; i--) {
            if (ir_protocol_parse_logic(parser, &logic_value) == ESP_OK) {
                addr |= (logic_value << i);
            }
        }
        for (int i = 23; i >= 0; i--) {
            if (ir_protocol_parse_logic(parser, &logic_value) == ESP_OK) {
                cmd |= (logic_value << i);
            }
        }
        //ESP_LOGI(TAG, "Scan Code --- addr: 0x%04x cmd: 0x%04x", addr, cmd);
        if(ir_protocol_parse_message(ir_protocol_parser)){
            if (ir_protocol_parse_head(ir_protocol_parser)) {
                for (int i = 23; i >= 0; i--) {
                    if (ir_protocol_parse_logic(parser, &logic_value) == ESP_OK) {
                        addrR |= (logic_value << i);
                    }
                }
                for (int i = 23; i >= 0; i--) {
                    if (ir_protocol_parse_logic(parser, &logic_value) == ESP_OK) {
                        cmdR |= (logic_value << i);
                    }
                }
            }
        }
        if ((addr == addrR) && (cmd == cmdR)) {
            *address = addr;
            *command = cmd;
            *repeat = false;
            // keep it as potential repeat code
            //toshibaAC_parser->last_address = addr;
            //toshibaAC_parser->last_command = cmd;
            ret = ESP_OK;
        }
       
    }
    //return ret;
out:
    return ret;
}

static esp_err_t toshibaAC_parser_get_scan_code_toshibaAC(ir_parser_t *parser, uint32_t *address, uint32_t *command, uint32_t *checksum, bool *repeat)
{
    esp_err_t ret = ESP_FAIL;
    uint32_t addr = 0;
    uint32_t cmd = 0;
    uint32_t cksum = 0;
    uint32_t addrR = 0;
    uint32_t cmdR = 0;
    uint32_t cksumR = 0;
    bool logic_value = false;
    ir_protocol_parser_t *ir_protocol_parser = __containerof(parser, ir_protocol_parser_t, parent);
    //TOSHIBAAC_CHECK(address && command && repeat, "address, command and repeat can't be null", out, ESP_ERR_INVALID_ARG);
    //ESP_LOGI("INFO", "Scan Code function called");
    if (ir_protocol_parse_head(ir_protocol_parser)) {
        for (int i = 31; i >= 0; i--) {
            if (ir_protocol_parse_logic(parser, &logic_value) == ESP_OK) {
                addr |= (logic_value << i);
            }
        }
        for (int i = 31; i >= 0; i--) {
            if (ir_protocol_parse_logic(parser, &logic_value) == ESP_OK) {
                cmd |= (logic_value << i);
            }
        }
        for (int i = 7; i >= 0; i--) {
            if (ir_protocol_parse_logic(parser, &logic_value) == ESP_OK) {
                cksum |= (logic_value << i);
            }
        }
        if(ir_protocol_parse_message(ir_protocol_parser)){
            if (ir_protocol_parse_head(ir_protocol_parser)) {
                for (int i = 31; i >= 0; i--) {
                    if (ir_protocol_parse_logic(parser, &logic_value) == ESP_OK) {
                        addrR |= (logic_value << i);
                    }
                }
                for (int i = 31; i >= 0; i--) {
                    if (ir_protocol_parse_logic(parser, &logic_value) == ESP_OK) {
                        cmdR |= (logic_value << i);
                    }
                }
                for (int i = 7; i >= 0; i--) {
                    if (ir_protocol_parse_logic(parser, &logic_value) == ESP_OK) {
                        cksumR |= (logic_value << i);
                    }
                }
            }
        }
        if((addr == addrR) && (cmd == cmdR) && (cksum == cksumR)){
            *address = addr;
            *command = cmd;
            *checksum = cksum;
            *repeat = false;
            // keep it as potential repeat code
            ir_protocol_parser->last_address = addr;
            ir_protocol_parser->last_command = cmd;
            ret = ESP_OK;
        }
    }
//out:
    return ret;
}

ir_parser_t *ir_parser_rmt_new_toshibaAC(const ir_parser_config_t *config)
{
    ir_parser_t *ret = NULL;
    IR_CHECK(config, "toshibaAC configuration can't be null", err, NULL);

    ir_protocol_parser_t *ir_protocol_parser = calloc(1, sizeof(ir_protocol_parser_t));
    IR_CHECK(ir_protocol_parser, "request memory for toshibaAC_parser failed", err, NULL);

    ir_protocol_parser->flags = config->flags;
    if (config->flags & IR_TOOLS_FLAGS_INVERSE) {
        ir_protocol_parser->inverse = true;
    }

    uint32_t counter_clk_hz = 0;
    IR_CHECK(rmt_get_counter_clock((rmt_channel_t)config->dev_hdl, &counter_clk_hz) == ESP_OK,
              "get rmt counter clock failed", err, NULL);
    float ratio = (float)counter_clk_hz / 1e6;

    #if CONFIG_EXAMPLE_IR_PROTOCOL_TOSHIBAAC72
    ir_protocol_parser->leading_code_high_ticks = (uint32_t)(ratio * TOSHIBAAC_LEADING_CODE_HIGH_US);
    ir_protocol_parser->leading_code_low_ticks = (uint32_t)(ratio * TOSHIBAAC_LEADING_CODE_LOW_US);
    ir_protocol_parser->message_gap_high_ticks = (uint32_t)(ratio * TOSHIBAAC_MESSAGE_SPACE_HIGH_US);
    ir_protocol_parser->message_gap_low_ticks = (uint32_t)(ratio * TOSHIBAAC_MESSAGE_SPACE_LOW_US);
    ir_protocol_parser->payload_logic0_high_ticks = (uint32_t)(ratio * TOSHIBAAC_PAYLOAD_ZERO_HIGH_US);
    ir_protocol_parser->payload_logic0_low_ticks = (uint32_t)(ratio * TOSHIBAAC_PAYLOAD_ZERO_LOW_US);
    ir_protocol_parser->payload_logic1_high_ticks = (uint32_t)(ratio * TOSHIBAAC_PAYLOAD_ONE_HIGH_US);
    ir_protocol_parser->payload_logic1_low_ticks = (uint32_t)(ratio * TOSHIBAAC_PAYLOAD_ONE_LOW_US);
    
    #else
    ir_protocol_parser->leading_code_high_ticks = (uint32_t)(ratio * TOSHIBAAC50_LEADING_CODE_HIGH_US);
    ir_protocol_parser->leading_code_low_ticks = (uint32_t)(ratio * TOSHIBAAC50_LEADING_CODE_LOW_US);
    ir_protocol_parser->message_gap_high_ticks = (uint32_t)(ratio * TOSHIBAAC50_MESSAGE_SPACE_HIGH_US);
    ir_protocol_parser->message_gap_low_ticks = (uint32_t)(ratio * TOSHIBAAC50_MESSAGE_SPACE_LOW_US);
    ir_protocol_parser->new_message_low_ticks = (uint32_t)(ratio * TOSHIBAAC50_NEW_MESSAGE_SPACE_LOW_US);
    ir_protocol_parser->payload_logic0_high_ticks = (uint32_t)(ratio * TOSHIBAAC50_PAYLOAD_ZERO_HIGH_US);
    ir_protocol_parser->payload_logic0_low_ticks = (uint32_t)(ratio * TOSHIBAAC50_PAYLOAD_ZERO_LOW_US);
    ir_protocol_parser->payload_logic1_high_ticks = (uint32_t)(ratio * TOSHIBAAC50_PAYLOAD_ONE_HIGH_US);
    ir_protocol_parser->payload_logic1_low_ticks = (uint32_t)(ratio * TOSHIBAAC50_PAYLOAD_ONE_LOW_US);
    #endif

    ir_protocol_parser->margin_ticks = (uint32_t)(ratio * config->margin_us);
    ir_protocol_parser->parent.input = toshibaAC_parser_input;
#if CONFIG_EXAMPLE_IR_PROTOCOL_TOSHIBAAC72
    ir_protocol_parser->parent.get_scan_code_toshibaAC = toshibaAC_parser_get_scan_code_toshibaAC; 
else 
    ir_protocol_parser->parent.get_scan_code = toshibaAC_parser_get_scan_code;
#endif
    ir_protocol_parser->parent.del = ir_protocol_parser_del;
    return &ir_protocol_parser->parent;
err:
    return ret;
}
