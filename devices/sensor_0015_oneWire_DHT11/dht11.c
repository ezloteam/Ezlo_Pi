

#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE

#include "esp_log.h"
#include "esp_err.h"
#include "driver/gpio.h"
#include "driver/timer.h"
#include "freertos/FreeRTOS.h"
#include "dht11.h"

/*
 *  Note:
 *  A suitable pull-up resistor should be connected to the selected GPIO line
 *
 *  __           ______          _______                              ___________________________
 *    \    A    /      \   C    /       \   DHT duration_data_low    /                           \
 *     \_______/   B    \______/    D    \__________________________/   DHT duration_data_high    \__
 *
 *
 *  Initializing communications with the DHT requires four 'phases' as follows:
 *
 *  Phase A - MCU pulls signal low for at least 18000 us
 *  Phase B - MCU allows signal to float back up and waits 20-40us for DHT to pull it low
 *  Phase C - DHT pulls signal low for ~80us
 *  Phase D - DHT lets signal float back up for ~80us
 *
 *  After this, the DHT transmits its first bit by holding the signal low for 50us
 *  and then letting it float back high for a period of time that depends on the data bit.
 *  duration_data_high is shorter than 50us for a logic '0' and longer than 50us for logic '1'.
 *
 *  There are a total of 40 data bits transmitted sequentially. These bits are read into a byte array
 *  of length 5.  The first and third bytes are humidity_dht11 (%) and temperature_dht11 (C), respectively.  Bytes 2 and 4
 *  are zero-filled and the fifth is a checksum such that:
 *
 *  byte_5 == (byte_1 + byte_2 + byte_3 + byte_4) & 0xFF
 *
 */

/********************************************************************************/
/*                    global defines                                            */
/********************************************************************************/
// DHT timer precision in microseconds
#define DHT_TIMER_INTERVAL 2
#define DHT_DATA_BITS 40
#define DHT_DATA_BYTES (DHT_DATA_BITS / 8)

#define PORT_ENTER_CRITICAL() portENTER_CRITICAL(&mux)
#define PORT_EXIT_CRITICAL() portEXIT_CRITICAL(&mux)

