/**
 * @file ultrasonic.c
 *
 * ESP-IDF driver for ultrasonic range meters, e.g. HC-SR04, HY-SRF05 and so on
 *
 * Ported from esp-open-rtos
 * Copyright (C) 2016, 2018 Ruslan V. Uss <unclerus@gmail.com>
 * BSD Licensed as described in the file LICENSE
 */
#include "hc_sr04_lib.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <sys/time.h>
#include "trace.h"

#define TRIGGER_LOW_DELAY 4
#define TRIGGER_HIGH_DELAY 10
#define PING_TIMEOUT 6000
#define ROUNDTRIP 58

static portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

static inline uint32_t get_time_us()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_usec;
}

#define timeout_expired(start, len) ((uint32_t)(get_time_us() - (start)) >= (len))

#define RETURN_CRTCAL(MUX, RES)  \
    do                           \
    {                            \
        portEXIT_CRITICAL(&MUX); \
        return RES;              \
    } while (0)

void ultrasonic_init(const ultrasonic_sensor_t *dev)
{
    gpio_reset_pin(dev->trigger_pin);
    gpio_reset_pin(dev->echo_pin);
    gpio_set_direction(dev->trigger_pin, GPIO_MODE_OUTPUT);
    gpio_set_direction(dev->echo_pin, GPIO_MODE_INPUT);

    gpio_set_level(dev->trigger_pin, 0);
}

esp_err_t ultrasonic_measure_cm(const ultrasonic_sensor_t *dev, uint32_t max_distance, uint32_t *distance)
{
    int x[6] = {0, 0, 0, 0, 0, 0};
    if (!distance)
        return ESP_ERR_INVALID_ARG;

    portENTER_CRITICAL(&mux);

    // Ping: Low for 2..4 us, then high 10 us
    gpio_set_level(dev->trigger_pin, 0);
    esp_rom_delay_us(TRIGGER_LOW_DELAY);
    gpio_set_level(dev->trigger_pin, 1);
    esp_rom_delay_us(TRIGGER_HIGH_DELAY);
    gpio_set_level(dev->trigger_pin, 0);

    // Previous ping isn't ended
    if (gpio_get_level(dev->echo_pin))
    {
        x[0] = 1;
        RETURN_CRTCAL(mux, ESP_ERR_ULTRASONIC_PING);
    }

    // Wait for echo
    uint32_t startx = get_time_us();
    while (!gpio_get_level(dev->echo_pin))
    {
        x[1] += 1;
        if (timeout_expired(startx, PING_TIMEOUT))
        {
            x[2] += 1;
            RETURN_CRTCAL(mux, ESP_ERR_ULTRASONIC_PING_TIMEOUT);
        }
    }

    // got echo, measuring
    uint32_t echo_start = get_time_us();
    uint32_t time = echo_start;
    uint32_t meas_timeout = echo_start + max_distance * ROUNDTRIP;
    while (gpio_get_level(dev->echo_pin))
    {
        x[3] += 1;
        // time = get_time_us();
        if (timeout_expired(echo_start, meas_timeout))
        {
            x[4] += 1;
            RETURN_CRTCAL(mux, ESP_ERR_ULTRASONIC_ECHO_TIMEOUT);
        }
    }
    time = get_time_us();

    x[5] += 1;
    portEXIT_CRITICAL(&mux);

    *distance = (time - echo_start) / ROUNDTRIP;
    TRACE_D("time spent: %dus", time - echo_start);
    TRACE_D("state: [%d, %d, %d, %d, %d, %d]", x[0], x[1], x[2], x[3], x[4], x[5]);
    return ESP_OK;
}