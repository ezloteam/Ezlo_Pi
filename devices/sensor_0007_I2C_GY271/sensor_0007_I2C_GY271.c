#include <cJSON.h>
#include <math.h>
#include "trace.h"

#include "ezlopi_timer.h"
#include "ezlopi_i2c_master.h"
#include "ezlopi_devices_list.h"
#include "ezlopi_valueformatter.h"
#include "ezlopi_cloud_constants.h"
#include "ezlopi_device_value_updated.h"

#include "sensor_0007_I2C_GY271.h"

static int __prepare(void *arg);
static int __init(l_ezlopi_item_t *item);
static int __get_cjson_value(l_ezlopi_item_t *item, void *arg);
static int __notify(l_ezlopi_item_t *item);
static void __prepare_device_cloud_properties(l_ezlopi_device_t *device, cJSON *cj_device);
static void __prepare_item_cloud_properties(l_ezlopi_item_t *item, void *user_data);
static void __prepare_item_interface_properties(l_ezlopi_item_t *item, cJSON *cj_device);
static void __gy271_calibration_task(void *params);
//------------------------------------------------------------------------------------------------------------------------------------------------------------

int sensor_0007_I2C_GY271(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg)
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
        if (ezlopi_item_name_magnetic_strength_x_axis == item->cloud_properties.item_name)
        {
            item->interface.i2c_master.enable = true;
            CJSON_GET_VALUE_INT(cj_device, "dev_type", item->interface_type);
            CJSON_GET_VALUE_INT(cj_device, "gpio_sda", item->interface.i2c_master.sda);
            CJSON_GET_VALUE_INT(cj_device, "gpio_scl", item->interface.i2c_master.scl);
            CJSON_GET_VALUE_INT(cj_device, "slave_addr", item->interface.i2c_master.address);

            item->interface.i2c_master.clock_speed = 100000;
            if (NULL == item->interface.i2c_master.address)
            {
                item->interface.i2c_master.address = GY271_ADDR;
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
        s_gy271_data_t *user_data = (s_gy271_data_t *)malloc(sizeof(s_gy271_data_t));
        if (NULL != user_data)
        {
            memset(user_data, 0, sizeof(s_gy271_data_t));
            l_ezlopi_device_t *gy271_device = ezlopi_device_add_device();
            if (gy271_device)
            {
                __prepare_device_cloud_properties(gy271_device, cj_device);
                l_ezlopi_item_t *gyro_x_item = ezlopi_device_add_item_to_device(gy271_device, sensor_0007_I2C_GY271);
                if (gyro_x_item)
                {
                    __prepare_item_cloud_properties(gyro_x_item, user_data);
                    gyro_x_item->cloud_properties.item_name = ezlopi_item_name_magnetic_strength_x_axis;
                    gyro_x_item->cloud_properties.value_type = value_type_int;
                    gyro_x_item->cloud_properties.scale = NULL;
                    __prepare_item_interface_properties(gyro_x_item, cj_device);
                }
                l_ezlopi_item_t *gyro_y_item = ezlopi_device_add_item_to_device(gy271_device, sensor_0007_I2C_GY271);
                if (gyro_y_item)
                {
                    __prepare_item_cloud_properties(gyro_y_item, user_data);
                    gyro_y_item->cloud_properties.item_name = ezlopi_item_name_magnetic_strength_y_axis;
                    gyro_y_item->cloud_properties.value_type = value_type_int;
                    gyro_y_item->cloud_properties.scale = NULL;
                    __prepare_item_interface_properties(gyro_y_item, cj_device);
                }
                l_ezlopi_item_t *gyro_z_item = ezlopi_device_add_item_to_device(gy271_device, sensor_0007_I2C_GY271);
                if (gyro_z_item)
                {
                    __prepare_item_cloud_properties(gyro_z_item, user_data);
                    gyro_z_item->cloud_properties.item_name = ezlopi_item_name_magnetic_strength_z_axis;
                    gyro_z_item->cloud_properties.value_type = value_type_int;
                    gyro_z_item->cloud_properties.scale = NULL;
                    __prepare_item_interface_properties(gyro_z_item, cj_device);
                }
                l_ezlopi_item_t *gyro_azi_item = ezlopi_device_add_item_to_device(gy271_device, sensor_0007_I2C_GY271);
                if (gyro_azi_item)
                {
                    __prepare_item_cloud_properties(gyro_azi_item, user_data);
                    gyro_azi_item->cloud_properties.item_name = ezlopi_item_name_angle_position;
                    gyro_azi_item->cloud_properties.value_type = value_type_angle;
                    gyro_azi_item->cloud_properties.scale = scales_north_pole_degress;
                    __prepare_item_interface_properties(gyro_azi_item, cj_device);
                }
                l_ezlopi_item_t *gyro_temp_item = ezlopi_device_add_item_to_device(gy271_device, sensor_0007_I2C_GY271);
                if (gyro_temp_item)
                {
                    __prepare_item_cloud_properties(gyro_temp_item, user_data);
                    gyro_temp_item->cloud_properties.item_name = ezlopi_item_name_temp;
                    gyro_temp_item->cloud_properties.value_type = value_type_temperature;
                    gyro_temp_item->cloud_properties.scale = scales_celsius;
                    __prepare_item_interface_properties(gyro_temp_item, cj_device);
                }
                else
                {
                    ezlopi_device_free_device(gy271_device);
                    free(user_data);
                }
            }
            else
            {
                ezlopi_device_free_device(gy271_device);
                free(user_data);
            }
            ret = 1;
        }
    }

    return ret;
}

