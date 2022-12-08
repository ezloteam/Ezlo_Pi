

#ifndef _EZLOPI_PWM_H
#define _EZLOPI_PWM_H

#include "driver/ledc.h"

typedef struct s_ezlopi_channel_speed{
    uint32_t channel;
    uint32_t speed_mode;
}s_ezlopi_channel_speed_t;

typedef struct s_ezlopi_pwm_object* ezlopi_pwm_object_handle_t;


s_ezlopi_channel_speed_t* ezlopi_pwm_init(uint8_t pwm_gpio_num, uint8_t pwm_resln, uint32_t freq_hz, uint32_t duty_cycle);
void ezlopi_pwm_change_duty(uint32_t channel, uint32_t speed, uint32_t duty);

#endif //_EZLOPI_PWM_H
