


// Reference for onewire: https://www.analog.com/en/technical-articles/1wire-communication-through-software.html


#ifndef _ONEWIRE_H_
#define _ONEWIRE_H_

#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "stdbool.h"

#define ONEWIRE_TAG             __FILE__
#define ONEWIRE_GET_LINE        __LINE__

#define onewireENTER_CRITICAL_REGION() portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;taskENTER_CRITICAL(&mux)
#define onewireEXIT_CRITICAL_REGION()  taskEXIT_CRITICAL(&mux)

// One wire timing delays for different data values
#define ONE_WIRE_WRITE_1_LINE_PULL_DOWN_HOLD_US                 6       // 6us
#define ONE_WIRE_WRITE_1_LINE_RELEASE_HOLD_US                   64      // 64us

#define ONE_WIRE_WRITE_0_LINE_PULL_DOWN_HOLD_US                 60      // 60us
#define ONE_WIRE_WRITE_0_LINE_RELEASE_HOLD_US                   10      // 10us

#define ONE_WIRE_READ_LINE_PULL_DOWN_HOLD_US                    6       // 6us
#define ONE_WIRE_READ_LINE_RELEASE_HOLD_US                      9       // 9us
#define ONE_WIRE_READ_LINE_SAMPLING_US                          55      // 55us

#define ONE_WIRE_HOLD_BEFORE_RESET_US                           0       // 0us
#define ONE_WIRE_RESET_LINE_PULL_DOWN_HOLD_US                   480     // 470us
#define ONE_WIRE_RESET_LINE_RELEASE_HOLD_US                     70      // 70us
#define ONE_WIRE_RESET_LINE_SAMPLING_US                         410     // 410us



esp_err_t one_wire_write_byte_to_line(uint8_t* data, uint32_t gpio_pin);
esp_err_t one_wire_read_byte_from_line(uint8_t* data, uint32_t gpio_pin);
bool one_wire_reset_line(uint32_t gpio_pin);

#endif // _ONEWIRE_H_

