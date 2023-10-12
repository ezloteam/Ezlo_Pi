#include "trace.h"
#include "ezlopi_cloud.h"
#include "ezlopi_device_value_updated.h"
#include "ezlopi_item_name_str.h"
#include "ezlopi_cloud_device_types_str.h"
#include "ezlopi_cloud_value_type_str.h"
#include "ezlopi_cloud_category_str.h"
#include "ezlopi_cloud_subcategory_str.h"
#include "ezlopi_valueformatter.h"

#include "sensor_0044_I2C_TSL2561_luminosity.h"

// defines
static uint32_t Lux_intensity = 0;

//----------------------------------------------------------------------------------------------------------------------------
//                  Declarations
//----------------------------------------------------------------------------------------------------------------------------
static void Power_Up_tsl2561(s_ezlopi_device_properties_t *properties);
static void Power_Down_tsl2561(s_ezlopi_device_properties_t *properties);

static s_ezlopi_device_properties_t *sensor_0044_TSL2561_prepare(cJSON *cjson_device);
static int sensor_0044_i2c_tsl2561_prep_and_add(void *arg);
static bool Check_PARTID(s_ezlopi_device_properties_t *properties);
static bool sensor_0044_i2c_tsl2561_init(s_ezlopi_device_properties_t *properties);
static void sensor_0044_tsl2561_configure_device(s_ezlopi_device_properties_t *properties);
static int sensor_0044_i2c_tsl2561_get_value_cjson(s_ezlopi_device_properties_t *properties, void *arg);
static int sensor_0044_i2c_tsl2561_update_value(s_ezlopi_device_properties_t *properties);

static uint32_t TSL2561_CalculateLux(uint16_t ch0, uint16_t ch1, integration_t conv_time, gain_t gain);
static uint8_t readRegister8(s_ezlopi_device_properties_t *properties, uint8_t address, uint8_t *reg);

//----------------------------------------------------------------------------------------------------------------------------
int sensor_0044_I2C_TSL2561_luminosity(e_ezlopi_actions_t action, s_ezlopi_device_properties_t *ezlopi_device, void *arg, void *usr_args)
{
    int ret = 0;
    static bool init_guard = false;
    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        ret = sensor_0044_i2c_tsl2561_prep_and_add(arg);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        init_guard = sensor_0044_i2c_tsl2561_init(ezlopi_device); // send true if init finished
        ret = (int)init_guard;
        break;
    }
    case EZLOPI_ACTION_HUB_GET_ITEM:
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        ret = sensor_0044_i2c_tsl2561_get_value_cjson(ezlopi_device, arg);
        break;
    }
    case EZLOPI_ACTION_NOTIFY_1000_MS:
    {
        static uint8_t count = 0;
        if (init_guard) // allow to send new data only if init is finished
        {
            if (count++ > 1)
            {
                ret = ezlopi_device_value_updated_from_device(ezlopi_device);
                count = 0;
            }
            else
            {
                sensor_0044_i2c_tsl2561_update_value(ezlopi_device);
            }
        }
        else
        {
            TRACE_E("Initializtion Not finished.....Please check your connection and Reboot the device");
        }

        break;
    }
    default:
        break;
    }
    return ret;
}
//----------------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------------
//                 Function Definations
//----------------------------------------------------------------------------------------------------------------------------

static int sensor_0044_i2c_tsl2561_prep_and_add(void *arg)
{
    int ret = 0;
    s_ezlopi_prep_arg_t *device_prep_arg = (s_ezlopi_prep_arg_t *)arg;

    if ((device_prep_arg) && (device_prep_arg->cjson_device))
    {
        // generating the device properties
        s_ezlopi_device_properties_t *sensor_0044_i2c_tsl2561_properties = sensor_0044_TSL2561_prepare(device_prep_arg->cjson_device);
        if (sensor_0044_i2c_tsl2561_properties)
        {
            if (0 == ezlopi_devices_list_add(device_prep_arg->device,
                                             sensor_0044_i2c_tsl2561_properties,
                                             NULL))
            {
                free(sensor_0044_i2c_tsl2561_properties); // free the pointer, if addition is not successful
            }
            else
            {
                ret = 1;
            }
        }
    }

    return ret;
}

