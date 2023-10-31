


#ifndef __DHT22_H__
#define __DHT22_H__

#if 0
#include "stdint.h"


#define DHT22_TAG __FILE__

#define DHT22_STANDARD_DELAY_TIME                   3000    // 2sec

#define DHT22_START_SIGNAL_LINE_PULLDOWN_HOLD_US    20000   // 18ms
#define DHT22_START_SIGHAL_LINE_HOLD_RELEASE_US     40      // 40us

#define DHT22_RECEIVE_BIT_LOW_TIME_US               54      // 50us
#define DHT22_RECEIVE_BIT_1_HIGH_TIME_US            26
#define DHT22_RECEIVE_BIT_0_HIGH_TIME_US            26

#define DHT22_RECEIVE_RESPONSE_HIGH_TIME_US         80      // 80us
#define DHT22_RECEIVE_RESPONSE_LOW_TIME_US          80      // 80us


#define onewireENTER_CRITICAL_REGION() portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;taskENTER_CRITICAL(&mux)
#define onewireEXIT_CRITICAL_REGION()  taskEXIT_CRITICAL(&mux)

typedef struct dht22_sensor{
    double temperature;
    double humidity;
    uint32_t gpio_pin;
}dht22_sensor_t;

void dht22_sensor_init(uint32_t gpio_pin);
int dht22_sensor_read_data(uint32_t gpio_pin, dht22_sensor_t* data);
#endif

/* 
	DHT22 temperature sensor driver
*/


#define DHT_OK 0
#define DHT_CHECKSUM_ERROR -1
#define DHT_TIMEOUT_ERROR -2

// == function prototypes =======================================

void setDHTgpio(int gpio);
void errorHandler(int response);
int readDHT();
float getHumidity();
float getTemperature();
int getSignalLevel(int usTimeOut, bool state);



#endif //__DHT22_H__