static int __init(l_ezlopi_item_t *item)
{
    int ret = 0;

    if (item->interface.i2c_master.enable)
    {
        ezlopi_i2c_master_init(&item->interface.i2c_master);
        TRACE_B("I2C channel is %d", item->interface.i2c_master.channel);
        TRACE_I("I2C initialized to channel %d", item->interface.i2c_master.channel);
        __gy271_configure(item);

        TRACE_W("Gathering Max-Min values for 2min.....");
        xTaskCreate(&__gy271_calibration_task, "GY271_Calibration_Task", 2048, item, 1, NULL);
    }
    return ret;
}

static int __get_cjson_value(l_ezlopi_item_t *item, void *arg)
{
    int ret = 0;
    cJSON *cj_result = (cJSON *)arg;
    if (cj_result && item)
    {
        s_gy271_data_t *user_data = (s_gy271_data_t *)item->user_arg;

        if (ezlopi_item_name_acceleration_x_axis == item->cloud_properties.item_name)
        {
            TRACE_I("X-axis field Strength : %.2fG", user_data->X);
            cJSON_AddNumberToObject(cj_result, "value", user_data->X);
            char *valueFormatted = ezlopi_valueformatter_float(user_data->X);
            cJSON_AddStringToObject(cj_result, "valueFormatted", valueFormatted);
            free(valueFormatted);
            cJSON_AddStringToObject(cj_result, "scales", item->cloud_properties.scale);
        }
        if (ezlopi_item_name_acceleration_y_axis == item->cloud_properties.item_name)
        {
            TRACE_I("Y-axis field Strength : %.2fG", user_data->Y);
            cJSON_AddNumberToObject(cj_result, "value", user_data->Y);
            char *valueFormatted = ezlopi_valueformatter_float(user_data->Y);
            cJSON_AddStringToObject(cj_result, "valueFormatted", valueFormatted);
            free(valueFormatted);
            cJSON_AddStringToObject(cj_result, "scales", item->cloud_properties.scale);
        }
        if (ezlopi_item_name_acceleration_z_axis == item->cloud_properties.item_name)
        {
            TRACE_I("Z-axis field Strength : %.2fG", user_data->Z);
            cJSON_AddNumberToObject(cj_result, "value", user_data->Z);
            char *valueFormatted = ezlopi_valueformatter_float(user_data->Z);
            cJSON_AddStringToObject(cj_result, "valueFormatted", valueFormatted);
            free(valueFormatted);
            cJSON_AddStringToObject(cj_result, "scales", item->cloud_properties.scale);
        }
        if (ezlopi_item_name_angle_position == item->cloud_properties.item_name)
        {

            TRACE_I("Azimuth : %d *deg", user_data->azimuth);
            cJSON_AddNumberToObject(cj_result, "value", (user_data->azimuth));
            char *valueFormatted = ezlopi_valueformatter_int(user_data->azimuth);
            cJSON_AddStringToObject(cj_result, "valueFormatted", valueFormatted);
            free(valueFormatted);
            cJSON_AddStringToObject(cj_result, "scales", item->cloud_properties.scale);
        }

        if (ezlopi_item_name_temp == item->cloud_properties.item_name)
        {
            TRACE_I("temperature : %.2f*C", user_data->T);
            cJSON_AddNumberToObject(cj_result, "value", user_data->T);
            char *valueFormatted = ezlopi_valueformatter_float(user_data->T);
            cJSON_AddStringToObject(cj_result, "valueFormatted", valueFormatted);
            free(valueFormatted);
            cJSON_AddStringToObject(cj_result, "scales", item->cloud_properties.scale);
        }

        ret = 1;
    }
    return ret;
}

