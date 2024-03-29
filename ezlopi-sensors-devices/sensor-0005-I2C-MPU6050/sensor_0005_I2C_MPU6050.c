#include <math.h>
#include "ezlopi_util_trace.h"

#include "ezlopi_core_timer.h"
#include "ezlopi_core_cloud.h"
#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_core_valueformatter.h"
#include "ezlopi_core_device_value_updated.h"

#include "ezlopi_hal_i2c_master.h"

#include "ezlopi_cloud_items.h"
#include "ezlopi_cloud_constants.h"

#include "sensor_0005_I2C_MPU6050.h"

static int __prepare(void *arg);
static int __init(l_ezlopi_item_t *item);
static int __get_cjson_value(l_ezlopi_item_t *item, void *arg);
static int __notify(l_ezlopi_item_t *item);
static void __prepare_device_cloud_properties(l_ezlopi_device_t *device, cJSON *cj_device);
static void __prepare_item_cloud_properties(l_ezlopi_item_t *item, void *user_data);
static void __prepare_item_interface_properties(l_ezlopi_item_t *item, cJSON *cj_device);
static void __mpu6050_calibration_task(void *params);
//------------------------------------------------------------------------------------------------------------------------------------------------------------
int sensor_0005_I2C_MPU6050(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg)
{
    int ret = 0;
    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        __prepare(arg);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        __init(item);
        break;
    }
    case EZLOPI_ACTION_HUB_GET_ITEM:
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        __get_cjson_value(item, arg);
        break;
    }
    case EZLOPI_ACTION_NOTIFY_1000_MS:
    {
        __notify(item);
        break;
    }
    default:
    {
        break;
    }
    }
    return ret;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------

static void __prepare_device_cloud_properties(l_ezlopi_device_t *device, cJSON *cj_device)
{
    char *device_name = NULL;
    CJSON_GET_VALUE_STRING(cj_device, "dev_name", device_name);
    ASSIGN_DEVICE_NAME_V2(device, device_name);
    device->cloud_properties.device_id = ezlopi_cloud_generate_device_id();
    device->cloud_properties.category = category_level_sensor;
    device->cloud_properties.subcategory = subcategory_not_defined;
    device->cloud_properties.device_type = dev_type_sensor;
    device->cloud_properties.info = NULL;
    device->cloud_properties.device_type_id = NULL;
}
static void __prepare_item_cloud_properties(l_ezlopi_item_t *item, void *user_data)
{
    if (item)
    {
        item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();
        item->cloud_properties.has_getter = true;
        item->cloud_properties.has_setter = false;
        item->cloud_properties.show = true;
        item->user_arg = user_data;
    }
}
static void __prepare_item_interface_properties(l_ezlopi_item_t *item, cJSON *cj_device)
{
    if (item && cj_device)
    {
        if (ezlopi_item_name_acceleration_x_axis == item->cloud_properties.item_name)
        {
            item->interface.i2c_master.enable = true;
            CJSON_GET_VALUE_INT(cj_device, ezlopi_dev_type_str, item->interface_type);
            CJSON_GET_VALUE_INT(cj_device, ezlopi_gpio_sda_str, item->interface.i2c_master.sda);
            CJSON_GET_VALUE_INT(cj_device,ezlopi_gpio_scl_str, item->interface.i2c_master.scl);
            CJSON_GET_VALUE_INT(cj_device, "slave_addr", item->interface.i2c_master.address);

            item->interface.i2c_master.clock_speed = 100000;
            if (0 == item->interface.i2c_master.address)
            {
                item->interface.i2c_master.address = MPU6050_ADDR;
            }
        }
        else
        {
            item->interface.i2c_master.enable = false;
        }
    }
}

