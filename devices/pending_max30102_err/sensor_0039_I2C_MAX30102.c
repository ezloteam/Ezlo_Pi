#include "trace.h"
#include "cJSON.h"
#include "esp_err.h"
#include "MAX30102_SPO2_algorithm.h"
#include "sensor_0039_I2C_MAX30102.h"
#include "ezlopi_timer.h"
#include "ezlopi_i2c_master.h"

#include "ezlopi_cloud_category_str.h"
#include "ezlopi_item_name_str.h"
#include "ezlopi_cloud_subcategory_str.h"
#include "ezlopi_cloud_device_types_str.h"
#include "ezlopi_cloud_value_type_str.h"
#include "ezlopi_device_value_updated.h"

/*************************************************************************************************/
/*                              DEFINES                                                     */
/*************************************************************************************************/
#define FIFO_COUNT 32
#define ActiveLEDs 2

static float Die_temp_val;
static int32_t SPO2;          // SPO2
static int8_t SPO2Valid;      // Flag to display if SPO2 calculation is valid
static int32_t heartRate;     // Heart-rate
static int8_t heartRateValid; // Flag to display if heart-rate calculation is valid
static sSenseBuf_t senseBuf = {0};

//------------------------------------------------------------------------------
#define ADD_PROPERTIES_DEVICE_LIST(device_id, category, subcategory, item_name, value_type, cjson_device)                        \
    {                                                                                                                            \
        s_ezlopi_device_properties_t *_properties = sensor_i2c_max30102_prepare_properties(device_id, category, subcategory,     \
                                                                                           item_name, value_type, cjson_device); \
        if (NULL != _properties)                                                                                                 \
        {                                                                                                                        \
            add_device_to_list(prep_arg, _properties, NULL);                                                                     \
        }                                                                                                                        \
    }

//------------------------------------------------------------------------------

static int add_device_to_list(s_ezlopi_prep_arg_t *prep_arg, s_ezlopi_device_properties_t *sensor_bme_device_properties, void *user_arg);
static int sensor_0039_I2C_max30102_prepare(void *arg);
static s_ezlopi_device_properties_t *sensor_i2c_max30102_prepare_properties(uint32_t DEV_ID, const char *CATEGORY, const char *SUB_CATEGORY, const char *ITEM_NAME, const char *VALUE_TYPE, cJSON *cjson_device);
static bool sensor_0039_I2C_max30102_init(s_ezlopi_device_properties_t *properties, void *user_arg);
static int sensor_i2c_max30102_configure_device(s_ezlopi_device_properties_t *properties, void *args);
static int sensor_0039_I2C_max30102_get_value_cjson(s_ezlopi_device_properties_t *properties, void *args);

static uint8_t MAX30102_getWritePointer(s_ezlopi_device_properties_t *properties);
static uint8_t MAX30102_getReadPointer(s_ezlopi_device_properties_t *properties);

static esp_err_t Check_fifo_full_flag(s_ezlopi_device_properties_t *properties, uint8_t *temp);
static void MAX30102_get_die_temperature(s_ezlopi_device_properties_t *properties, float *temp_val);

static void MAX30102_getNewData(s_ezlopi_device_properties_t *properties);
static void MAX30102_heartrateAndOxygenSaturation(s_ezlopi_device_properties_t *properties, int32_t *SPO2, int8_t *SPO2Valid, int32_t *heartRate, int8_t *heartRateValid);

static uint8_t readRegister8(s_ezlopi_device_properties_t *properties, uint8_t address, uint8_t *reg);
//------------------------------------------------------------------------------
static bool Check_PARTID(s_ezlopi_device_properties_t *properties);
static esp_err_t SoftReset(s_ezlopi_device_properties_t *properties);
static esp_err_t Activate_interrupts(s_ezlopi_device_properties_t *properties);
static esp_err_t FIFO_config(s_ezlopi_device_properties_t *properties);
static esp_err_t MODE_config(s_ezlopi_device_properties_t *properties);
static esp_err_t SPO2_config(s_ezlopi_device_properties_t *properties);
static esp_err_t LED_PA_config(s_ezlopi_device_properties_t *properties);
static esp_err_t MUL_MODE_CTRL_config(s_ezlopi_device_properties_t *properties);
static esp_err_t FIFO_RESET(s_ezlopi_device_properties_t *properties);

//------------------------------------------------------------------------------

