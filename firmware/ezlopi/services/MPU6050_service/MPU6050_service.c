#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "debug.h"
#include "mpu6050.h"
#include "string.h"
#include "items.h"
#include "wss.h"
#include "driver/i2c.h"

static uint16_t accel_x;
static uint16_t accel_y;
static uint16_t accel_z;
static uint16_t gyro_x;
static uint16_t gyro_y;
static uint16_t gyro_z;
static uint32_t device_index = 0xFF;
static uint8_t SDA = 22;
static uint8_t SCL = 23;

static void mpu_service_process(void *pv);
uint32_t web_provisioning_get_message_count(void);

void mpu_service_init(uint8_t scl_pin, uint8_t sda_pin, uint32_t dev_idx)
{
    SDA = sda_pin;
    SCL = scl_pin;
    i2c_master_init(I2C_NUM_0, sda_pin, scl_pin, 100000);
    xTaskCreatePinnedToCore(mpu_service_process, "mpu6050-service", 3072, NULL, 3, NULL, 1);
}

static void mpu_service_process(void *pv)
{
    while (1)
    {
        // ESP_ERROR_CHECK(MPU6050_i2c_master_init(SDA, SCL));
        ESP_ERROR_CHECK(mpu6050_wake());
        MPU_TASK();

        char *ret = items_update_with_device_index(NULL, 0, NULL, web_provisioning_get_message_count(), device_index);

        if (ret)
        {
            // TRACE_W(">> DHT-service TX(ret): %s", ret);
            wss_client_send(ret, strlen(ret));
            vPortFree(ret);
            ret = NULL;
        }

        vTaskDelay(5000 / portTICK_RATE_MS);
    }
}