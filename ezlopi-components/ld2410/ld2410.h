#pragma once

#include "ld2410_driver.h"
#include "driver/gpio.h"

#define TEST_SETTINGS 1
#define stringify(s) #s

#define LD2410_BAUDRATE 256000
#define RADAR_TX_PIN GPIO_NUM_12
#define RADAR_RX_PIN GPIO_NUM_13

typedef enum ld2410_template
{
    SLEEP_TEMPLATE_CLOSE_RANGE,
    SLEEP_TEMPLATE_MID_RANGE,
    SLEEP_TEMPLATE_LONG_RANGE,
    MOVEMENT_TEMPLATE_CLOSE_RANGE,
    MOVEMENT_TEMPLATE_MID_RANGE,
    MOVEMENT_TEMPLATE_LONG_RANGE,
    BASIC_TEMPLATE_CLOSE_RANGE,
    BASIC_TEMPLATE_MID_RANGE,
    BASIC_TEMPLATE_LONG_RANGE,
    CUSTOM_TEMPLATE
} ld2410_template_t;

typedef struct ld2410_settings
{
    distance_t  max_still_distance;
    distance_t  min_still_distance;
    distance_t  max_move_distance;
    distance_t  min_move_distance;
    uint16_t    no_one_duration;
} ld2410_settings_t;

typedef enum target_direction
{
    STATIONARY,
    APPROACHING,
    MOVING_AWAY
} target_direction_t;

typedef struct ld2410_outputs
{
    uint16_t moving_target_distance; // in cm
    uint16_t stationary_target_distance; // in cm
    target_direction_t direction;
    bool presence;
} ld2410_outputs_t;

esp_err_t ld2410_setup(s_ezlopi_uart_t ld2410_setup);
esp_err_t ld2410_get_data(ld2410_outputs_t* output);
esp_err_t ld2410_set_template(ld2410_template_t template, ld2410_settings_t* p_settings);
void ld2410_start_reading();
void ld2410_stop_reading();