#define CHECK_LOGE(x, msg, ...)                \
    do                                         \
    {                                          \
        esp_err_t __;                          \
        if ((__ = x) != ESP_OK)                \
        {                                      \
            PORT_EXIT_CRITICAL();              \
            ESP_LOGE(TAG, msg, ##__VA_ARGS__); \
            return __;                         \
        }                                      \
    } while (0)

/********************************************************************************/
/*                    Static defines                                            */
/********************************************************************************/
static int DHT11gpio = GPIO_NUM_4; // my default DHT pin = 4

static const char *TAG = "DHT11";

static portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

/********************************************************************************/
/*                      Function Defination                                     */
/********************************************************************************/

// == set the DHT used pin=========================================

void setDHT11gpio(int gpio)
{
    DHT11gpio = gpio;
}

/**
 * This Function, Waits a specified time for pin to go to a specified state.
 * If timeout is reached and pin doesn't go to a requested state
 * false is returned.
 * The elapsed time is returned in pointer 'duration' if it is not NULL.
 */
static esp_err_t dht11_await_pin_state(gpio_num_t pin, uint32_t timeout,
                                       int expected_pin_state, uint32_t *duration)
{
    /* XXX dht11_await_pin_state() should save pin direction and restore
     * the direction before return. however, the SDK does not provide
     * gpio_get_direction().
     */
    gpio_set_direction(pin, GPIO_MODE_INPUT);
    for (uint32_t i = 0; i < timeout; i += DHT_TIMER_INTERVAL)
    {
        // need to wait at least a single interval to prevent reading a jitter
        esp_rom_delay_us(DHT_TIMER_INTERVAL);
        if (gpio_get_level(pin) == expected_pin_state)
        {
            if (duration)
            {
                *duration = i;
            }
            return ESP_OK;
        }
    }
    return ESP_ERR_TIMEOUT;
}

/**
 * Request data from DHT and read raw bit stream.
 * The function call should be protected from task switching.
 * Return false if error occurred.
 */
static inline esp_err_t dht11_fetch_data(gpio_num_t pin, uint8_t data[DHT_DATA_BYTES])
{
    uint32_t low_duration;
    uint32_t high_duration;

    // Phase 'A' pulling signal low to initiate read sequence
    gpio_set_direction(pin, GPIO_MODE_OUTPUT_OD);
    gpio_set_level(pin, 0);
    esp_rom_delay_us(20000);
    gpio_set_level(pin, 1);

    // uint32_t test_dur = 0;

    // Step through Phase 'B', 40us
    CHECK_LOGE(dht11_await_pin_state(pin, 40, 0, NULL),
               "Initialization error, problem in phase 'B'");
    // Step through Phase 'C', 88us
    CHECK_LOGE(dht11_await_pin_state(pin, 88, 1, NULL),
               "Initialization error, problem in phase 'C'");
    // Step through Phase 'D', 88us
    CHECK_LOGE(dht11_await_pin_state(pin, 88, 0, NULL),
               "Initialization error, problem in phase 'D'");

    // Read in each of the 40 bits of data...
    for (int i = 0; i < DHT_DATA_BITS; i++)
    {
        CHECK_LOGE(dht11_await_pin_state(pin, 65, 1, &low_duration),
                   "LOW bit timeout");
        CHECK_LOGE(dht11_await_pin_state(pin, 75, 0, &high_duration),
                   "HIGH bit timeout");
        uint8_t b = i / 8;
        uint8_t m = i % 8;
        if (!m)
        {
            data[b] = 0;
        }
        data[b] |= (high_duration > low_duration) << (7 - m);
    }

    return ESP_OK;
}

/**
 * Read 'Temp' & 'Humi' function
 *
 */
esp_err_t dht11_read_data(float *humidity_dht11, float *temperature_dht11)
{
    uint8_t data[DHT_DATA_BYTES] = {0};

    gpio_set_direction(DHT11gpio, GPIO_MODE_OUTPUT_OD);
    gpio_set_level(DHT11gpio, 1);

    PORT_ENTER_CRITICAL();
    esp_err_t result = dht11_fetch_data(DHT11gpio, data);
    if (result == ESP_OK)
    {
        PORT_EXIT_CRITICAL();
    }
    /* restore GPIO direction because, after calling dht11_fetch_data(), the
     * GPIO direction mode changes */
    gpio_set_direction(DHT11gpio, GPIO_MODE_OUTPUT_OD);
    gpio_set_level(DHT11gpio, 1);

    if (result != ESP_OK)
    {
        return result; // Break away from this function ,if result => error
    }

    if (data[4] != ((data[0] + data[1] + data[2] + data[3]) & 0xFF))
    {
        ESP_LOGE(TAG, "Checksum failed, invalid data received from sensor");
        return ESP_ERR_INVALID_CRC;
    }
    // ESP_LOGE("READ__TAG", "data[0] = %d", data[0]);
    // ESP_LOGE("READ__TAG", "data[1] = %d", data[1]);
    // ESP_LOGE("READ__TAG", "data[2] = %d", data[2]);
    // ESP_LOGE("READ__TAG", "data[3] = %d", data[3]);
    if (NULL != humidity_dht11)
    {
        *humidity_dht11 = (data[0] + (data[1] * 0.1));
    }
    if (NULL != temperature_dht11)
    {
        *temperature_dht11 = (data[2] + (data[3] * 0.1));
    }

    return ESP_OK;
}

/******************************************************************************/

/*-------------------------------------------------------------------------------
;	get DHT data
;   ************
;   This function extracts data using one-wire protocol
;
;   -> DATA: Hum = 16 bits, Temp = 16 Bits, check-sum = 8 Bits
;
;   Example: MCU has received 40 bits data from AM2302 as
;   0000 0010 1000 1100 0000 0001 0101 1111 1110 1110
;   16 bits RH data + 16 bits T data + check sum
;
;   1)  bits[1] = x * 0.1;   => LSB  of Humidity_dht11
;       bits[3] = y * 0.1;   => LSB  of Temperature_dht11
;
;   2)  humidity_dht11    = bits[0];   => MSB  of Humidity_dht11
;       temperature_dht11 = bits[2];   => MSB  of Temperature_dht11
;
;   3) Check Sum=0000 0010+1000 1100+0000 0001+0101 1111=1110 1110 Check-sum=the last 8 bits of Sum=11101110
;
;   Signal & Timings:
;
;   The interval of whole process must be beyond 2 seconds.
;
;       To request data from DHT:
;
;   1) Sent low pulse for > 18 ms (MILI SEC)
;   2) Sent high pulse for > 20~40 us (Micros).
;   3) When DHT detects the start signal, it will pull low the bus 80us as response signal,
;      then the DHT pulls up 80us for preparation to send data.
;   4) When DHT is sending data to MCU, every bit's transmission begin with low-voltage-level that last 50us,
;      the following high-voltage-level signal's length decide the bit is "1" or "0".
;       0: 26~28 us
;       1: 70 us
;
;----------------------------------------------------------------------------*/
