/**
 * Copyright (c) 2021 Bosch Sensortec GmbH. All rights reserved.
 *
 * BSD-3-Clause
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * @file    bme68xLibrary.h
 * @date    8 Feb 2022
 * @version 1.1.40407
 *
 */

#ifndef BME68X_LIBRARY_H
#define BME68X_LIBRARY_H

#include <stdio.h>
#include <string.h>

#include "bme68x.h"
#include <driver/i2c.h>
#include "ezlopi_i2c_master.h"

#define BME68X_ERROR            INT8_C(-1)
#define BME68X_WARNING          INT8_C(1)

#define I2C_MASTER_FREQ_HZ 400000

/** Datatype to keep consistent with camel casing */
typedef struct bme68x_data          bme68x_data;
typedef struct bme68x_dev           bme68x_dev;
typedef enum   bme68x_intf          bme68x_intf;
typedef struct bme68x_conf          bme68x_conf;
typedef struct bme68x_heatr_conf    bme68x_heatr_conf;

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t bme68x_i2c_master_init(gpio_num_t sda, gpio_num_t scl);

/**
 * @brief Function that implements the default microsecond delay callback
 * @param periodUs : Duration of the delay in microseconds
 * @param intfPtr  : Pointer to the interface descriptor
 */
void bme68x_delay_us(uint32_t periodUs, void *intfPtr);

/**
 * @brief Function that implements the default I2C write transaction
 * @param reg_addr : Register address of the sensor
 * @param reg_data : Pointer to the data to be written to the sensor
 * @param length   : Length of the transfer
 * @param intfPtr : Pointer to the interface descriptor
 * @return 0 if successful, non-zero otherwise
 */
int8_t bme68x_i2c_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t length, void *intfPtr);

/**
 * @brief Function that implements the default I2C read transaction
 * @param reg_addr : Register address of the sensor
 * @param reg_data : Pointer to the data to be written to the sensor
 * @param length   : Length of the transfer
 * @param intfPtr : Pointer to the interface descriptor
 * @return 0 if successful, non-zero otherwise
 */
int8_t bme68x_i2c_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t length, void *intfPtr);



/**
 * Class constructor
 */
void bme68xlib_setup(s_ezlopi_i2c_master_t* bme68x_i2c_master_conf);

/**
 * @brief Function to initialize the sensor based on custom callbacks
 * @param intf     : BME68X_SPI_INTF or BME68X_I2C_INTF interface
 * @param read     : Read callback
 * @param write    : Write callback
 * @param idleTask : Delay or Idle function
 * @param intfPtr : Pointer to the interface descriptor
 */
void bme68xlib_begin_custom(bme68x_intf intf, bme68x_read_fptr_t read, bme68x_write_fptr_t write,
        bme68x_delay_us_fptr_t idle_task, void *intf_ptr);

/**
 * @brief Function to initialize the sensor based on the Wire library
 * @param i2cAddr  : The I2C address the sensor is at
 * @param i2c      : The TwoWire object
 * @param idleTask : Delay or Idle function
 */
void bme68xlib_begin(void);

/**
 * @brief Function to read a register
 * @param reg_addr : Register address
 * @return Data at that register
 */
uint8_t bme68xlib_read_reg_byte(uint8_t reg_addr);

/**
 * @brief Function to read multiple registers
 * @param reg_addr : Start register address
 * @param reg_data : Pointer to store the data
 * @param length  : Number of registers to read
 */
void bme68xlib_read_reg(uint8_t reg_addr, uint8_t *reg_data, uint32_t length);

/**
 * @brief Function to write data to a register
 * @param reg_addr : Register addresses
 * @param reg_data : Data for that register
 */
void bme68xlib_write_reg_byte(uint8_t reg_addr, uint8_t reg_data);

/**
 * @brief Function to write multiple registers
 * @param reg_addr : Pointer to the register addresses
 * @param reg_data : Pointer to the data for those registers
 * @param length  : Number of register to write
 */
void bme68xlib_write_reg(uint8_t *reg_addr, const uint8_t *reg_data, uint32_t length);

/**
 * @brief Function to trigger a soft reset
 */
void bme68xlib_soft_reset(void);

/**
 * @brief Function to set the ambient temperature for better configuration
 * @param temp : Temperature in degree Celsius. Default is 25 deg C
 */
void bme68xlib_set_ambient_temp(int8_t temp); //int8_t temp = 25)

/**
 * @brief Function to get the measurement duration in microseconds
 * @param op_mode : Operation mode of the sensor. Attempts to use the last one if nothing is set
 * @return Temperature, Pressure, Humidity measurement time in microseconds
 */
uint32_t bme68xlib_get_meas_dur(uint8_t op_mode); // uint8_t op_mode = BME68X_SLEEP_MODE

/**
 * @brief Function to set the operation mode
 * @param op_mode : BME68X_SLEEP_MODE, BME68X_FORCED_MODE, BME68X_PARALLEL_MODE, BME68X_SEQUENTIAL_MODE
 */
