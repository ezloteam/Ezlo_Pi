#ifndef _EZLOPI_HAL_GPIO_H_
#define _EZLOPI_HAL_GPIO_H_

#include "stdio.h"
#include "driver/gpio.h"
// #include "ezlopi_core_devices.h"

typedef gpio_num_t e_ezlopi_gpio_num_t;
typedef gpio_mode_t e_ezlopi_gpio_mode_t;
typedef gpio_pull_mode_t e_ezlopi_gpio_pull_mode_t;
typedef gpio_int_type_t e_ezlopi_gpio_interrupt_t;

typedef enum e_ezlopi_gpio_value
{
    EZLOPI_GPIO_LOW = 0,
    EZLOPI_GPIO_HIGH,
    EZLOPI_GPIO_VALUE_MAX
} e_ezlopi_gpio_value_t;

typedef enum e_ezlopi_gpio_logic_invert
{
    EZLOPI_GPIO_LOGIC_NONINVERTED = 0,
    EZLOPI_GPIO_LOGIC_INVERTED,
    EZLOPI_GPIO_LOGIC_MAX
} e_ezlopi_gpio_logic_invert_t;

typedef struct s_ezlopi_gpio
{
    bool enable;
    e_ezlopi_gpio_num_t gpio_num;
    e_ezlopi_gpio_mode_t mode;
    e_ezlopi_gpio_value_t value;
    e_ezlopi_gpio_pull_mode_t pull;
    e_ezlopi_gpio_logic_invert_t invert;
    e_ezlopi_gpio_interrupt_t interrupt;
} s_ezlopi_gpio_t;

typedef struct s_ezlopi_gpios
{
    s_ezlopi_gpio_t gpio_in;
    s_ezlopi_gpio_t gpio_out;
} s_ezlopi_gpios_t;

// int ezlopi_gpio_init(s_ezlopi_device_properties_t *properties);

#endif // _EZLOPI_HAL_GPIO_H_
