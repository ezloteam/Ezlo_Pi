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
#ifndef ld2410_h
#define ld2410_h

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "ezlopi_hal_uart.h"

typedef unsigned char byte;
#define delay(x) vTaskDelay(x / portTICK_RATE_MS)
#define millis() esp_timer_get_time() / 1000

#define LD2410_MAX_FRAME_LENGTH 40
// #define LD2410_DEBUG_INITIALIZATION
// #define LD2410_DEBUG_DATA
// #define LD2410_DEBUG_COMMANDS
// #define LD2410_DEBUG_PARSE

#define RESET_ERROR 0
#define ALIGNMENT_ERROR 1

typedef enum distance
{
	DISTANCE_0CM = 0,
	DISTANCE_75CM,
	DISTANCE_150CM,
	DISTANCE_225CM,
	DISTANCE_300CM,
	DISTANCE_375CM,
	DISTANCE_450CM,
	DISTANCE_525CM,
	DISTANCE_600CM,
} distance_t;

#ifdef __cplusplus
extern "C"
{
#endif

	bool ld2410_begin(bool wait_for_radar, s_ezlopi_uart_t uart_settings); // Start the ld2410 // defaults to true
	bool ld2410_is_connected();
	bool ld2410_read();
	bool ld2410_presence_detected();
	bool ld2410_stationary_target_detected();
	uint16_t ld2410_stationary_target_distance();
	uint8_t ld2410_stationary_target_energy();
	bool ld2410_moving_target_detected();
	uint16_t ld2410_moving_target_distance();
	uint8_t ld2410_moving_target_energy();
	bool ld2410_request_firmware_version();		 // Request the firmware version
	bool ld2410_request_current_configuration(); // Request current configuration

	bool ld2410_request_restart();
	bool ld2410_request_factory_reset();
	bool ld2410_request_start_engineering_mode();
	bool ld2410_request_end_engineering_mode();
	bool ld2410_set_max_values(uint16_t moving, uint16_t stationary, uint16_t inactivity_timer); // Realistically gate values are 0-8 but sent as uint16_t
	bool ld2410_set_gate_sensitivity_threshold(uint8_t gate, uint8_t moving, uint8_t stationary);
	bool ld2410_set_gates_sensitivity_threshold(uint8_t *moving, uint8_t *stationary);
	void ld2410_driver_start_reading();
	void ld2410_driver_stop_reading();
#ifdef __cplusplus
}
#endif
#endif
