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

#include "LTR303.h"

s_ezlopi_i2c_master_t g_ltr303_i2c_master_conf;

byte _error;

boolean ltr303_begin(s_ezlopi_i2c_master_t* bme68x_i2c_master_conf) {

	memcpy(&g_ltr303_i2c_master_conf, bme68x_i2c_master_conf, sizeof(s_ezlopi_i2c_master_t));
	if(ezlopi_i2c_master_init(&g_ltr303_i2c_master_conf) != ESP_OK)
	{
		return false;
	}
	
	return(true);
}

boolean ltr303_set_powerup(void) {
	// Turn on LTR303, begin integrations
	// Returns true (1) if successful, false (0) if there was an I2C error
	// (Also see getError() below)

	// Write 0x01 (reset = 0 & mode = 1) to command byte (power on)
	return(ltr303_write_byte(LTR303_CONTR,0x01));
}

boolean ltr303_set_powerdown(void) {
	// Turn off LTR303
	// Returns true (1) if successful, false (0) if there was an I2C error
	// (Also see getError() below)

	// Clear command byte (reset = 0 & mode = 0)(power off)
	return(ltr303_write_byte(LTR303_CONTR,0x00));
}

boolean ltr303_set_control(byte gain, boolean reset, boolean mode) {
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
	
	byte control = 0x00;
	
	// sanity check for gain
	if (gain > 3 && gain < 6) {
		gain = 0x00;
	}
	else if(gain >= 7) {
		gain = 0x00;
	}
	
	// control byte logic
	control |= gain << 2;
	if(reset) {
		control |= 0x02;
	}
	
	if(mode) {
		control |= 0x01;
	}
	
	return(ltr303_write_byte(LTR303_CONTR,control));
}			
			
boolean ltr303_get_control(byte* gain, boolean* reset, boolean* mode) {
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
	
	byte control;
	
	// Reading the control byte
	if(ltr303_read_byte(LTR303_CONTR, &control)) {
		// Extract gain
		*gain = (control & 0x1C) >> 2;
		
		// Extract reset
		*reset = (control & 0x02) ? true : false; 
		
		// Extract mode
		*mode = (control & 0x01) ? true : false;
		
		// return if successful
		return(true);
	}
	return(false);
}

boolean ltr303_set_measurement_rate(byte integration_time, byte measurement_rate) {
	// Sets the integration time and measurement rate of the sensor
	// integration_time is the measurement time for each ALs cycle
	// measurement_rate is the interval between DATA_REGISTERS update
	// measurement_rate must be set to be equal or greater than integration_time
	// Default value is 0x03
	// If integration_time = 0, integration_time will be 100ms (default)
	// If integration_time = 1, integration_time will be 50ms
	// If integration_time = 2, integration_time will be 200ms
	// If integration_time = 3, integration_time will be 400ms
	// If integration_time = 4, integration_time will be 150ms
	// If integration_time = 5, integration_time will be 250ms
	// If integration_time = 6, integration_time will be 300ms
	// If integration_time = 7, integration_time will be 350ms
	//------------------------------------------------------
	// If measurement_rate = 0, measurement_rate will be 50ms
	// If measurement_rate = 1, measurement_rate will be 100ms
	// If measurement_rate = 2, measurement_rate will be 200ms
	// If measurement_rate = 3, measurement_rate will be 500ms (default)
	// If measurement_rate = 4, measurement_rate will be 1000ms
	// If measurement_rate = 5, measurement_rate will be 2000ms
	// If measurement_rate = 6, measurement_rate will be 2000ms
	// If measurement_rate = 7, measurement_rate will be 2000ms
	// Returns true (1) if successful, false (0) if there was an I2C error
	// (Also see getError() below)
	
	byte measurement = 0x00;
	
	// Perform sanity checks
	if(integration_time >= 0x07) {
		integration_time = 0x00;
	}
	
	if(measurement_rate >= 0x07) {
		measurement_rate = 0x00;
	}
	
	measurement |= integration_time << 3;
	measurement |= measurement_rate;
	
	return(ltr303_write_byte(LTR303_MEAS_RATE, measurement));
}

