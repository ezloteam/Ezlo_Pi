#if 0
#pragma once 

#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "driver/gpio.h"
// #include "driver/uart.h"
#include "ezlopi_uart.h"

// Ambient Pro - Constants
// TODO
// #define PMS_SET_PIN     GPIO_NUM_39
// #define PMS_RESET_PIN   GPIO_NUM_42
// #define PMS_RX_PIN      GPIO_NUM_41
// #define PMS_TX_PIN      GPIO_NUM_40
// #define PMS_UART_PORT   UART_NUM_1

#define PMS_SET_PIN         GPIO_NUM_2
#define PMS_RESET_PIN       GPIO_NUM_3
#define PMS_RX_PIN          GPIO_NUM_4
#define PMS_TX_PIN          GPIO_NUM_5
#define PMS_UART_PORT       UART_NUM_1

#define PMS_UART_BUF_SIZE 128
#define PMS_UART_BUAD_RATE 9600

#define PMS_ACTIVE_TIME         30
#define PMS_SLEEP_TIME          120  
#define PMS_STABILITY_TIME      15      // Here  
#define PMS_TOTAL_TIME          (PMS_ACTIVE_TIME + PMS_SLEEP_TIME)

#define PMS_CHECK_ACTIVE_STATUS(status) (status & 0x01)
#define PMS_CHECK_SLEEP_STATUS(status)  (status & 0x02)

#define PMS_SET_ACTIVE_STATUS(status)   ( status |= 0x01 )
#define PMS_SET_SLEEP_STATUS(status)    ( status |= 0x02 )
#define PMS_SET_READ_STATUS(status)     ( status |= 0x04 )

/**! Structure holding Plantower's standard packet **/
typedef struct PMSAQIdata {
  uint16_t framelen;       ///< How long this data chunk is
  float pm10_standard,  ///< Standard PM1.0
      pm25_standard,       ///< Standard PM2.5
      pm100_standard;      ///< Standard PM10.0
  float pm10_env,       ///< Environmental PM1.0
      pm25_env,            ///< Environmental PM2.5
      pm100_env;           ///< Environmental PM10.0
  float particles_03um, ///< 0.3um Particle Count
      particles_05um,      ///< 0.5um Particle Count
      particles_10um,      ///< 1.0um Particle Count
      particles_25um,      ///< 2.5um Particle Count
      particles_50um,      ///< 5.0um Particle Count
      particles_100um;     ///< 10.0um Particle Count
  uint16_t unused;         ///< Unused
  uint16_t checksum;       ///< Packet checksum
  uint8_t available;
} PM25_AQI_Data;

void pms_init(PM25_AQI_Data* data, uint32_t set_pin, uint32_t reset_pin);
void pms_uart_setup(PM25_AQI_Data* data, uint32_t tx, uint32_t rx, uint32_t baudrate);
esp_err_t pms_setup_control_gpio(gpio_num_t set_pin, gpio_num_t reset_pin);
esp_err_t pms_gpio_config_output(gpio_num_t pin);
void pms_startup(gpio_num_t set_pin, gpio_num_t reset_pin);
esp_err_t pms_read_upcall(uint8_t* buffer, PM25_AQI_Data* data);
esp_err_t pms_read(PM25_AQI_Data *data);
void pms_print_data(PM25_AQI_Data* data);
void pms_sleep_mode(gpio_num_t set_pin);
void pms_active_mode(gpio_num_t set_pin);
void pms_create_sleep_timer(PM25_AQI_Data* data);
bool pms_is_data_available(PM25_AQI_Data *data);
void pms_set_data_available_to_false(PM25_AQI_Data *data);
static void pms_timer_callback(void* arg);

static void ezlopi_pms5003_upcall(uint8_t* buffer, s_ezlopi_uart_object_handle_t uart_object_handle, void* user_args);

#endif // #if 0