static int __prepare(void *arg)
{
    int ret = 0;
    s_ezlopi_prep_arg_t *prep_arg = (s_ezlopi_prep_arg_t *)arg;
    if (prep_arg && prep_arg->cjson_device)
    {
        cJSON *cj_device = prep_arg->cjson_device;
        s_mpu6050_data_t *user_data = (s_mpu6050_data_t *)malloc(sizeof(s_mpu6050_data_t));
        if (NULL != user_data)
        {
            memset(user_data, 0, sizeof(s_mpu6050_data_t));
            l_ezlopi_device_t *mpu6050_device = ezlopi_device_add_device(cj_device);
            if (mpu6050_device)
            {
                __prepare_device_cloud_properties(mpu6050_device, cj_device);
                l_ezlopi_item_t *mpu6050_acc_x_item = ezlopi_device_add_item_to_device(mpu6050_device, sensor_0005_I2C_MPU6050);
                if (mpu6050_acc_x_item)
                {
                    __prepare_item_cloud_properties(mpu6050_acc_x_item, user_data);
                    mpu6050_acc_x_item->cloud_properties.item_name = ezlopi_item_name_acceleration_x_axis;
                    mpu6050_acc_x_item->cloud_properties.value_type = value_type_acceleration;
                    mpu6050_acc_x_item->cloud_properties.scale = scales_meter_per_square_second;
                    __prepare_item_interface_properties(mpu6050_acc_x_item, cj_device);
                }
                l_ezlopi_item_t *mpu6050_acc_y_item = ezlopi_device_add_item_to_device(mpu6050_device, sensor_0005_I2C_MPU6050);
                if (mpu6050_acc_y_item)
                {
                    __prepare_item_cloud_properties(mpu6050_acc_y_item, user_data);
                    mpu6050_acc_y_item->cloud_properties.item_name = ezlopi_item_name_acceleration_y_axis;
                    mpu6050_acc_y_item->cloud_properties.value_type = value_type_acceleration;
                    mpu6050_acc_y_item->cloud_properties.scale = scales_meter_per_square_second;
                    __prepare_item_interface_properties(mpu6050_acc_y_item, cj_device);
                }
                l_ezlopi_item_t *mpu6050_acc_z_item = ezlopi_device_add_item_to_device(mpu6050_device, sensor_0005_I2C_MPU6050);
                if (mpu6050_acc_z_item)
                {
                    __prepare_item_cloud_properties(mpu6050_acc_z_item, user_data);
                    mpu6050_acc_z_item->cloud_properties.item_name = ezlopi_item_name_acceleration_z_axis;
                    mpu6050_acc_z_item->cloud_properties.value_type = value_type_acceleration;
                    mpu6050_acc_z_item->cloud_properties.scale = scales_meter_per_square_second;
                    __prepare_item_interface_properties(mpu6050_acc_z_item, cj_device);
                }

                l_ezlopi_item_t *mpu6050_temp_item = ezlopi_device_add_item_to_device(mpu6050_device, sensor_0005_I2C_MPU6050);
                if (mpu6050_temp_item)
                {
                    __prepare_item_cloud_properties(mpu6050_temp_item, user_data);
                    mpu6050_temp_item->cloud_properties.item_name = ezlopi_item_name_temp;
                    mpu6050_temp_item->cloud_properties.value_type = value_type_temperature;
                    mpu6050_temp_item->cloud_properties.scale = scales_celsius;
                    __prepare_item_interface_properties(mpu6050_temp_item, cj_device);
                }

                l_ezlopi_item_t *mpu6050_gyro_x_item = ezlopi_device_add_item_to_device(mpu6050_device, sensor_0005_I2C_MPU6050);
                if (mpu6050_gyro_x_item)
                {
                    __prepare_item_cloud_properties(mpu6050_gyro_x_item, user_data);
                    mpu6050_gyro_x_item->cloud_properties.item_name = ezlopi_item_name_gyroscope_x_axis;
                    mpu6050_gyro_x_item->cloud_properties.value_type = value_type_frequency;
                    mpu6050_gyro_x_item->cloud_properties.scale = scales_revolutions_per_minute;
                    __prepare_item_interface_properties(mpu6050_gyro_x_item, cj_device);
                }
                l_ezlopi_item_t *mpu6050_gyro_y_item = ezlopi_device_add_item_to_device(mpu6050_device, sensor_0005_I2C_MPU6050);
                if (mpu6050_gyro_y_item)
                {
                    __prepare_item_cloud_properties(mpu6050_gyro_y_item, user_data);
                    mpu6050_gyro_y_item->cloud_properties.item_name = ezlopi_item_name_gyroscope_y_axis;
                    mpu6050_gyro_y_item->cloud_properties.value_type = value_type_frequency;
                    mpu6050_gyro_y_item->cloud_properties.scale = scales_revolutions_per_minute;
                    __prepare_item_interface_properties(mpu6050_gyro_y_item, cj_device);
                }
                l_ezlopi_item_t *mpu6050_gyro_z_item = ezlopi_device_add_item_to_device(mpu6050_device, sensor_0005_I2C_MPU6050);
                if (mpu6050_gyro_z_item)
                {
                    __prepare_item_cloud_properties(mpu6050_gyro_z_item, user_data);
                    mpu6050_gyro_z_item->cloud_properties.item_name = ezlopi_item_name_gyroscope_z_axis;
                    mpu6050_gyro_z_item->cloud_properties.value_type = value_type_frequency;
                    mpu6050_gyro_z_item->cloud_properties.scale = scales_revolutions_per_minute;
                    __prepare_item_interface_properties(mpu6050_gyro_z_item, cj_device);
                }
                ret = 1;

                if ((NULL == mpu6050_acc_x_item) && (NULL == mpu6050_acc_y_item) && (NULL == mpu6050_acc_z_item) && (NULL == mpu6050_temp_item) && (NULL == mpu6050_gyro_x_item) && (NULL == mpu6050_gyro_y_item) && (NULL == mpu6050_gyro_z_item))
                {
                    ezlopi_device_free_device(mpu6050_device);
                    free(user_data);
                    ret = -1;
                }
            }
            else
            {
                free(user_data);
                ret = -1;
            }
        }
    }

    return ret;
}
static int __init(l_ezlopi_item_t *item)
{
    int ret = 0;
    if (item)
    {
        s_mpu6050_data_t *user_data = (s_mpu6050_data_t *)item->user_arg;
        if (user_data)
        {
            if (item->interface.i2c_master.enable)
            {
                ezlopi_i2c_master_init(&item->interface.i2c_master);
                if (MPU6050_ERR_OK == __mpu6050_config_device(item))
                {
                    TRACE_I("Configuration Complete.... ");
                    xTaskCreate(__mpu6050_calibration_task, "MPU6050_Calibration_Task", 2048, item, 1, NULL);
                    ret = 1;
                }
                else
                {
                    // ret = -1;
                    // free(item->user_arg); // this will free ; memory address linked to all items
                    // item->user_arg = NULL;
                    // ezlopi_device_free_device_by_item(item);
                }
            }
        }
        // else
        // {
        //     ret = -1;
        //     ezlopi_device_free_device_by_item(item);
        // }
    }
    return ret;
}

