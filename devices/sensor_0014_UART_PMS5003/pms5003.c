
#include "sensor_0014_UART_PMS5003.h"
#include "trace.h"
#include "ezlopi_uart.h"

static const char *TAG = "PMS";
uint8_t volatile pmsStatusReg = 0x00;
/*
 * pmsStatusReg =  | X | X | X | X | X | EN_READ | SLEEP_MODE | ACTIVE_MODE |
 *                 | 7 | 6 | 5 | 4 | 3 |   2     |      1     |      0     |
 */

static void pms_timer_callback(void* arg);

static void ezlopi_pms5003_upcall(uint8_t* buffer, s_ezlopi_uart_object_handle_t uart_object_handle, void* user_args);

/*!
     @brief  Setups the hardware and detects a valid UART PM2.5
*/
// void pms_init(PMS_sensor_structure_t* pms5003_sensor_structure)
// {
//   pms_uart_setup(pms5003_sensor_structure, 0, 0, 0); /* Passing the data parameter for the uart tasl : ezlopi_uart_channel_task() */
//   pms_setup_control_gpio(pms5003_sensor_structure->pms_set_pin, pms5003_sensor_structure->pms_reset_pin);
//   pms_startup(pms5003_sensor_structure->pms_set_pin, pms5003_sensor_structure->pms_reset_pin);
//   pms_create_sleep_timer(pms5003_sensor_structure->pms_data); /* Passing the data parameter for the timer callback : pms_timer_callback()*/
// }

/*!
    @brief Install UART Driver for PMS5003.
*/
void pms_uart_setup(PMS_sensor_structure_t* pms5003_sensor_structure, uint32_t tx, uint32_t rx, uint32_t baudrate)
{
  TRACE_B("UART setup!!");
  ezlopi_uart_init(baudrate, tx, rx, ezlopi_pms5003_upcall, &pms5003_sensor_structure->pms_data);
}

/*!
    @brief Configures Control GPIO pins (SET & RESET) for PMS5003.
*/
esp_err_t pms_setup_control_gpio(gpio_num_t set_pin, gpio_num_t reset_pin)
{
  TRACE_B("GPIO setup")
  esp_err_t ret = ESP_OK;
  ret |= pms_gpio_config_output(reset_pin);
  ret |= pms_gpio_config_output(set_pin);

  return ret;
}