/*************************************************************************************************/
/*                              DECLARATIONS                                                     */
/*************************************************************************************************/

//------------------------------------------------------------------------------
int sensor_0039_I2C_MAX30102(e_ezlopi_actions_t action, s_ezlopi_device_properties_t *properties, void *arg, void *user_arg)
{
    static bool init_guard = 0;
    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        sensor_0039_I2C_max30102_prepare(arg);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        init_guard = sensor_0039_I2C_max30102_init(properties, user_arg);
        break;
    }
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        sensor_0039_I2C_max30102_get_value_cjson(properties, arg);
        break;
    }
    case EZLOPI_ACTION_NOTIFY_1000_MS:
    {
        static uint8_t count = 0;
        // update all data
        MAX30102_heartrateAndOxygenSaturation(properties, &SPO2, &SPO2Valid, &heartRate, &heartRateValid);
        MAX30102_get_die_temperature(properties, &Die_temp_val);
        if (count++ > 5)
        {
            if (init_guard)
            {
                ezlopi_device_value_updated_from_device(properties);
            }
            count = 0;
        }
        break;
    }
    default:
    {
        break;
    }
    }
    return 0;
}
//------------------------------------------------------------------------------

static int sensor_0039_I2C_max30102_prepare(void *arg)
{
    int ret = 0;
    s_ezlopi_prep_arg_t *prep_arg = (s_ezlopi_prep_arg_t *)arg;

    if ((NULL != prep_arg) && (NULL != prep_arg->cjson_device))
    {
        uint32_t device_id = ezlopi_cloud_generate_device_id();
        // need Heart Beat measurement unit (%)
        ADD_PROPERTIES_DEVICE_LIST(device_id, category_generic_sensor, subcategory_not_defined, ezlopi_item_name_heart_rate, value_type_frequency, prep_arg->cjson_device);
        // need Oxygen measurement unit (%)
        device_id = ezlopi_cloud_generate_device_id();
        ADD_PROPERTIES_DEVICE_LIST(device_id, category_generic_sensor, subcategory_not_defined, ezlopi_item_name_acceleration_x_axis, value_type_int, prep_arg->cjson_device);

        device_id = ezlopi_cloud_generate_device_id();
        ADD_PROPERTIES_DEVICE_LIST(device_id, category_temperature, subcategory_not_defined, ezlopi_item_name_temp, value_type_temperature, prep_arg->cjson_device);
    }
    return ret;
}

static int add_device_to_list(s_ezlopi_prep_arg_t *prep_arg, s_ezlopi_device_properties_t *sensor_max30102_device_properties, void *user_arg)
{
    int ret = 0;
    if (sensor_max30102_device_properties)
    {
        if (0 == ezlopi_devices_list_add(prep_arg->device, sensor_max30102_device_properties, user_arg))
        {
            free(sensor_max30102_device_properties);
        }
        else
        {
            ret = 1;
        }
    }
    return ret;
}