static int __get_cjson_value(l_ezlopi_item_t *item, void *arg)
{
    int ret = 0;
    if (item && arg)
    {
        cJSON *cj_result = (cJSON *)arg;
        if (cj_result)
        {
            s_mpu6050_data_t *user_data = (s_mpu6050_data_t *)item->user_arg;
            if (user_data)
            {
                if (ezlopi_item_name_acceleration_x_axis == item->cloud_properties.item_name)
                {
                    TRACE_I("Accel-x : %.2fm/s^2", user_data->ax);
                    cJSON_AddNumberToObject(cj_result, ezlopi_value_str, user_data->ax);
                    char *valueFormatted = ezlopi_valueformatter_float(user_data->ax);
                    if (valueFormatted)
                    {
                        cJSON_AddStringToObject(cj_result, ezlopi_valueFormatted_str, valueFormatted);
                        free(valueFormatted);
                    }
                }
                if (ezlopi_item_name_acceleration_y_axis == item->cloud_properties.item_name)
                {
                    TRACE_I("Accel-y : %.2fm/s^2", user_data->ay);
                    cJSON_AddNumberToObject(cj_result, ezlopi_value_str, user_data->ay);
                    char *valueFormatted = ezlopi_valueformatter_float(user_data->ay);
                    if (valueFormatted)
                    {
                        cJSON_AddStringToObject(cj_result, ezlopi_valueFormatted_str, valueFormatted);
                        free(valueFormatted);
                    }
                }
                if (ezlopi_item_name_acceleration_z_axis == item->cloud_properties.item_name)
                {
                    TRACE_I("Accel-z : %.2fm/s^2", user_data->az);
                    cJSON_AddNumberToObject(cj_result, ezlopi_value_str, user_data->az);
                    char *valueFormatted = ezlopi_valueformatter_float(user_data->az);
                    if (valueFormatted)
                    {
                        cJSON_AddStringToObject(cj_result, ezlopi_valueFormatted_str, valueFormatted);
                        free(valueFormatted);
                    }
                }
                if (ezlopi_item_name_temp == item->cloud_properties.item_name)
                {
                    TRACE_I("Temp : %.2f*C", user_data->tmp);
                    cJSON_AddNumberToObject(cj_result, ezlopi_value_str, user_data->tmp);
                    char *valueFormatted = ezlopi_valueformatter_float(user_data->tmp);
                    if (valueFormatted)
                    {
                        cJSON_AddStringToObject(cj_result, ezlopi_valueFormatted_str, valueFormatted);
                        free(valueFormatted);
                    }
                }
                if (ezlopi_item_name_gyroscope_x_axis == item->cloud_properties.item_name)
                {
                    TRACE_I("Gyro-x : %d rpm", (int)user_data->gx);
                    cJSON_AddNumberToObject(cj_result, ezlopi_value_str, ((int)user_data->gx));
                    char *valueFormatted = ezlopi_valueformatter_int((int)user_data->gx);
                    if (valueFormatted)
                    {
                        cJSON_AddStringToObject(cj_result, ezlopi_valueFormatted_str, valueFormatted);
                        free(valueFormatted);
                    }
                }
                if (ezlopi_item_name_gyroscope_y_axis == item->cloud_properties.item_name)
                {
                    TRACE_I("Gyro-y : %d rpm", (int)user_data->gy);
                    cJSON_AddNumberToObject(cj_result, ezlopi_value_str, ((int)user_data->gy));
                    char *valueFormatted = ezlopi_valueformatter_int((int)user_data->gy);
                    if (valueFormatted)
                    {
                        cJSON_AddStringToObject(cj_result, ezlopi_valueFormatted_str, valueFormatted);
                        free(valueFormatted);
                    }
                }
                if (ezlopi_item_name_gyroscope_z_axis == item->cloud_properties.item_name)
                {
                    TRACE_I("Gyro-z : %d rpm", (int)user_data->gz);
                    cJSON_AddNumberToObject(cj_result, ezlopi_value_str, ((int)user_data->gz));
                    char *valueFormatted = ezlopi_valueformatter_int((int)user_data->gz);
                    if (valueFormatted)
                    {
                        cJSON_AddStringToObject(cj_result, ezlopi_valueFormatted_str, valueFormatted);
                        free(valueFormatted);
                    }
                }
                ret = 1;
            }
        }
    }
    return ret;
}

