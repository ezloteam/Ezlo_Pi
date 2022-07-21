#ifndef __INTERFACE_COMMON_H__
#define __INTERFACE_COMMON_H__

#include <iostream>

#include "driver/gpio.h"

class interface_common
{
private:
public:
    void init(void);
    static interface_common *get_instance(void);

    uint8_t get_config(int idx);
    void set_config(uint8_t val, int idx);

    void gpio_config_sets(uint32_t n);
    uint32_t gpio_state_get(uint32_t pin);
    uint32_t get_gpio_isr_event(uint32_t wait_ms);
    void gpio_state_set(uint32_t pin, uint32_t state);
    void inst_input_button(gpio_num_t gpioPin, gpio_pull_mode_t pull);
    void inst_out_button(gpio_num_t gpioPin, uint8_t state, gpio_pull_mode_t pull);

    interface_common(interface_common &other) = delete;
    void operator=(const interface_common &) = delete;

protected:
    interface_common() {}
    static interface_common *interface_common_;
};

#endif // __INTERFACE_COMMON_H__