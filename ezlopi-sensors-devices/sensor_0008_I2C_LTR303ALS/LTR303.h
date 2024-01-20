/*
	LTR303 illumination sensor library for Arduino
	Lovelesh, thingTronics
	
The MIT License (MIT)

Copyright (c) 2015 thingTronics Limited

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

version 0.1
*/

#ifndef LTR303_h
#define LTR303_h

// Edited
#include <stdio.h>
#include <math.h>
#include "esp_log.h"
#include "ezlopi_i2c_master.h"
#include <string.h>

typedef uint8_t byte;
typedef bool boolean;
#define lowByte(word) ((uint8_t)(word & 0xFF))
#define highByte(word) ((uint8_t)((word >> 8) & 0xFF))

#define I2C_MASTER_FREQ_HZ 400000
// Edited

#define LTR303_ADDR   0x29 // default address

// LTR303 register addresses
#define LTR303_CONTR         0x80
#define LTR303_MEAS_RATE     0x85
#define LTR303_PART_ID       0x86
#define LTR303_MANUFAC_ID    0x87
#define LTR303_DATA_CH1_0    0x88
#define LTR303_DATA_CH1_1    0x89
#define LTR303_DATA_CH0_0    0x8A
#define LTR303_DATA_CH0_1    0x8B
#define LTR303_STATUS		 0x8C
#define LTR303_INTERRUPT     0x8F
#define LTR303_THRES_UP_0    0x97
#define LTR303_THRES_UP_1	 0x98
#define LTR303_THRES_LOW_0   0x99
#define LTR303_THRES_LOW_1   0x9A
#define LTR303_INTR_PERS     0x9E

typedef enum {
  LTR3XX_GAIN_1 = 0,
  LTR3XX_GAIN_2 = 1,
  LTR3XX_GAIN_4 = 2,
  LTR3XX_GAIN_8 = 3,
  // 4 & 5 unused!
  LTR3XX_GAIN_48 = 6,
  LTR3XX_GAIN_96 = 7,
} ltr329_gain_t;

/*!    @brief Integration times, in milliseconds */
typedef enum {
  LTR3XX_INTEGTIME_100,
  LTR3XX_INTEGTIME_50,
  LTR3XX_INTEGTIME_200,
  LTR3XX_INTEGTIME_400,
  LTR3XX_INTEGTIME_150,
  LTR3XX_INTEGTIME_250,
  LTR3XX_INTEGTIME_300,
  LTR3XX_INTEGTIME_350,
} ltr329_integrationtime_t;

/*!    @brief Measurement rates, in milliseconds */
typedef enum {
  LTR3XX_MEASRATE_50,
  LTR3XX_MEASRATE_100,
  LTR3XX_MEASRATE_200,
  LTR3XX_MEASRATE_500,
  LTR3XX_MEASRATE_1000,
  LTR3XX_MEASRATE_2000,
} ltr329_measurerate_t;
			
boolean ltr303_begin(s_ezlopi_i2c_master_t* bme68x_i2c_master_conf);
	// Initialize LTR303 library with default address (0x29)
	// Always returns true

boolean ltr303_set_powerup(void);
	// Turn on LTR303, begin integration
	// Returns true (1) if successful, false (0) if there was an I2C error
	// (Also see getError() below)

boolean ltr303_set_powerdown(void);
	// Turn off LTR303
	// Returns true (1) if successful, false (0) if there was an I2C error
	// (Also see getError() below)

boolean ltr303_set_control(byte gain, boolean reset, boolean mode);
	// Sets the gain, SW reset and mode of LTR303
	// Default value is 0x00
	// If gain = 0, device is set to 1X gain (default)
	// If gain = 1, device is set to 2X gain
	// If gain = 2, device is set to 4X gain
	// If gain = 3, device is set to 8X gain
	// If gain = 4, invalid
	// If gain = 5, invalid
	// If gain = 6, device is set to 48X gain
	// If gain = 7, device is set to 96X gain
	//----------------------------------------
	// If reset = false(0), initial start-up procedure not started (default)
	// If reset = true(1), initial start-up procedure started
	//----------------------------------------
	// If mode = false(0), stand-by mode (default)
	// If mode = true(1), active mode
	
