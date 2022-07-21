#ifndef __PIN_CONF_H__
#define __PIN_CONF_H__

typedef struct s_pin_def
{
    uint32_t gpio_num;
    uint32_t gpio_options[39];
} s_pin_def_t;

#endif // __PIN_CONF_H__