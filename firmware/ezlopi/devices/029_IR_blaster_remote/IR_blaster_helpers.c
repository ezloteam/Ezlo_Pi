

#include "IR_blaster_helpers.h"
#include "IR_Blaster_data_operation.h"
#include "029_IR_blaster_remote.h"

#define TIMEOUT_MS 30000 // 30 seconds in milliseconds

uint32_t timing[500]; // always make static for the global variable [benefits: scope of variable remains within the file]
static ir_protocol_parser_t ir_protocol_parser;
static ir_protocol_builder_t *ir_protocol_builder;

char *item_id; // always make static for the global variable [benefits: scope of variable remains within the file]
size_t length; // always make static for the global variable [benefits: scope of variable remains within the file]

esp_err_t ir_remote_blaster_learned_code(char *Hex_string_data) // confusing naming  convention
{
    esp_err_t ret = ESP_FAIL;
    rmt_item32_t *items = NULL;
    ir_builder_config_t ir_builder_config = rmt_tx_init();

    uint32_t timing_array_len = 0;
    timing_array_length(Hex_string_data, &timing_array_len);

    if (0 < timing_array_len)
    {
        uint32_t decoded_timing_data[timing_array_len];
        hex_string_2_timing_array(Hex_string_data, decoded_timing_data);

        if (ESP_OK == build(&items, &ir_builder_config, decoded_timing_data, timing_array_len))
        {
            ret = ESP_OK;
        }
    }

    if (ir_protocol_builder)
    {
        free(ir_protocol_builder);
        ir_protocol_builder = NULL;
    }

    rmt_driver_uninstall(RMT_TX_CHANNEL); // has parameter while un-installing, but not in install

    return ret;
}

// LEARNER MODE
esp_err_t ir_remote_learner()
{
    esp_err_t ret = ESP_OK;
    int r = 0;
    // TRACE_I("LEARNING MODE ON: PLEASE PRESS A REMOTE BUTTON ONCE ONLY \n");

    TRACE_I("LEARNING MODE ON: PLEASE PRESS A REMOTE BUTTON ONCE ONLY \n");
    r = capture(); // name-convention
    if (1 == r)
    {
        TRACE_I("LEARNED A REMOTE CODE");
    }
    else
    {
        TRACE_I("NOT LEARNED");
    }
    return ret;
}

esp_err_t build(void *result, const ir_builder_config_t *config, uint32_t *buffer1, uint32_t timing_array_len)
{
    esp_err_t return_val = ESP_FAIL;
    ir_protocol_builder_t *ret = NULL;
    IR_CHECK(config, "IR_PROTOCOL configuration can't be null", err, NULL);
    IR_CHECK(config->buffer_size, "buffer size can't be zero", err, NULL);

    uint32_t builder_size = sizeof(ir_protocol_builder_t) + config->buffer_size * sizeof(rmt_item32_t);
    ir_protocol_builder = calloc(1, builder_size);
    IR_CHECK(ir_protocol_builder, "request memory for ir_builder failed", err, NULL);

    ir_protocol_builder->buffer_size = config->buffer_size;
    ir_protocol_builder->flags = config->flags;
    if (config->flags & IR_TOOLS_FLAGS_INVERSE)
    {
        ir_protocol_builder->inverse = true;
    }
    uint32_t counter_clk_hz = 0;
    IR_CHECK(rmt_get_counter_clock((rmt_channel_t)config->dev_hdl, &counter_clk_hz) == ESP_OK,
             "get rmt counter clock failed", err, NULL);
    float ratio = (float)counter_clk_hz / 1e6;
    IR_CHECK(buffer1, "data to be built can't be null", err, NULL);
    // TRACE_B("length OF DATA TO BE BUILT = %d", timing_array_len);
    length = timing_array_len;
    memset(timing, 0, sizeof(timing));

    if (timing_array_len)
    {
        int i = 0;
        for (int j = 0; j < timing_array_len / 2; j++)
        {
            if (j < ir_protocol_builder->buffer_size)
            {
                // ESP_LOGE("ERROR","BUILDING FRAME \r\n");
                ir_protocol_builder->cursor = j;
                ir_protocol_builder->buffer[ir_protocol_builder->cursor].level0 = !ir_protocol_builder->inverse;
                ir_protocol_builder->buffer[ir_protocol_builder->cursor].duration0 = (uint32_t)(ratio * buffer1[i]);
                timing[i] = ir_protocol_builder->buffer[ir_protocol_builder->cursor].duration0;
                // TRACE_B("timing%d = %d", i, ir_protocol_builder->buffer[ir_protocol_builder->cursor].duration0);

                ir_protocol_builder->buffer[ir_protocol_builder->cursor].level1 = ir_protocol_builder->inverse;
                ir_protocol_builder->buffer[ir_protocol_builder->cursor].duration1 = (uint32_t)(ratio * buffer1[i + 1]);
                timing[i + 1] = ir_protocol_builder->buffer[ir_protocol_builder->cursor].duration1;
                // TRACE_B("timing%d = %d", i+1, ir_protocol_builder->buffer[ir_protocol_builder->cursor].duration1);
                i += 2;
            }
            else
            {
                TRACE_E("Buffer overflow \n");
            }
        }
        ir_protocol_builder->cursor += 1;
        ir_protocol_builder->buffer[ir_protocol_builder->cursor].val = 0;
        ir_protocol_builder->cursor += 1;
        // TRACE_E("BUILD SUCCESSFUL");
        if (ESP_OK == rmt_write_items(RMT_TX_CHANNEL, ir_protocol_builder->buffer, ir_protocol_builder->cursor, true))
        {
            // TRACE_E("LENGTH WRITTEN IN CHANNEL = %d", ir_protocol_builder->cursor);
            TRACE_E("DATA Written on channel Successful");
            return_val = ESP_OK;
        }
        // *(rmt_item32_t **)result = ir_protocol_builder->buffer;
        // return ESP_OK;
    }
    else
    {
        TRACE_E("No Data feed \n");
    }
err: // minimize using goto
    return return_val;
}