boolean ltr303_get_measurement_rate(byte* integration_time, byte* measurement_rate) {
	// Gets the value of Measurement Rate register
	// Default value is 0x03
	// If integration_time = 0, integration_time will be 100ms (default)
	// If integration_time = 1, integration_time will be 50ms
	// If integration_time = 2, integration_time will be 200ms
	// If integration_time = 3, integration_time will be 400ms
	// If integration_time = 4, integration_time will be 150ms
	// If integration_time = 5, integration_time will be 250ms
	// If integration_time = 6, integration_time will be 300ms
	// If integration_time = 7, integration_time will be 350ms
	//------------------------------------------------------
	// If measurement_rate = 0, measurement_rate will be 50ms
	// If measurement_rate = 1, measurement_rate will be 100ms
	// If measurement_rate = 2, measurement_rate will be 200ms
	// If measurement_rate = 3, measurement_rate will be 500ms (default)
	// If measurement_rate = 4, measurement_rate will be 1000ms
	// If measurement_rate = 5, measurement_rate will be 2000ms
	// If measurement_rate = 6, measurement_rate will be 2000ms
	// If measurement_rate = 7, measurement_rate will be 2000ms
	// Returns true (1) if successful, false (0) if there was an I2C error
	// (Also see getError() below)
			
	byte measurement = 0x00;
	
	// Reading the measurement byte
	if(ltr303_read_byte(LTR303_MEAS_RATE, &measurement)) {
		// Extract integration Time
		*integration_time = (measurement & 0x38) >> 3;
		
		// Extract measurement Rate
		*measurement_rate = measurement & 0x07; 
		
		// return if successful
		return(true);
	}
	return(false);		
}

boolean ltr303_get_part_id(byte* part_id) {
	// Gets the part number ID and revision ID of the chip
	// Default value is 0x0A
	// part number ID = 0x0A (default)
	// Revision ID = 0x00
	// Returns true (1) if successful, false (0) if there was an I2C error
	// (Also see getError() below)
			
	return(ltr303_read_byte(LTR303_PART_ID, part_id));
}

boolean ltr303_get_manufac_id(byte* manufac_id) {
	// Gets the Manufacturers ID
	// Default value is 0x05
	// Returns true (1) if successful, false (0) if there was an I2C error
	// (Also see getError() below)
	
	return(ltr303_read_byte(LTR303_MANUFAC_ID, manufac_id));
}

boolean ltr303_get_data(unsigned int* ch0, unsigned int* ch1) {
	// Gets the 16-bit channel 0 and channel 1 data
	// Default value of both channels is 0x00
	// Returns true (1) if successful, false (0) if there was an I2C error
	// (Also see getError() below)
	byte status=0;
	status = ltr303_read_uint(LTR303_DATA_CH1_0,ch1);
	status &= ltr303_read_uint(LTR303_DATA_CH0_0,ch0);
	return(status);
}

boolean ltr303_get_status(boolean* valid, byte* gain, boolean* intr_status, boolean* data_status) {
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
	// If intr_status = false(0), INTR in inactive (default)
	// If intr_status = true(1), INTR in active
	//---------------------------------------------
	// If data_status = false(0), OLD data (already read) (default)
	// If data_status = true(1), NEW data
	// Returns true (1) if successful, false (0) if there was an I2C error
	// (Also see getError() below)
	
	byte status = 0x00;
	
	// Reading the status byte
	if(ltr303_read_byte(LTR303_STATUS, &status)) {
		// Extract validity
		*valid = (status & 0x80) ? true : false;
	
		// Extract gain
		*gain = (status & 0x70) >> 4;
	
		// Extract interrupt status
		*intr_status = (status & 0x08) ? true : false;
	
		// Extract data status
		*data_status = (status & 0x04) ? true : false;
		
		// return if successful
		return(true);
	}
	return(false);
}

boolean ltr303_set_interrupt_control(boolean intr_mode, boolean polarity) {
	// Sets up interrupt operations
	// Default value is 0x08
	// If intr_mode = false(0), INT pin is inactive (default)
	// If intr_mode = true(1), INT pin is active
	//------------------------------------------------------
	// If polarity = false(0), INT pin is active at logic 0 (default)
	// If polarity = true(1), INT pin is active at logic 1
	//------------------------------------------------------
	// Returns true (1) if successful, false (0) if there was an I2C error
	// (Also see getError() below)
	
	byte intr_control = 0x00;
	
	intr_control |= polarity << 2;
	intr_control |= intr_mode << 1;

	return(ltr303_write_byte(LTR303_INTERRUPT, intr_control));
}

