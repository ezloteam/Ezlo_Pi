#include "029_IR_blaster_remote.h"
#include "ezlopi_devices_list.h"
#include "ezlopi_cloud.h"
#include "ezlopi_cloud_device_types_str.h"
#include "ezlopi_cloud_value_type_str.h"
#include "IR_Blaster_data_operation.h"
#include "IR_Blaster_encoder_decoder.h"
#include "esp_log.h"

#define TIMEOUT_MS 30000 // 30 seconds in milliseconds

uint32_t timing[500]; // always make static for the global variable [benefits: scope of variable remains within the file]
static ir_protocol_parser_t ir_protocol_parser;
static ir_protocol_builder_t *ir_protocol_builder;

char *item_id; // always make static for the global variable [benefits: scope of variable remains within the file]
size_t length; // always make static for the global variable [benefits: scope of variable remains within the file]

#define ADD_PROPERTIES_DEVICE_LIST(device_id, category, subcategory, item_name, value_type, cjson_device)           \
    {                                                                                                               \
        s_ezlopi_device_properties_t *_properties = IR_Blaster_Remote_prepare(device_id, category, subcategory,     \
                                                                              item_name, value_type, cjson_device); \
        if (NULL != _properties)                                                                                    \
        {                                                                                                           \
            add_device_to_list(prep_arg, _properties, NULL);                                                        \
        }                                                                                                           \
    }

int IR_blaster_remote(e_ezlopi_actions_t action, s_ezlopi_device_properties_t *properties, void *arg, void *user_arg)
{
    int ret = 0;

    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        TRACE_B("PREPARE");      // remove not-required trace after test
        IR_Blaster_prepare(arg); // put full file name for function prefix (benefits: increases redability, help identify which file contains it)
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        TRACE_B("INIT"); // remove not-required trace after test
        IR_BLaster_Remote_init(properties);
        break;
    }
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        IR_BLaster_Remote_get_value_cjson(properties, arg);
        break;
    }
    // case EZLOPI_ACTION_NOTIFY_1000_MS:
    // {
    //     ezlopi_device_value_updated_from_device(properties);
    //     break;
    // }
    case EZLOPI_ACTION_SET_VALUE:
    {
        IR_BLaster_Remote_set_value(properties, arg);
        break;
    }
    default:
    {
        break;
    }
    }

    return ret;
}

static int IR_Blaster_prepare(void *arg)
{
    int ret = 0;
    s_ezlopi_prep_arg_t *prep_arg = (s_ezlopi_prep_arg_t *)arg;

    if ((NULL != prep_arg) && (NULL != prep_arg->cjson_device))
    {
        uint32_t device_id = ezlopi_cloud_generate_device_id();
        ADD_PROPERTIES_DEVICE_LIST(device_id, category_ir_tx, subcategory_irt, ezlopi_item_name_send_ir_code, value_type_string, prep_arg->cjson_device);
        // device_id = ezlopi_device_generate_device_id();
        ADD_PROPERTIES_DEVICE_LIST(device_id, category_ir_tx, subcategory_irt, ezlopi_item_name_learn_ir_code, value_type_int, prep_arg->cjson_device);
    }
    return ret;
}

static int add_device_to_list(s_ezlopi_prep_arg_t *prep_arg, s_ezlopi_device_properties_t *properties, void *user_arg)
{
    int ret = 0;
    if (properties)
    {
        if (0 == ezlopi_devices_list_add(prep_arg->device, properties, user_arg))
        {
            free(properties);
        }
        else
        {
            ret = 1;
        }
    }
    return ret;
}