static s_ezlopi_device_properties_t *sensor_0044_TSL2561_prepare(cJSON *cjson_device)
{
    s_ezlopi_device_properties_t *sensor_tsl2561_properties = malloc(sizeof(s_ezlopi_device_properties_t));
    if (sensor_tsl2561_properties)
    {
        memset(sensor_tsl2561_properties, 0, sizeof(s_ezlopi_device_properties_t));
        sensor_tsl2561_properties->interface_type = EZLOPI_DEVICE_INTERFACE_I2C_MASTER;

        char *device_name = NULL;
        CJSON_GET_VALUE_STRING(cjson_device, "dev_name", device_name);
        ASSIGN_DEVICE_NAME(sensor_tsl2561_properties, device_name);

        sensor_tsl2561_properties->ezlopi_cloud.category = category_light_sensor;
        sensor_tsl2561_properties->ezlopi_cloud.subcategory = subcategory_not_defined;
        sensor_tsl2561_properties->ezlopi_cloud.item_name = ezlopi_item_name_lux;
        sensor_tsl2561_properties->ezlopi_cloud.device_type = dev_type_sensor;
        sensor_tsl2561_properties->ezlopi_cloud.value_type = value_type_illuminance;
        sensor_tsl2561_properties->ezlopi_cloud.battery_powered = false;
        sensor_tsl2561_properties->ezlopi_cloud.has_getter = true;
        sensor_tsl2561_properties->ezlopi_cloud.has_setter = false;
        sensor_tsl2561_properties->ezlopi_cloud.reachable = true;
        sensor_tsl2561_properties->ezlopi_cloud.room_name[0] = '\0';
        sensor_tsl2561_properties->ezlopi_cloud.room_id = ezlopi_cloud_generate_room_id();
        sensor_tsl2561_properties->ezlopi_cloud.item_id = ezlopi_cloud_generate_item_id();
        sensor_tsl2561_properties->ezlopi_cloud.device_id = ezlopi_cloud_generate_device_id();

        CJSON_GET_VALUE_INT(cjson_device, "gpio_sda", sensor_tsl2561_properties->interface.i2c_master.sda);
        CJSON_GET_VALUE_INT(cjson_device, "gpio_scl", sensor_tsl2561_properties->interface.i2c_master.scl);
        CJSON_GET_VALUE_INT(cjson_device, "slave_addr", sensor_tsl2561_properties->interface.i2c_master.address);

        sensor_tsl2561_properties->interface.i2c_master.enable = true;
        sensor_tsl2561_properties->interface.i2c_master.clock_speed = 100000;
        if (NULL == (sensor_tsl2561_properties->interface.i2c_master.address))
        {
            sensor_tsl2561_properties->interface.i2c_master.address = TSL2561_ADDRESS;
        }
    }
    return sensor_tsl2561_properties;
}

static bool sensor_0044_i2c_tsl2561_init(s_ezlopi_device_properties_t *properties)
{
    static bool guard = false;
    static bool i2c_init_flag = false;
    if (!guard)
    {
        if (!i2c_init_flag)
        {
            ezlopi_i2c_master_init(&properties->interface.i2c_master);
            i2c_init_flag = true;
        }
        TRACE_I("I2C initialized to channel %d", properties->interface.i2c_master.channel);

        // check if the sensor is stable..
        if (Check_PARTID(properties))
        {
            TRACE_E("TSL561 initialization finished.........");
            sensor_0044_tsl2561_configure_device(properties);
            guard = true;
        }
        else
        {
            TRACE_E("TSL561 not found!....... Please Restart!! or Check your I2C connection...");
            guard = false;
        }
    }
    return guard;
}

//// Step 1: Initial Communication and Verification
static bool Check_PARTID(s_ezlopi_device_properties_t *properties)
{

    Power_Up_tsl2561(properties);

    // Read -> PART_ID value
    uint8_t read_buffer = 0;
    uint8_t command_code = (SELECT_CMD_REGISTER | TSL2561_REGISTER_ID);
    // uint8_t write_buffer1[] = {command_code};
    // ezlopi_i2c_master_write_to_device(&properties->interface.i2c_master, write_buffer1, 1);
    // ezlopi_i2c_master_read_from_device(&properties->interface.i2c_master, &read_buffer, 1);
    if (readRegister8(properties, command_code, &read_buffer))
    {
        TRACE_E(" PART_ID : {%x}", (read_buffer & (0xF0))); // required [0b0101xxxx]
        TRACE_E(" REV_NO  : {%x}", (read_buffer & (0x0F)));
    }
    else
    {
        TRACE_E(" Unable to read the  PART_ID register [0x0A].....");
    }

    Power_Down_tsl2561(properties);

    return (((TSL2561_PART_NUMBER) == (read_buffer & 0xF0)) ? true : false);
}

static void sensor_0044_tsl2561_configure_device(s_ezlopi_device_properties_t *properties)
{
    Power_Up_tsl2561(properties);
    // this controls both intergration time and gain of ADC
    // Set the timing and gain

    uint8_t command_code = (SELECT_CMD_REGISTER | TSL2561_REGISTER_TIMING);
    uint8_t setup_code = (TSL2561_HIGH_GAIN_MODE_x16 | TSL2561_STOP_MANNUAL_INTEGRATION | TSL2561_INTEGRATION_TIME_101_MS);
    uint8_t write_buffer_timing[] = {command_code, setup_code};
    ezlopi_i2c_master_write_to_device(&properties->interface.i2c_master, write_buffer_timing, 2);

    // Power_Down_tsl2561(properties);
}

