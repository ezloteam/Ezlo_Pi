#ifndef __DHT11_H__
#define __DHT11_H__

/*
    DHT11 temperature sensor driver
*/

#define DHT_OK 0
#define DHT_CHECKSUM_ERROR -1
#define DHT_TIMEOUT_ERROR -2

// == function prototypes =======================================

void setDHT11gpio(int gpio);
// void errorHandler(int response);
// int readDHT(void);
// float get_dht11_Humidity();
// float get_dht11_Temperature();
// int getSignalLevel(int usTimeOut, bool state);
esp_err_t dht11_read_data(float *humidity_dht11, float *temperature_dht11);

#endif //__DHT11_H__
