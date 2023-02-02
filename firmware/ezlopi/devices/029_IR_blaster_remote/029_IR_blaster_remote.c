

#include "029_IR_blaster_remote.h"
#include "cJSON.h"
#include "ezlopi_devices_list.h"
#include "string.h"
#include "stdlib.h"
#include "ezlopi_cloud_category_str.h"
#include "ezlopi_cloud_subcategory_str.h"
#include "ezlopi_item_name_str.h"
#include "ezlopi_cloud_device_types_str.h"
#include "ezlopi_cloud_value_type_str.h"
#include "sdkconfig.h"
#include "driver/rmt.h"
#include "ir_tools.h"
#include "ir_timings.h"
#include "ir_init.h"
#include "ezlopi_device_value_updated.h"
#include "string.h"


static ir_parser_config_t* ir_parser_config = NULL;
static ir_builder_config_t* ir_builder_config = NULL;


static int IR_Blaster_Remote_prepare_and_add(void* args);
static s_ezlopi_device_properties_t* IR_Blaster_Remote_prepare(cJSON *cjson_device);
static int IR_BLaster_Remote_init(s_ezlopi_device_properties_t* properties);
static int IR_BLaster_Remote_set_value(s_ezlopi_device_properties_t *properties, void *arg);
static int IR_BLaster_Remote_get_value_cjson(s_ezlopi_device_properties_t *properties, void *args);





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
        case EZLOPI_ACTION_NOTIFY_1000_MS:
        {
            ezlopi_device_value_updated_from_device(properties);
            break;
        }
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


// ir_parser_config_t rmt_rx_init(uint32_t ezlopi_ir_blaster_rx_gpio_num, rmt_channel_t ezlopi_ir_blaster_rx_channel) {
//     rmt_config_t rmt_rx_config = RMT_DEFAULT_CONFIG_RX(ezlopi_ir_blaster_rx_gpio_num, ezlopi_ir_blaster_rx_channel);
//     rmt_config(&rmt_rx_config);
//     rmt_driver_install(ezlopi_ir_blaster_rx_channel, 3000, 0);
//     ir_parser_config_t ir_parser_config = IR_PARSER_DEFAULT_CONFIG((ir_dev_t)ezlopi_ir_blaster_rx_channel);
//     ir_parser_config.flags |= IR_TOOLS_FLAGS_PROTO_EXT;// Using extended IR protocols (both NEC and RC5 have extended version)
//     return ir_parser_config;
// }

// ir_builder_config_t rmt_tx_init(uint32_t ezlopi_ir_blaster_tx_gpio_num, rmt_channel_t ezlopi_ir_blaster_tx_channel) {
//     rmt_config_t rmt_tx_config = RMT_DEFAULT_CONFIG_TX(ezlopi_ir_blaster_tx_gpio_num, ezlopi_ir_blaster_tx_channel);
//     rmt_tx_config.tx_config.carrier_en = true;
//     rmt_config(&rmt_tx_config);
//     rmt_driver_install(ezlopi_ir_blaster_tx_channel, 0, 0);
//     ir_builder_config_t ir_builder_config = IR_BUILDER_DEFAULT_CONFIG((ir_dev_t)ezlopi_ir_blaster_tx_channel);
//     ir_builder_config.flags |= IR_TOOLS_FLAGS_PROTO_EXT; // Using extended IR protocols (both NEC and RC5 have extended version)
//     return ir_builder_config;
// }

static int IR_Blaster_Remote_prepare_and_add(void* args)
{
    int ret = 0;
    s_ezlopi_prep_arg_t *device_prep_arg = (s_ezlopi_prep_arg_t *)args;

    if ((NULL != device_prep_arg) && (NULL != device_prep_arg->cjson_device))
    {
        s_ezlopi_device_properties_t *IR_Blaster_Remote_properties = IR_Blaster_Remote_prepare(device_prep_arg->cjson_device);
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
        IR_Blaster_Remote_properties->interface_type = EZLOPI_DEVICE_INTERFACE_ANALOG_OUTPUT;
        
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

    char* ir_remote_command = NULL;
    char* ir_remote_address = NULL;

    ir_remote_info_t *ir_remote_info_handler = (ir_remote_info_t*)malloc(sizeof(ir_remote_info_t));
    memset(ir_remote_info_handler, 0, sizeof(ir_remote_info_t));

    cJSON* device_details = (cJSON*)arg;

    if(device_details && ir_remote_info_handler)
    {
        CJSON_GET_VALUE_INT(device_details, "device", ir_remote_info_handler->ir_remote_device_type);
        CJSON_GET_VALUE_INT(device_details, "brand", ir_remote_info_handler->ir_remote_brand_type);
        CJSON_GET_VALUE_INT(device_details, "model", ir_remote_info_handler->ir_remote_model_type);
        CJSON_GET_VALUE_STRING(device_details, "address", ir_remote_address);
        CJSON_GET_VALUE_STRING(device_details, "command", ir_remote_command);

        // Call function to initialize the protocol.
        ir_protocol_init_t* ir_protocol_init_props = ir_protocol_init(ir_remote_info_handler, ir_parser_config, ir_builder_config);

        // Call the function for RX and TX.

    }

    return ret;

}


static int IR_BLaster_Remote_init(s_ezlopi_device_properties_t* properties)
{
    int ret = 0;
    ir_parser_config = rmt_rx_init();
    ir_builder_config = rmt_tx_init();
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

