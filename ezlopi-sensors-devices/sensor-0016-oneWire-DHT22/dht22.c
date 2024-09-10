

/*------------------------------------------------------------------------------

    DHT22 temperature_dht22 & humidity_dht22 sensor AM2302 (DHT22) driver for ESP32

    Jun 2017:	Ricardo Timmermann, new for DHT22

    Code Based on Adafruit Industries and Sam Johnston and Coffe & Beer. Please help
    to improve this code.

    This example code is in the Public Domain (or CC0 licensed, at your option.)

    Unless required by applicable law or agreed to in writing, this
    software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
    CONDITIONS OF ANY KIND, either express or implied.

    PLEASE KEEP THIS CODE IN LESS THAN 0XFF LINES. EACH LINE MAY CONTAIN ONE BUG !!!

---------------------------------------------------------------------------------*/

#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE

#include "../../build/config/sdkconfig.h"
#if (CONFIG_IDF_TARGET_ESP32S3 || CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S2)

// #include "esp_log.h"
#include "driver/gpio.h"
#if CONFIG_IDF_TARGET_ESP32S3
#include "esp32s3/rom/ets_sys.h"
#elif CONFIG_IDF_TARGET_ESP32
#include "esp32/rom/ets_sys.h"
#elif CONFIG_IDF_TARGET_ESP32S2
#include "esp32s2/rom/ets_sys.h"
#endif

#include "dht22.h"

static int DHT22gpio = GPIO_NUM_4; // my default DHT pin = 4
static float humidity_dht22 = 0.;
static float temperature_dht22 = 0.;

// == set the DHT used pin=========================================

void setDHT22gpio(int gpio)
{
    DHT22gpio = gpio;
}

// == get temp & hum =============================================

float getHumidity_dht22() { return humidity_dht22; }
float getTemperature_dht22() { return temperature_dht22; }

// == error handler ===============================================

// void errorHandler(int response)
// {
//     switch (response)
//     {

//     case DHT_TIMEOUT_ERROR:
//         ESP_LOGE(TAG, "Sensor Timeout\n");
//         break;

//     case DHT_CHECKSUM_ERROR:
//         ESP_LOGE(TAG, "CheckSum error\n");
//         break;

//     case DHT_OK:
//         break;

//     default:
//         ESP_LOGE(TAG, "Unknown error\n");
//     }
// }

/*-------------------------------------------------------------------------------
;
;	get next state
;
;	I don't like this logic. It needs some interrupt blocking / priority
;	to ensure it runs in realtime.
;
;--------------------------------------------------------------------------------*/

int dht22_getSignalLevel(int usTimeOut, bool state)
{

    int uSec = 0;
    while (gpio_get_level(DHT22gpio) == state)
    {

        if (uSec > usTimeOut)
            return -1;

        ++uSec;
        ets_delay_us(1); // uSec delay
    }

    return uSec;
}

/*----------------------------------------------------------------------------
;
;	read DHT22 sensor

copy/paste from AM2302/DHT22 Docu:

DATA: Hum = 16 bits, Temp = 16 Bits, check-sum = 8 Bits

Example: MCU has received 40 bits data from AM2302 as
0000 0010 1000 1100 0000 0001 0101 1111 1110 1110
16 bits RH data + 16 bits T data + check sum

1) we convert 16 bits RH data from binary system to decimal system, 0000 0010 1000 1100 → 652
Binary system Decimal system: RH=652/10=65.2%RH

2) we convert 16 bits T data from binary system to decimal system, 0000 0001 0101 1111 → 351
Binary system Decimal system: T=351/10=35.1°C

When highest bit of temperature_dht22 is 1, it means the temperature_dht22 is below 0 degree Celsius.
Example: 1000 0000 0110 0101, T= minus 10.1°C: 16 bits T data

3) Check Sum=0000 0010+1000 1100+0000 0001+0101 1111=1110 1110 Check-sum=the last 8 bits of Sum=11101110

Signal & Timings:

The interval of whole process must be beyond 2 seconds.

To request data from DHT:

1) Sent low pulse for > 1~10 ms (MILI SEC)
2) Sent high pulse for > 20~40 us (Micros).
3) When DHT detects the start signal, it will pull low the bus 80us as response signal,
   then the DHT pulls up 80us for preparation to send data.
4) When DHT is sending data to MCU, every bit's transmission begin with low-voltage-level that last 50us,
   the following high-voltage-level signal's length decide the bit is "1" or "0".
    0: 26~28 us
    1: 70 us

;----------------------------------------------------------------------------*/

