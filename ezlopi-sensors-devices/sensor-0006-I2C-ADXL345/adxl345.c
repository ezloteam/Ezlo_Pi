#include "ezlopi_util_trace.h"
#include "esp_err.h"
#include "ezlopi_cloud_item_name_str.h"
#include "sensor_0006_I2C_ADXL345.h"
#include "ezlopi_core_errors.h"

//------------------------------------------------------------------------------

static esp_err_t get_device_id(l_ezlopi_item_t *item)
{
    esp_err_t err = ESP_OK;
    if (item)
    {
        uint8_t dev_id = 0;
        uint8_t write_buffer[] = {ADXL345_DEVICE_ID_REGISTER}; // REG_INTR_STATUS;
        err = ezlopi_i2c_master_write_to_device(&item->interface.i2c_master, write_buffer, 1);
        err = ezlopi_i2c_master_read_from_device(&item->interface.i2c_master, &dev_id, 1);
        TRACE_E("The device id is {%#x}", dev_id);
    }
    return err;
}
static esp_err_t data_formatting(l_ezlopi_item_t *item)
{
    esp_err_t err = ESP_OK;
    if (item)
    {
        uint8_t write_byte[] = {ADXL345_DATA_FORMAT_REGISTER, ADXL345_FORMAT_REGISTER_DATA};
        err = ezlopi_i2c_master_write_to_device(&item->interface.i2c_master, write_byte, 2);
    }
    return err;
}
static esp_err_t set_to_measure_mode(l_ezlopi_item_t *item)
{
    esp_err_t err = ESP_OK;
    if (item)
    {
        uint8_t write_byte[] = {ADXL345_DEVICE_POWER_CTRL, ADXL345_POWER_CTRL_SET_TO_MEASUTEMENT};
        err = ezlopi_i2c_master_write_to_device(&item->interface.i2c_master, write_byte, 2);
    }
    return err;
}
static esp_err_t enable_data_ready_interrupt(l_ezlopi_item_t *item)
{
    esp_err_t err = ESP_OK;
    if (item)
    {
        uint8_t write_byte[] = {ADXL345_INT_ENABLE_REGISTER, ADXL345_INT_EN};
        err = ezlopi_i2c_master_write_to_device(&item->interface.i2c_master, write_byte, 2);
    }
    return err;
}
static esp_err_t reset_measure_mode(l_ezlopi_item_t *item)
{
    esp_err_t err = ESP_OK;
    if (item)
    {
        uint8_t write_byte[] = {ADXL345_DEVICE_POWER_CTRL, ADXL345_POWER_CTRL_RESET};
        err = ezlopi_i2c_master_write_to_device(&item->interface.i2c_master, write_byte, 2);
    }
    return err;
}
static esp_err_t adxl345_check_data_ready_INTR(l_ezlopi_item_t *item, uint8_t *temp)
{
    esp_err_t err = ESP_OK;
    if (item)
    {
        uint8_t write_buffer[] = {ADXL345_INT_SOURCE_REGISTER}; // REG_INTR_STATUS;
        ezlopi_i2c_master_write_to_device(&item->interface.i2c_master, write_buffer, 1);
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

ezlopi_error_t __adxl345_configure_device(l_ezlopi_item_t *item)
{
    ezlopi_error_t ret = EZPI_FAILED;
    if (item)
    {
        ret = (int)(reset_measure_mode(item));
        ret = (int)(data_formatting(item));
        ret = (int)(get_device_id(item));
        ret = (int)(set_to_measure_mode(item));
        ret = (int)(enable_data_ready_interrupt(item));
        ret = EZPI_SUCCESS;
    }
    return ret;
}
void __adxl345_get_axis_value(l_ezlopi_item_t *item)
{
    if (item)
    {
        s_adxl345_data_t *user_data = (s_adxl345_data_t *)item->user_arg;
        bool valid_data = false;
        static uint8_t buffer[ADXL345_ODR_CNT] = {0};
        uint8_t Check_Register = 0;
        uint8_t address_val;
        esp_err_t err = ESP_OK;
        if ((err = adxl345_check_data_ready_INTR(item, &Check_Register)) == ESP_OK)
        {
            // TRACE_S("chk: Reg @ 0x30H:{%#x}:", Check_Register);
            if ((Check_Register & ADXL345_DATA_READY_FLAG))
            {
                address_val = ADXL345_DATA_X_0_REGISTER;
                ezlopi_i2c_master_write_to_device(&item->interface.i2c_master, &address_val, 1);
                ezlopi_i2c_master_read_from_device(&item->interface.i2c_master, (buffer), ADXL345_ODR_CNT);
            }
            valid_data = true;
        }
        else
        {
            TRACE_E("Data not ready......");
            for (uint8_t try = 10; ((try > 0) && (!(Check_Register & ADXL345_DATA_READY_FLAG))); try--)
            {
                if (ESP_OK == adxl345_check_data_ready_INTR(item, &Check_Register))
                {
                    TRACE_W("Reg @ 0x30H: -> {%#x}", Check_Register);
                    break;
                }
                TRACE_W("Extracting.... Reg @ 0x30H: -> {%#x}", Check_Register);
            }
        }

        if (valid_data)
        {
            user_data->acc_x = (float)((int16_t)((buffer[1] << 8) | buffer[0]));
            user_data->acc_y = (float)((int16_t)((buffer[3] << 8) | buffer[2]));
            user_data->acc_z = (float)((int16_t)((buffer[5] << 8) | buffer[4]));
        }
        else
        {
            TRACE_E("........Invalid Data");
        }
    }
}