static s_ezlopi_device_properties_t *sensor_i2c_max30102_prepare_properties(uint32_t DEV_ID, const char *CATEGORY, const char *SUB_CATEGORY, const char *ITEM_NAME, const char *VALUE_TYPE, cJSON *cjson_device)
{
    s_ezlopi_device_properties_t *sensor_i2c_max30102_properties = NULL;

    if ((NULL != cjson_device))
    {
        sensor_i2c_max30102_properties = malloc(sizeof(s_ezlopi_device_properties_t));
        if (sensor_i2c_max30102_properties)
        {
            memset(sensor_i2c_max30102_properties, 0, sizeof(s_ezlopi_device_properties_t));
            sensor_i2c_max30102_properties->interface_type = EZLOPI_DEVICE_INTERFACE_I2C_MASTER;

            char *device_name = NULL;
            // manual_device name {according to item_type}
            if (ezlopi_item_name_heart_rate == ITEM_NAME)
            {
                device_name = "max30102_HR";
            }
            if (ezlopi_item_name_acceleration_x_axis == ITEM_NAME) // change the item_name -> SPO2
            {
                device_name = "max30102_SPO2";
            }
            if (ezlopi_item_name_temp == ITEM_NAME)
            {
                device_name = "max30102_HR";
            }
            ASSIGN_DEVICE_NAME(sensor_i2c_max30102_properties, device_name);
            sensor_i2c_max30102_properties->ezlopi_cloud.category = CATEGORY;
            sensor_i2c_max30102_properties->ezlopi_cloud.subcategory = SUB_CATEGORY;
            sensor_i2c_max30102_properties->ezlopi_cloud.item_name = ITEM_NAME;
            sensor_i2c_max30102_properties->ezlopi_cloud.device_type = dev_type_sensor;
            sensor_i2c_max30102_properties->ezlopi_cloud.value_type = VALUE_TYPE;
            sensor_i2c_max30102_properties->ezlopi_cloud.has_getter = true;
            sensor_i2c_max30102_properties->ezlopi_cloud.has_setter = false;
            sensor_i2c_max30102_properties->ezlopi_cloud.reachable = true;
            sensor_i2c_max30102_properties->ezlopi_cloud.battery_powered = false;
            sensor_i2c_max30102_properties->ezlopi_cloud.show = true;
            sensor_i2c_max30102_properties->ezlopi_cloud.room_name[0] = '\0';
            sensor_i2c_max30102_properties->ezlopi_cloud.device_id = DEV_ID;
            sensor_i2c_max30102_properties->ezlopi_cloud.room_id = ezlopi_cloud_generate_room_id();
            sensor_i2c_max30102_properties->ezlopi_cloud.item_id = ezlopi_cloud_generate_item_id();

            CJSON_GET_VALUE_INT(cjson_device, "gpio_scl", sensor_i2c_max30102_properties->interface.i2c_master.scl);
            CJSON_GET_VALUE_INT(cjson_device, "gpio_sda", sensor_i2c_max30102_properties->interface.i2c_master.sda);

            sensor_i2c_max30102_properties->interface.i2c_master.enable = true;
            sensor_i2c_max30102_properties->interface.i2c_master.clock_speed = 100000;
            sensor_i2c_max30102_properties->interface.i2c_master.address = MAX30102_SLAVEID;
        }
    }
    return sensor_i2c_max30102_properties;
}

static bool sensor_0039_I2C_max30102_init(s_ezlopi_device_properties_t *properties, void *user_arg)
{
    static bool i2c_init_flag = false;
    static bool guard = false;
    if (!guard)
    {
        if (!i2c_init_flag)
        {
            ezlopi_i2c_master_init(&properties->interface.i2c_master);
            i2c_init_flag = true;
        }
        if (Check_PARTID(properties))
        {
            TRACE_I("I2C initialized to channel %d", properties->interface.i2c_master.channel);
            sensor_i2c_max30102_configure_device(properties, user_arg);
            guard = true;
        }
        else
        {
            /*NOTE : NEED TO REMOVE THIS 'i2c_deinit' code*/
            TRACE_E("MAX30102 not found!....... Please Restart!! or Check your I2C connection...");
        }
    }
    return guard;
}

static int sensor_i2c_max30102_configure_device(s_ezlopi_device_properties_t *properties, void *args)
{
    int ret = 0;

    // first check "PWR_RDY" to proceed
    ESP_ERROR_CHECK_WITHOUT_ABORT(SoftReset(properties));
    ESP_ERROR_CHECK_WITHOUT_ABORT(Activate_interrupts(properties));
    ESP_ERROR_CHECK_WITHOUT_ABORT(FIFO_config(properties));
    ESP_ERROR_CHECK_WITHOUT_ABORT(MODE_config(properties)); // Choose Multi-mode to use both IR and Red-led
    ESP_ERROR_CHECK_WITHOUT_ABORT(SPO2_config(properties));
    ESP_ERROR_CHECK_WITHOUT_ABORT(LED_PA_config(properties));
    ESP_ERROR_CHECK_WITHOUT_ABORT(MUL_MODE_CTRL_config(properties));
    ESP_ERROR_CHECK_WITHOUT_ABORT(FIFO_RESET(properties));

    return ret;
}