static int __notify(l_ezlopi_item_t *item)
{
    static float __prev[7];
    int ret = 0;
    if (item)
    {
        s_mpu6050_data_t *user_data = (s_mpu6050_data_t *)item->user_arg;
        if ((user_data) && user_data->calibration_complete)
        {
            if (ezlopi_item_name_acceleration_x_axis == item->cloud_properties.item_name)
            {
                __prev[0] = user_data->ax;
                __prev[1] = user_data->ay;
                __prev[2] = user_data->az;
                __prev[3] = user_data->tmp;
                __prev[4] = user_data->gx;
                __prev[5] = user_data->gy;
                __prev[6] = user_data->gz;

                // if (user_data->extract_counts >= RECALIBRATE_ITERAION_COUNT)
                // {
                //     user_data->extract_counts = 0;
                //     user_data->calibration_complete = false;
                //     xTaskCreate(__mpu6050_calibration_task, "MPU6050_ReCalibration_Task", 2048, item, 1, NULL);
                // }
                // else
                // {
                __mpu6050_get_data(item); // update the sensor data
                if (fabs(__prev[0] - user_data->ax) > 0.5)
                {
                    ezlopi_device_value_updated_from_device_v3(item);
                }
                // }
            }
            else if (ezlopi_item_name_acceleration_y_axis == item->cloud_properties.item_name)
            {
                if (fabs(__prev[1] - user_data->ay) > 0.5)
                {
                    ezlopi_device_value_updated_from_device_v3(item);
                }
            }
            else if (ezlopi_item_name_acceleration_z_axis == item->cloud_properties.item_name)
            {
                if (fabs(__prev[2] - user_data->az) > 0.5)
                {
                    ezlopi_device_value_updated_from_device_v3(item);
                }
            }
            else if (ezlopi_item_name_temp == item->cloud_properties.item_name)
            {
                if (fabs(__prev[3] - user_data->tmp) > 0.5)
                {
                    ezlopi_device_value_updated_from_device_v3(item);
                }
            }
            else if (ezlopi_item_name_gyroscope_x_axis == item->cloud_properties.item_name)
            {
                if (fabs(__prev[4] - user_data->gx) > 0.5)
                {
                    ezlopi_device_value_updated_from_device_v3(item);
                }
            }
            else if (ezlopi_item_name_gyroscope_y_axis == item->cloud_properties.item_name)
            {
                if (fabs(__prev[5] - user_data->gy) > 0.5)
                {
                    ezlopi_device_value_updated_from_device_v3(item);
                }
            }
            else if (ezlopi_item_name_gyroscope_z_axis == item->cloud_properties.item_name)
            {
                if (fabs(__prev[6] - user_data->gz) > 0.5)
                {
                    ezlopi_device_value_updated_from_device_v3(item);
                }
            }
        }
        ret = 1;
    }
    return ret;
}

