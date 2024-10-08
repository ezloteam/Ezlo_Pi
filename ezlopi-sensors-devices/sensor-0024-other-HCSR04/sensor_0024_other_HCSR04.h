

#ifndef _SENSOR_0024_OTHER_HCSR04_H_
#define _SENSOR_0024_OTHER_HCSR04_H_

#include "ezlopi_core_actions.h"
#include "ezlopi_core_devices.h"
#include "ezlopi_core_errors.h"

#define ESP_ERR_ULTRASONIC_PING 0x200
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

#define CHECK_ARG(VAL)                  \
    do                                  \
    {                                   \
        if (!(VAL))                     \
            return ESP_ERR_INVALID_ARG; \
    } while (0)
#define CHECK(x)                \
    do                          \
    {                           \
        esp_err_t __;           \
        if ((__ = x) != ESP_OK) \
            return __;          \
    } while (0)
#define RETURN_CRITICAL(RES) \
    do                       \
    {                        \
        PORT_EXIT_CRITICAL;  \
        return RES;          \
    } while (0)

ezlopi_error_t sensor_0024_other_HCSR04_v3(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg);

#endif //_SENSOR_0024_OTHER_HCSR04_H_