

#ifndef _EZLOPI_PWM_H_
#define _EZLOPI_PWM_H_

#include "driver/ledc.h"

typedef struct s_ezlopi_channel_speed
{
    uint32_t channel;
    uint32_t speed_mode;
} s_ezlopi_channel_speed_t;

typedef struct ezlopi_pwm
{
    uint8_t gpio_num;
    uint32_t channel;
    uint32_t speed_mode;
    uint8_t pwm_resln; // pwm resolution
    uint32_t freq_hz;
    uint32_t duty_cycle;
    int value;
} s_ezlopi_pwm_t;

s_ezlopi_channel_speed_t *ezlopi_pwm_init(uint8_t pwm_gpio_num, uint8_t pwm_resln, uint32_t freq_hz, uint32_t duty_cycle);
void ezlopi_pwm_change_duty(uint32_t channel, uint32_t speed, uint32_t duty);
uint32_t ezlopi_pwm_get_duty(uint32_t channel, uint32_t speed);

#endif //_EZLOPI_PWM_H_