void bme68xlib_set_op_mode(uint8_t op_mode);

/**
 * @brief Function to get the operation mode
 * @return Operation mode : BME68X_SLEEP_MODE, BME68X_FORCED_MODE, BME68X_PARALLEL_MODE, BME68X_SEQUENTIAL_MODE
 */
uint8_t bme68xlib_get_op_mode(void);

/**
 * @brief Function to get the Temperature, Pressure and Humidity over-sampling
 * @param os_hum  : BME68X_OS_NONE to BME68X_OS_16X
 * @param os_temp : BME68X_OS_NONE to BME68X_OS_16X
 * @param os_pres : BME68X_OS_NONE to BME68X_OS_16X
 */
void bme68xlib_get_tph(uint8_t* os_hum, uint8_t* os_temp, uint8_t* os_pres);

/**
 * @brief Function to set the Temperature, Pressure and Humidity over-sampling.
 *        Passing no arguments sets the defaults.
 * @param os_temp : BME68X_OS_NONE to BME68X_OS_16X
 * @param os_pres : BME68X_OS_NONE to BME68X_OS_16X
 * @param os_hum  : BME68X_OS_NONE to BME68X_OS_16X
 */
void bme68xlib_set_tph(uint8_t os_temp, uint8_t os_pres, uint8_t os_hum); // uint8_t os_temp = BME68X_OS_2X, uint8_t os_pres = BME68X_OS_16X, uint8_t os_hum = BME68X_OS_1X

/**
 * @brief Function to get the filter configuration
 * @return BME68X_FILTER_OFF to BME68X_FILTER_SIZE_127
 */
uint8_t bme68xlib_get_filter(void);

/**
 * @brief Function to set the filter configuration
 * @param filter : BME68X_FILTER_OFF to BME68X_FILTER_SIZE_127
 */
void bme68xlib_set_filter(uint8_t filter); // uint8_t filter = BME68X_FILTER_OFF

/**
 * @brief Function to get the sleep duration during Sequential mode
 * @return BME68X_ODR_NONE to BME68X_ODR_1000_MS
 */
uint8_t bme68xlib_get_seq_sleep(void);

/**
 * @brief Function to set the sleep duration during Sequential mode
 * @param odr : BME68X_ODR_NONE to BME68X_ODR_1000_MS
 */
void bme68x_set_seq_sleep(uint8_t odr); //uint8_t odr = BME68X_ODR_0_59_MS

/**
 * @brief Function to set the heater profile for Forced mode
 * @param temp : Heater temperature in degree Celsius
 * @param dur  : Heating duration in milliseconds
 */
void bme68xlib_set_heater_prof_p2(uint16_t temp, uint16_t dur);

/**
 * @brief Function to set the heater profile for Sequential mode
 * @param temp       : Heater temperature profile in degree Celsius
 * @param dur        : Heating duration profile in milliseconds
 * @param profile_len : Length of the profile
 */
void bme68xlib_set_heater_prof_p3(uint16_t *temp, uint16_t *dur, uint8_t profile_len);

/**
 * @brief Function to set the heater profile for Parallel mode
 * @param temp           : Heater temperature profile in degree Celsius
 * @param mul            : Profile of number of repetitions
 * @param shared_heatr_dur : Shared heating duration in milliseconds
 * @param profile_len     : Length of the profile
 */
void bme68xlib_set_heater_prof_p4(uint16_t *temp, uint16_t *mul, uint16_t shared_heatr_dur, uint8_t profile_len);

/**
 * @brief Function to fetch data from the sensor into the local buffer
 * @return Number of new data fields
 */
uint8_t bme68xlib_fetch_data(void);

/**
 * @brief Function to get a single data field
 * @param data : Structure where the data is to be stored
 * @return Number of new fields remaining
 */
uint8_t bme68xlib_get_data(bme68x_data* data);

    /**
 * @brief Function to get whole sensor data
 * @return Sensor data
 */
bme68x_data* bme68xlib_get_all_data(void);

/**
 * @brief Function to get the BME68x heater configuration
 */
const bme68x_heatr_conf* bme68xlib_get_heater_configuration(void);

/**
 * @brief Function to retrieve the sensor's unique ID
 * @return Unique ID
 */
uint32_t bme68xlib_get_unique_id(void);

/**
 * @brief Function to get the error code of the interface functions
 * @return Interface return code
 */
BME68X_INTF_RET_TYPE bme68xlib_intf_error(void);

/**
 * @brief Function to check if an error / warning has occurred
 * @return -1 if an error occurred, 1 if warning occured else 0
 */
int8_t bme68xlib_check_status(void);

/**
 * @brief Function to get a brief text description of the error
 * @return Returns a string describing the error code
 */
const char* bme68xlib_status_string(void);

/**
 * @brief Function to get the status
 * @return Returns the value of status
 */
int8_t bme68xlib_get_status(void);

#ifdef __cplusplus
}
#endif

#endif /* BME68X_CLASS_H */
