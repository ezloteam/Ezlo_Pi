#include "bme680_bsec.h"

/**
 * @brief : This function is called by the BSEC library when a new output is available
 * @param[in] input     : BME68X sensor data before processing
 * @param[in] outputs   : Processed BSEC BSEC output data
 * @param[in] bsec      : Instance of BSEC2 calling the callback
 */
static void bme680_data_callback(const bme68x_data data, const bsec_outputs outputs);

static bool callback_status = false;
static bme680_data_t bme680_data;

/* Entry point for the example */
void bme680_setup(uint32_t sda, uint32_t scl, bool initialize_i2c)
{
    /* Desired subscription list of BSEC2 outputs */
    bsec_sensor sensor_list[] = {
        BSEC_OUTPUT_IAQ,
        BSEC_OUTPUT_RAW_TEMPERATURE,
        BSEC_OUTPUT_RAW_PRESSURE,
        BSEC_OUTPUT_RAW_HUMIDITY,
        BSEC_OUTPUT_RAW_GAS,
        BSEC_OUTPUT_CO2_EQUIVALENT,
        BSEC_OUTPUT_BREATH_VOC_EQUIVALENT,
        BSEC_OUTPUT_STABILIZATION_STATUS,
        BSEC_OUTPUT_RUN_IN_STATUS};

    s_ezlopi_i2c_master_t bme68x_i2c_master_conf = {
        .enable = true,
        .address = BME68X_I2C_ADDR_HIGH,
        .channel = ACTIVE_I2C,
        .sda = sda,
        .scl = scl,
        .clock_speed = I2C_MASTER_FREQ_HZ,
    };

    // bme68x_i2c_master_conf.enable = true;
    // bme68x_i2c_master_conf.address = BME68X_I2C_ADDR_HIGH;
    // bme68x_i2c_master_conf.channel = ACTIVE_I2C;
    // bme68x_i2c_master_conf.sda = sda;
    // bme68x_i2c_master_conf.scl = scl;
    // bme68x_i2c_master_conf.clock_speed = I2C_MASTER_FREQ_HZ;

    if (!initialize_i2c)
    {
        bme68x_i2c_master_conf.enable = false;
    }

    bsec2_setup(&bme68x_i2c_master_conf);
    /* Initialize the library and interfaces */
    if (!bsec2_begin())
    {
        check_bsec_status();
    }

    /* Subsribe to the desired BSEC2 outputs */
    bool subscription_status = bsec2_update_subscription(sensor_list, ARRAY_LEN(sensor_list), BSEC_SAMPLE_RATE_LP);
    // printf("subscription_status is %d\n", subscription_status);
    if (!subscription_status)
    {
        check_bsec_status();
    }

    /* Whenever new data is available call the data_callback function */
    bsec2_attach_callback(bme680_data_callback);

    bsec_version_t bsec2_version = bsec2_get_version();
    printf("BSEC library version %d.%d.%d.%d\n", bsec2_version.major, bsec2_version.minor, bsec2_version.major_bugfix, bsec2_version.minor_bugfix);
}

/* Function to copy sensor data */
static bool bme680_copy_data(bme680_data_t *dest, bme680_data_t *src)
{
    if ((dest == NULL) && (src == NULL))
    {
        return false;
    }

    memcpy(dest, src, sizeof(bme680_data_t));
    return true;
}

/* Function that is looped forever */
bool bme680_get_data(bme680_data_t *data)
{
    /* Call the run function often so that the library can
     * check if it is time to read new data from the sensor
     * and process it.
     */
    if (!bsec2_run())
    {
        check_bsec_status();
        return false;
    }
    if (callback_status)
    {
        if (bme680_copy_data(data, &bme680_data))
        {
            callback_status = false;
            return true;
        }
    }

    return false;
}