static int sensor_0039_I2C_max30102_get_value_cjson(s_ezlopi_device_properties_t *properties, void *args)
{
    int ret = 0;
    cJSON *cjson_properties = (cJSON *)args;
    static int data_value = 0;
    if (cjson_properties)
    {
        if (ezlopi_item_name_heart_rate == properties->ezlopi_cloud.item_name)
        {
            if (heartRateValid)
            {
                data_value = (int)(heartRate);
            }

            cJSON_AddNumberToObject(cjson_properties, "value", data_value);
            cJSON_AddStringToObject(cjson_properties, "scale", "bpm");
        }

        if (ezlopi_item_name_acceleration_x_axis == properties->ezlopi_cloud.item_name)
        {
            if (SPO2Valid)
            {
                data_value = (int)(SPO2);
            }
            TRACE_I("SpO2 : {%d percent}", data_value);
            cJSON_AddNumberToObject(cjson_properties, "value", data_value);
            cJSON_AddStringToObject(cjson_properties, "scale", "percent");
        }
        if (ezlopi_item_name_temp == properties->ezlopi_cloud.item_name)
        {
            data_value = (int)(Die_temp_val);
            TRACE_I("Temperature : {%d *C}", data_value);
            cJSON_AddNumberToObject(cjson_properties, "value", data_value);
            cJSON_AddStringToObject(cjson_properties, "scale", "Celcius");
        }

        ret = 1;
    }
    return ret;
}

//------------------------------------------------------------------------------
//// Step 1: Initial Communication and Verification
static bool Check_PARTID(s_ezlopi_device_properties_t *properties)
{
    uint8_t read_buffer = 0;
    uint8_t write_buffer1[] = {MAX30102_INTSTAT1};
    vTaskDelay(10); // 100ms delay
    ezlopi_i2c_master_write_to_device(&properties->interface.i2c_master, write_buffer1, 1);
    // Read -> PART_ID value
    ezlopi_i2c_master_read_from_device(&properties->interface.i2c_master, &read_buffer, 1);
    TRACE_E(" Power_ready value : {%x}", read_buffer);
    vTaskDelay(1); // 10ms delay
    uint8_t write_buffer2[] = {MAX30102_PARTID};
    ezlopi_i2c_master_write_to_device(&properties->interface.i2c_master, write_buffer2, 1);
    // Read -> PART_ID value
    ezlopi_i2c_master_read_from_device(&properties->interface.i2c_master, &read_buffer, 1);
    TRACE_E(" CHIP_ID value : {%x}", read_buffer);
    // return ((read_buffer == MAX30102_EXPECTEDPARTID) ? true : false);
    return true;
}
static esp_err_t SoftReset(s_ezlopi_device_properties_t *properties)
{
    uint8_t Check_Register;
    uint8_t flag_hex_value = MAX30102_RESET;
    uint8_t write_byte[] = {MAX30102_MODECONFIG, flag_hex_value};
    ezlopi_i2c_master_write_to_device(&properties->interface.i2c_master, write_byte, 2);
    vTaskDelay(1); // 10ms delay

    // poll for 100 ms
    unsigned long startTime = esp_timer_get_time();
    while ((esp_timer_get_time() - startTime) < 100)
    {
        readRegister8(properties, MAX30102_MODECONFIG, &Check_Register);
        // if 'bit1' in INTR_status2 register is set ; then read procced :
        if ((Check_Register & MAX30102_RESET) == 0)
        {
            break;
        }
        vTaskDelay(1); // 10ms delay
    }
    return ESP_OK;
}
static esp_err_t Activate_interrupts(s_ezlopi_device_properties_t *properties)
{
    uint8_t flag_hex_value = FIFO_A_FULL_EN | PPG_READY_EN | ALC_OVF_EN;
    uint8_t write_byte1[] = {MAX30102_INTENABLE1, flag_hex_value};
    ezlopi_i2c_master_write_to_device(&properties->interface.i2c_master, write_byte1, 2);

    flag_hex_value = DIE_TEMP_RDY_EN;
    uint8_t write_byte2[] = {MAX30102_INTENABLE2, flag_hex_value};
    ezlopi_i2c_master_write_to_device(&properties->interface.i2c_master, write_byte2, 2);
    vTaskDelay(5);
    return ESP_OK;
}
static esp_err_t FIFO_config(s_ezlopi_device_properties_t *properties)
{

    uint8_t flag_hex_value = MAX30102_FIFO_SAMPLEAVG_4 | FIFO_ROLLOVER_ENABLE | FIFO_FULL_INTERRUPT_AT_0_SPACE_LEFT;
    uint8_t write_byte[] = {MAX30102_FIFOCONFIG, flag_hex_value};
    ezlopi_i2c_master_write_to_device(&properties->interface.i2c_master, write_byte, 2);
    vTaskDelay(5);

    return ESP_OK;
}
static esp_err_t MODE_config(s_ezlopi_device_properties_t *properties)
{

    uint8_t write_byte[] = {MAX30102_MODECONFIG, MAX30102_MLED_MODE};
    ezlopi_i2c_master_write_to_device(&properties->interface.i2c_master, write_byte, 2);
    vTaskDelay(5);
    return ESP_OK;
}
static esp_err_t SPO2_config(s_ezlopi_device_properties_t *properties)
{
    uint8_t flag_hex_value = MAX30102_SPO2_ADC_FS_4096 | MAX30102_SPO2_SR_100 | MAX30102_SPO2_LedPulseWidth_18;
    uint8_t write_byte[] = {MAX30102_SPO2CONFIG, flag_hex_value};
    ezlopi_i2c_master_write_to_device(&properties->interface.i2c_master, write_byte, 2);
    vTaskDelay(5);
    return ESP_OK;
}
static esp_err_t LED_PA_config(s_ezlopi_device_properties_t *properties)
{
    uint8_t write_byte1[] = {MAX30102_LED1_PULSEAMP, MAX30102_RED_LED1_PA_6_2mA};
    ezlopi_i2c_master_write_to_device(&properties->interface.i2c_master, write_byte1, 2);

    uint8_t write_byte2[] = {MAX30102_LED2_PULSEAMP, MAX30102_IR_LED2_PA_6_2mA};
    ezlopi_i2c_master_write_to_device(&properties->interface.i2c_master, write_byte2, 2);
    vTaskDelay(5);
    return ESP_OK;
}
static esp_err_t MUL_MODE_CTRL_config(s_ezlopi_device_properties_t *properties)
{
    uint8_t flag_hex_value = MAX30102_SLOT2_IR | MAX30102_SLOT1_RED;
    uint8_t write_byte1[] = {MAX30102_MULTILEDCONFIG1, flag_hex_value};
    ezlopi_i2c_master_write_to_device(&properties->interface.i2c_master, write_byte1, 2);

    flag_hex_value = MAX30102_SLOT2_IR | MAX30102_SLOT1_RED;
    uint8_t write_byte2[] = {MAX30102_MULTILEDCONFIG2, flag_hex_value};
    ezlopi_i2c_master_write_to_device(&properties->interface.i2c_master, write_byte2, 2);
    vTaskDelay(5);

    return ESP_OK;
}
static esp_err_t FIFO_RESET(s_ezlopi_device_properties_t *properties)
{

    uint8_t write_byte1[] = {MAX30102_FIFO_WRITEPTR, 0x00};
    ezlopi_i2c_master_write_to_device(&properties->interface.i2c_master, write_byte1, 2);

    uint8_t write_byte2[] = {MAX30102_FIFO_OVERFLOW, 0x00};
    ezlopi_i2c_master_write_to_device(&properties->interface.i2c_master, write_byte2, 2);

    uint8_t write_byte3[] = {MAX30102_FIFO_READPTR, 0x00};
    ezlopi_i2c_master_write_to_device(&properties->interface.i2c_master, write_byte3, 2);

    vTaskDelay(5);

    return ESP_OK;
}
//-----------------------------------------------------------------------------