static int sensor_0044_i2c_tsl2561_update_value(s_ezlopi_device_properties_t *properties)
{
    bool clear_to_read = false;
    // first check if sensor is powered on    // Read -> PART_ID value
    uint8_t read_buffer = 0;
    uint8_t command_code = (SELECT_CMD_REGISTER | TSL2561_REGISTER_ID);
    if (readRegister8(properties, command_code, &read_buffer))
    {
        // TRACE_E(" Updating Data.... found PART_ID : {%x}", (read_buffer & (0xF0))); // required [0b0101xxxx]
        // Powered ON ? [Part_No : 0b0101xxxx]
        if ((TSL2561_PART_NUMBER) == (read_buffer & 0xF0))
        {
            read_buffer = 0;
            command_code = (SELECT_CMD_REGISTER | TSL2561_REGISTER_TIMING);
            // if powered ON, check the Gain & Integration time from 0x01H
            if (readRegister8(properties, command_code, &read_buffer))
            {
                // TRACE_E(" Timing Register Byte : ...............{%x}", (read_buffer));
                if (read_buffer & (TSL2561_HIGH_GAIN_MODE_x16 | TSL2561_INTEGRATION_TIME_101_MS))
                {
                    // if the gain = x16 and integration_time = 101ms , set the "clear_to_read" flag
                    clear_to_read = true;
                }
            }
        }
        else
        {
            TRACE_E(" Error : PART_ID register [0x0Ah] data mis-match.....");
        }
    }
    else
    {
        TRACE_E(" Unable to read the  PART_ID register ....");
    }

    // If "clear_to_read" flag is set extract data from sensor and update the data structure
    if (clear_to_read)
    {
        // Wait x ms for ADC to complete
        switch (TSL2561_INTEGRATIONTIME_101MS)
        {
        case TSL2561_INTEGRATIONTIME_13MS:
            vTaskDelay(14);
            break;
        case TSL2561_INTEGRATIONTIME_101MS:
            vTaskDelay(102);
            break;
        default:
            vTaskDelay(403);
            break;
        }

        uint16_t IR = 0;
        uint16_t Visible_Ir = 0;
        uint8_t target_address = 0;
        // extract the CH1-bits first
        target_address = (SELECT_CMD_REGISTER | DO_WORD_TRANSACTION | TSL2561_REGISTER_CHAN1_LOW);
        ezlopi_i2c_master_write_to_device(&properties->interface.i2c_master, &target_address, 1);
        ezlopi_i2c_master_read_from_device(&properties->interface.i2c_master, &IR, 2); // extracts CH1-data

        // extract the CH0-bits first
        target_address = (SELECT_CMD_REGISTER | DO_WORD_TRANSACTION | TSL2561_REGISTER_CHAN0_LOW);
        ezlopi_i2c_master_write_to_device(&properties->interface.i2c_master, &target_address, 1);
        ezlopi_i2c_master_read_from_device(&properties->interface.i2c_master, &Visible_Ir, 2); // extracts CH2-data

        // Calculate the lux value
        Lux_intensity = TSL2561_CalculateLux(Visible_Ir,                    // CH0
                                             IR,                            // CH1
                                             TSL2561_INTEGRATIONTIME_101MS, // conv_time
                                             TSL2561_GAIN_x1);              // adc_gain
        // TRACE_B("IR : %d", IR);
        // TRACE_B("Visible : %d", Visible_Ir - IR);
        // TRACE_B("Lux : %d", Lux_intensity);
        // TRACE_E(" Data update completed......");
    }
    else
    {
        TRACE_E(" Data update Failed ......");
    }
    return 1;
}

static int sensor_0044_i2c_tsl2561_get_value_cjson(s_ezlopi_device_properties_t *properties, void *arg)
{
    int ret = 0;
    cJSON *cjson_properties = (cJSON *)arg;
    if (NULL != cjson_properties)
    {
        if (ezlopi_item_name_lux == properties->ezlopi_cloud.item_name)
        {
            cJSON_AddNumberToObject(cjson_properties, "values", Lux_intensity);
            char *valueFormatted = ezlopi_valueformatter_uint32(Lux_intensity);
            cJSON_AddStringToObject(cjson_properties, "valueFormatted", valueFormatted);
            free(valueFormatted);
            cJSON_AddStringToObject(cjson_properties, "scale", "lux");
        }
        ret = 1;
    }
    return ret;
}

//----------------------------------------------------------------------------------------------------------------------------

