#include "ALS_LTR303.h"

unsigned char gain = LTR3XX_GAIN_1;                    // Gain setting, values = 0-7
unsigned char integration_time = LTR3XX_INTEGTIME_200; // Integration ("shutter") time in milliseconds
unsigned char measurement_rate = LTR3XX_MEASRATE_200;  // Interval between DATA_REGISTERS update
bool valid = 0, intr_status = 0, data_status = 0;
byte error;

bool ltr303_is_data_available(void)
{
    ltr303_get_status(&valid, &gain, &intr_status, &data_status);
    return data_status;
}

esp_err_t ltr303_setup(uint32_t sda, uint32_t scl, bool initialize_i2c)
{
    s_ezlopi_i2c_master_t ltr303_i2c_master_conf = {
        .enable = true,
        .address = LTR303_ADDR,
        .channel = I2C_NUM_0,
        .sda = sda,
        .scl = scl,
        .clock_speed = I2C_MASTER_FREQ_HZ,
    };
    if (!initialize_i2c)
    {
        ltr303_i2c_master_conf.enable = false;
    }
    ltr303_begin(&ltr303_i2c_master_conf);
    unsigned char ID;

    if (ltr303_get_part_id(&ID))
    {
        printf("Got Sensor Part ID: %02X\n", ID);
    }
    if (ltr303_get_manufac_id(&ID))
    {
        printf("Got Manuf Part ID: %02X\n", ID);
    }

    printf("Setting Gain...\n");
    ltr303_set_control(gain, false, false);

    printf("Set timing...\n");
    ltr303_set_measurement_rate(integration_time, measurement_rate);

    printf("Powerup...\n");
    ltr303_set_powerup();

    return ESP_OK;
}

esp_err_t ltr303_loop(void)
{

    return ESP_OK;
}

esp_err_t ltr303_get_val(ltr303_data_t *ltr303_data)
{
    if (ltr303_is_data_available())
    {
        unsigned int data0, data1;
        if (ltr303_get_data(&data0, &data1))
        {

// getData() returned true, communication was successful
#if DEBUG
            printf("data0: %d\n", data0);
            printf("data1: %d\n", data1);
#endif
            // To calculate lux, pass all your settings and readings
            // to the getLux() function.

            // The getLux() function will return 1 if the calculation
            // was successful, or 0 if one or both of the sensors was
            // saturated (too much light). If this happens, you can
            // reduce the integration time and/or gain.

            // Perform lux calculation:
            if (ltr303_get_lux(gain, integration_time, data0, data1, &(ltr303_data->lux)))
            {
                return ESP_OK;
            }
        }
    }
    return ESP_FAIL;
}