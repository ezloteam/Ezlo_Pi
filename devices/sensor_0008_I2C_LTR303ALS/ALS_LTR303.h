#pragma once

#include "LTR303.h"

#define LTR303_SDA          GPIO_NUM_18
#define LTR303_SCL          GPIO_NUM_17
#define LTR303_INT_ALS_PIN  GPIO_NUM_16
#define ACTIVE_I2C          I2C_NUM_0

#define DEBUG 0
typedef struct 
{
    double lux; 
}   ltr303_data_t;
/**
* @brief : This function initializes the LTR303 Sensor
* @param[in] initialize_i2c  :  true : initialize i2c,
                                false : skips i2c initialization
*/
esp_err_t ltr303_setup(uint32_t sda, uint32_t scl, bool initialize_i2c); 

esp_err_t ltr303_loop(void); 
esp_err_t ltr303_get_val(ltr303_data_t* ltr303_data); 
bool ltr303_is_data_available(void);