boolean ltr303_get_control(byte* gain, boolean* reset, boolean* mode);
	// Gets the control register values
	// Default value is 0x00
	// If gain = 0, device is set to 1X gain (default)
	// If gain = 1, device is set to 2X gain
	// If gain = 2, device is set to 4X gain
	// If gain = 3, device is set to 8X gain
	// If gain = 4, invalid
	// If gain = 5, invalid
	// If gain = 6, device is set to 48X gain
	// If gain = 7, device is set to 96X gain
	//----------------------------------------
	// If reset = false(0), initial start-up procedure not started (default)
	// If reset = true(1), initial start-up procedure started
	//----------------------------------------
	// If mode = false(0), stand-by mode (default)
	// If mode = true(1), active mode
	// Returns true (1) if successful, false (0) if there was an I2C error
	// (Also see getError() below)
	
	
boolean ltr303_set_measurement_rate(byte integration_time, byte measurement_rate);
	// Sets the integration time and measurement rate of the sensor
	// integrationTime is the measurement time for each ALs cycle
	// measurementRate is the interval between DATA_REGISTERS update
	// measurementRate must be set to be equal or greater than integrationTime
	// Default value is 0x03
	// If integrationTime = 0, integrationTime will be 100ms (default)
	// If integrationTime = 1, integrationTime will be 50ms
	// If integrationTime = 2, integrationTime will be 200ms
	// If integrationTime = 3, integrationTime will be 400ms
	// If integrationTime = 4, integrationTime will be 150ms
	// If integrationTime = 5, integrationTime will be 250ms
	// If integrationTime = 6, integrationTime will be 300ms
	// If integrationTime = 7, integrationTime will be 350ms
	//------------------------------------------------------
	// If measurementRate = 0, measurementRate will be 50ms
	// If measurementRate = 1, measurementRate will be 100ms
	// If measurementRate = 2, measurementRate will be 200ms
	// If measurementRate = 3, measurementRate will be 500ms (default)
	// If measurementRate = 4, measurementRate will be 1000ms
	// If measurementRate = 5, measurementRate will be 2000ms
	// If measurementRate = 6, measurementRate will be 2000ms
	// If measurementRate = 7, measurementRate will be 2000ms
	// Returns true (1) if successful, false (0) if there was an I2C error
	// (Also see getError() below)
	
boolean ltr303_get_measurement_rate(byte* integration_time, byte* measurement_rate);
	// Gets the value of Measurement Rate register
	// Default value is 0x03
	// If integrationTime = 0, integrationTime will be 100ms (default)
	// If integrationTime = 1, integrationTime will be 50ms
	// If integrationTime = 2, integrationTime will be 200ms
	// If integrationTime = 3, integrationTime will be 400ms
	// If integrationTime = 4, integrationTime will be 150ms
	// If integrationTime = 5, integrationTime will be 250ms
	// If integrationTime = 6, integrationTime will be 300ms
	// If integrationTime = 7, integrationTime will be 350ms
	//------------------------------------------------------
	// If measurementRate = 0, measurementRate will be 50ms
	// If measurementRate = 1, measurementRate will be 100ms
	// If measurementRate = 2, measurementRate will be 200ms
	// If measurementRate = 3, measurementRate will be 500ms (default)
	// If measurementRate = 4, measurementRate will be 1000ms
	// If measurementRate = 5, measurementRate will be 2000ms
	// If measurementRate = 6, measurementRate will be 2000ms
	// If measurementRate = 7, measurementRate will be 2000ms
	// Returns true (1) if successful, false (0) if there was an I2C error
	// (Also see getError() below)

boolean ltr303_get_part_id(byte* part_id);
	// Gets the part number ID and revision ID of the chip
	// Default value is 0x0A
	// part number ID = 0x0A (default)
	// Revision ID = 0x00
	// Returns true (1) if successful, false (0) if there was an I2C error
	// (Also see getError() below)