int capture(void)
{
    int ret = 0;
    RingbufHandle_t rb = NULL;
    rmt_item32_t *items = NULL;
    // size_t length = 0;
    ir_parser_config_t ir_parser_config = rmt_rx_init();
    rmt_get_ringbuf_handle(RMT_RX_CHANNEL, &rb);
    assert(rb != NULL);
    // Start receive
    rmt_rx_start(RMT_RX_CHANNEL, true);
    int count = 0;
    size_t len = 0;
    // wait for an IR code for up to 30 seconds
    TickType_t elapsed_ticks;

    TickType_t start_ticks = xTaskGetTickCount();
    TickType_t timeout_ticks = pdMS_TO_TICKS(TIMEOUT_MS);

    do
    {
        elapsed_ticks = xTaskGetTickCount() - start_ticks;
        items = (rmt_item32_t *)xRingbufferReceive(rb, &len, timeout_ticks - elapsed_ticks);
        len /= 4; // one RMT = 4 Bytes
        // ESP_LOGI("INFO", "count = %d  and RECEIVED LENGTH = %d", count, len);
    } while (len < 5 && elapsed_ticks < timeout_ticks);

    // while((count <= 15) && (len < 5));
    if (items)
    {
        vRingbufferReturnItem(rb, (void *)items);
        vTaskDelay(pdMS_TO_TICKS(200));
    }

    if (len > 5)
    {

        // length = len;
        store(items, len); // comment
        // ir_protocol_parser.buffer = items;
        ret = 1;
    }

    rmt_rx_stop(RMT_RX_CHANNEL);
    rmt_driver_uninstall(RMT_RX_CHANNEL);
    return ret;
}

void store(rmt_item32_t *items, uint32_t len)
{
    // ir_protocol_parser_t ir_protocol_parser;
    ir_protocol_parser.buffer = items;
    // TRACE_B("NOW STORING DATA");
    int j = 0;
    length = len * 2;
    memset(timing, 0, sizeof(timing));
    for (int i = 0; i < len; i++)

    {
        ir_protocol_parser.cursor = i;
        timing[j] = ir_protocol_parser.buffer[ir_protocol_parser.cursor].duration0;
        // ESP_LOGI("TIME INFO","bit %d High timing: %d", i, ir_protocol_parser.buffer[ir_protocol_parser.cursor].duration0);

        timing[j + 1] = ir_protocol_parser.buffer[ir_protocol_parser.cursor].duration1;
        // ESP_LOGI("TIME INFO","bit %d Low timing: %d", i, ir_protocol_parser.buffer[ir_protocol_parser.cursor].duration1);

        j += 2;
    }
}

esp_err_t blaster_mode_get_value_cjson(cJSON *params) // make it static
{
    esp_err_t ret = ESP_OK;
    // TRACE_B("BLASTER");
    // TRACE_B("length = %d", length);
    char *base64_data = create_base64_learned_data_packet(timing, length);
    // ESP_LOGI(TAG, "decoded hex_string_data: %s\n", base64_data ? base64_data : "NULL");
    cJSON_AddStringToObject(params, "value", base64_data);
    // TRACE_B("%s", cJSON_Print(params));
    length = 0;
    return ret;
}

esp_err_t learner_mode_get_value_cjson(cJSON *params)
{
    esp_err_t ret = ESP_OK;
    // TRACE_B("LEARNER");
    // TRACE_B("length = %d", length);
    char *base64_data = "";
    if (length > 5)
    {
        base64_data = create_base64_learned_data_packet(timing, length);
    }

    // ESP_LOGI(TAG, "decoded hex_string_data: %s\n", base64_data ? base64_data : "NULL");
    cJSON_AddStringToObject(params, "value", base64_data);
    // TRACE_B("%s", cJSON_Print(params));
    length = 0;
    return ret;
}