#define MAXdht22Data 5 // to complete 40 = 5*8 Bits

int readDHT22()
{
    int uSec = 0;

    uint8_t dhtData[MAXdht22Data];
    uint8_t byteInx = 0;
    uint8_t bitInx = 7;

    for (int k = 0; k < MAXdht22Data; k++)
        dhtData[k] = 0;

    // == Send start signal to DHT sensor ===========

    gpio_set_direction(DHT22gpio, GPIO_MODE_OUTPUT);

    // pull down for 3 ms for a smooth and nice wake up
    gpio_set_level(DHT22gpio, 0);
    ets_delay_us(3000);

    // pull up for 25 us for a gentile asking for data
    gpio_set_level(DHT22gpio, 1);
    ets_delay_us(25);

    gpio_set_direction(DHT22gpio, GPIO_MODE_INPUT); // change to input mode

    // == DHT will keep the line low for 80 us and then high for 80us ====

    uSec = dht22_getSignalLevel(85, 0);
    // ESP_LOGI(TAG, "Response = %d", uSec);
    if (uSec < 0)
        return DHT_TIMEOUT_ERROR;

    // -- 80us up ------------------------

    uSec = dht22_getSignalLevel(85, 1);
    // ESP_LOGI(TAG, "Response = %d", uSec);
    if (uSec < 0)
        return DHT_TIMEOUT_ERROR;

    // == No errors, read the 40 data bits ================

    for (int k = 0; k < 40; k++)
    {

        // -- starts new data transmission with >50us low signal

        uSec = dht22_getSignalLevel(56, 0);
        if (uSec < 0)
            return DHT_TIMEOUT_ERROR;

        // -- check to see if after >70us rx data is a 0 or a 1

        uSec = dht22_getSignalLevel(75, 1);
        if (uSec < 0)
            return DHT_TIMEOUT_ERROR;

        // add the current read to the output data
        // since all dhtData array where set to 0 at the start,
        // only look for "1" (>28us us)

        if (uSec > 40)
        {
            dhtData[byteInx] |= (1 << bitInx);
        }

        // index to next byte

        if (bitInx == 0)
        {
            bitInx = 7;
            ++byteInx;
        }
        else
            bitInx--;
    }

    // == get humidity_dht22 from Data[0] and Data[1] ==========================

    humidity_dht22 = dhtData[0];
    humidity_dht22 *= 0x100; // >> 8
    humidity_dht22 += dhtData[1];
    humidity_dht22 /= 10; // get the decimal

    // == get temp from Data[2] and Data[3]

    temperature_dht22 = dhtData[2] & 0x7F;
    temperature_dht22 *= 0x100; // >> 8
    temperature_dht22 += dhtData[3];
    temperature_dht22 /= 10;

    if (dhtData[2] & 0x80) // negative temp, brrr it's freezing
        temperature_dht22 *= -1;

    // == verify if checksum is ok ===========================================
    // Checksum is the sum of Data 8 bits masked out 0xFF

    if (dhtData[4] == ((dhtData[0] + dhtData[1] + dhtData[2] + dhtData[3]) & 0xFF))
        return DHT_OK;

    else
        return DHT_CHECKSUM_ERROR;
}

#endif // CONFIG_IDF_TARGET_ESP32S3 || CONFIG_IDF_TARGET_ESP32