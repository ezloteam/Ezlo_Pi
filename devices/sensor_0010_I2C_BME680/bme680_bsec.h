#pragma once

#include "bme68xLibrary.h"
#include "bsec2.h"
#include <math.h>

#define BME680_SDA              GPIO_NUM_18
#define BME680_SCL              GPIO_NUM_17
#define ACTIVE_I2C              I2C_NUM_0

#define BME680_TEST 0
#define SEALEVELPRESSURE_HPA (1013.25)

typedef struct 
{
    float iaq;
    bool iaq_accuracy;
    float co2_equivalent;
    float voc_equivalent;
    float temperature;
    float pressure;
    float humidity;
    float gas_resistance;
    float altitude;
    bool stabilization_status;
    bool run_in_status;
}   bme680_data_t;
/**
 * @brief : This function checks the BSEC status, prints the respective error code. Halts in case of error
 * @param[in] bsec  : Bsec2 class object
 */
void check_bsec_status();



/**
* @brief : This function initializes the BSEC and BME680 Sensor
* @param[in] initialize_i2c  :  true : initialize i2c,
                                false : skips i2c initialization
*/
void bme680_setup(uint32_t sda, uint32_t scl, bool initialize_i2c);

bool bme680_get_data(bme680_data_t* data);

bool bme680_print_data(bme680_data_t* data);

float bme680_read_altitude(float pressure, float seaLevel);

bool get_data_status();