static uint32_t TSL2561_CalculateLux(uint16_t ch0, uint16_t ch1, integration_t conv_time, gain_t gain)
{
    unsigned long chScale;
    unsigned long channel1;
    unsigned long channel0;

    // first, scale the channel values depending on the gain and integration time
    // 16X, 402mS is nominal.
    // scale if integration time is NOT 402 msec
    switch (conv_time)
    {
    case TSL2561_INTEGRATIONTIME_13MS:
        chScale = TSL2561_LUX_CHSCALE_13_7_MS;
        break;
    case TSL2561_INTEGRATIONTIME_101MS:
        chScale = TSL2561_LUX_CHSCALE_101_MS;
        break;
    default: // No scaling ... integration time = 402ms
        chScale = TSL2561_LUX_CHSCALE_402_MS;
        break;
    }

    // Scale for gain (1x or 16x)
    if (gain == TSL2561_GAIN_x1)
    { // scale if gain != x16
        chScale = chScale << 4;
    }

    // scale the channel values
    channel0 = (ch0 * chScale) >> TSL2561_LUX_CHSCALE;
    channel1 = (ch1 * chScale) >> TSL2561_LUX_CHSCALE;

    // find the ratio of the channel values (Channel1/Channel0)
    // -> protect against dividing by zero
    unsigned long ratio1 = 0;
    if (channel0 != 0)
    {
        ratio1 = (channel1 << (TSL2561_LUX_RATIOSCALE + 1)) / channel0;
    }

    // round the ratio value
    unsigned long ratio = (ratio1 + 1) >> 1;
    // is ratio <= eachBreak ?
    unsigned int b, m;
    if ((ratio >= 0) && (ratio <= TSL2561_LUX_K1T))
    {
        b = TSL2561_LUX_B1T;
        m = TSL2561_LUX_M1T;
    }
    else if (ratio <= TSL2561_LUX_K2T)
    {
        b = TSL2561_LUX_B2T;
        m = TSL2561_LUX_M2T;
    }
    else if (ratio <= TSL2561_LUX_K3T)
    {
        b = TSL2561_LUX_B3T;
        m = TSL2561_LUX_M3T;
    }
    else if (ratio <= TSL2561_LUX_K4T)
    {
        b = TSL2561_LUX_B4T;
        m = TSL2561_LUX_M4T;
    }
    else if (ratio <= TSL2561_LUX_K5T)
    {
        b = TSL2561_LUX_B5T;
        m = TSL2561_LUX_M5T;
    }
    else if (ratio <= TSL2561_LUX_K6T)
    {
        b = TSL2561_LUX_B6T;
        m = TSL2561_LUX_M6T;
    }
    else if (ratio <= TSL2561_LUX_K7T)
    {
        b = TSL2561_LUX_B7T;
        m = TSL2561_LUX_M7T;
    }
    else if (ratio > TSL2561_LUX_K8T)
    {
        b = TSL2561_LUX_B8T;
        m = TSL2561_LUX_M8T;
    }

    // now calculation of final lux_value
    unsigned long temp;
    temp = ((channel0 * b) - (channel1 * m));

    // do not allow negative lux value
    if (temp < 0)
    {
        temp = 0;
    }

    // round lsb (2^(LUX_SCALE-1))
    temp += (1 << (TSL2561_LUX_LUXSCALE - 1));

    // strip off fractional portion
    unsigned long lux_val = temp >> TSL2561_LUX_LUXSCALE;

    // Signal I2C had no errors
    return ((uint32_t)lux_val);
}

static void Power_Up_tsl2561(s_ezlopi_device_properties_t *properties)
{
    // first power-up the device
    uint8_t command_code = (SELECT_CMD_REGISTER | TSL2561_REGISTER_CONTROL);
    uint8_t write_buffer_power_up[] = {command_code, TSL2561_POWER_ON};
    ezlopi_i2c_master_write_to_device(&properties->interface.i2c_master, write_buffer_power_up, 2);
}

static void Power_Down_tsl2561(s_ezlopi_device_properties_t *properties)
{
    // Now, power-down the device
    uint8_t command_code = (SELECT_CMD_REGISTER | TSL2561_REGISTER_CONTROL);
    uint8_t write_buffer_power_down[] = {command_code, TSL2561_POWER_OFF};
    ezlopi_i2c_master_write_to_device(&properties->interface.i2c_master, write_buffer_power_down, 2);
}

//------------------------------------------------------------

static uint8_t readRegister8(s_ezlopi_device_properties_t *properties, uint8_t target_address, uint8_t *reg)
{
    ezlopi_i2c_master_write_to_device(&properties->interface.i2c_master, &target_address, 1);
    ezlopi_i2c_master_read_from_device(&properties->interface.i2c_master, reg, 1); // extracts data into @reg
    return 1;
}

//------------------------------------------------------------