static void bme680_data_callback(const bme68x_data data, const bsec_outputs outputs)
{
    // printf("HERE!! %s\n", __func__);
    if (!outputs.n_outputs)
    {
        return;
    }
#if BME680_TEST
    printf("BSEC outputs:\n\ttimestamp = %d\n", (int)(outputs.output[0].time_stamp / INT64_C(1000000)));
#endif

    callback_status = true;

    for (uint8_t i = 0; i < outputs.n_outputs; i++)
    {
        const bsec_data output = outputs.output[i];
        switch (output.sensor_id)
        {
        case BSEC_OUTPUT_IAQ:
#if BME680_TEST
            printf("\tiaq = %0.2f\n", output.signal);
            printf("\tiaq accuracy = %d\n", (int)output.accuracy);
#endif
            bme680_data.iaq = output.signal;
            bme680_data.iaq_accuracy = output.accuracy;
            break;

        case BSEC_OUTPUT_RAW_TEMPERATURE:
#if BME680_TEST
            printf("\ttemperature = %0.2f\n", output.signal);
#endif
            bme680_data.temperature = output.signal;
            break;

        case BSEC_OUTPUT_RAW_PRESSURE:
#if BME680_TEST
            printf("\tpressure = %0.2f\n", output.signal);
#endif
            bme680_data.pressure = output.signal;
            bme680_data.altitude = bme680_read_altitude(bme680_data.pressure, SEALEVELPRESSURE_HPA);
            break;

        case BSEC_OUTPUT_RAW_HUMIDITY:
#if BME680_TEST
            printf("\thumidity = %0.2f\n", output.signal);
#endif
            bme680_data.humidity = output.signal;
            break;

        case BSEC_OUTPUT_RAW_GAS:
#if BME680_TEST
            printf("\tgas resistance = %0.2f\n", output.signal);
#endif
            bme680_data.gas_resistance = output.signal;
            break;

        case BSEC_OUTPUT_CO2_EQUIVALENT:
#if BME680_TEST
            printf("\tCO2 Equivalent = %0.2f\n", output.signal);
#endif
            bme680_data.co2_equivalent = output.signal;
            break;

        case BSEC_OUTPUT_BREATH_VOC_EQUIVALENT:
#if BME680_TEST
            printf("\tVOC Equivalent = %0.2f\n", output.signal);
#endif
            bme680_data.voc_equivalent = output.signal;
            break;

        case BSEC_OUTPUT_STABILIZATION_STATUS:
#if BME680_TEST
            printf("\tstabilization status = %0.2f\n", output.signal);
#endif
            bme680_data.stabilization_status = output.signal;
            break;

        case BSEC_OUTPUT_RUN_IN_STATUS:
#if BME680_TEST
            printf("\trun in status = %0.2f\n", output.signal);
#endif
            bme680_data.run_in_status = output.signal;
            break;

        default:
            break;
        }
    }
}

float bme680_read_altitude(float pressure, float seaLevel)
{
    // Equation taken from BMP180 datasheet (page 16):
    //  http://www.adafruit.com/datasheets/BST-BMP180-DS000-09.pdf

    // Note that using the equation from wikipedia can give bad results
    // at high altitude. See this thread for more information:
    //  http://forums.adafruit.com/viewtopic.php?f=22&t=58064

    float atmospheric = pressure / 100.0F;
    return 44330.0 * (1.0 - pow(atmospheric / seaLevel, 0.1903));
}

void check_bsec_status()
{
    if (bsec2_get_status() < BSEC_OK)
    {
        printf("BSEC error code : %d\n", bsec2_get_status());
    }
    else if (bsec2_get_status() > BSEC_OK)
    {
        printf("BSEC warning code : %d\n", bsec2_get_status());
    }

    if (bsec2_get_sensor_status() < BME68X_OK)
    {
        printf("BME68X error code : %d\n", bsec2_get_sensor_status());
    }
    else if (bsec2_get_sensor_status() > BME68X_OK)
    {
        printf("BME68X warning code : 0x%X\n", bsec2_get_sensor_status());
    }
}

bool bme680_print_data(bme680_data_t *data)
{
    if (data == NULL)
    {
        return false;
    }

    printf("BSEC Outputs:\n");
    printf("\tiaq = %0.2f\n", data->iaq);
    printf("\tiaq accuracy = %d\n", (int)data->iaq_accuracy);
    printf("\ttemperature = %0.2f °C\n", data->temperature);
    printf("\tpressure = %0.2f hPa\n", data->pressure / 100.0f);
    printf("\thumidity = %0.2f %%\n", data->humidity);
    printf("\tgas resistance = %0.2f KΩ\n", data->gas_resistance / 1000.0);
    printf("\taltitude = %0.2f m\n", data->altitude);
    printf("\tCO2 Equivalent = %0.2f\n", data->co2_equivalent);
    printf("\tVOC Equivalent = %0.2f\n", data->voc_equivalent);
    printf("\tstabilization status = %d\n", data->stabilization_status);
    printf("\trun in status = %d\n", data->run_in_status);
    return true;
}

bool get_data_status()
{
    return callback_status;
}