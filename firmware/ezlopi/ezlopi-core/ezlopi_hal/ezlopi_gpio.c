// #include "ezlopi_gpio.h"
// #include "ezlopi_devices.h"

// int ezlopi_gpio_init(s_ezlopi_device_properties_t *properties)
// {
//     int ret = 0;

//     if (GPIO_IS_VALID_OUTPUT_GPIO(properties->interface.gpio.gpio_out.gpio_num))
//     {
//         const gpio_config_t io_conf = {
//             .pin_bit_mask = (1ULL << properties->interface.gpio.gpio_out.gpio_num),
//             .mode = GPIO_MODE_OUTPUT,
//             .pull_up_en = ((properties->interface.gpio.gpio_out.pull == GPIO_PULLUP_ONLY) ||
//                            (properties->interface.gpio.gpio_out.pull == GPIO_PULLUP_PULLDOWN))
//                               ? GPIO_PULLUP_ENABLE
//                               : GPIO_PULLUP_DISABLE,
//             .pull_down_en = ((properties->interface.gpio.gpio_out.pull == GPIO_PULLDOWN_ONLY) ||
//                              (properties->interface.gpio.gpio_out.pull == GPIO_PULLUP_PULLDOWN))
//                                 ? GPIO_PULLDOWN_ENABLE
//                                 : GPIO_PULLDOWN_DISABLE,
//             .intr_type = GPIO_INTR_DISABLE,
//         };

//         gpio_config(&io_conf);
//         // digital_io_write_gpio_value(properties);
//     }

//     if (GPIO_IS_VALID_GPIO(properties->interface.gpio.gpio_in.gpio_num))
//     {
//         const gpio_config_t io_conf = {
//             .pin_bit_mask = (1ULL << properties->interface.gpio.gpio_in.gpio_num),
//             .mode = GPIO_MODE_OUTPUT,
//             .pull_up_en = ((properties->interface.gpio.gpio_in.pull == GPIO_PULLUP_ONLY) ||
//                            (properties->interface.gpio.gpio_in.pull == GPIO_PULLUP_PULLDOWN))
//                               ? GPIO_PULLUP_ENABLE
//                               : GPIO_PULLUP_DISABLE,
//             .pull_down_en = ((properties->interface.gpio.gpio_in.pull == GPIO_PULLDOWN_ONLY) ||
//                              (properties->interface.gpio.gpio_in.pull == GPIO_PULLUP_PULLDOWN))
//                                 ? GPIO_PULLDOWN_ENABLE
//                                 : GPIO_PULLDOWN_DISABLE,
//             .intr_type = (GPIO_PULLUP_ONLY == properties->interface.gpio.gpio_in.pull)
//                              ? GPIO_INTR_POSEDGE
//                              : GPIO_INTR_NEGEDGE,
//         };

//         gpio_config(&io_conf);
//         // digital_io_isr_service_init(properties);
//     }

//     return ret;
// }