static int __notify(l_ezlopi_item_t *item)
{
    int ret = 0;
    if (item)
    {
        s_gy271_data_t *user_data = (s_gy271_data_t *)item->user_arg;
        float prev_X = user_data->X;
        float prev_Y = user_data->Y;
        float prev_Z = user_data->Z;
        float prev_T = user_data->T;
        int prev_azimuth = user_data->azimuth;
        if (__gy271_update_value(item))
        {
            if ((prev_X != user_data->X) || (prev_Y != user_data->Y) || (prev_Z != user_data->Z) || (prev_azimuth != user_data->azimuth) || (prev_T != user_data->T))
            {
                ezlopi_device_value_updated_from_device_v3(item);
            }
        }
        ret = 1;
    }
    return ret;
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------------------------------------------------------------------

static void __gy271_calibration_task(void *params) // calibrate task
{

    l_ezlopi_item_t *item = (l_ezlopi_item_t *)params;
    if (item)
    {
        int calibrationData[3][2] = {{0, 0},  // xmin,xmax
                                     {0, 0},  // ymin,ymax
                                     {0, 0}}; // zmin,zmax// Initialization added!
        s_gy271_data_t *user_data = (s_gy271_data_t *)item->user_arg;

        vTaskDelay(5000 / portTICK_PERIOD_MS); // 5 sec
        for (uint16_t i = 0; i <= 100; i++)
        {
            // call the data gathering function [100 * 20 ms = 20 sec]
            __gy271_get_raw_max_min_values(item, calibrationData);
            if (i % 5 == 0)
            {
                TRACE_I(" ------------------------------------------------- Time :- {%u sec} ", (i / 5));
            }
            vTaskDelay(200 / portTICK_PERIOD_MS); // 200ms
        }

        TRACE_W("..........................Calculating Paramter.......................");
        // Calculate the : 1.bias_axis , 2.delta_axis , 3.delta_avg , 4.scale_axis
        // 1. bias_axis{x,y,z}
        user_data->calib_factor.bias_axis[0] = ((long)(calibrationData[0][1] + calibrationData[0][0]) / 2); // x-axis
        user_data->calib_factor.bias_axis[1] = ((long)(calibrationData[1][1] + calibrationData[1][0]) / 2); // y-axis
        user_data->calib_factor.bias_axis[2] = ((long)(calibrationData[2][1] + calibrationData[2][0]) / 2); // z-axis

        // 2. delta_axis{x,y,z}
        user_data->calib_factor.delta_axis[0] = (long)(calibrationData[0][1] - calibrationData[0][0]); // x-axis
        user_data->calib_factor.delta_axis[1] = (long)(calibrationData[1][1] - calibrationData[1][0]); // y-axis
        user_data->calib_factor.delta_axis[2] = (long)(calibrationData[2][1] - calibrationData[2][0]); // z-axis

        // 3. delta_avg
        user_data->calib_factor.delta_avg = ((float)((user_data->calib_factor.delta_axis[0]) +
                                                     (user_data->calib_factor.delta_axis[1]) +
                                                     (user_data->calib_factor.delta_axis[2])) /
                                             3.0f);

        // 4. Scale_axis{x,y,z}
        user_data->calib_factor.scale_axis[0] = user_data->calib_factor.delta_avg / user_data->calib_factor.delta_axis[0]; // x-axis
        user_data->calib_factor.scale_axis[1] = user_data->calib_factor.delta_avg / user_data->calib_factor.delta_axis[1]; // y-axis
        user_data->calib_factor.scale_axis[2] = user_data->calib_factor.delta_avg / user_data->calib_factor.delta_axis[2]; // z-axis

        TRACE_B("Bias :--- _Xaxis=%6ld | _Yaxis=%6ld | _Zaxis=%6ld ",
                user_data->calib_factor.bias_axis[0],
                user_data->calib_factor.bias_axis[1],
                user_data->calib_factor.bias_axis[2]);

        TRACE_B("Delta :--- _Xaxis=%6ld | _Yaxis=%6ld | _Zaxis=%6ld ",
                user_data->calib_factor.delta_axis[0],
                user_data->calib_factor.delta_axis[1],
                user_data->calib_factor.delta_axis[2]);
        TRACE_B("Delta_AVG :--- %6f", user_data->calib_factor.delta_avg);

        TRACE_B("Scale :--- _Xaxis=%6f | _Yaxis=%6f | _Zaxis=%6f ",
                user_data->calib_factor.scale_axis[0],
                user_data->calib_factor.scale_axis[1],
                user_data->calib_factor.scale_axis[0]);
        TRACE_W("...............................................................\n\n");

        user_data->calibration_complete = true;
    }
    vTaskDelete(NULL);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
