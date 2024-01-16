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
 * @file    bme68xLibrary.cpp
 * @date    8 Feb 2022
 * @version 1.1.40407
 *
 */

#include "bme68xLibrary.h"

/* Maximum transaction size. Field size 17 x 3 */
#define BME68X_MAX_READ_LENGTH 51

#ifdef ARDUINO_ARCH_ESP32
#define BME68X_I2C_BUFFER_SIZE I2C_BUFFER_LENGTH

#define BME68X_BURST_SPI_TRANSFER
#endif

#ifdef ARDUINO_ARCH_MBED
/* Assuming all MBED implementations of Wire have 256 byte sized buffers */
#define BME68X_I2C_BUFFER_SIZE 256

#define BME68X_BURST_SPI_TRANSFER
#endif

#ifdef ARDUINO_ARCH_ESP8266
#define BME68X_I2C_BUFFER_SIZE BUFFER_LENGTH

#define BME68X_BURST_SPI_TRANSFER
#endif

#ifdef ARDUINO_ARCH_AVR
#define BME68X_I2C_BUFFER_SIZE BUFFER_LENGTH

#define BME68X_BURST_SPI_TRANSFER
#endif

#ifdef ARDUINO_ARCH_NRF52
#define BME68X_I2C_BUFFER_SIZE SERIAL_BUFFER_SIZE

#define BME68X_BURST_SPI_TRANSFER
#endif

#ifdef ARDUINO_ARCH_SAMD
/* Assuming all Arduino's and Adafruit's SAMD
 * implementations of Wire have 256 byte sized buffers */
#define BME68X_I2C_BUFFER_SIZE 256

#define BME68X_BURST_SPI_TRANSFER
#endif

#ifdef ARDUINO_ARCH_SAM
#define BME68X_I2C_BUFFER_SIZE BUFFER_LENGTH

#define BME68X_BURST_SPI_TRANSFER
#endif

/* Optimistically assume support for at least 64 byte reads */
#ifndef BME68X_I2C_BUFFER_SIZE
#define BME68X_I2C_BUFFER_SIZE 64
#endif

#if BME68X_MAX_READ_LENGTH > BME68X_I2C_BUFFER_SIZE
#warning "Wire read requires a larger buffer size. Use SPI"
#endif

static int8_t status;
static bme68x_dev bme6;
static bme68x_conf conf;
static bme68x_heatr_conf heatrConf;
static bme68x_data sensorData[3];
static uint8_t n_fields, i_fields;
static uint8_t last_op_mode;
s_ezlopi_i2c_master_t g_bme68x_i2c_master_conf;

void bme68xlib_setup(s_ezlopi_i2c_master_t* bme68x_i2c_master_conf)
{
	status = BME68X_OK;
	memset(&bme6, 0, sizeof(bme6));
	memset(&conf, 0, sizeof(conf));
	memset(&heatrConf, 0, sizeof(heatrConf));
	memset(sensorData, 0, sizeof(sensorData));
	bme6.amb_temp = 25; /* Typical room temperature in Celsius */
	n_fields = 0;
	i_fields = 0;
	last_op_mode = BME68X_SLEEP_MODE;

	memcpy(&g_bme68x_i2c_master_conf, bme68x_i2c_master_conf, sizeof(s_ezlopi_i2c_master_t));
	ezlopi_i2c_master_init(&g_bme68x_i2c_master_conf);
}

/**
 * @brief Function to initialize the sensor based on custom callbacks
*/
void bme68xlib_begin_custom(bme68x_intf intf, bme68x_read_fptr_t read, bme68x_write_fptr_t write,
		bme68x_delay_us_fptr_t idle_task, void *intf_ptr)
{

	bme6.intf = intf;
	bme6.read = read;
	bme6.write = write;
	bme6.delay_us = idle_task;
	bme6.intf_ptr = intf_ptr;
	bme6.amb_temp = 25;

	status = bme68x_init(&bme6);
}

