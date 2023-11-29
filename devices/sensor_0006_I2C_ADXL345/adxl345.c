#include "trace.h"
#include "esp_err.h"
#include "ezlopi_cloud_item_name_str.h"
#include "sensor_0006_I2C_ADXL345.h"

//------------------------------------------------------------------------------

static esp_err_t get_device_id(l_ezlopi_item_t *item)
{
    if (item)
    {
        uint8_t dev_id = 0;
        uint8_t write_buffer[] = {ADXL345_DEVICE_ID_REGISTER}; // REG_INTR_STATUS;
        ezlopi_i2c_master_write_to_device(&item->interface.i2c_master, write_buffer, 1);
        ezlopi_i2c_master_read_from_device(&item->interface.i2c_master, &dev_id, 1);
        vTaskDelay(10);
        TRACE_B("The device id is %d", dev_id);
    }
    return ESP_OK;
}
static esp_err_t data_formatting(l_ezlopi_item_t *item)
{
    if (item)
    {
        uint8_t write_byte[] = {ADXL345_DATA_FORMAT_REGISTER, ADXL345_FORMAT_REGISTER_DATA};
        ezlopi_i2c_master_write_to_device(&item->interface.i2c_master, write_byte, 2);
        vTaskDelay(10);
    }
    return ESP_OK;
}
static esp_err_t set_to_measure_mode(l_ezlopi_item_t *item)
{
    if (item)
    {
        uint8_t write_byte[] = {ADXL345_DEVICE_POWER_CTRL, ADXL345_POWER_CTRL_SET_TO_MEASUTEMENT};
        ezlopi_i2c_master_write_to_device(&item->interface.i2c_master, write_byte, 2);
        vTaskDelay(10);
    }
    return ESP_OK;
}
static esp_err_t enable_data_ready_interrupt(l_ezlopi_item_t *item)
{
    if (item)
    {
        uint8_t write_byte[] = {ADXL345_INT_ENABLE_REGISTER, ADXL345_INT_EN};
        ezlopi_i2c_master_write_to_device(&item->interface.i2c_master, write_byte, 2);
        vTaskDelay(10);
    }
    return ESP_OK;
}
static esp_err_t reset_measure_mode(l_ezlopi_item_t *item)
{
    if (item)
    {
        uint8_t write_byte[] = {ADXL345_DEVICE_POWER_CTRL, ADXL345_POWER_CTRL_RESET};
        ezlopi_i2c_master_write_to_device(&item->interface.i2c_master, write_byte, 2);
        vTaskDelay(10);
    }
    return ESP_OK;
}
static esp_err_t adxl345_check_data_ready_INTR(l_ezlopi_item_t *item, uint8_t *temp)
{
    esp_err_t err = ESP_OK;
    if (item)
    {
        uint8_t write_buffer[] = {ADXL345_INT_SOURCE_REGISTER}; // REG_INTR_STATUS;
        ezlopi_i2c_master_write_to_device(&item->interface.i2c_master, write_buffer, 1);
        // Read -> INTR_BIT
        ezlopi_i2c_master_read_from_device(&item->interface.i2c_master, temp, 1);
        if (NULL != temp)
        {
            err = ESP_OK;
        }
        else
        {
            err = ESP_ERR_TIMEOUT;
        }
    }
    return err;
}

int __adxl345_configure_device(l_ezlopi_item_t *item)
{
    int ret = 0;
    if (item)
    {
        ESP_ERROR_CHECK(reset_measure_mode(item));
        ESP_ERROR_CHECK(data_formatting(item));
        ESP_ERROR_CHECK(get_device_id(item));
        ESP_ERROR_CHECK(set_to_measure_mode(item));
        ESP_ERROR_CHECK(enable_data_ready_interrupt(item));
    }
    return ret;
}
int16_t __adxl345_get_axis_value(l_ezlopi_item_t *item)
{
    int16_t axis_data = 0;
    if (item)
    {
        uint8_t buffer_0 = 0, buffer_1 = 0;
        uint8_t Check_Register = 0;
        uint8_t address_val;
        esp_err_t err = ESP_OK;
        if ((err = adxl345_check_data_ready_INTR(item, &Check_Register)) == ESP_OK)
        {
            TRACE_I("chk: %d:",Check_Register);
            if ((Check_Register & (1 << 7)))
            {
                if (ezlopi_item_name_acceleration_x_axis == item->cloud_properties.item_name)
                {
                    address_val = ADXL345_DATA_X_0_REGISTER;
                }
                if (ezlopi_item_name_acceleration_y_axis == item->cloud_properties.item_name)
                {
                    address_val = ADXL345_DATA_Y_0_REGISTER;
                }
                if (ezlopi_item_name_acceleration_z_axis == item->cloud_properties.item_name)
                {
                    address_val = ADXL345_DATA_Z_0_REGISTER;
                }
                ezlopi_i2c_master_write_to_device(&item->interface.i2c_master, &address_val, 1);
                ezlopi_i2c_master_read_from_device(&item->interface.i2c_master, (&buffer_0), 1);

                address_val++; // next register
                ezlopi_i2c_master_write_to_device(&item->interface.i2c_master, &address_val, 1);
                ezlopi_i2c_master_read_from_device(&item->interface.i2c_master, (&buffer_1), 1);
            }
        }
        else
        {
            TRACE_E("Data not ready... Error type:-ESP_ERR_%d ", (err));
        }

        axis_data = (int16_t)((buffer_1 << 8) | buffer_0);
    }
    return axis_data;
}