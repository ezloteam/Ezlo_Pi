#include "pms5003.h"

static const char *TAG = "PMS";


static void pms_uart_setup(s_pms5003_sensor_object *pms_object);
static esp_err_t pms_setup_control_gpio(gpio_num_t set_pin, gpio_num_t reset_pin);
static esp_err_t pms_gpio_config_output(gpio_num_t pin);
static void pms_startup(uint32_t set_pin, uint32_t reset_pin);
static esp_err_t pms_read_upcall(uint8_t *buffer, PM25_AQI_Data *pms_data);
static void pms_sleep_mode(gpio_num_t set_pin);
static void pms_active_mode(gpio_num_t set_pin);
static void pms_create_sleep_timer(s_pms5003_sensor_object *pms_object);
static void pms_timer_callback(void *arg);
static void ezlopi_pms5003_upcall(uint8_t *buffer, uint32_t output_len, s_ezlopi_uart_object_handle_t uart_object_handle);
static void pms_timer_callback(void *arg);

/*!
     @brief  Setups the hardware and detects a valid UART PM2.5
*/
void pms_init(s_pms5003_sensor_object *pms_object)
{
  pms_uart_setup(pms_object); /* Passing the data parameter for the uart tasl : ezlopi_uart_channel_task() */
  pms_setup_control_gpio(pms_object->pms_set_pin, pms_object->pms_reset_pin);
  pms_startup(pms_object->pms_set_pin, pms_object->pms_reset_pin);
  pms_create_sleep_timer(pms_object); /* Passing the data parameter for the timer callback : pms_timer_callback()*/
}

/*!
    @brief Install UART Driver for PMS5003.
*/
static void pms_uart_setup(s_pms5003_sensor_object *pms_object)
{
  ezlopi_uart_init(pms_object->pms_baud_rate, pms_object->pms_tx_pin, pms_object->pms_rx_pin, ezlopi_pms5003_upcall, &pms_object->pms_data);
}

/*!
    @brief Configures Control GPIO pins (SET & RESET) for PMS5003.
*/
static esp_err_t pms_setup_control_gpio(gpio_num_t set_pin, gpio_num_t reset_pin)
{
  esp_err_t ret = ESP_OK;
  ret |= pms_gpio_config_output(reset_pin);
  ret |= pms_gpio_config_output(set_pin);

  return ret;
}

/*!
    @brief Configures GPIO pins (SET & RESET) for PMS5003.
*/
static esp_err_t pms_gpio_config_output(gpio_num_t pin)
{
  esp_err_t ret = ESP_OK;

  gpio_config_t io_conf;
  io_conf.intr_type = GPIO_INTR_DISABLE;
  io_conf.mode = GPIO_MODE_OUTPUT;
  io_conf.pin_bit_mask = (1ULL << pin);
  io_conf.pull_down_en = GPIO_PULLDOWN_ENABLE;
  io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
  ret |= gpio_config(&io_conf);

  return ret;
}

/*!
    @brief Starts the PMS5003 in sleep-mode.
*/
static void pms_startup(uint32_t set_pin, uint32_t reset_pin)
{
  pms_sleep_mode(set_pin);
  gpio_set_level(reset_pin, 1);
}
/*!
 *  @brief  Setup timer for PMS5003 Sleep Mode.
            [Purpose: To extend the life span of the PMS5003 sensor.]
            [Approach: Activate the sensor for 30s after every 2-Mintues, do the reading and then let’s it go back to sleep.]
            [source : https://forum.airgradient.com/t/extending-the-life-span-of-the-pms5003-sensor/114/9]

 *  @return ESP_OK on success, ESP_FAIL if failed to create the timer.
 */

static void pms_create_sleep_timer(s_pms5003_sensor_object *pms_object)
{
  const esp_timer_create_args_t pms_sleep_timer_args = {
      .callback = &pms_timer_callback,
      .name = "pms_sleep_mode_timer",
      .arg = pms_object,
  };

  esp_timer_handle_t pms_sleep_timer;
  ESP_ERROR_CHECK(esp_timer_create(&pms_sleep_timer_args, &pms_sleep_timer));
  /* The timer has been created but is not running yet */

  /* Start the timer of 1sec*/
  ESP_ERROR_CHECK(esp_timer_start_periodic(pms_sleep_timer, 1000000));
}