/**
 * @brief Function to initialize the sensor based on the Wire library
 */
void bme68xlib_begin(void)
{
	bme6.intf = BME68X_I2C_INTF;
	bme6.read = bme68x_i2c_read;
	bme6.write = bme68x_i2c_write;
	bme6.delay_us = bme68x_delay_us;
	bme6.amb_temp = 25;

	status = bme68x_init(&bme6);
}

/**
 * @brief Function to read a register
 */
uint8_t bme68xlib_read_reg_byte(uint8_t reg_addr)
{
	uint8_t reg_data;
	bme68xlib_read_reg(reg_addr, &reg_data, 1);
	return reg_data;
}

/**
 * @brief Function to read multiple registers
 */
void bme68xlib_read_reg(uint8_t reg_addr, uint8_t *reg_data, uint32_t length)
{
	status = bme68x_get_regs(reg_addr, reg_data, length, &bme6);
}

/**
 * @brief Function to write data to a register
 */
void bme68xlib_write_reg_byte(uint8_t reg_addr, uint8_t reg_data)
{
	status = bme68x_set_regs(&reg_addr, &reg_data, 1, &bme6);
}

/**
 * @brief Function to write multiple registers
 */
void bme68xlib_write_reg(uint8_t *reg_addr, const uint8_t *reg_data, uint32_t length)
{
	status = bme68x_set_regs(reg_addr, reg_data, length, &bme6);
}

/**
 * @brief Function to trigger a soft reset
 */
void bme68xlib_soft_reset(void)
{
	status = bme68x_soft_reset(&bme6);
}

/**
 * @brief Function to set the ambient temperature for better configuration
 */ 
void bme68xlib_set_ambient_temp(int8_t temp)
{
	bme6.amb_temp = temp;
}

/**
 * @brief Function to get the measurement duration in microseconds
 */
uint32_t bme68xlib_get_meas_dur(uint8_t op_mode)
{
	if (op_mode == BME68X_SLEEP_MODE)
		op_mode = last_op_mode;

	return bme68x_get_meas_dur(op_mode, &conf, &bme6);
}

/**
 * @brief Function to set the operation mode
 */
void bme68xlib_set_op_mode(uint8_t op_mode)
{
	status = bme68x_set_op_mode(op_mode, &bme6);
	if ((status == BME68X_OK) && (op_mode != BME68X_SLEEP_MODE))
		last_op_mode = op_mode;
}

/**
 * @brief Function to get the operation mode
 */
uint8_t bme68xlib_get_op_mode(void)
{
	uint8_t op_mode;
	status = bme68x_get_op_mode(&op_mode, &bme6);
	return op_mode;
}

/**
 * @brief Function to get the Temperature, Pressure and Humidity over-sampling
 */
void bme68xlib_get_tph(uint8_t* os_hum, uint8_t* os_temp, uint8_t* os_pres)
{
	status = bme68x_get_conf(&conf, &bme6);

	if (status == BME68X_OK)
	{
		*os_hum = conf.os_hum;
		*os_temp = conf.os_temp;
		*os_pres = conf.os_pres;
	}
}

/**
 * @brief Function to set the Temperature, Pressure and Humidity over-sampling
 */
void bme68xlib_set_tph(uint8_t os_temp, uint8_t os_pres, uint8_t os_hum)
{
	status = bme68x_get_conf(&conf, &bme6);

	if (status == BME68X_OK)
	{
		conf.os_hum = os_hum;
		conf.os_temp = os_temp;
		conf.os_pres = os_pres;

		status = bme68x_set_conf(&conf, &bme6);
	}
}

/**
 * @brief Function to get the filter configuration
 */
uint8_t bme68xlib_get_filter(void)
{
	status = bme68x_get_conf(&conf, &bme6);

	return conf.filter;
}

/**
 * @brief Function to set the filter configuration
 */