static void __mpu6050_calibration_task(void *params) // calibrate task
{
    vTaskDelay(4000 / portTICK_PERIOD_MS);
    l_ezlopi_item_t *item = (l_ezlopi_item_t *)params;
    if (item)
    {
        s_mpu6050_data_t *user_data = (s_mpu6050_data_t *)item->user_arg;
        if (user_data)
        {
            uint8_t buf[MPU6050_REG_COUNT_LEN] = {0}; // 0 - 13
            uint8_t dummy[MPU6050_REG_COUNT_LEN] = {0};

            float calibrationData[3] = {0};
            uint8_t Check_Register = 0;
            esp_err_t err = ESP_OK;
            TRACE_W(".....................Calculating Paramter");

            uint8_t write_buffer[] = {REG_INTR_STATUS}; // REG_INTR_STATUS;
            uint8_t address_val = (ACCEL_X_H);

            for (uint8_t i = CALIBRATION_SAMPLES + 50; i > 0; i--)
            {
                err = ezlopi_i2c_master_write_to_device(&item->interface.i2c_master, write_buffer, 1);
                err = ezlopi_i2c_master_read_from_device(&item->interface.i2c_master, &Check_Register, 1);

                if (ESP_OK == err)
                {
                    if (Check_Register & DATA_RDY_INT_FLAG)
                    {
                        err = ezlopi_i2c_master_write_to_device(&item->interface.i2c_master, &address_val, 1);
                        err = ezlopi_i2c_master_read_from_device(&item->interface.i2c_master, (buf), MPU6050_REG_COUNT_LEN);
                    }
                    if (i <= CALIBRATION_SAMPLES)
                    {
                        if (ESP_OK != err)
                        {
                            buf[8] = dummy[8];
                            buf[9] = dummy[9];
                            buf[10] = dummy[10];
                            buf[11] = dummy[11];
                            buf[12] = dummy[12];
                            buf[13] = dummy[13];
                        }
                        else
                        {
                            dummy[8] = buf[8];
                            dummy[9] = buf[9];
                            dummy[10] = buf[10];
                            dummy[11] = buf[11];
                            dummy[12] = buf[12];
                            dummy[13] = buf[13];
                        }
                        calibrationData[0] += (float)((int16_t)(buf[8] << 8 | buf[9]));   // mean_raw_gx = 67 & 68;
                        calibrationData[1] += (float)((int16_t)(buf[10] << 8 | buf[11])); // mean_raw_gy = 69 & 70;
                        calibrationData[2] += (float)((int16_t)(buf[12] << 8 | buf[13])); // mean_raw_gz = 71 & 72;
                    }
                }
                else
                {
                    TRACE_E("Data not ready ... [%d]", i);
                }
            }

            user_data->gyro_x_offset = calibrationData[0] / (CALIBRATION_SAMPLES);
            user_data->gyro_y_offset = calibrationData[1] / (CALIBRATION_SAMPLES);
            user_data->gyro_z_offset = calibrationData[2] / (CALIBRATION_SAMPLES);

            TRACE_S("Scale :--- new_gy_offset_X=%.2f | new_gy_offset_Y=%.2f | new_gy_offset_Z=%.2f ",
                    user_data->gyro_x_offset,
                    user_data->gyro_y_offset,
                    user_data->gyro_z_offset);
            TRACE_W("......................CALIBRATION COMPLETE");
            user_data->calibration_complete = true;
        }
    }
    vTaskDelete(NULL);
}
