
#include "sensor_0044_I2C_TSL256_luminosity.h"
#include "ezlopi_util_trace.h"

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

#warning "Nabin needs to check this!, Note: Unsigned value type is always true for (ratio >= 0)."
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
#warning "Nabin needs to check this!, Note: Unsigned value type is always false for (temp < 0)."
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

static void Power_Up_tsl2561(s_ezlopi_i2c_master_t *i2c_master)
{
    // first power-up the device
    uint8_t command_code = (SELECT_CMD_REGISTER | TSL2561_REGISTER_CONTROL);
    uint8_t write_buffer_power_up[] = {command_code, TSL2561_POWER_ON};
    ezlopi_i2c_master_write_to_device(i2c_master, write_buffer_power_up, 2);
}

static void Power_Down_tsl2561(s_ezlopi_i2c_master_t *i2c_master)
{
    // Now, power-down the device
    uint8_t command_code = (SELECT_CMD_REGISTER | TSL2561_REGISTER_CONTROL);
    uint8_t write_buffer_power_down[] = {command_code, TSL2561_POWER_OFF};
    ezlopi_i2c_master_write_to_device(i2c_master, write_buffer_power_down, 2);
}

//------------------------------------------------------------

static uint8_t readRegister8(s_ezlopi_i2c_master_t *i2c_master, uint8_t target_address, size_t address_len, uint8_t *reg, size_t reg_len)
{
    ezlopi_i2c_master_write_to_device(i2c_master, &target_address, address_len);
    ezlopi_i2c_master_read_from_device(i2c_master, reg, reg_len); // extracts data into @reg
    return 1;
}

bool Check_PARTID(s_ezlopi_i2c_master_t *i2c_master)
{

    Power_Up_tsl2561(i2c_master);

    // Read -> PART_ID value
    uint8_t read_buffer = 0;
    uint8_t command_code = (SELECT_CMD_REGISTER | TSL2561_REGISTER_ID);
    // uint8_t write_buffer1[] = {command_code};
    // ezlopi_i2c_master_write_to_device(i2c_master, write_buffer1, 1);
    // ezlopi_i2c_master_read_from_device(i2c_master, &read_buffer, 1);
    if (readRegister8(i2c_master, command_code, 1, &read_buffer, 1))
    {
        TRACE_E(" PART_ID : {%x}", (read_buffer & (0xF0))); // required [0b0101xxxx]
        TRACE_E(" REV_NO  : {%x}", (read_buffer & (0x0F)));
    }
    else
    {
        TRACE_E(" Unable to read the  PART_ID register [0x0A].....");
    }

    Power_Down_tsl2561(i2c_master);

    return (((TSL2561_PART_NUMBER) == (read_buffer & 0xF0)) ? true : false);
}

void sensor_0044_tsl2561_configure_device(s_ezlopi_i2c_master_t *i2c_master)
{
    Power_Up_tsl2561(i2c_master);
    // this controls both intergration time and gain of ADC
    // Set the timing and gain

    uint8_t command_code = (SELECT_CMD_REGISTER | TSL2561_REGISTER_TIMING);
    uint8_t setup_code = (TSL2561_HIGH_GAIN_MODE_x16 | TSL2561_STOP_MANNUAL_INTEGRATION | TSL2561_INTEGRATION_TIME_101_MS);
    uint8_t write_buffer_timing[] = {command_code, setup_code};
    ezlopi_i2c_master_write_to_device(i2c_master, write_buffer_timing, 2);

    // Power_Down_tsl2561(i2c_master);
}

uint32_t tsl2561_get_intensity_value(s_ezlopi_i2c_master_t *i2c_master)
{
    uint32_t illuminance_value = 0;
    bool clear_to_read = false;
    // first check if sensor is powered on    // Read -> PART_ID value
    uint8_t read_buffer = 0;
    uint8_t command_code = (SELECT_CMD_REGISTER | TSL2561_REGISTER_ID);
    if (readRegister8(i2c_master, command_code, 1, &read_buffer, 1))
    {
        // TRACE_E(" Updating Data.... found PART_ID : {%x}", (read_buffer & (0xF0))); // required [0b0101xxxx]
        // Powered ON ? [Part_No : 0b0101xxxx]
        if ((TSL2561_PART_NUMBER) == (read_buffer & 0xF0))
        {
            read_buffer = 0;
            command_code = (SELECT_CMD_REGISTER | TSL2561_REGISTER_TIMING);
            // if powered ON, check the Gain & Integration time from 0x01H
            if (readRegister8(i2c_master, command_code, 1, &read_buffer, 1))
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
        ezlopi_i2c_master_write_to_device(i2c_master, &target_address, 1);
        ezlopi_i2c_master_read_from_device(i2c_master, (uint8_t *)&IR, 2); // extracts CH1-data
        // readRegister8(i2c_master, &target_address, 1, &IR, 2);

        // extract the CH0-bits first
        target_address = (SELECT_CMD_REGISTER | DO_WORD_TRANSACTION | TSL2561_REGISTER_CHAN0_LOW);
        ezlopi_i2c_master_write_to_device(i2c_master, &target_address, 1);
        ezlopi_i2c_master_read_from_device(i2c_master, (uint8_t *)&Visible_Ir, 2); // extracts CH2-data
        // readRegister8(i2c_master, &target_address, 1, &Visible_Ir, 2);

        // Calculate the lux value
        illuminance_value = TSL2561_CalculateLux(Visible_Ir,                    // CH0
                                                 IR,                            // CH1
                                                 TSL2561_INTEGRATIONTIME_101MS, // conv_time
                                                 TSL2561_GAIN_x1);              // adc_gain
        // TRACE_I("IR : %d", IR);
        // TRACE_I("Visible : %d", Visible_Ir - IR);
        // TRACE_I("Lux : %d", Lux_intensity);
        // TRACE_E(" Data update completed......");
    }
    else
    {
        TRACE_E(" Data update Failed ......");
    }
    return illuminance_value;
}