void bme68xlib_set_filter(uint8_t filter)
{
	status = bme68x_get_conf(&conf, &bme6);

	if (status == BME68X_OK)
	{
		conf.filter = filter;

		status = bme68x_set_conf(&conf, &bme6);
	}
}

/**
 * @brief Function to get the sleep duration during Sequential mode
 */
uint8_t bme68xlib_get_seq_sleep(void)
{
	status = bme68x_get_conf(&conf, &bme6);

	return conf.odr;
}

/**
 * @brief Function to set the sleep duration during Sequential mode
 */
void bme68x_set_seq_sleep(uint8_t odr)
{
	status = bme68x_get_conf(&conf, &bme6);

	if (status == BME68X_OK)
	{
		conf.odr = odr;

		status = bme68x_set_conf(&conf, &bme6);
	}
}

/**
 * @brief Function to set the heater profile for Forced mode
 */
void bme68xlib_set_heater_prof_p2(uint16_t temp, uint16_t dur)
{
	heatrConf.enable = BME68X_ENABLE;
	heatrConf.heatr_temp = temp;
	heatrConf.heatr_dur = dur;

	status = bme68x_set_heatr_conf(BME68X_FORCED_MODE, &heatrConf, &bme6);
}

/**
 * @brief Function to set the heater profile for Sequential mode
 */
void bme68xlib_set_heater_prof_p3(uint16_t *temp, uint16_t *dur, uint8_t profile_len)
{
	heatrConf.enable = BME68X_ENABLE;
	heatrConf.heatr_temp_prof = temp;
	heatrConf.heatr_dur_prof = dur;
	heatrConf.profile_len = profile_len;

	status = bme68x_set_heatr_conf(BME68X_SEQUENTIAL_MODE, &heatrConf, &bme6);

}

/**
 * @brief Function to set the heater profile for Parallel mode
 */
void bme68xlib_set_heater_prof_p4(uint16_t *temp, uint16_t *mul, uint16_t shared_heatr_dur, uint8_t profile_len)
{
	heatrConf.enable = BME68X_ENABLE;
	heatrConf.heatr_temp_prof = temp;
	heatrConf.heatr_dur_prof = mul;
	heatrConf.shared_heatr_dur = shared_heatr_dur;
	heatrConf.profile_len = profile_len;

	status = bme68x_set_heatr_conf(BME68X_PARALLEL_MODE, &heatrConf, &bme6);
}

/**
 * @brief Function to fetch data from the sensor into the local buffer
 */
uint8_t bme68xlib_fetch_data(void)
{
	n_fields = 0;
	status = bme68x_get_data(last_op_mode, sensorData, &n_fields, &bme6);
	i_fields = 0;

	return n_fields;
}

/**
 * @brief Function to get a single data field
 */
uint8_t bme68xlib_get_data(bme68x_data* data)
{
	if (last_op_mode == BME68X_FORCED_MODE)
	{
		*data = sensorData[0];
	} else
	{
		if (n_fields)
		{
			/* i_fields spans from 0-2 while n_fields spans from
			 * 0-3, where 0 means that there is no new data
			 */
			*data = sensorData[i_fields];
			i_fields++;

			/* Limit reading continuously to the last fields read */
			if (i_fields >= n_fields)
			{
				i_fields = n_fields - 1;
				return 0;
			}

			/* Indicate if there is something left to read */
			return n_fields - i_fields;
		}
	}

	return 0;
}

/**
 * @brief Function to get whole sensor data
 */
bme68x_data* bme68xlib_get_all_data(void)
{
	return sensorData;
}

/**
 * @brief Function to get the BME68x heater configuration
 */
const bme68x_heatr_conf* bme68xlib_get_heater_configuration(void)
{
	return &heatrConf;
}

/**
 * @brief Function to retrieve the sensor's unique ID
 */