static esp_err_t Check_fifo_full_flag(s_ezlopi_device_properties_t *properties, uint8_t *temp)
{
    esp_err_t err = ESP_OK;
    uint8_t write_buffer[] = {MAX30102_INTSTAT1};
    ezlopi_i2c_master_write_to_device(&properties->interface.i2c_master, write_buffer, 1);
    ezlopi_i2c_master_read_from_device(&properties->interface.i2c_master, temp, 1);
    if (NULL != temp)
    {
        err = ESP_OK;
    }
    else
    {
        err = ESP_ERR_TIMEOUT;
    }
    return err;
}

// return temperature
static void MAX30102_get_die_temperature(s_ezlopi_device_properties_t *properties, float *temp_val)
{
    uint8_t tempInt = 0;
    uint8_t tempFrac = 0;
    uint8_t Check_Register;
    uint8_t address_val;
    esp_err_t err = ESP_OK;

    // Step 1: Config die temperature register to take 1 temperature sample

    // enable the guard in [0x21H] to read die-temperature
    uint8_t flag_hex_value = DIE_TEMP_GAURD;
    uint8_t write_buffer[] = {MAX30102_DIETEMPCONFIG, flag_hex_value};
    ezlopi_i2c_master_write_to_device(&properties->interface.i2c_master, write_buffer, 2);

    // Poll for bit to clear, reading is then complete
    // Timeout after 100ms
    unsigned long startTime = esp_timer_get_time();
    while ((esp_timer_get_time() - startTime) < 100)
    {
        /*
         * extract data from register 0x01 (LSB)
         */
        readRegister8(properties, MAX30102_INTSTAT2, &Check_Register);
        // if 'bit1' in INTR_status2 register is set ; then read procced :
        if ((Check_Register == DIE_TEMP_RDY))
        {
            // read the DIE-temp
            readRegister8(properties, MAX30102_DIETEMPINT, &tempInt);
            readRegister8(properties, MAX30102_DIETEMPFRAC, &tempFrac);
            break;
        }
        else
        {
            TRACE_E("Temperature_INT @reg{%x}... Not set ", MAX30102_INTSTAT2);
        }
        vTaskDelay(1); // 10ms delay
    }

    // First test this data ; then add [2,4,8 ] *C ; for correction

    (*temp_val) = (float)tempInt + ((float)tempFrac * 0.0625f); // in *C
}

