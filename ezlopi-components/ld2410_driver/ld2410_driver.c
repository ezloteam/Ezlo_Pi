/*
 *	An Arduino library for the Hi-Link LD2410 24Ghz FMCW radar sensor.
 *
 *  This sensor is a Frequency Modulated Continuous Wave radar, which makes it good for presence detection and its sensitivity at different ranges to both static and moving targets can be configured.
 *
 *	The code in this library is based off the manufacturer datasheet and reading of this initial piece of work for ESPHome https://github.com/rain931215/ESPHome-LD2410.
 *
 *	https://github.com/ncmreynolds/ld2410
 *
 *	Released under LGPL-2.1 see https://github.com/ncmreynolds/ld2410/LICENSE for full license
 *
 */

#include "ld2410_driver.h"
#include "ezlopi_hal_uart.h"

s_ezlopi_uart_object_handle_t ezlo_ld2410_uart_handle = NULL;

uint8_t firmware_major_version = 0;	  // Reported major version
uint8_t firmware_minor_version = 0;	  // Reported minor version
uint32_t firmware_bugfix_version = 0; // Reported bugfix version (coded as hex)
uint8_t max_gate = 0;
uint8_t max_moving_gate = 0;
uint8_t max_stationary_gate = 0;
uint16_t sensor_idle_time = 0;
uint8_t motion_sensitivity[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
uint8_t stationary_sensitivity[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

/*	private	*/
static uint32_t radar_uart_timeout = 100;		   // How long to give up on receiving some useful data from the LD2410
static uint32_t radar_uart_last_packet_ = 0;	   // Time of the last packet from the radar
static uint32_t radar_uart_last_command_ = 0;	   // Time of the last command sent to the radar
static uint32_t radar_uart_command_timeout_ = 100; // Timeout for sending commands
static uint8_t latest_ack_ = 0;
static bool latest_command_success_ = false;
static uint8_t radar_data_frame_[LD2410_MAX_FRAME_LENGTH]; // Store the incoming data from the radar, to check it's in a valid format
static uint8_t radar_data_frame_position_ = 0;			   // Where in the frame we are currently writing
static bool frame_started_ = false;						   // Whether a frame is currently being read
static bool ack_frame_ = false;							   // Whether the incoming frame is LIKELY an ACK frame
// static bool waiting_for_ack_ = false;									//Whether a command has just been sent
static uint8_t target_type_ = 0;
static uint16_t moving_target_distance_ = 0;
static uint8_t moving_target_energy_ = 0;
static uint16_t stationary_target_distance_ = 0;
static uint8_t stationary_target_energy_ = 0;

static size_t ld2410_uart_write_byte(uint8_t byte);
static bool ld2410_read_frame_(uint8_t l_byte); // Try to read a frame from the UART
static bool ld2410_parse_data_frame_();			// Is the current data frame valid?
static bool ld2410_parse_command_frame_();		// Is the current command frame valid?
// static void ld2410_print_frame_();				// Print the frame for debugging
static void ld2410_send_command_preamble_();	// Commands have the same preamble
static void ld2410_send_command_postamble_();	// Commands have the same postamble
static bool ld2410_enter_configuration_mode_(); // Necessary before sending any command
static bool ld2410_leave_configuration_mode_(); // Will not read values without leaving command mode

#define DATA_BUF_LENGTH 23
static void ld2410_callback(uint8_t *buffer, uint32_t output_len, s_ezlopi_uart_object_handle_t uart_object_handle)
{
	// printf("[ ");
	for (size_t i = 0; i < output_len; i++)
	{
		// printf("0x%X ", buffer[i]);
		ld2410_read_frame_(buffer[i]);
	}
	// printf("]\n");
}

/*	Function definition	*/
bool ld2410_begin(bool wait_for_radar, s_ezlopi_uart_t uart_settings)
{
	ezlo_ld2410_uart_handle = ezlopi_uart_init(uart_settings.baudrate, uart_settings.tx, uart_settings.rx, ld2410_callback, NULL);

#ifdef LD2410_DEBUG_INITIALIZATION
	// printf("ld2410 started");
#endif
	if (wait_for_radar)
	{
#ifdef LD2410_DEBUG_INITIALIZATION
		// printf("\nLD2410 firmware: ");
#endif
		if (ld2410_request_firmware_version())
		{
#ifdef LD2410_DEBUG_INITIALIZATION
			printf(" v%d.%d.%d\n", firmware_major_version, firmware_minor_version, firmware_bugfix_version);
#endif
			return true;
		}
		else
		{
#ifdef LD2410_DEBUG_INITIALIZATION
			// printf("no response\n");
#endif
		}
	}
	else
	{
#ifdef LD2410_DEBUG_INITIALIZATION
		// printf("\nLD2410 library configured");
#endif
		return true;
	}
	return false;
}

bool ld2410_is_connected()
{
	if (millis() - radar_uart_last_packet_ < radar_uart_timeout) // Use the last reading
	{
		return true;
	}
	return false;
}

bool ld2410_presence_detected()
{
	return target_type_ != 0;
}

bool ld2410_stationary_target_detected()
{
	if ((target_type_ & 0x02) && stationary_target_distance_ > 0 && stationary_target_energy_ > 0)
	{
		return true;
	}
	return false;
}

uint16_t ld2410_stationary_target_distance()
{
	// if(stationary_target_energy_ > 0)
	{
		return stationary_target_distance_;
	}
	// return 0;
}

uint8_t ld2410_stationary_target_energy()
{
	// if(stationary_target_distance_ > 0)
	{
		return stationary_target_energy_;
	}
	// return 0;
}

bool ld2410_moving_target_detected()
{
	if ((target_type_ & 0x01) && moving_target_distance_ > 0 && moving_target_energy_ > 0)
	{
		return true;
	}
	return false;
}

uint16_t ld2410_moving_target_distance()
{
	// if(moving_target_energy_ > 0)
	{
		return moving_target_distance_;
	}
	// return 0;
}

uint8_t ld2410_moving_target_energy()
{
	// if(moving_target_distance_ > 0)
	{
		return moving_target_energy_;
	}
	// return 0;
}

static bool ld2410_read_frame_(uint8_t l_byte)
{
	if (frame_started_ == false)
	{
		uint8_t byte_read_ = l_byte;
		if (byte_read_ == 0xF4)
		{
#ifdef LD2410_DEBUG_DATA
			printf("\nRcvd : 00 ");
#endif
			radar_data_frame_[radar_data_frame_position_++] = byte_read_;
			frame_started_ = true;
			ack_frame_ = false;
		}
		else if (byte_read_ == 0xFD)
		{
#ifdef LD2410_DEBUG_COMMANDS
			printf("\nRcvd : 00 ");
#endif
			radar_data_frame_[radar_data_frame_position_++] = byte_read_;
			frame_started_ = true;
			ack_frame_ = true;
		}
	}
	else
	{
		if (radar_data_frame_position_ < LD2410_MAX_FRAME_LENGTH)
		{
#ifdef LD2410_DEBUG_DATA
			if (ack_frame_ == false)
			{
				if (radar_data_frame_position_ < 0x10)
				{
					printf("0");
				}
				printf("%x ", radar_data_frame_position_);
			}
#endif
#ifdef LD2410_DEBUG_COMMANDS
			if (ack_frame_ == true)
			{
				if (radar_data_frame_position_ < 0x10)
				{
					printf("0");
				}
				printf("%x ", radar_data_frame_position_);
			}
#endif
			radar_data_frame_[radar_data_frame_position_++] = l_byte;
			if (radar_data_frame_position_ > 7) // Can check for start and end
			{
				if (radar_data_frame_[0] == 0xF4 && // Data frame end state
					radar_data_frame_[1] == 0xF3 &&
					radar_data_frame_[2] == 0xF2 &&
					radar_data_frame_[3] == 0xF1 &&
					radar_data_frame_[radar_data_frame_position_ - 4] == 0xF8 &&
					radar_data_frame_[radar_data_frame_position_ - 3] == 0xF7 &&
					radar_data_frame_[radar_data_frame_position_ - 2] == 0xF6 &&
					radar_data_frame_[radar_data_frame_position_ - 1] == 0xF5)
				{
					if (ld2410_parse_data_frame_())
					{
#ifdef LD2410_DEBUG_DATA
						printf("parsed data OK\n");
#endif
						frame_started_ = false;
						radar_data_frame_position_ = 0;
						return true;
					}
					else
					{
#ifdef LD2410_DEBUG_DATA
						printf("failed to parse data\n");
#endif
						frame_started_ = false;
						radar_data_frame_position_ = 0;
					}
				}
				else if (radar_data_frame_[0] == 0xFD && // Command frame end state
						 radar_data_frame_[1] == 0xFC &&
						 radar_data_frame_[2] == 0xFB &&
						 radar_data_frame_[3] == 0xFA &&
						 radar_data_frame_[radar_data_frame_position_ - 4] == 0x04 &&
						 radar_data_frame_[radar_data_frame_position_ - 3] == 0x03 &&
						 radar_data_frame_[radar_data_frame_position_ - 2] == 0x02 &&
						 radar_data_frame_[radar_data_frame_position_ - 1] == 0x01)
				{
					if (ld2410_parse_command_frame_())
					{
#ifdef LD2410_DEBUG_COMMANDS
						printf("parsed command OK\n");
#endif
						frame_started_ = false;
						radar_data_frame_position_ = 0;
						return true;
					}
					else
					{
#ifdef LD2410_DEBUG_COMMANDS
						printf("failed to parse command");
#endif
						frame_started_ = false;
						radar_data_frame_position_ = 0;
					}
				}
			}
		}
		else
		{
#if defined(LD2410_DEBUG_DATA) || defined(LD2410_DEBUG_COMMANDS)
			printf("\nLD2410 frame overran");
#endif
			frame_started_ = false;
			radar_data_frame_position_ = 0;
		}
	}
	return false;
}

#if 0
static void ld2410_print_frame_()
{
	if (ack_frame_ == true)
	{
		// printf("\nCmnd : ");
	}
	else
	{
		// printf("\nData : ");
	}
	for (uint8_t i = 0; i < radar_data_frame_position_; i++)
	{
		if (radar_data_frame_[i] < 0x10)
		{
			// printf("0");
		}
		// printf("%x ", radar_data_frame_[i]);
	}
}

#endif

static bool ld2410_parse_data_frame_()
{
	uint16_t intra_frame_data_length_ = radar_data_frame_[4] + (radar_data_frame_[5] << 8);
	if (radar_data_frame_position_ == intra_frame_data_length_ + 10)
	{
#ifdef LD2410_DEBUG_DATA
		// ld2410_print_frame_();
#endif
#ifdef LD2410_DEBUG_COMMANDS
		if (ack_frame_ == true)
		{
			// ld2410_print_frame_();
		}
#endif
		if (radar_data_frame_[6] == 0x01 && radar_data_frame_[7] == 0xAA) // Engineering mode data
		{
			target_type_ = radar_data_frame_[8];
#if 0
#ifdef LD2410_DEBUG_PARSE
			printf("\nEngineering data - ");
			if (target_type_ == 0x00)
			{
				printf("no target");
			}
			else if (target_type_ == 0x01)
			{
				printf("moving target:");
			}
			else if (target_type_ == 0x02)
			{
				printf("stationary target:");
			}
			else if (target_type_ == 0x03)
			{
				printf("moving & stationary targets:");
			}
#endif
#endif

			/*
			 *
			 *	To-do support engineering mode
			 *
			 */
		}
		else if (intra_frame_data_length_ == 13 && radar_data_frame_[6] == 0x02 && radar_data_frame_[7] == 0xAA && radar_data_frame_[17] == 0x55 && radar_data_frame_[18] == 0x00) // Normal target data
		{
			target_type_ = radar_data_frame_[8];
			// moving_target_distance_ = radar_data_frame_[9] + (radar_data_frame_[10] << 8);
			stationary_target_distance_ = radar_data_frame_[9] + (radar_data_frame_[10] << 8);
			stationary_target_energy_ = radar_data_frame_[14];
			moving_target_energy_ = radar_data_frame_[11];
			// stationary_target_distance_ = radar_data_frame_[12] + (radar_data_frame_[13] << 8);
			moving_target_distance_ = radar_data_frame_[15] + (radar_data_frame_[16] << 8);
#if 0
#ifdef LD2410_DEBUG_PARSE
			printf("\nNormal data - ");
			if (target_type_ == 0x00)
			{
				printf("no target");
			}
			else if (target_type_ == 0x01)
			{
				printf("moving target:");
			}
			else if (target_type_ == 0x02)
			{
				printf("stationary target:");
			}
			else if (target_type_ == 0x03)
			{
				printf("moving & stationary targets:");
			}
			if (radar_data_frame_[8] & 0x01)
			{
				printf(" moving at %d cm power %d", moving_target_distance_, moving_target_energy_);
			}
			if (radar_data_frame_[8] & 0x02)
			{
				printf(" stationary at %d cm", stationary_target_distance_);
				// printf(F("cm power "));
				// printf(stationary_target_energy_);
			}
#endif
#endif
			radar_uart_last_packet_ = millis();
			return true;
		}
		else
		{
			// #ifdef LD2410_DEBUG_DATA
			// 			printf("\nUnknown frame type");
			// ld2410_print_frame_();
			// #endif
		}
	}
	else
	{
		// #ifdef LD2410_DEBUG_DATA
		// 		printf("\nFrame length unexpected: %d not %d", radar_data_frame_position_, intra_frame_data_length_ + 10);
		// #endif
	}
	return false;
}

static bool ld2410_parse_command_frame_()
{
	uint16_t intra_frame_data_length_ = radar_data_frame_[4] + (radar_data_frame_[5] << 8);
	// #ifdef LD2410_DEBUG_COMMANDS
	// 	ld2410_print_frame_();
	// 	printf("\nACK frame payload: %d bytes", intra_frame_data_length_);
	// #endif
	latest_ack_ = radar_data_frame_[6];
	latest_command_success_ = (radar_data_frame_[8] == 0x00 && radar_data_frame_[9] == 0x00);
	if (intra_frame_data_length_ == 8 && latest_ack_ == 0xFF)
	{
		// #ifdef LD2410_DEBUG_COMMANDS
		// 		printf("\nACK for entering configuration mode: ");
		// #endif
		if (latest_command_success_)
		{
			radar_uart_last_packet_ = millis();
			// #ifdef LD2410_DEBUG_COMMANDS
			// 			printf("OK");
			// #endif
			return true;
		}
		else
		{
			// printf("failed");
			return false;
		}
	}
	else if (intra_frame_data_length_ == 4 && latest_ack_ == 0xFE)
	{
		// #ifdef LD2410_DEBUG_COMMANDS
		// 		printf("\nACK for leaving configuration mode: ");
		// #endif
		if (latest_command_success_)
		{
			radar_uart_last_packet_ = millis();
			// #ifdef LD2410_DEBUG_COMMANDS
			// 			printf("OK");
			// #endif
			return true;
		}
		else
		{
			// printf("failed");
			return false;
		}
	}
	else if (intra_frame_data_length_ == 4 && latest_ack_ == 0x60)
	{
		// #ifdef LD2410_DEBUG_COMMANDS
		// 		printf("\nACK for setting max values: ");
		// #endif
		if (latest_command_success_)
		{
			radar_uart_last_packet_ = millis();
			// #ifdef LD2410_DEBUG_COMMANDS
			// 			printf("OK");
			// #endif
			return true;
		}
		else
		{
			// printf("failed");
			return false;
		}
	}
	else if (intra_frame_data_length_ == 28 && latest_ack_ == 0x61)
	{
		// #ifdef LD2410_DEBUG_COMMANDS
		// 		printf("\nACK for current configuration: ");
		// #endif
		if (latest_command_success_)
		{
			radar_uart_last_packet_ = millis();
			// #ifdef LD2410_DEBUG_COMMANDS
			// 			printf("OK");
			// #endif
			max_gate = radar_data_frame_[11];
			max_moving_gate = radar_data_frame_[12];
			max_stationary_gate = radar_data_frame_[13];
			motion_sensitivity[0] = radar_data_frame_[14];
			motion_sensitivity[1] = radar_data_frame_[15];
			motion_sensitivity[2] = radar_data_frame_[16];
			motion_sensitivity[3] = radar_data_frame_[17];
			motion_sensitivity[4] = radar_data_frame_[18];
			motion_sensitivity[5] = radar_data_frame_[19];
			motion_sensitivity[6] = radar_data_frame_[20];
			motion_sensitivity[7] = radar_data_frame_[21];
			motion_sensitivity[8] = radar_data_frame_[22];
			stationary_sensitivity[0] = radar_data_frame_[23];
			stationary_sensitivity[1] = radar_data_frame_[24];
			stationary_sensitivity[2] = radar_data_frame_[25];
			stationary_sensitivity[3] = radar_data_frame_[26];
			stationary_sensitivity[4] = radar_data_frame_[27];
			stationary_sensitivity[5] = radar_data_frame_[28];
			stationary_sensitivity[6] = radar_data_frame_[29];
			stationary_sensitivity[7] = radar_data_frame_[30];
			stationary_sensitivity[8] = radar_data_frame_[31];
			sensor_idle_time = radar_data_frame_[32];
			sensor_idle_time += (radar_data_frame_[33] << 8);
#if 0
#ifdef LD2410_DEBUG_COMMANDS
			printf("\nMax gate distance: %d", max_gate);
			printf("\nMax motion detecting gate distance: %d", max_moving_gate);
			printf("\nMax stationary detecting gate distance: %d", max_stationary_gate);
			printf("\nSensitivity per gate");
			for (uint8_t i = 0; i < 9; i++)
			{
				printf("\nGate %d (%0.2f-%0.2f", i, i * 0.75, (i + 1) * 0.75);
				printf(" metres) Motion: %d", motion_sensitivity[i]);
				printf(" Stationary: %d", stationary_sensitivity[i]);
			}
			printf("\nSensor idle timeout: %ds", sensor_idle_time);
#endif
#endif

			return true;
		}
		else
		{
			// printf("failed");
			return false;
		}
	}
	else if (intra_frame_data_length_ == 4 && latest_ack_ == 0x64)
	{
		// #ifdef LD2410_DEBUG_COMMANDS
		// 		printf("\nACK for setting sensitivity values: ");
		// #endif
		if (latest_command_success_)
		{
			radar_uart_last_packet_ = millis();
			// #ifdef LD2410_DEBUG_COMMANDS
			// 			printf("OK");
			// #endif
			return true;
		}
		else
		{
			// printf("failed");
			return false;
		}
	}
	else if (intra_frame_data_length_ == 12 && latest_ack_ == 0xA0)
	{
		// #ifdef LD2410_DEBUG_COMMANDS
		// 		printf("\nACK for firmware version: ");
		// #endif
		if (latest_command_success_)
		{
			firmware_major_version = radar_data_frame_[13];
			firmware_minor_version = radar_data_frame_[12];
			firmware_bugfix_version = radar_data_frame_[14];
			firmware_bugfix_version += radar_data_frame_[15] << 8;
			firmware_bugfix_version += radar_data_frame_[16] << 16;
			firmware_bugfix_version += radar_data_frame_[17] << 24;
			radar_uart_last_packet_ = millis();
			// #ifdef LD2410_DEBUG_COMMANDS
			// 			printf("OK");
			// #endif
			return true;
		}
		else
		{
			// printf("failed");
			return false;
		}
	}
	else if (intra_frame_data_length_ == 4 && latest_ack_ == 0xA2)
	{
		// #ifdef LD2410_DEBUG_COMMANDS
		// 		printf("\nACK for factory reset: ");
		// #endif
		if (latest_command_success_)
		{
			radar_uart_last_packet_ = millis();
			// #ifdef LD2410_DEBUG_COMMANDS
			// 			printf("OK");
			// #endif
			return true;
		}
		else
		{
			// printf("failed");
			return false;
		}
	}
	else if (intra_frame_data_length_ == 4 && latest_ack_ == 0xA3)
	{
		// #ifdef LD2410_DEBUG_COMMANDS
		// 		printf("\nACK for restart: ");
		// #endif
		if (latest_command_success_)
		{
			radar_uart_last_packet_ = millis();
			// #ifdef LD2410_DEBUG_COMMANDS
			// 			printf("OK");
			// #endif
			return true;
		}
		else
		{
			// printf("failed");
			return false;
		}
	}
	else
	{
		// #ifdef LD2410_DEBUG_COMMANDS
		// 		printf("\nUnknown ACK");
		// #endif
	}
	return false;
}

static void ld2410_send_command_preamble_()
{
	// Command preamble
	ld2410_uart_write_byte((byte)0xFD);
	ld2410_uart_write_byte((byte)0xFC);
	ld2410_uart_write_byte((byte)0xFB);
	ld2410_uart_write_byte((byte)0xFA);
}

static void ld2410_send_command_postamble_()
{
	// Command end
	ld2410_uart_write_byte((byte)0x04);
	ld2410_uart_write_byte((byte)0x03);
	ld2410_uart_write_byte((byte)0x02);
	ld2410_uart_write_byte((byte)0x01);
}

static bool ld2410_enter_configuration_mode_()
{
	ld2410_send_command_preamble_();
	// Request firmware
	ld2410_uart_write_byte((byte)0x04); // Command is four bytes long
	ld2410_uart_write_byte((byte)0x00);
	ld2410_uart_write_byte((byte)0xFF); // Request enter command mode
	ld2410_uart_write_byte((byte)0x00);
	ld2410_uart_write_byte((byte)0x01);
	ld2410_uart_write_byte((byte)0x00);
	ld2410_send_command_postamble_();
	radar_uart_last_command_ = millis();
	while (millis() - radar_uart_last_command_ < radar_uart_command_timeout_)
	{
		if (latest_ack_ == 0xFF && latest_command_success_)
		{
			return true;
		}
	}
	return false;
}

static bool ld2410_leave_configuration_mode_()
{
	ld2410_send_command_preamble_();
	// Request firmware
	ld2410_uart_write_byte((byte)0x02); // Command is four bytes long
	ld2410_uart_write_byte((byte)0x00);
	ld2410_uart_write_byte((byte)0xFE); // Request leave command mode
	ld2410_uart_write_byte((byte)0x00);
	ld2410_send_command_postamble_();
	radar_uart_last_command_ = millis();
	while (millis() - radar_uart_last_command_ < radar_uart_command_timeout_)
	{
		if (latest_ack_ == 0xFE && latest_command_success_)
		{
			return true;
		}
	}
	return false;
}

bool ld2410_request_start_engineering_mode()
{
	ld2410_send_command_preamble_();
	// Request firmware
	ld2410_uart_write_byte((byte)0x02); // Command is four bytes long
	ld2410_uart_write_byte((byte)0x00);
	ld2410_uart_write_byte((byte)0x62); // Request enter command mode
	ld2410_uart_write_byte((byte)0x00);
	ld2410_send_command_postamble_();
	radar_uart_last_command_ = millis();
	while (millis() - radar_uart_last_command_ < radar_uart_command_timeout_)
	{
		if (latest_ack_ == 0x62 && latest_command_success_)
		{
			return true;
		}
	}
	return false;
}

bool ld2410_requestEndEngineeringMode()
{
	ld2410_send_command_preamble_();
	// Request firmware
	ld2410_uart_write_byte((byte)0x02); // Command is four bytes long
	ld2410_uart_write_byte((byte)0x00);
	ld2410_uart_write_byte((byte)0x63); // Request leave command mode
	ld2410_uart_write_byte((byte)0x00);
	ld2410_send_command_postamble_();
	radar_uart_last_command_ = millis();
	while (millis() - radar_uart_last_command_ < radar_uart_command_timeout_)
	{
		if (latest_ack_ == 0x63 && latest_command_success_)
		{
			return true;
		}
	}
	return false;
}

bool ld2410_request_current_configuration()
{
	if (ld2410_enter_configuration_mode_())
	{
		delay(50);
		ld2410_send_command_preamble_();
		// Request firmware
		ld2410_uart_write_byte((byte)0x02); // Command is two bytes long
		ld2410_uart_write_byte((byte)0x00);
		ld2410_uart_write_byte((byte)0x61); // Request current configuration
		ld2410_uart_write_byte((byte)0x00);
		ld2410_send_command_postamble_();
		radar_uart_last_command_ = millis();
		while (millis() - radar_uart_last_command_ < radar_uart_command_timeout_)
		{
			if (latest_ack_ == 0x61 && latest_command_success_)
			{
				delay(50);
				ld2410_leave_configuration_mode_();
				return true;
			}
		}
	}
	delay(50);
	ld2410_leave_configuration_mode_();
	return false;
}

bool ld2410_request_firmware_version()
{
	if (ld2410_enter_configuration_mode_())
	{
		delay(50);
		ld2410_send_command_preamble_();
		// Request firmware
		ld2410_uart_write_byte((byte)0x02); // Command is two bytes long
		ld2410_uart_write_byte((byte)0x00);
		ld2410_uart_write_byte((byte)0xA0); // Request firmware version
		ld2410_uart_write_byte((byte)0x00);
		ld2410_send_command_postamble_();
		radar_uart_last_command_ = millis();
		while (millis() - radar_uart_last_command_ < radar_uart_command_timeout_)
		{
			if (latest_ack_ == 0xA0 && latest_command_success_)
			{
				delay(50);
				ld2410_leave_configuration_mode_();
				return true;
			}
		}
	}
	delay(50);
	ld2410_leave_configuration_mode_();
	return false;
}

bool ld2410_requestRestart()
{
	if (ld2410_enter_configuration_mode_())
	{
		delay(50);
		ld2410_send_command_preamble_();
		// Request firmware
		ld2410_uart_write_byte((byte)0x02); // Command is two bytes long
		ld2410_uart_write_byte((byte)0x00);
		ld2410_uart_write_byte((byte)0xA3); // Request restart
		ld2410_uart_write_byte((byte)0x00);
		ld2410_send_command_postamble_();
		radar_uart_last_command_ = millis();
		while (millis() - radar_uart_last_command_ < radar_uart_command_timeout_)
		{
			if (latest_ack_ == 0xA3 && latest_command_success_)
			{
				delay(50);
				ld2410_leave_configuration_mode_();
				return true;
			}
		}
	}
	delay(50);
	ld2410_leave_configuration_mode_();
	return false;
}

bool ld2410_requestFactoryReset()
{
	if (ld2410_enter_configuration_mode_())
	{
		delay(50);
		ld2410_send_command_preamble_();
		// Request firmware
		ld2410_uart_write_byte((byte)0x02); // Command is two bytes long
		ld2410_uart_write_byte((byte)0x00);
		ld2410_uart_write_byte((byte)0xA2); // Request factory reset
		ld2410_uart_write_byte((byte)0x00);
		ld2410_send_command_postamble_();
		radar_uart_last_command_ = millis();
		while (millis() - radar_uart_last_command_ < radar_uart_command_timeout_)
		{
			if (latest_ack_ == 0xA2 && latest_command_success_)
			{
				delay(50);
				ld2410_leave_configuration_mode_();
				return true;
			}
		}
	}
	delay(50);
	ld2410_leave_configuration_mode_();
	return false;
}

bool ld2410_set_max_values(uint16_t moving, uint16_t stationary, uint16_t inactivity_timer)
{
	if (ld2410_enter_configuration_mode_())
	{
		delay(50);
		ld2410_send_command_preamble_();
		ld2410_uart_write_byte((byte)0x14); // Command is 20 bytes long
		ld2410_uart_write_byte((byte)0x00);
		ld2410_uart_write_byte((byte)0x60); // Request set max values
		ld2410_uart_write_byte((byte)0x00);
		ld2410_uart_write_byte((byte)0x00); // Moving gate command
		ld2410_uart_write_byte((byte)0x00);
		ld2410_uart_write_byte((byte)(moving & 0x00FF)); // Moving gate value
		ld2410_uart_write_byte((byte)((moving & 0xFF00) >> 8));
		ld2410_uart_write_byte((byte)0x00); // Spacer
		ld2410_uart_write_byte((byte)0x00);
		ld2410_uart_write_byte((byte)0x01); // Stationary gate command
		ld2410_uart_write_byte((byte)0x00);
		ld2410_uart_write_byte((byte)(stationary & 0x00FF)); // Stationary gate value
		ld2410_uart_write_byte((byte)((stationary & 0xFF00) >> 8));
		ld2410_uart_write_byte((byte)0x00); // Spacer
		ld2410_uart_write_byte((byte)0x00);
		ld2410_uart_write_byte((byte)0x02); // Inactivity timer command
		ld2410_uart_write_byte((byte)0x00);
		ld2410_uart_write_byte((byte)(inactivity_timer & 0x00FF)); // Inactivity timer
		ld2410_uart_write_byte((byte)((inactivity_timer & 0xFF00) >> 8));
		ld2410_uart_write_byte((byte)0x00); // Spacer
		ld2410_uart_write_byte((byte)0x00);
		ld2410_send_command_postamble_();
		radar_uart_last_command_ = millis();
		while (millis() - radar_uart_last_command_ < radar_uart_command_timeout_)
		{
			if (latest_ack_ == 0x60 && latest_command_success_)
			{
				delay(50);
				ld2410_leave_configuration_mode_();
				return true;
			}
		}
	}
	delay(50);
	ld2410_leave_configuration_mode_();
	return false;
}

bool ld2410_set_gate_sensitivity_threshold(uint8_t gate, uint8_t moving, uint8_t stationary)
{
	if (ld2410_enter_configuration_mode_())
	{
		delay(50);
		ld2410_send_command_preamble_();
		ld2410_uart_write_byte((byte)0x14); // Command is 20 bytes long
		ld2410_uart_write_byte((byte)0x00);
		ld2410_uart_write_byte((byte)0x64); // Request set sensitivity values
		ld2410_uart_write_byte((byte)0x00);
		ld2410_uart_write_byte((byte)0x00); // Gate command
		ld2410_uart_write_byte((byte)0x00);
		ld2410_uart_write_byte(gate); // Gate value
		ld2410_uart_write_byte((byte)0x00);
		ld2410_uart_write_byte((byte)0x00); // Spacer
		ld2410_uart_write_byte((byte)0x00);
		ld2410_uart_write_byte((byte)0x01); // Motion sensitivity command
		ld2410_uart_write_byte((byte)0x00);
		ld2410_uart_write_byte(moving); // Motion sensitivity value
		ld2410_uart_write_byte((byte)0x00);
		ld2410_uart_write_byte((byte)0x00); // Spacer
		ld2410_uart_write_byte((byte)0x00);
		ld2410_uart_write_byte((byte)0x02); // Stationary sensitivity command
		ld2410_uart_write_byte((byte)0x00);
		ld2410_uart_write_byte(stationary); // Stationary sensitivity value
		ld2410_uart_write_byte((byte)0x00);
		ld2410_uart_write_byte((byte)0x00); // Spacer
		ld2410_uart_write_byte((byte)0x00);
		ld2410_send_command_postamble_();
		radar_uart_last_command_ = millis();
		while (millis() - radar_uart_last_command_ < radar_uart_command_timeout_)
		{
			if (latest_ack_ == 0x64 && latest_command_success_)
			{
				delay(50);
				ld2410_leave_configuration_mode_();
				return true;
			}
		}
	}
	delay(50);
	ld2410_leave_configuration_mode_();
	return false;
}

bool ld2410_set_gates_sensitivity_threshold(uint8_t *moving, uint8_t *stationary)
{
	bool ret = false;
	for (uint8_t gate = 0; gate < 9; gate++)
	{
		ret = ld2410_set_gate_sensitivity_threshold(gate, moving[gate], stationary[gate]);
	}
	return ret;
}

static size_t ld2410_uart_write_byte(uint8_t byte)
{
	uart_write_bytes(ezlo_ld2410_uart_handle->ezlopi_uart.channel, &byte, 1); // returns true always if there is no parameter error
	return 1;
}

void ld2410_driver_start_reading()
{
	vTaskResume(ezlo_ld2410_uart_handle->taskHandle);
}

void ld2410_driver_stop_reading()
{
	vTaskSuspend(ezlo_ld2410_uart_handle->taskHandle);
}