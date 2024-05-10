
#include "gxhtc3.h"
#include "driver/i2c.h"
#include "ezlopi_util_trace.h"
#include "EZLOPI_USER_CONFIG.h"

static uint8_t gxhtc3_get_crc8(uint8_t *data, int len)
{
    uint8_t crc = 0xff;
    uint8_t crc_poly = GXHTC3_CRC_POLY;

    for (int i = 0; i < len; i++)
    {
        crc ^= data[i];

        for (int i = 0; i < 8; i++)
        {
            bool xor = crc & 0x80;
            crc = crc << 1;
            crc = xor ? crc ^ crc_poly : crc;
        }
    }

    return crc;
}

static bool gxhtc3_check_crc8(uint8_t calulated, uint8_t read)
{
    if (calulated != read)
    {
        return false;
    }
    else
    {
        return true;
    }
}

static bool gxhtc3_send_command(s_gxhtc3_sensor_handler_t *handler, uint16_t cmd)
{
    bool ret = true;
    if (handler)
    {
        uint8_t data[2] = { cmd >> 8, cmd & 0xff };
        esp_err_t err = i2c_master_write_to_device(handler->i2c_ch_num, handler->i2c_slave_addr, data, 2, GXHTC3_I2C_TIMEOUT);
        if (err != ESP_OK)
        {
            TRACE_E("I2c Write error !");
            ret = false;
        }
    }
    else
    {
        ret = false;
    }
    return ret;
}

static bool gxhtc3_read_data(s_gxhtc3_sensor_handler_t *handler, uint8_t *read_buf, uint16_t len)
{
    bool ret = true;
    if (handler)
    {
        esp_err_t err = i2c_master_read_from_device(handler->i2c_ch_num, handler->i2c_slave_addr, read_buf, len, GXHTC3_I2C_TIMEOUT);
        if (err != ESP_OK)
        {
            ret = false;
        }
    }
    return ret;
}

static bool gxhtc3_reset(s_gxhtc3_sensor_handler_t *handler)
{
    bool ret = true;
    if (handler)
    {
        if (!gxhtc3_send_command(handler, GXHTC3_I2C_CMD_SOFT_RESET))
        {
            TRACE_E("Error issuing reset command !");
            ret = false;
        }
    }
    else
    {
        ret = false;
    }
    return ret;
}

static bool gxhtc3_read_id(s_gxhtc3_sensor_handler_t *handler)
{
    bool ret = true;
    uint8_t reg_id[GXHTC3_I2C_REG_ID_READ_LEN];
    if (handler)
    {
        if (gxhtc3_send_command(handler, GXHTC3_I2C_CMD_READ_REG_ID))
        {
            vTaskDelay(5 / portTICK_PERIOD_MS);
            if (gxhtc3_read_data(handler, reg_id, GXHTC3_I2C_REG_ID_READ_LEN))
            {
                if (gxhtc3_check_crc8(gxhtc3_get_crc8(reg_id, 2), reg_id[2]))
                {
                    uint16_t id_val_raw = (reg_id[1] << 8) | reg_id[0];
                    uint16_t bit_11 = (id_val_raw >> 11) & 0x01;
                    uint8_t id_val = ((uint8_t)(id_val_raw & 0xFF)) | (uint8_t)(bit_11 << 6);
                    handler->id.id = id_val;
                    handler->id.status = true;
                }
                else
                {
                    TRACE_E("CRC did not match !!");
                    ret = false;
                }
            }
        }
    }
    return ret;
}

bool gxhtc3_wake_sensor(s_gxhtc3_sensor_handler_t *handler)
{
    bool ret = true;
    if (handler)
    {
        if (!gxhtc3_send_command(handler, GXHTC3_I2C_CMD_WAKEUP))
        {
            ret = false;
        }
    }
    return ret;
}
bool gxhtc3_sleep_sensor(s_gxhtc3_sensor_handler_t *handler)
{
    bool ret = true;
    if (handler)
    {
        if (!gxhtc3_send_command(handler, GXHTC3_I2C_CMD_SLEEP))
        {
            ret = false;
        }
    }
    return ret;
}

bool gxhtc3_start_measurement(s_gxhtc3_sensor_handler_t *handler)
{
    bool ret = true;
    if (handler)
    {
        handler->read_status = false;
        if (!gxhtc3_send_command(handler, GXHTC3_I2C_CMD_READ_NM_CSEN_TF))
        {
            ret = false;
        }
    }
    return ret;
}

bool gxhtc3_compute_values(s_gxhtc3_sensor_handler_t *handler)
{
    bool ret = true;

    if (handler->raw_data_reg)
    {
        {
            handler->reading_temp_c = -45.0 + (175 * ((handler->raw_data_reg[0] << 8 | handler->raw_data_reg[1])) / 65536.0);
            handler->reading_rh = 100.0 * (handler->raw_data_reg[3] << 8 | handler->raw_data_reg[4]) / 65536.0;

            // handler->reading_temp_c = ((((handler->raw_data_reg[0] * 256.0) + handler->raw_data_reg[1]) * 175) / 65535.0) - 45;
            // handler->reading_rh = ((((handler->raw_data_reg[3] * 256.0) + handler->raw_data_reg[4]) * 100) / 65535.0);
        }
    }
    else
    {
        ret = false;
    }

    return ret;
}

bool GXHTC3_read_sensor(s_gxhtc3_sensor_handler_t *handler)
{
    bool ret = true;
    if (handler)
    {
        if (!gxhtc3_wake_sensor(handler))
            ret = false;
        vTaskDelay(1 / portTICK_RATE_MS);
        if (!gxhtc3_start_measurement(handler))
            ret = false;
        vTaskDelay(5 / portTICK_RATE_MS);
        esp_err_t err = i2c_master_read_from_device(handler->i2c_ch_num, handler->i2c_slave_addr, handler->raw_data_reg, GXHTC3_I2C_RAW_DATA_LEN, (GXHTC3_I2C_TIMEOUT / portTICK_RATE_MS));
        if (err != ESP_OK)
        {
            ret = false;
        }

        if ((gxhtc3_check_crc8(gxhtc3_get_crc8(handler->raw_data_reg, 2), handler->raw_data_reg[2])) && (gxhtc3_check_crc8(gxhtc3_get_crc8(&handler->raw_data_reg[3], 2), handler->raw_data_reg[5])))
        {
            if (!gxhtc3_compute_values(handler))
                ret = false;
        }
        else
        {
            TRACE_E("CRC Did not match");
            ret = false;
        }

        // if (gxhtc3_sleep_sensor(handler))
        //     ret = false;
    }

    return ret;
}

s_gxhtc3_sensor_handler_t *GXHTC3_init(int32_t i2c_ch_num, uint8_t i2c_slave_addr)
{
    s_gxhtc3_sensor_handler_t *gxhtc3_handler = (s_gxhtc3_sensor_handler_t *)ezlopi_malloc(__FUNCTION__, sizeof(s_gxhtc3_sensor_handler_t));

    if (gxhtc3_handler)
    {
        gxhtc3_handler->i2c_ch_num = i2c_ch_num;
        gxhtc3_handler->i2c_slave_addr = i2c_slave_addr;
        gxhtc3_handler->id.status = false;

        if (!gxhtc3_reset(gxhtc3_handler))
        {
            ezlopi_free(__FUNCTION__, gxhtc3_handler);
            gxhtc3_handler = NULL;
        }
        else
        {
            gxhtc3_read_id(gxhtc3_handler);
        }
    }

    return gxhtc3_handler;
}