boolean ltr303_get_interrupt_control(boolean* polarity, boolean* intr_mode) {
	// Sets up interrupt operations
	// Default value is 0x08
	// If polarity = false(0), INT pin is active at logic 0 (default)
	// If polarity = true(1), INT pin is active at logic 1
	//------------------------------------------------------
	// If intr_mode = false(0), INT pin is inactive (default)
	// If intr_mode = true(1), INT pin is active
	//------------------------------------------------------
	// Returns true (1) if successful, false (0) if there was an I2C error
	// (Also see getError() below)
	
	byte intr_control = 0x00;
	
	// Reading the interrupt byte
	if(ltr303_read_byte(LTR303_INTERRUPT, &intr_control)) {
		// Extract polarity
		*polarity = (intr_control & 0x04) ? true : false;
	
		// Extract mode
		*intr_mode = (intr_control & 0x02) ? true : false;
	
		// return if successful
		printf("Interrupt Mode = %d, Interrupt Polarity %d\n",*intr_mode, *polarity);
		return(true);
	}
	return(false);
}

boolean ltr303_set_threshold(unsigned int upper_limit, unsigned int lower_limit) {
	// Sets the upper limit and lower limit of the threshold
	// Default value of upper threshold is 0xFF and lower threshold is 0x00
	// Both the threshold are 16-bit integer values
	// Returns true (1) if successful, false (0) if there was an I2C error
	// (Also see getError() below)
	
	return(ltr303_write_uint(LTR303_THRES_UP_0,upper_limit) && ltr303_write_uint(LTR303_THRES_LOW_0,lower_limit));
}

boolean ltr303_get_threshold(unsigned int* upper_limit, unsigned int* lower_limit) {
	// Gets the upper limit and lower limit of the threshold
	// Default value of upper threshold is 0xFF and lower threshold is 0x00
	// Both the threshold are 16-bit integer values
	// Returns true (1) if successful, false (0) if there was an I2C error
	// (Also see getError() below)
			
	return(ltr303_read_uint(LTR303_THRES_UP_0,upper_limit) && ltr303_read_uint(LTR303_THRES_LOW_0,lower_limit));		
}

boolean ltr303_set_intr_persist(byte persist) {
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
	
	// sanity check
	if(persist >= 15) {
		persist = 0x00;
	}
			
	return(ltr303_write_byte(LTR303_INTR_PERS,persist));
}

boolean ltr303_get_intr_persist(byte* persist) {
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
	
	return(ltr303_read_byte(LTR303_INTR_PERS,persist));
}

// Get the right lux algorithm
boolean ltr303_get_lux(byte gain, byte integration_time, unsigned int ch0, unsigned int ch1, double* lux) {
	// Convert raw data to lux
	// gain: 0 (1X) or 7 (96X), see getControl()
	// integration_time: integration time in ms, from getmeasurement_rate()
	// ch0, ch1: results from getData()
	// lux will be set to resulting lux calculation
	// returns true (1) if calculation was successful
	// returns false (0) AND lux = 0.0 IF EITHER SENSOR WAS SATURATED (0XFFFF)

	double ratio, ALS_INT;
	int ALS_GAIN;
	
	// Determine if either sensor saturated (0xFFFF)
	// If so, abandon ship (calculation will not be accurate)
	if ((ch0 == 0xFFFF) || (ch1 == 0xFFFF)) {
		*lux = 0.0;
		return(false);
	}

	// We will need the ratio for subsequent calculations
	ratio = ((double)ch1) / (((double)ch0) + ((double)ch1));


	// Gain can take any value from 0-7, except 4 & 5
	// If gain = 4, invalid
	// If gain = 5, invalid
	switch(gain){
		case 0:			   // If gain = 0, device is set to 1X gain (default)
			ALS_GAIN = 1;
			break;
		case 1:			   // If gain = 1, device is set to 2X gain
			ALS_GAIN = 2;
			break;
		case 2:			  // If gain = 2, device is set to 4X gain	 
			ALS_GAIN = 4;
			break;
		case 3:			  // If gain = 3, device is set to 8X gain	  
			ALS_GAIN = 8;
			break;
		case 6:		     // If gain = 6, device is set to 48X gain
			ALS_GAIN = 48;
			break;	
		case 7:			  // If gain = 7, device is set to 96X gain  
			ALS_GAIN = 96;
			break;
		default:		  // If gain = 0, device is set to 1X gain (default)	 	 
			ALS_GAIN = 1;
			break;
	}


	switch(integration_time){
		case 0:				 // If integration_time = 0, integration_time will be 100ms (default)
			ALS_INT = 1;
			break;
		case 1:			 	 // If integration_time = 1, integration_time will be 50ms
			ALS_INT = 0.5;
			break;
		case 2:				 // If integration_time = 2, integration_time will be 200ms
			ALS_INT = 2;
			break;
		case 3:				  // If integration_time = 3, integration_time will be 400ms
			ALS_INT = 4;
			break;
		case 4:				  // If integration_time = 4, integration_time will be 150ms
			ALS_INT = 1.5;
			break;
		case 5:				  // If integration_time = 5, integration_time will be 250ms
			ALS_INT = 2.5;
			break;
		case 6:				  // If integration_time = 6, integration_time will be 300ms
			ALS_INT = 3;
			break;	
		case 7:				  // If integration_time = 7, integration_time will be 350ms
			ALS_INT = 3.5;
			break;
		default:		 	 // If integration_time = 0, integration_time will be 100ms (default)
			ALS_INT = 1;
			break;
	}

	// Determine lux per datasheet equations:
	if (ratio < 0.45) {
		*lux = ((1.7743 * ch0) + (1.1059 * ch1))/ALS_GAIN/ALS_INT;
		return(true);
	}

	else if ((ratio < 0.64) && (ratio >= 0.45)){
		*lux = ((4.2785 * ch0) - (1.9548 * ch1))/ALS_GAIN/ALS_INT;
		return(true);
	}

	else if ((ratio < 0.85) && (ratio >= 0.64)){
		*lux = ((0.5926 * ch0) + (0.1185 * ch1))/ALS_GAIN/ALS_INT;
		return(true);
	}

	// if (ratio >= 0.85)
	else {
		*lux = 0.0;
		return(true);
	}	
}

