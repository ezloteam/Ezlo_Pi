#ifndef _PMS5003_H_
#define _PMS5003_H_


#include <stdio.h>
#include <string.h>

#include "esp_log.h"
#include "driver/gpio.h"

#include "ezlopi_hal_uart.h"

// Ambient Pro - Constants
#define PMS_SET_PIN GPIO_NUM_39
#define PMS_RESET_PIN GPIO_NUM_42
#define PMS_RX_PIN GPIO_NUM_41
#define PMS_TX_PIN GPIO_NUM_40
#define PMS_UART_PORT UART_NUM_1
#define PMS_UART_BUF_SIZE 128
#define PMS_UART_BUAD_RATE 9600

#define PMS_ACTIVE_TIME 30
#define PMS_SLEEP_TIME 60
#define PMS_STABILITY_TIME 15 // Here
#define PMS_TOTAL_TIME (PMS_ACTIVE_TIME + PMS_SLEEP_TIME)

#define PMS_GET_TOTAL_TIME(x, y) (x + y)

#define PMS_CHECK_ACTIVE_STATUS(status) (status & 0x01)
#define PMS_CHECK_SLEEP_STATUS(status) (status & 0x02)

#define PMS_SET_ACTIVE_STATUS(status) (status |= 0x01)
#define PMS_SET_SLEEP_STATUS(status) (status |= 0x02)
#define PMS_SET_READ_STATUS(status) (status |= 0x04)

/**! Structure holding Plantower's standard packet **/
typedef struct PMSAQIdata
{
    uint16_t framelen;       ///< How long this data chunk is
    uint16_t pm10_standard,  ///< Standard PM1.0
        pm25_standard,       ///< Standard PM2.5
        pm100_standard;      ///< Standard PM10.0
    uint16_t pm10_env,       ///< Environmental PM1.0
        pm25_env,            ///< Environmental PM2.5
        pm100_env;           ///< Environmental PM10.0
    uint16_t particles_03um, ///< 0.3um Particle Count
        particles_05um,      ///< 0.5um Particle Count
        particles_10um,      ///< 1.0um Particle Count
        particles_25um,      ///< 2.5um Particle Count
        particles_50um,      ///< 5.0um Particle Count
        particles_100um;     ///< 10.0um Particle Count
    uint16_t unused;         ///< Unused
    uint16_t checksum;       ///< Packet checksum
    uint8_t available;
} PM25_AQI_Data;

typedef struct
{
    uint32_t pms_set_pin;
    uint32_t pms_reset_pin;
    uint32_t pms_tx_pin;
    uint32_t pms_rx_pin;
    uint32_t pms_baud_rate;
    uint32_t pms_active_time;
    uint32_t pms_sleep_time;
    uint32_t pms_stability_time;
    uint8_t volatile pmsStatusReg;
    /*
     * pmsStatusReg =  | X | X | X | X | X | EN_READ | SLEEP_MODE | ACTIVE_MODE |
     *                 | 7 | 6 | 5 | 4 | 3 |   2     |      1     |      0     |
     */
    PM25_AQI_Data pms_data;
    int counter;
} s_pms5003_sensor_object;

void pms_init(s_pms5003_sensor_object *pms_object);
void pms_print_data(PM25_AQI_Data *data);
bool pms_is_data_available(PM25_AQI_Data *data);
void pms_set_data_available_to_false(PM25_AQI_Data *data);

#endif// _PMS5003_H_
