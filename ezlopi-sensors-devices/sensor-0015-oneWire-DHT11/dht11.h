#ifndef __DHT11_H__
#define __DHT11_H__

/*
    DHT11 temperature sensor driver
*/

#define DHT_OK 0
#define DHT_CHECKSUM_ERROR -1
#define DHT_TIMEOUT_ERROR -2

// == function prototypes =======================================

// void errorHandler(int response);
void setDHT11gpio(int gpio);
int readDHT11(void);
float getTemperature_dht11();
float getHumidity_dht11();
int dht11_getSignalLevel(int usTimeOut, bool state);

#endif //__DHT11_H__