//-----------------------------------------------------------------------------

static uint8_t MAX30102_getWritePointer(s_ezlopi_device_properties_t *properties)
{
    uint8_t temp;
    readRegister8(properties, MAX30102_FIFO_WRITEPTR, &temp); // write into temp register
    return temp;
}

static uint8_t MAX30102_getReadPointer(s_ezlopi_device_properties_t *properties)
{
    uint8_t temp;
    readRegister8(properties, MAX30102_FIFO_READPTR, &temp); // write into temp register
    return temp;
}

// extracts new FIFO data into [sSenseBuf_t]
static void MAX30102_getNewData(s_ezlopi_device_properties_t *properties)
{
    int32_t numberOfSamples = 0;
    uint8_t readPointer = 0;
    uint8_t writePointer = 0;
    while (1)
    {
        readPointer = MAX30102_getReadPointer(properties);
        writePointer = MAX30102_getWritePointer(properties);

        if (readPointer == writePointer)
        {
            TRACE_E("FIFO : ......No New Data.....");
        }
        else
        {
            numberOfSamples = writePointer - readPointer;
            if (numberOfSamples < 0)
            {
                numberOfSamples += 32;
            }
            int32_t bytesNeedToRead = numberOfSamples * ActiveLEDs * 3;

            while (bytesNeedToRead > 0)
            {
                senseBuf.head++;                          // increase the current head counter val by 1
                senseBuf.head %= MAX30102_SENSE_BUF_SIZE; // 0-29
                uint32_t tempBuf = 0;
                if (ActiveLEDs > 1)
                {
                    uint8_t temp[6];
                    uint8_t tempex;

                    // I2C burst read
                    uint8_t addr = MAX30102_FIFO_DATA;
                    ezlopi_i2c_master_write_to_device(&properties->interface.i2c_master, &addr, 1);
                    ezlopi_i2c_master_read_from_device(&properties->interface.i2c_master, temp, 6); // extracts data into @temp[6]

                    // re-arrange from MSB to LSB [both RED & IR]
                    for (uint8_t i = 0; i < 3; i++)
                    { // reversing bytes of temp[6] i.e. RED,IR -> IR,RED
                        tempex = temp[i];
                        temp[i] = temp[5 - i];
                        temp[5 - i] = tempex;
                    }

                    // extract first 3 bytes (IR)
                    memcpy(&tempBuf, temp, 3 * sizeof(temp[0])); // copying 5,4,3 - 24 bits
                    tempBuf &= 0x3FFFF;                          // allowing 18-bits only
                    senseBuf.IR[senseBuf.head] = tempBuf;        // e.g : senseBuf.IR[1,2,3...30] = (uint32_t value)

                    // extract second 3 bytes (RED)
                    memcpy(&tempBuf, temp + 3, 3 * sizeof(temp[0])); // copying 2,1,0 - 24 bits
                    tempBuf &= 0x3FFFF;                              // allowing 18-bits only
                    senseBuf.red[senseBuf.head] = tempBuf;           // e.g : senseBuf.red[1,2,3...30] = (uint32_t value)
                }
                else
                {
                    uint8_t temp[3];
                    uint8_t tempex;

                    // I2C burst read
                    uint8_t addr = MAX30102_FIFO_DATA;
                    ezlopi_i2c_master_write_to_device(&properties->interface.i2c_master, &addr, 1);
                    ezlopi_i2c_master_read_from_device(&properties->interface.i2c_master, temp, 3); // extracts data into @temp[6]

                    // rearrange from MSB to LSB
                    tempex = temp[0];
                    temp[0] = temp[2];
                    temp[2] = tempex;

                    memcpy(&tempBuf, temp, 3 * sizeof(temp[0])); // copy value to temp_buffer
                    tempBuf &= 0x3FFFF;                          // 18-bits only
                    senseBuf.red[senseBuf.head] = tempBuf;       // copy temp_buffer to structure
                }
                bytesNeedToRead -= ActiveLEDs * 3;
            }
            return;
        }
        vTaskDelay(1);
    }
}