uint32_t bme68xlib_get_unique_id(void)
{
    uint8_t id_regs[4];
    uint32_t uid;
    bme68xlib_read_reg(BME68X_REG_UNIQUE_ID, id_regs, 4);

    uint32_t id1 = ((uint32_t) id_regs[3] + ((uint32_t) id_regs[2] << 8)) & 0x7fff;
    uid = (id1 << 16) + (((uint32_t) id_regs[1]) << 8) + (uint32_t) id_regs[0];

    return uid;
}

/**
 * @brief Function to get the error code of the interface functions
 */
BME68X_INTF_RET_TYPE bme68xlib_intf_error(void)
{
	return bme6.intf_rslt;
}

/**
 * @brief Function to check if an error / warning has occurred
 */
int8_t bme68xlib_check_status(void)
{
	if (status < BME68X_OK)
	{
		return BME68X_ERROR;
	}
	else if(status > BME68X_OK)
	{
		return BME68X_WARNING;
	}
	else
	{
		return BME68X_OK;
	}
}

/**
 * @brief Function to get a brief text description of the error
 */
const char* bme68xlib_status_string(void)
{
	const char* ret = "";
	switch (status)
	{
	case BME68X_OK:
		/* Don't return a text for OK */
		break;
	case BME68X_E_NULL_PTR:
		ret = "Null pointer";
		break;
	case BME68X_E_COM_FAIL:
		ret = "Communication failure";
		break;
	case BME68X_E_DEV_NOT_FOUND:
		ret = "Sensor not found";
		break;
	case BME68X_E_INVALID_LENGTH:
		ret = "Invalid length";
		break;
	case BME68X_W_DEFINE_OP_MODE:
		ret = "Set the operation mode";
		break;
	case BME68X_W_NO_NEW_DATA:
		ret = "No new data";
		break;
	case BME68X_W_DEFINE_SHD_HEATR_DUR:
		ret = "Set the shared heater duration";
		break;
	default:
		ret = "Undefined error code";
	}

	return ret;
}

int8_t bme68xlib_get_status(void)
{
	return status;
}

#define NOP() asm volatile ("nop")

void IRAM_ATTR delay_microseconds(uint32_t us)
{
  uint64_t m = (uint64_t)esp_timer_get_time();
  if(us){
      uint64_t e = (m + us);
      if(m > e){ //overflow
          while((uint64_t)esp_timer_get_time() > e){
              NOP();
          }
      }
      while((uint64_t)esp_timer_get_time() < e){
          NOP();
      }
  }
}

/**
 * @brief Function that implements the default microsecond delay callback
 */
void bme68x_delay_us(uint32_t periodUs, void *intfPtr) {
    (void) intfPtr;
    delay_microseconds(periodUs);
}

/**
 * @brief Function that implements the default I2C write transaction
 */
int8_t bme68x_i2c_write(uint8_t reg_addr, const uint8_t *reg_data,
        uint32_t length, void *intfPtr) 
{
    (void)intfPtr; // Unused parameter
    esp_err_t _error;
    uint8_t write_buf[length+1];
    write_buf[0] = reg_addr;
    for (uint16_t i = 0; i < length; i++)
    {
        write_buf[i+1] = reg_data[i];
    }
  
	_error = ezlopi_i2c_master_write_to_device(&g_bme68x_i2c_master_conf, write_buf, length+1);

	if (_error != ESP_OK)
	{
		return -1;
	}
    return (int8_t)_error;
}

/**
 * @brief Function that implements the default I2C read transaction
 */ 
int8_t bme68x_i2c_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t length,
        void *intfPtr) {
   	esp_err_t _error;

	_error = ezlopi_i2c_master_write_to_device(&g_bme68x_i2c_master_conf, &reg_addr, 1);
	_error = ezlopi_i2c_master_read_from_device(&g_bme68x_i2c_master_conf, reg_data, length);

	if (_error != ESP_OK)
	{
		return -1;
	}

    return _error;
}