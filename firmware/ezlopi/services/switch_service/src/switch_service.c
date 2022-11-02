// #include <string.h>

// #include "driver/gpio.h"
// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"

// #include "switch_service.h"
// #include "web_provisioning.h"
// #include "items.h"
// #include "trace.h"
// #include "freertos/portmacro.h"
// #include "ezlopi_wss.h"
// #include "ezlopi_devices.h"
// #include "ezlopi_devices_list.h"

// #define NUMBER_OF_GPIO 28
// static const uint32_t switch_debounce_time = 1000;
// static TickType_t gpio_intr_tick_arr[NUMBER_OF_GPIO];

// static void __gpio_intr_process(void *pv);

// void switch_service_init(void)
// {
//     memset(gpio_intr_tick_arr, 0, sizeof(gpio_intr_tick_arr));
//     xTaskCreate(__gpio_intr_process, "__gpio_intr_process", 10 * 2048, NULL, 3, NULL);
// }

// static void __gpio_intr_process(void *pv)
// {
//     char value_buf[128];
//     // s_device_properties_t *device_list = devices_common_device_list();
//     l_ezlopi_configured_devices_t *p_devices_list = ezlopi_devices_list_get_configured_items();

//     while (1)
//     {
//         const uint32_t event_gpio_n = interface_common_get_gpio_isr_event(UINT32_MAX);
//         // TRACE_B("Queue size: %d", interface_get_message_count_in_queue());
//         TRACE_D("Got gpio event -> GPIO num: %d", event_gpio_n);
//         TickType_t tick_now = xTaskGetTickCount();

//         if ((UINT32_MAX != event_gpio_n) &&
//             (event_gpio_n < NUMBER_OF_GPIO) &&
//             ((tick_now - gpio_intr_tick_arr[event_gpio_n]) > switch_debounce_time)) // debounce time is 100 miliseconds
//         {
//             // uint32_t _state = interface_common_gpio_get_output_state(device_list[event_gpio_n].out_gpio);

//             TRACE_B("debounce time: %u", tick_now - gpio_intr_tick_arr[event_gpio_n]);
//             gpio_intr_tick_arr[event_gpio_n] = tick_now;

//             while (NULL != p_devices_list)
//             {
//                 if (event_gpio_n == p_devices_list->properties->interface_type)
//                 {
//                 }
//             }

//             for (int idx = 0; idx < MAX_DEV; idx++)
//             {
//                 if (event_gpio_n == device_list[idx].input_gpio)
//                 {
//                     uint32_t new_state = interface_common_gpio_get_output_state(device_list[idx].out_gpio) ? 0 : 1;
//                     TRACE_B("Setting pin: %d -> %d", device_list[idx].out_gpio, new_state);
//                     interface_common_gpio_state_set(device_list[idx].out_gpio, new_state);

//                     memset(value_buf, 0, sizeof(value_buf));
//                     snprintf(value_buf, sizeof(value_buf), "%s", new_state ? "true" : "false");

//                     char *j_response = items_update_from_sensor(idx, value_buf);

//                     if (j_response)
//                     {
//                         TRACE_B(">> WS Tx - 'hub.item.updated' [%d]\r\n%s", strlen(j_response), j_response);
//                         wss_client_send(j_response, strlen(j_response));
//                         free(j_response);
//                         j_response = NULL;
//                     }

//                     break;
//                 }
//             }
//         }
//     }
// }
