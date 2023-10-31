#ifndef __QT_SERIAL_H__
#define __QT_SERIAL_H__

#include <string.h>
#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#ifdef __cplusplus
extern "C"
{
#endif

// typedef enum e_ezlopi_item_type
// {
//     EZLOPI_ITEM_NONE = 0,
//     EZLOPI_ITEM_LED = 1,
//     EZLOPI_ITEM_RELAY = 2,
//     EZLOPI_ITEM_PLUG = 3,
//     EZLOPI_ITEM_SPK = 4, // SPK -> [S: Switch][P: Push Buttom][K: Key]
//     EZLOPI_ITEM_MPU6050 =5,
//     EZLOPI_ITEM_ADXL345 = 6,
//     EZLOPI_ITEM_
// } e_ezlopi_item_type_t;

    void qt_serial_init(void);
    int qt_serial_tx_data(int len, uint8_t *data);

#ifdef __cplusplus
}
#endif

#endif // __QT_SERIAL_H__