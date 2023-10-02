

#ifndef _ULTRASONIC_JSNSR04T_H_
#define _ULTRASONIC_JSNSR04T_H_


#include "ezlopi_actions.h"
#include "ezlopi_devices.h"
#include "ezlopi_uart.h"

#include <driver/gpio.h>
#include <esp_err.h>


#define ESP_ERR_ULTRASONIC_PING         0x200
#define ESP_ERR_ULTRASONIC_PING_TIMEOUT 0x201
#define ESP_ERR_ULTRASONIC_ECHO_TIMEOUT 0x202

#define TRIGGER_LOW_DELAY 4
#define TRIGGER_HIGH_DELAY 10
#define PING_TIMEOUT 6000
#define ROUNDTRIP_M 5800.0f
#define ROUNDTRIP_CM 58
#define MAX_DISTANCE_CM 500

#define PORT_ENTER_CRITICAL portENTER_CRITICAL(&mux)
#define PORT_EXIT_CRITICAL portEXIT_CRITICAL(&mux)

#define timeout_expired(start, len) ((esp_timer_get_time() - (start)) >= (len))

#define CHECK_ARG(VAL) do { if (!(VAL)) return ESP_ERR_INVALID_ARG; } while (0)
#define CHECK(x) do { esp_err_t __; if ((__ = x) != ESP_OK) return __; } while (0)
#define RETURN_CRITICAL(RES) do { PORT_EXIT_CRITICAL; return RES; } while(0)

/**
 * Device descriptor
 */
typedef struct
{
    gpio_num_t trigger_pin; //!< GPIO output pin for trigger
    gpio_num_t echo_pin; //!< GPIO input pin for echo
    uint32_t distance;  // distance in cm
} ultrasonic_JSNSR04T_sensor_t;


int sensor_0031_other_JSNSR04T(e_ezlopi_actions_t action, s_ezlopi_device_properties_t *ezlo_device, void *arg, void *user_arg);


#endif //_ULTRASONIC_JSNSR04T_H_