boolean ltr303_get_manufac_id(byte* manufac_id);
	// Gets the Manufacturers ID
	// Default value is 0x05
	// Returns true (1) if successful, false (0) if there was an I2C error
	// (Also see getError() below)

boolean ltr303_get_data(unsigned int* ch0, unsigned int* ch1);
	// Gets the 16-bit channel 0 and channel 1 data
	// Default value of both channels is 0x00
	// Returns true (1) if successful, false (0) if there was an I2C error
	// (Also see getError() below)

boolean ltr303_get_status(boolean* valid, byte* gain, boolean* intr_status, boolean* data_status);
	// Gets the status information of LTR303
	// Default value is 0x00
	// If valid = false(0), Sensor data is valid (default)
	// If valid = true(1), Sensor data is invalid
	//--------------------------------------------
	// If gain = 0, device is set to 1X gain (default)
	// If gain = 1, device is set to 2X gain
	// If gain = 2, device is set to 4X gain
	// If gain = 3, device is set to 8X gain
	// If gain = 4, invalid
	// If gain = 5, invalid
	// If gain = 6, device is set to 48X gain
	// If gain = 7, device is set to 96X gain
	//---------------------------------------------
	// If intrStatus = false(0), INTR in inactive (default)
	// If intrStatus = true(1), INTR in active
	//---------------------------------------------
	// If dataStatus = false(0), OLD data (already read) (default)
	// If dataStatus = true(1), NEW data
	// Returns true (1) if successful, false (0) if there was an I2C error
	// (Also see getError() below)
	
boolean ltr303_set_interrupt_control(boolean intr_mode, boolean polarity);
	// Sets up interrupt operations
	// Default value is 0x08
	// If intrMode = false(0), INT pin is inactive (default)
	// If intrMode = true(1), INT pin is active
	//------------------------------------------------------
	// If polarity = false(0), INT pin is active at logic 0 (default)
	// If polarity = true(1), INT pin is active at logic 1
	//------------------------------------------------------
	// Returns true (1) if successful, false (0) if there was an I2C error
	// (Also see getError() below)
	
boolean ltr303_get_interrupt_control(boolean* polarity, boolean* intr_mode);
	// Sets up interrupt operations
	// Default value is 0x08
	// If polarity = false(0), INT pin is active at logic 0 (default)
	// If polarity = true(1), INT pin is active at logic 1
	//------------------------------------------------------
	// If intrMode = false(0), INT pin is inactive (default)
	// If intrMode = true(1), INT pin is active
	//------------------------------------------------------
	// Returns true (1) if successful, false (0) if there was an I2C error
	// (Also see getError() below)
	
boolean ltr303_set_threshold(unsigned int upper_limit, unsigned int lower_limit);
	// Sets the upper limit and lower limit of the threshold
	// Default value of upper threshold is 0xFF and lower threshold is 0x00
	// Both the threshold are 16-bit integer values
	// Returns true (1) if successful, false (0) if there was an I2C error
	// (Also see getError() below)
	
boolean ltr303_get_threshold(unsigned int* upper_limit, unsigned int* lower_limit);
	// Gets the upper limit and lower limit of the threshold
	// Default value of upper threshold is 0xFF and lower threshold is 0x00
	// Both the threshold are 16-bit integer values
	// Returns true (1) if successful, false (0) if there was an I2C error
	// (Also see getError() below)
	
boolean ltr303_set_intr_persist(byte persist);
	// Sets the interrupt persistance i.e. controls the N number of times the 
	// measurement data is outside the range defined by upper and lower threshold
	// Default value is 0x00
	// If persist = 0, every sensor value out of threshold range (default)
	// If persist = 1, every 2 consecutive value out of threshold range
	// If persist = 2, every 3 consecutive value out of threshold range
	// If persist = 3, every 4 consecutive value out of threshold range
	// If persist = 4, every 5 consecutive value out of threshold range
	// If persist = 5, every 6 consecutive value out of threshold range
	// If persist = 6, every 7 consecutive value out of threshold range
	// If persist = 7, every 8 consecutive value out of threshold range
	// If persist = 8, every 9 consecutive value out of threshold range
	// If persist = 9, every 10 consecutive value out of threshold range
	// If persist = 10, every 11 consecutive value out of threshold range
	// If persist = 11, every 12 consecutive value out of threshold range
	// If persist = 12, every 13 consecutive value out of threshold range
	// If persist = 13, every 14 consecutive value out of threshold range
	// If persist = 14, every 15 consecutive value out of threshold range
	// If persist = 15, every 16 consecutive value out of threshold range
	// Returns true (1) if successful, false (0) if there was an I2C error
	// (Also see getError() below)
	