static void pms_timer_callback(void *arg)
{
  static int sec;
  sec++;
  s_pms5003_sensor_object *pms_object = (s_pms5003_sensor_object *)arg;

  if (sec <= pms_object->pms_active_time)
  {
    if (!PMS_CHECK_ACTIVE_STATUS(pms_object->pmsStatusReg))
    {
      pms_active_mode(pms_object->pms_set_pin);
      PMS_SET_ACTIVE_STATUS(pms_object->pmsStatusReg);
    }

    if (sec == pms_object->pms_stability_time)
    {
      PMS_SET_READ_STATUS(pms_object->pmsStatusReg); // Enable Read Status
    }
  }

  else if (sec <= PMS_GET_TOTAL_TIME(pms_object->pms_active_time, pms_object->pms_sleep_time))
  {
    if (!PMS_CHECK_SLEEP_STATUS(pms_object->pmsStatusReg))
    {
      PMS_SET_SLEEP_STATUS(pms_object->pmsStatusReg);
      pms_object->pms_data.available = true;
      pms_sleep_mode(pms_object->pms_set_pin);
    }
  }

  else if (sec > PMS_GET_TOTAL_TIME(pms_object->pms_active_time, pms_object->pms_sleep_time))
  {
    sec = 0; // Reset
    pms_object->pmsStatusReg = 0;
  }
}
/*!
 *  @brief  Set PMS5003 in Sleep-mode and stop the fan.
 */
static void pms_sleep_mode(gpio_num_t set_pin)
{
  gpio_set_level(set_pin, 0);
}
/*!
 *  @brief  Set PMS5003 in Active-mode and start the fan.
 */
static void pms_active_mode(gpio_num_t set_pin)
{
  gpio_set_level(set_pin, 1);
}

/*!
 *  @brief  Prints the PMS Sensor Data
 *  @param  data
 *          Pointer to PM25_AQI_Data that was filled by pms_read()
 */
void pms_print_data(PM25_AQI_Data *data)
{
  ESP_LOGI(TAG, "AQI reading success");

  ESP_LOGI(TAG, "---------------------------------------");
  ESP_LOGI(TAG, "Concentration Units (standard)");
  ESP_LOGI(TAG, "---------------------------------------");
  ESP_LOGI(TAG, "PM 1.0: %d", data->pm10_standard);
  ESP_LOGI(TAG, "PM 2.5: %d", data->pm25_standard);
  ESP_LOGI(TAG, "PM 10:  %d", data->pm100_standard);

  ESP_LOGI(TAG, "---------------------------------------");
  ESP_LOGI(TAG, "Concentration Units (environmental)");
  ESP_LOGI(TAG, "---------------------------------------");
  ESP_LOGI(TAG, "PM 1.0: %d", data->pm10_env);
  ESP_LOGI(TAG, "PM 2.5: %d", data->pm25_env);
  ESP_LOGI(TAG, "PM 10:  %d", data->pm100_env);

  ESP_LOGI(TAG, "---------------------------------------");
  ESP_LOGI(TAG, "Particles > 0.3um / 0.1L air: %d", data->particles_03um);
  ESP_LOGI(TAG, "Particles > 0.5um / 0.1L air: %d", data->particles_05um);
  ESP_LOGI(TAG, "Particles > 1.0um / 0.1L air: %d", data->particles_10um);
  ESP_LOGI(TAG, "Particles > 2.5um / 0.1L air: %d", data->particles_25um);
  ESP_LOGI(TAG, "Particles > 5.0um / 0.1L air: %d", data->particles_50um);
  ESP_LOGI(TAG, "Particles > 10 um / 0.1L air: %d", data->particles_100um);
  ESP_LOGI(TAG, "---------------------------------------");
}

static void ezlopi_pms5003_upcall(uint8_t *buffer, uint32_t output_len, s_ezlopi_uart_object_handle_t uart_object_handle)
{
  PM25_AQI_Data *data = (PM25_AQI_Data *)uart_object_handle->arg;
  pms_read_upcall(buffer, data);
}

esp_err_t pms_read_upcall(uint8_t *buffer, PM25_AQI_Data *data)
{
  uint16_t sum = 0;
  memset(data, 0, sizeof(PM25_AQI_Data));
  /* Check that start byte is correct! */
  if (buffer[0] != 0x42)
  {
    return ESP_FAIL;
  }

  /* Get checksum ready */
  for (uint8_t i = 0; i < 30; i++)
  {
    sum += buffer[i];
  }

  /* The data comes in endian'd, this solves it so it works on all platforms */
  uint16_t buffer_u16[15];
  for (uint8_t i = 0; i < 15; i++)
  {
    buffer_u16[i] = buffer[2 + i * 2 + 1];
    buffer_u16[i] += (buffer[2 + i * 2] << 8);
  }

  /* put it into a nice struct :) */
  memcpy((void *)data, (void *)buffer_u16, 30);

  if (sum != data->checksum)
  {
    ESP_LOGE("", "CheckSum Failed");
    return ESP_FAIL;
  }
  /* success! */
  return ESP_OK;
}

bool pms_is_data_available(PM25_AQI_Data *data)
{
  bool ret = false;

  if (NULL != data)
  {
    ret = data->available;
  }

  return ret;
}

void pms_set_data_available_to_false(PM25_AQI_Data *data)
{
  if (NULL != data)
  {
    data->available = false;
  }
}
