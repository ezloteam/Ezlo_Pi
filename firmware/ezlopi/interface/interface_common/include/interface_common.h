#ifndef __INTERFACE_COMMON_H__
#define __INTERFACE_COMMON_H__

#include "driver/gpio.h"

#ifdef __cplusplus
extern "C"
{
#endif

    void interface_common_init(void);

    uint8_t interface_common_get_config(int idx);
    void interface_common_set_config(uint8_t val, int idx);

    void interface_common_gpio_config_sets(uint32_t n);
    uint32_t interface_common_gpio_get_input_state(uint32_t pin);
    uint32_t interface_common_gpio_get_output_state(uint32_t pin);
    uint32_t interface_common_get_gpio_isr_event(uint32_t wait_ms);
    void interface_common_gpio_state_set(uint32_t pin, uint32_t state);
    void interface_common_inst_input_button(gpio_num_t gpioPin, gpio_pull_mode_t pull);
    void interface_common_inst_out_button(gpio_num_t gpioPin, uint8_t state, gpio_pull_mode_t pull);

    uint32_t interface_get_message_count_in_queue(void);

#ifdef __cplusplus
}
#endif

#endif // __INTERFACE_COMMON_H__