byte ltr303_get_error(void) {
	// If any library command fails, you can retrieve an extended
	// error code using this command. Errors are from the wire library: 
	// 0 = Success
	// 1 = Data too long to fit in transmit buffer
	// 2 = Received NACK on transmit of address
	// 3 = Received NACK on transmit of data
	// 4 = Other error

	return(_error);
}

// Private functions:

boolean ltr303_read_byte(byte address, byte* value) {
	// Reads a byte from a LTR303 address
	// Address: LTR303 address (0 to 15)
	// Value will be set to stored byte
	// Returns true (1) if successful, false (0) if there was an I2C error
	// (Also see getError() above)
	// Check if sensor present for read
	uint8_t data;
	_error = ezlopi_i2c_master_write_to_device(&g_ltr303_i2c_master_conf, &address, 1);
	_error = ezlopi_i2c_master_read_from_device(&g_ltr303_i2c_master_conf, &data, 1);

	if (_error == ESP_OK)
	{
		*value = data;
		return(true);
	}
	return(false);
}

boolean ltr303_write_byte(byte address, byte value) {
	// Write a byte to a LTR303 address
	// Address: LTR303 address (0 to 15)
	// Value: byte to write to address
	// Returns true (1) if successful, false (0) if there was an I2C error
	// (Also see getError() above)

	uint8_t write_buf[2] = { address, value};
	_error = ezlopi_i2c_master_write_to_device(&g_ltr303_i2c_master_conf, write_buf, 2);

	if (_error == ESP_OK)
	{
		return(true);
	}
	return(false);
}

boolean ltr303_read_uint(byte address, unsigned int* value) {
	// Reads an unsigned integer (16 bits) from a LTR303 address (low byte first)
	// Address: LTR303 address (0 to 15), low byte first
	// Value will be set to stored unsigned integer
	// Returns true (1) if successful, false (0) if there was an I2C error
	// (Also see getError() above)
	uint16_t data;
	_error = ezlopi_i2c_master_write_to_device(&g_ltr303_i2c_master_conf, &address, 1);
	_error = ezlopi_i2c_master_read_from_device(&g_ltr303_i2c_master_conf, (uint8_t*)&data, 2);

	if (_error == ESP_OK)
	{
		*value = data;
		return(true);
	}	
	return(false);
}

boolean ltr303_write_uint(byte address, unsigned int value) {
	// Write an unsigned integer (16 bits) to a LTR303 address (low byte first)
	// Address: LTR303 address (0 to 15), low byte first
	// Value: unsigned int to write to address
	// Returns true (1) if successful, false (0) if there was an I2C error
	// (Also see getError() above)

	// Split int into lower and upper bytes, write each byte
	if (ltr303_write_byte(address,lowByte(value)) 
		&& ltr303_write_byte(address + 1,highByte(value)))
		return(true);

	return(false);
}

void ltr303_print_error(byte error) {
  // If there's an I2C error, this function will
  // print out an explanation.

  printf("I2C error: %d,", error);
  
  switch(error) {
    case 0:
      printf("success");
      break;
    case 1:
      printf("data too long for transmit buffer\n");
      break;
    case 2:
      printf("received NACK on address (disconnected?)\n");
      break;
    case 3:
      printf("received NACK on data\n");
      break;
    case 4:
      printf("other error\n");
      break;
    default:
      printf("unknown error\n");
  }
}
