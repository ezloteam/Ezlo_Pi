#include "029_IR_blaster_remote.h"
 #include "ezlopi_devices_list.h"
#include "ezlopi_cloud_device_types_str.h"
#include "ezlopi_cloud_value_type_str.h"
#include "ir_init.h"
#include "IR_Blaster_data_operation.h"
#include "IR_Blaster_encoder_decoder.h"



ir_parser_config_t ir_parser_config;
ir_builder_config_t ir_builder_config;
static ir_protocol_init_t ir_protocol_init_props;

// static int IR_Blaster_Remote_prepare_and_add(void* args);
// static s_ezlopi_device_properties_t* IR_Blaster_Remote_prepare(cJSON *cjson_device);
// static int IR_BLaster_Remote_init(s_ezlopi_device_properties_t* properties);
// static int IR_BLaster_Remote_set_value(s_ezlopi_device_properties_t *properties, void *arg);
// static int IR_BLaster_Remote_get_value_cjson(s_ezlopi_device_properties_t *properties, void *args);





int IR_blaster_remote(e_ezlopi_actions_t action, s_ezlopi_device_properties_t *properties, void *arg, void *user_arg)
{
    int ret = 0;

     switch (action){
        case EZLOPI_ACTION_PREPARE:
        {
            IR_Blaster_Remote_prepare_and_add(arg);
            break;
        }
        case EZLOPI_ACTION_INITIALIZE:
        {
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


static int IR_Blaster_Remote_prepare_and_add(void* args)
{
    int ret = 0;
    s_ezlopi_prep_arg_t *device_prep_arg = (s_ezlopi_prep_arg_t *)args;
    cJSON *cjson_device = device_prep_arg->cjson_device;

    s_ezlopi_device_properties_t *IR_Blaster_Remote_properties = NULL;

    //if ((NULL != device_prep_arg) && (NULL != device_prep_arg->cjson_device))
    if((NULL == IR_Blaster_Remote_properties) && (NULL != cjson_device))
    {
        IR_Blaster_Remote_properties = IR_Blaster_Remote_prepare(cjson_device);
        if (IR_Blaster_Remote_properties)
        {
            if (0 == ezlopi_devices_list_add(device_prep_arg->device, IR_Blaster_Remote_properties, NULL))
            {
                free(IR_Blaster_Remote_properties);
            }
            else
            {
                ret = 1;
            }
        }
    }

    return ret;
}

static s_ezlopi_device_properties_t* IR_Blaster_Remote_prepare(cJSON *cjson_device)
{
    s_ezlopi_device_properties_t* IR_Blaster_Remote_properties = malloc(sizeof(s_ezlopi_device_properties_t));
    if(IR_Blaster_Remote_properties)
    {
        memset(IR_Blaster_Remote_properties, 0, sizeof(s_ezlopi_device_properties_t));
        IR_Blaster_Remote_properties->interface_type = EZLOPI_DEVICE_INTERFACE_PWM;
        
        char *device_name = NULL;
        CJSON_GET_VALUE_STRING(cjson_device, "dev_name", device_name);

        ASSIGN_DEVICE_NAME(IR_Blaster_Remote_properties, device_name);
    
        IR_Blaster_Remote_properties->ezlopi_cloud.category = category_ir_tx;
        IR_Blaster_Remote_properties->ezlopi_cloud.subcategory = subcategory_irt;
        IR_Blaster_Remote_properties->ezlopi_cloud.item_name = ezlopi_item_name_ir_blaster;
        IR_Blaster_Remote_properties->ezlopi_cloud.device_type = dev_type_device;
        IR_Blaster_Remote_properties->ezlopi_cloud.value_type = value_type_ir_blaster;
        IR_Blaster_Remote_properties->ezlopi_cloud.has_getter = true;
        IR_Blaster_Remote_properties->ezlopi_cloud.has_setter = true;
        IR_Blaster_Remote_properties->ezlopi_cloud.reachable = true;
        IR_Blaster_Remote_properties->ezlopi_cloud.battery_powered = false;
        IR_Blaster_Remote_properties->ezlopi_cloud.show = true;
        IR_Blaster_Remote_properties->ezlopi_cloud.room_name[0] = '\0';
        IR_Blaster_Remote_properties->ezlopi_cloud.device_id = ezlopi_device_generate_device_id();
        IR_Blaster_Remote_properties->ezlopi_cloud.room_id = ezlopi_device_generate_room_id();
        IR_Blaster_Remote_properties->ezlopi_cloud.item_id = ezlopi_device_generate_item_id();
    }
    return IR_Blaster_Remote_properties;    
}


static int IR_BLaster_Remote_set_value(s_ezlopi_device_properties_t *properties, void *arg)
{

    int ret = 0;

    char* ir_remote_command = 0;
    char* ir_remote_address = 0;
    uint32_t addr;
    uint32_t cmd;

    ir_remote_info_t *ir_remote_info_handler = (ir_remote_info_t*)malloc(sizeof(ir_remote_info_t));
    memset(ir_remote_info_handler, 0, sizeof(ir_remote_info_t));

    cJSON* device_details = (cJSON*)arg;

    if(device_details && ir_remote_info_handler)
    {
        char* printable_device_details = cJSON_Print(device_details);
        TRACE_E("%s", printable_device_details);        
    }
    return ret;
}


static int IR_BLaster_Remote_init(s_ezlopi_device_properties_t* properties)
{
    int ret = 0;
    TRACE_E("CONFIG INIT");
    ir_builder_config = rmt_tx_init();
    ir_parser_config = rmt_rx_init();
    return ret;
}


static int IR_BLaster_Remote_get_value_cjson(s_ezlopi_device_properties_t *properties, void *args)
{
    int ret = 0;

    cJSON* params = (cJSON*)args;

    cJSON* value = cJSON_CreateObject();

    cJSON_AddNumberToObject(value, "device", 1);
    cJSON_AddNumberToObject(value, "brand", 2);
    cJSON_AddNumberToObject(value, "model", 3);
    cJSON_AddStringToObject(value, "address", "0x0F0F");
    cJSON_AddStringToObject(value, "commmand", "0xA0A0");

    char* val = cJSON_Print(value);

    cJSON_AddRawToObject(params, "value", val);

    free(value);

    return ret;
}

void ezlopi_ir_tx(uint32_t address, uint32_t command)
{
    rmt_item32_t *items = NULL;
    size_t length = 0;
   
 
    ESP_ERROR_CHECK(ir_protocol_init_props.ir_builder->frame_builder_t.build_frame(ir_protocol_init_props.ir_builder, address, command));

    ESP_ERROR_CHECK(ir_protocol_init_props.ir_builder->get_result(ir_protocol_init_props.ir_builder, &items, &length));

    rmt_write_items(RMT_TX_CHANNEL, items, length, false);
   
}

/**
 * @brief RMT Receive Task
 *
 */
static void ezlopi_ir_rx_task(void *arg)
{
    uint32_t addr = 0;
    uint32_t cmd = 0;
    bool repeat = false;
    size_t length = 0;
    RingbufHandle_t rb = NULL;
    rmt_item32_t *items = NULL;

    //get RMT RX ringbuffer
    rmt_get_ringbuf_handle(RMT_RX_CHANNEL, &rb);
    assert(rb != NULL);
    // Start receive
    //ESP_LOGI("INFO", "START");
    rmt_rx_start(RMT_RX_CHANNEL, true);
    while (1) {
        items = (rmt_item32_t *) xRingbufferReceive(rb, &length, portMAX_DELAY);
        //TRACE_E(" Received raw length = %d", length);
        if (items) 
        {
            length /= 4; // one RMT = 4 Bytes
            //TRACE_I(" Received RMT BYTE length = %d", length);
            if (ir_protocol_init_props.ir_parser->input(ir_protocol_init_props.ir_parser, items, length) == ESP_OK) 
            {
                 //TRACE_I(" Input ok");
                if (ir_protocol_init_props.ir_parser->scan_code_t.get_scan_code(ir_protocol_init_props.ir_parser, &addr, &cmd, &repeat) == ESP_OK) 
                {
                    TRACE_I("Scan Code %s --- address: 0x%08x command: 0x%08x", repeat ? "(repeat)" : "", addr, cmd);
                }
            }
            //after parsing the data, return spaces to ringbuffer.
            vRingbufferReturnItem(rb, (void *) items);
        }
    }
    //delete ring buffer
    vRingbufferDelete(rb);
    ir_protocol_init_props.ir_parser->del(ir_protocol_init_props.ir_parser);
    rmt_driver_uninstall(RMT_RX_CHANNEL);
    vTaskDelete(NULL);
}