/*!
    @brief Configures GPIO pins (SET & RESET) for PMS5003.
*/
esp_err_t pms_gpio_config_output(gpio_num_t pin)
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
void pms_startup(gpio_num_t set_pin, gpio_num_t reset_pin)
{
  TRACE_B("PMS startup!!");
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

void pms_create_sleep_timer(PMS_sensor_structure_t* pms5003_sensor_structure)
{
  TRACE_B("Creating sleep timer");
  const esp_timer_create_args_t pms_sleep_timer_args = {
      .callback = &pms_timer_callback,
      .name = "pms_sleep_mode_timer",
      .arg = pms5003_sensor_structure
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
  PMS_sensor_structure_t *pms5003_sensor_structure = (PMS_sensor_structure_t *)arg;
  // TRACE_B("Sec is %d, PMS_TOTAL_TIME %d", sec, PMS_TOTAL_TIME);
  if (sec <= PMS_ACTIVE_TIME)
  {

    if (!PMS_CHECK_ACTIVE_STATUS(pmsStatusReg))
    {
      TRACE_B("set pin is %d", pms5003_sensor_structure->pms_set_pin)
      pms_active_mode(pms5003_sensor_structure->pms_set_pin);
      PMS_SET_ACTIVE_STATUS(pmsStatusReg);
    }

    if (sec == PMS_STABILITY_TIME)
    {
      PMS_SET_READ_STATUS(pmsStatusReg); // Enable Read Status
    }
  }

  else if (sec <= PMS_TOTAL_TIME)
  {
    if (!PMS_CHECK_SLEEP_STATUS(pmsStatusReg))
    {
      PMS_SET_SLEEP_STATUS(pmsStatusReg);
      pms5003_sensor_structure->pms_data->available = true;
      pms_print_data(pms5003_sensor_structure->pms_data);
      pms_sleep_mode(pms5003_sensor_structure->pms_set_pin);
    }
  }
  
  else if (sec > PMS_TOTAL_TIME)
  {
    sec = 0; // Reset
    pmsStatusReg = 0;
  }
}
/*!
 *  @brief  Set PMS5003 in Sleep-mode and stop the fan.
 */
void pms_sleep_mode(gpio_num_t set_pin)
{
  gpio_set_level(set_pin, 0);
}
/*!
 *  @brief  Set PMS5003 in Active-mode and start the fan.
 */
void pms_active_mode(gpio_num_t set_pin)
{
  gpio_set_level(set_pin, 1);
}

/*!
 *  @brief  Prints the PMS Sensor Data
 *  @param  data
 *          Pointer to PM25_AQI_Data that was filled by pms_read()
 */
// void pms_print_data(PM25_AQI_Data* data)
// {
//   if(data->available == true)
//   {
//     ESP_LOGI(TAG ,"AQI reading success");

//     ESP_LOGI(TAG,"---------------------------------------");
//     ESP_LOGI(TAG,"Concentration Units (standard)");
//     ESP_LOGI(TAG,"---------------------------------------");
//     ESP_LOGI(TAG,"PM 1.0: %d", data->pm10_standard);
//     ESP_LOGI(TAG,"PM 2.5: %d", data->pm25_standard);
//     ESP_LOGI(TAG,"PM 10:  %d", data->pm100_standard);

//     ESP_LOGI(TAG,"---------------------------------------");
//     ESP_LOGI(TAG,"Concentration Units (environmental)");
//     ESP_LOGI(TAG,"---------------------------------------");
//     ESP_LOGI(TAG,"PM 1.0: %d", data->pm10_env);
//     ESP_LOGI(TAG,"PM 2.5: %d", data->pm25_env);
//     ESP_LOGI(TAG,"PM 10:  %d", data->pm100_env);

//     ESP_LOGI(TAG,"---------------------------------------");
//     ESP_LOGI(TAG,"Particles > 0.3um / 0.1L air: %d", data->particles_03um);
//     ESP_LOGI(TAG,"Particles > 0.5um / 0.1L air: %d", data->particles_05um);
//     ESP_LOGI(TAG,"Particles > 1.0um / 0.1L air: %d", data->particles_10um);
//     ESP_LOGI(TAG,"Particles > 2.5um / 0.1L air: %d", data->particles_25um);
//     ESP_LOGI(TAG,"Particles > 5.0um / 0.1L air: %d", data->particles_50um);
//     ESP_LOGI(TAG,"Particles > 10 um / 0.1L air: %d", data->particles_100um);
//     ESP_LOGI(TAG,"---------------------------------------");
//     data->available = false;
//   }

// }

void pms_print_data(PM25_AQI_Data *data)
{
  TRACE_B("AQI reading success");

  TRACE_B("---------------------------------------");
  TRACE_B("Concentration Units (standard)");
  TRACE_B("---------------------------------------");
  TRACE_B("PM 1.0: %f", data->pm10_standard);
  TRACE_B("PM 2.5: %f", data->pm25_standard);
  TRACE_B("PM 10:  %f", data->pm100_standard);

  TRACE_B("---------------------------------------");
  TRACE_B("Concentration Units (environmental)");
  TRACE_B("---------------------------------------");
  TRACE_B("PM 1.0: %f", data->pm10_env);
  TRACE_B("PM 2.5: %f", data->pm25_env);
  TRACE_B("PM 10:  %f", data->pm100_env);

  TRACE_B("---------------------------------------");
  TRACE_B("Particles > 0.3um / 0.1L air: %f", data->particles_03um);
  TRACE_B("Particles > 0.5um / 0.1L air: %f", data->particles_05um);
  TRACE_B("Particles > 1.0um / 0.1L air: %f", data->particles_10um);
  TRACE_B("Particles > 2.5um / 0.1L air: %f", data->particles_25um);
  TRACE_B("Particles > 5.0um / 0.1L air: %f", data->particles_50um);
  TRACE_B("Particles > 10 um / 0.1L air: %f", data->particles_100um);
  TRACE_B("---------------------------------------");
}

static void ezlopi_pms5003_upcall(uint8_t *buffer, s_ezlopi_uart_object_handle_t uart_object_handle, void *user_args)
{
  PMS_sensor_structure_t* pms5003_sensor_structure = (PMS_sensor_structure_t *)user_args;
  pms_read_upcall(buffer, pms5003_sensor_structure->pms_data);
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
    return ESP_FAIL;
  }
  /* success! */
  return ESP_OK;
}

/*!
 *  @brief  Function to read PMS sensor with default uart. ( without using ezlopi_uart_init() & _upcall functions )
 *  @param  data
 *          Pointer to PM25_AQI_Data that will be filled by read()ing
 *  @return ESP_OK on successful read, ESP_FAIL if timed out or bad data
 */
esp_err_t pms_read(PMS_sensor_structure_t* pms5003_sensor_structure, int channel)
{
  if ((pmsStatusReg & 0x01) && (pmsStatusReg & 0x04))
  {

    /*  pmsStatusReg : Update from the "pms_timer_callback".
        If Active Mode and EN_Read is HIGH, this function will read the PMS_Sensor Data
    */
    uint8_t buffer[128];
    uint16_t sum = 0;
    memset(pms5003_sensor_structure->pms_data, 0, sizeof(PM25_AQI_Data));

    if (!pms5003_sensor_structure->pms_data)
    {
      return ESP_FAIL;
    }

    int length = 0;
    length = uart_read_bytes(channel, buffer, 128, 1);

    /* Length test */
    if (length < 32)
      return ESP_FAIL;

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
    memcpy((void *)pms5003_sensor_structure->pms_data, (void *)buffer_u16, 30);

    if (sum != pms5003_sensor_structure->pms_data->checksum)
    {
      return ESP_FAIL;
    }
    ESP_LOGE(TAG, "PMS_READ DONE");
    /* success! */
    return ESP_OK;
  }
  else
    return ESP_FAIL;
}

bool pms_is_data_available(PM25_AQI_Data *data)
{
  bool ret = false;
  {
    if (NULL != data)
    {
      ret = data->available;
    }
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