static s_ezlopi_device_properties_t *IR_Blaster_Remote_prepare(uint32_t dev_id, const char *category, const char *sub_category, const char *item_name, const char *value_type, cJSON *cjson_device)
{
    s_ezlopi_device_properties_t *IR_Blaster_Remote_properties = malloc(sizeof(s_ezlopi_device_properties_t));
    if (IR_Blaster_Remote_properties)
    {
        memset(IR_Blaster_Remote_properties, 0, sizeof(s_ezlopi_device_properties_t));
        IR_Blaster_Remote_properties->interface_type = EZLOPI_DEVICE_INTERFACE_PWM;

        char *device_name = NULL;
        CJSON_GET_VALUE_STRING(cjson_device, "dev_name", device_name);

        ASSIGN_DEVICE_NAME(IR_Blaster_Remote_properties, device_name);

        IR_Blaster_Remote_properties->ezlopi_cloud.category = category;
        IR_Blaster_Remote_properties->ezlopi_cloud.subcategory = sub_category;
        IR_Blaster_Remote_properties->ezlopi_cloud.item_name = item_name;
        IR_Blaster_Remote_properties->ezlopi_cloud.device_type = dev_type_transmitter_ir;
        IR_Blaster_Remote_properties->ezlopi_cloud.value_type = value_type;
        IR_Blaster_Remote_properties->ezlopi_cloud.has_getter = true;
        IR_Blaster_Remote_properties->ezlopi_cloud.has_setter = true;
        IR_Blaster_Remote_properties->ezlopi_cloud.reachable = true;
        IR_Blaster_Remote_properties->ezlopi_cloud.battery_powered = false;
        IR_Blaster_Remote_properties->ezlopi_cloud.show = true;
        IR_Blaster_Remote_properties->ezlopi_cloud.room_name[0] = '\0';
        IR_Blaster_Remote_properties->ezlopi_cloud.device_id = dev_id;
        IR_Blaster_Remote_properties->ezlopi_cloud.room_id = ezlopi_cloud_generate_room_id();
        IR_Blaster_Remote_properties->ezlopi_cloud.item_id = ezlopi_cloud_generate_item_id();
    }
    return IR_Blaster_Remote_properties;
}

static int IR_BLaster_Remote_init(s_ezlopi_device_properties_t *properties)
{
    int ret = 0;
    // TRACE_E("CONFIG INIT");
    build_decoding_table();
    return ret;
}

static int IR_BLaster_Remote_set_value(s_ezlopi_device_properties_t *properties, void *arg)
{
    int ret = 0;
    cJSON *device_details = (cJSON *)arg;

    if (device_details)
    {
        // char* printed = cJSON_Print(device_details);
        // TRACE_E("%s", printed);
        if (ezlopi_item_name_send_ir_code == properties->ezlopi_cloud.item_name)
        {
            char *Base64_string_value_from_Cloud = NULL;
            CJSON_GET_VALUE_STRING(device_details, "value", Base64_string_value_from_Cloud); // Gets the string Base64
            // TRACE_E("%s", Base64_string_value_from_Cloud);

            char *Hex_string = base64_to_string(Base64_string_value_from_Cloud);

            if (ir_remote_blaster_learned_code(Hex_string) == ESP_OK)
            {
                // capture();
                TRACE_B("DATA BLASTED");
            }
            else
            {
                TRACE_E("NOTHING BLASTING");
            }
        }
        else if (ezlopi_item_name_learn_ir_code == properties->ezlopi_cloud.item_name)
        {
            int val = 5;
            CJSON_GET_VALUE_INT(device_details, "value", val);
            if (val)
            {
                ir_remote_learner();
            }
        }
        else
        {
            TRACE_E("NO ITEM SELECTED");
        }
    }
    return ret;
}

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

static int IR_BLaster_Remote_get_value_cjson(s_ezlopi_device_properties_t *properties, void *args)
{
    int ret = 0;
    cJSON *params = (cJSON *)args; // confirm for  NULL

    if (ezlopi_item_name_send_ir_code == properties->ezlopi_cloud.item_name)
    {
        blaster_mode_get_value_cjson(params);
    }
    else
    {
        learner_mode_get_value_cjson(params);
    }

    return ret;
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