// this fuction will write requried values into arguments
static void MAX30102_heartrateAndOxygenSaturation(s_ezlopi_device_properties_t *properties, int32_t *SPO2, int8_t *SPO2Valid, int32_t *heartRate, int8_t *heartRateValid)
{
    uint32_t irBuffer[100];
    uint32_t redBuffer[100];

    int32_t bufferLength = 100; // max 192bytes

    for (uint8_t i = 0; i < bufferLength;)
    {
        MAX30102_getNewData(properties);
        int8_t numberOfSamples = senseBuf.head - senseBuf.tail;
        if (numberOfSamples < 0)
        {
            numberOfSamples += MAX30102_SENSE_BUF_SIZE; // 0-29
        }

        while (numberOfSamples--)
        {
            redBuffer[i] = senseBuf.red[senseBuf.tail];
            irBuffer[i] = senseBuf.IR[senseBuf.tail];

            senseBuf.tail++;
            senseBuf.tail %= MAX30102_SENSE_BUF_SIZE; // 0-29
            i++;
            if (i == bufferLength)
            {
                break;
            }
        }
    }

    maxim_heart_rate_and_oxygen_saturation(irBuffer,
                                           bufferLength,
                                           redBuffer,
                                           SPO2,
                                           SPO2Valid,
                                           heartRate,
                                           heartRateValid);
}
//------------------------------------------------------------
static uint8_t readRegister8(s_ezlopi_device_properties_t *properties, uint8_t address, uint8_t *reg)
{

    ezlopi_i2c_master_write_to_device(&properties->interface.i2c_master, &address, 1);
    ezlopi_i2c_master_read_from_device(&properties->interface.i2c_master, reg, 1); // extracts data into @reg
    return 1;
}

//------------------------------------------------------------

// // getRed
// uint32_t MAX30102_getRed(s_ezlopi_device_properties_t *properties)
// {
//     MAX30102_getNewData(properties);
//     return (senseBuf.red[senseBuf.head]);
// }

// // getIR
// uint32_t MAX30102_getIR(s_ezlopi_device_properties_t *properties)
// {
//     MAX30102_getNewData(properties);
//     return (senseBuf.IR[senseBuf.head]);
// }

// static void get_FIFO_DATA(s_ezlopi_device_properties_t *properties)
// {
//     uint8_t buffer_0, buffer_1;
//     uint8_t Check_Register = 0;
//     uint8_t address_val;
//     uint8_t temp[6];
//     uint8_t tempex;
//     esp_err_t err = ESP_OK;
//     /*
//      * extract data from register 0 (LSB)
//      */
//     if ((err = Check_fifo_full_flag(properties, &Check_Register)) == ESP_OK)
//     {
//         // if 'bit7' in INTR register is set ; then read procced to read :- acc registers
//         if ((Check_Register & (1 << 7))) // FIFO_A_FULL => bit7
//         {
//             // I2C burst read
//             ezlopi_i2c_master_write_to_device(&properties->interface.i2c_master, MAX30102_FIFO_DATA, 1);
//             ezlopi_i2c_master_read_from_device(&properties->interface.i2c_master, &temp, 6); // extracts data into @temp[6]
//              // [now re-arrange MSB-LSB] and [filter out 18-bit from 24-bit]
//              // 1.rearrange from MSB to LSB
//               tempex = temp[0];
//               temp[0] = temp[2];
//               temp[2] = tempex;
//              // 2.filter out 18-bit from 24-bit
//              }
//     }
//     else
//     {
//         TRACE_E("Data not ready @reg{%x}... Error type:-ESP_ERR_%d ", ADXL345_DATA_X_0_REGISTER, (err));
//     }
//
// }
