

#ifndef _EZLOPI_SERVICE_LED_INDICATOR_H_
#define _EZLOPI_SERVICE_LED_INDICATOR_H_

typedef enum {
    PRIORITY_POWER = 0,
    PRIORITY_WIFI,
    PRIORITY_INTERNET,
    PRIORITY_CLOUD,
    PRIORITY_MAX,
}e_indicator_led_priority_t;

int ezlopi_service_led_indicator_init();


#endif // _EZLOPI_SERVICE_LED_INDICATOR_H_