boolean ltr303_get_intr_persist(byte* persist);
	// Gets the interrupt persistance i.e. controls the N number of times the measurement data is outside the range defined by upper and lower threshold
	// Default value is 0x00
	// If persist = 0, every sensor value out of threshold range (default)
	// If persist = 1, every 2 consecutive value out of threshold range
	// If persist = 2, every 3 consecutive value out of threshold range
	// If persist = 3, every 4 consecutive value out of threshold range
	// If persist = 4, every 5 consecutive value out of threshold range
	// If persist = 5, every 6 consecutive value out of threshold range
	// If persist = 6, every 7 consecutive value out of threshold range
	// If persist = 7, every 8 consecutive value out of threshold range
	// If persist = 8, every 9 consecutive value out of threshold range
	// If persist = 9, every 10 consecutive value out of threshold range
	// If persist = 10, every 11 consecutive value out of threshold range
	// If persist = 11, every 12 consecutive value out of threshold range
	// If persist = 12, every 13 consecutive value out of threshold range
	// If persist = 13, every 14 consecutive value out of threshold range
	// If persist = 14, every 15 consecutive value out of threshold range
	// If persist = 15, every 16 consecutive value out of threshold range
	// Returns true (1) if successful, false (0) if there was an I2C error
	// (Also see getError() below)

boolean ltr303_get_lux(byte gain, byte integration_time, unsigned int ch0, unsigned int ch1, double* lux);
	// Convert raw data to lux
	// gain: 0 (1X) or 7 (96X), see getControl()
	// integrationTime: integration time in ms, from getMeasurementRate()
	// CH0, CH1: results from getData()
	// lux will be set to resulting lux calculation
	// returns true (1) if calculation was successful
	// returns false (0) AND lux = 0.0 IF EITHER SENSOR WAS SATURATED (0XFFFF)

byte ltr303_get_error(void);
	// If any library command fails, you can retrieve an extended
	// error code using this command. Errors are from the wire library: 
	// 0 = Success
	// 1 = Data too long to fit in transmit buffer
	// 2 = Received NACK on transmit of address
	// 3 = Received NACK on transmit of data
	// 4 = Other error
	
void ltr303_print_error(byte error);
	// Print Errors


boolean ltr303_read_byte(byte address, byte* value);
	// Reads a byte from a LTR303 address
	// Address: LTR303 address (0 to 15)
	// Value will be set to stored byte
	// Returns true (1) if successful, false (0) if there was an I2C error
	// (Also see getError() above)

boolean ltr303_write_byte(byte address, byte value);
	// Write a byte to a LTR303 address
	// Address: LTR303 address (0 to 15)
	// Value: byte to write to address
	// Returns true (1) if successful, false (0) if there was an I2C error
	// (Also see getError() above)

boolean ltr303_read_uint(byte address, unsigned int* value);
	// Reads an unsigned integer (16 bits) from a LTR303 address (low byte first)
	// Address: LTR303 address (0 to 15), low byte first
	// Value will be set to stored unsigned integer
	// Returns true (1) if successful, false (0) if there was an I2C error
	// (Also see getError() above)

boolean ltr303_write_uint(byte address, unsigned int value);
	// Write an unsigned integer (16 bits) to a LTR303 address (low byte first)
	// Address: LTR303 address (0 to 15), low byte first
	// Value: unsigned int to write to address
	// Returns true (1) if successful, false (0) if there was an I2C error
	// (Also see getError() above)
#endif