#include <math.h>
#include "ezlopi_util_trace.h"

#include "ezlopi_core_timer.h"
#include "ezlopi_core_cloud.h"
#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_core_valueformatter.h"
#include "ezlopi_core_device_value_updated.h"
#include "ezlopi_core_processes.h"

#include "ezlopi_hal_i2c_master.h"

#include "ezlopi_cloud_items.h"
#include "ezlopi_cloud_constants.h"

#include "sensor_0007_I2C_GY271.h"
#include "EZLOPI_USER_CONFIG.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------
static int __prepare(void* arg);
static int __init(l_ezlopi_item_t* item);
static int __get_cjson_value(l_ezlopi_item_t* item, void* arg);
static int __notify(l_ezlopi_item_t* item);
static void __prepare_item_cloud_properties(l_ezlopi_item_t* item, void* user_data);
static void __prepare_item_interface_properties(l_ezlopi_item_t* item, cJSON* cj_device);
static void __gy271_calibration_task(void* params);
//------------------------------------------------------------------------------------------------------------------------------------------------------------
int sensor_0007_I2C_GY271(e_ezlopi_actions_t action, l_ezlopi_item_t* item, void* arg, void* user_arg)
{
    int ret = 0;
    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        ret = __prepare(arg);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        ret = __init(item);
        break;
    }
    case EZLOPI_ACTION_HUB_GET_ITEM:
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        ret = __get_cjson_value(item, arg);
        break;
    }
    case EZLOPI_ACTION_NOTIFY_1000_MS:
    {
        ret = __notify(item);
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
static void __prepare_device_cloud_properties_parent_x(l_ezlopi_device_t* device, cJSON* cj_device)
{
    device->cloud_properties.category = category_level_sensor;
    device->cloud_properties.subcategory = subcategory_not_defined;
    device->cloud_properties.device_type = dev_type_sensor;
    device->cloud_properties.info = NULL;
    device->cloud_properties.device_type_id = NULL;
}

static void __prepare_device_cloud_properties_child_y(l_ezlopi_device_t* device, cJSON* cj_device)
{
    device->cloud_properties.category = category_level_sensor;
    device->cloud_properties.subcategory = subcategory_not_defined;
    device->cloud_properties.device_type = dev_type_sensor;
    device->cloud_properties.info = NULL;
    device->cloud_properties.device_type_id = NULL;
}

static void __prepare_device_cloud_properties_child_z(l_ezlopi_device_t* device, cJSON* cj_device)
{
    device->cloud_properties.category = category_level_sensor;
    device->cloud_properties.subcategory = subcategory_not_defined;
    device->cloud_properties.device_type = dev_type_sensor;
    device->cloud_properties.info = NULL;
    device->cloud_properties.device_type_id = NULL;
}

static void __prepare_device_cloud_properties_child_azi(l_ezlopi_device_t* device, cJSON* cj_device)
{
    device->cloud_properties.category = category_level_sensor;
    device->cloud_properties.subcategory = subcategory_navigation;
    device->cloud_properties.device_type = dev_type_sensor;
    device->cloud_properties.info = NULL;
    device->cloud_properties.device_type_id = NULL;
}

static void __prepare_device_cloud_properties_child_temp(l_ezlopi_device_t* device, cJSON* cj_device)
{
    device->cloud_properties.category = category_temperature;
    device->cloud_properties.subcategory = subcategory_not_defined;
    device->cloud_properties.device_type = dev_type_sensor;
    device->cloud_properties.info = NULL;
    device->cloud_properties.device_type_id = NULL;
}

static void __prepare_item_cloud_properties(l_ezlopi_item_t* item, void* user_data)
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
static void __prepare_item_interface_properties(l_ezlopi_item_t* item, cJSON* cj_device)
{
    if (item && cj_device)
    {
        if (ezlopi_item_name_magnetic_strength_x_axis == item->cloud_properties.item_name)
        {
            item->is_user_arg_unique = true;
            item->interface.i2c_master.enable = true;
            CJSON_GET_VALUE_DOUBLE(cj_device, ezlopi_dev_type_str, item->interface_type);
            CJSON_GET_VALUE_DOUBLE(cj_device, ezlopi_gpio_sda_str, item->interface.i2c_master.sda);
            CJSON_GET_VALUE_DOUBLE(cj_device, ezlopi_gpio_scl_str, item->interface.i2c_master.scl);
            CJSON_GET_VALUE_DOUBLE(cj_device, ezlopi_slave_addr_str, item->interface.i2c_master.address);

            item->interface.i2c_master.clock_speed = 100000;
            if (0 == item->interface.i2c_master.address)
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

static int __prepare(void* arg)
{
    int ret = 0;
    s_ezlopi_prep_arg_t* prep_arg = (s_ezlopi_prep_arg_t*)arg;
    if (prep_arg && prep_arg->cjson_device)
    {
        cJSON* cj_device = prep_arg->cjson_device;
        s_gy271_data_t* user_data = (s_gy271_data_t*)malloc(__FUNCTION__, sizeof(s_gy271_data_t));
        if (user_data)
        {
            memset(user_data, 0, sizeof(s_gy271_data_t));
            l_ezlopi_device_t* gy271_device_parent_x_device = ezlopi_device_add_device(cj_device, "Mag_strength_X");
            if (gy271_device_parent_x_device)
            {
                ret = 1;
                TRACE_I("Parent_gy271-x-[0x%x] ", gy271_device_parent_x_device->cloud_properties.device_id);
                __prepare_device_cloud_properties_parent_x(gy271_device_parent_x_device, cj_device);
                l_ezlopi_item_t* gyro_x_item = ezlopi_device_add_item_to_device(gy271_device_parent_x_device, sensor_0007_I2C_GY271);
                if (gyro_x_item)
                {
                    __prepare_item_cloud_properties(gyro_x_item, user_data);
                    gyro_x_item->cloud_properties.item_name = ezlopi_item_name_magnetic_strength_x_axis;
                    gyro_x_item->cloud_properties.value_type = value_type_float;
                    gyro_x_item->cloud_properties.scale = scales_guass;
                    __prepare_item_interface_properties(gyro_x_item, cj_device);
                }

                l_ezlopi_device_t* gy271_device_child_y_device = ezlopi_device_add_device(cj_device, "Mag_strength_Y");
                if (gy271_device_child_y_device)
                {
                    TRACE_I("Child_gy271-y-[0x%x] ", gy271_device_child_y_device->cloud_properties.device_id);
                    __prepare_device_cloud_properties_child_y(gy271_device_child_y_device, cj_device);
                    gy271_device_child_y_device->cloud_properties.parent_device_id = gy271_device_parent_x_device->cloud_properties.device_id;
                    l_ezlopi_item_t* gyro_y_item = ezlopi_device_add_item_to_device(gy271_device_child_y_device, sensor_0007_I2C_GY271);
                    if (gyro_y_item)
                    {
                        __prepare_item_cloud_properties(gyro_y_item, user_data);
                        gyro_y_item->cloud_properties.item_name = ezlopi_item_name_magnetic_strength_y_axis;
                        gyro_y_item->cloud_properties.value_type = value_type_float;
                        gyro_y_item->cloud_properties.scale = scales_guass;
                        __prepare_item_interface_properties(gyro_y_item, cj_device);
                    }
                    else
                    {
                        ret = -1;
                        ezlopi_device_free_device(gy271_device_child_y_device);
                    }
                }

                l_ezlopi_device_t* gy271_device_child_z_device = ezlopi_device_add_device(cj_device, "Mag_strength_Z");
                if (gy271_device_child_z_device)
                {
                    TRACE_I("Child_gy271-z-[0x%x] ", gy271_device_child_z_device->cloud_properties.device_id);
                    __prepare_device_cloud_properties_child_z(gy271_device_child_z_device, cj_device);
                    gy271_device_child_z_device->cloud_properties.parent_device_id = gy271_device_parent_x_device->cloud_properties.device_id;
                    l_ezlopi_item_t* gyro_z_item = ezlopi_device_add_item_to_device(gy271_device_child_z_device, sensor_0007_I2C_GY271);
                    if (gyro_z_item)
                    {
                        __prepare_item_cloud_properties(gyro_z_item, user_data);
                        gyro_z_item->cloud_properties.item_name = ezlopi_item_name_magnetic_strength_z_axis;
                        gyro_z_item->cloud_properties.value_type = value_type_float;
                        gyro_z_item->cloud_properties.scale = scales_guass;
                        __prepare_item_interface_properties(gyro_z_item, cj_device);
                    }
                    else
                    {
                        ret = -1;
                        ezlopi_device_free_device(gy271_device_child_z_device);
                    }
                }
                l_ezlopi_device_t* gy271_device_child_azi_device = ezlopi_device_add_device(cj_device, "Azimuth_Angle");
                if (gy271_device_child_azi_device)
                {
                    TRACE_I("Child_gy271-azi-[0x%x] ", gy271_device_child_azi_device->cloud_properties.device_id);
                    __prepare_device_cloud_properties_child_azi(gy271_device_child_azi_device, cj_device);
                    gy271_device_child_azi_device->cloud_properties.parent_device_id = gy271_device_parent_x_device->cloud_properties.device_id;
                    l_ezlopi_item_t* gyro_azi_item = ezlopi_device_add_item_to_device(gy271_device_child_azi_device, sensor_0007_I2C_GY271);
                    if (gyro_azi_item)
                    {
                        __prepare_item_cloud_properties(gyro_azi_item, user_data);
                        gyro_azi_item->cloud_properties.item_name = ezlopi_item_name_angle_position;
                        gyro_azi_item->cloud_properties.value_type = value_type_angle;
                        gyro_azi_item->cloud_properties.scale = scales_north_pole_degress;
                        __prepare_item_interface_properties(gyro_azi_item, cj_device);
                    }
                    else
                    {
                        ret = -1;
                        ezlopi_device_free_device(gy271_device_child_azi_device);
                    }
                }
                l_ezlopi_device_t* gy271_device_child_temp_device = ezlopi_device_add_device(cj_device, "Temp");
                if (gy271_device_child_temp_device)
                {
                    TRACE_I("Child_gy271-temp-[0x%x] ", gy271_device_child_temp_device->cloud_properties.device_id);
                    __prepare_device_cloud_properties_child_temp(gy271_device_child_temp_device, cj_device);
                    gy271_device_child_temp_device->cloud_properties.parent_device_id = gy271_device_parent_x_device->cloud_properties.device_id;
                    l_ezlopi_item_t* gyro_temp_item = ezlopi_device_add_item_to_device(gy271_device_child_temp_device, sensor_0007_I2C_GY271);
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
                        ret = -1;
                        ezlopi_device_free_device(gy271_device_child_temp_device);
                    }
                }

                if ((NULL == gyro_x_item) &&
                    (NULL == gy271_device_child_y_device) &&
                    (NULL == gy271_device_child_z_device) &&
                    (NULL == gy271_device_child_azi_device) &&
                    (NULL == gy271_device_child_temp_device))
                {
                    ret = -1;
                    ezlopi_device_free_device(gy271_device_parent_x_device);
                }
            }
            else // if the parent_device dosenot exsist then dealloc the 'user_data'
            {
                free(__FUNCTION__, user_data);
                ret = -1;
            }
        }
        else
        {
            ret = -1;
        }
    }
    return ret;
}

static int __init(l_ezlopi_item_t* item)
{
    int ret = 0;
    if (item)
    {
        s_gy271_data_t* user_data = (s_gy271_data_t*)item->user_arg;
        if (user_data)
        {
            if (item->interface.i2c_master.enable)
            {
                ezlopi_i2c_master_init(&item->interface.i2c_master);
                TRACE_I("I2C initialized to channel %d", item->interface.i2c_master.channel);
                if (0 == __gy271_configure(item)) // ESP_OK
                {
                    // TRACE_D(" CONFIGURATION  Compplete _____ Calibration Started _____");
                    TaskHandle_t ezlopi_sensor_gy271_callibrationb_task_handle = NULL;
                    xTaskCreate(__gy271_calibration_task, "GY271_Calibration_Task", EZLOPI_SENSOR_GY271_CALLIBRATION_TASK_DEPTH, item, 1, &ezlopi_sensor_gy271_callibrationb_task_handle);
                    ezpi_core_process_set_process_info(ENUM_EZLOPI_SENSOR_GY271_CALLIBRATION_TASK, &ezlopi_sensor_gy271_callibrationb_task_handle, EZLOPI_SENSOR_GY271_CALLIBRATION_TASK_DEPTH);
                    ret = 1;
                }
                else
                {
                    ret = -1;
                }
            }
            else
            {
                ret = -1;
            }
        }
        else
        {
            ret = -1;
        }
    }
    return ret;
}

static int __get_cjson_value(l_ezlopi_item_t* item, void* arg)
{
    int ret = 0;
    if (item && arg)
    {
        cJSON* cj_result = (cJSON*)arg;
        if (cj_result)
        {
            s_gy271_data_t* user_data = (s_gy271_data_t*)item->user_arg;
            if (user_data)
            {
                if (ezlopi_item_name_magnetic_strength_x_axis == item->cloud_properties.item_name)
                {
                    ezlopi_valueformatter_float_to_cjson(item, cj_result, user_data->X);
                }

                if (ezlopi_item_name_magnetic_strength_y_axis == item->cloud_properties.item_name)
                {
                    ezlopi_valueformatter_float_to_cjson(item, cj_result, user_data->Y);
                }

                if (ezlopi_item_name_magnetic_strength_z_axis == item->cloud_properties.item_name)
                {
                    ezlopi_valueformatter_float_to_cjson(item, cj_result, user_data->Z);
                }

                if (ezlopi_item_name_angle_position == item->cloud_properties.item_name)
                {
                    ezlopi_valueformatter_int32_to_cjson(item, cj_result, user_data->azimuth);
                }

                if (ezlopi_item_name_temp == item->cloud_properties.item_name)
                {
                    ezlopi_valueformatter_float_to_cjson(item, cj_result, user_data->T);
                }
            }

            ret = 1;
        }
    }

    return ret;
}

static int __notify(l_ezlopi_item_t* item)
{
    static float __prev[5] = { 0 };
    int ret = 0;
    if (item)
    {
        s_gy271_data_t* user_data = (s_gy271_data_t*)item->user_arg;
        if (user_data)
        {
            if (user_data->calibration_complete)
            {
                if (ezlopi_item_name_magnetic_strength_x_axis == item->cloud_properties.item_name)
                {
                    __prev[0] = user_data->X;
                    __prev[1] = user_data->Y;
                    __prev[2] = user_data->Z;
                    __prev[3] = user_data->T;
                    __prev[4] = user_data->azimuth;
                    if (__gy271_update_value(item))
                    {
                        if (fabs(__prev[0] - user_data->X) > 0.1)
                        {
                            ezlopi_device_value_updated_from_device_broadcast(item);
                        }
                    }
                }
                if (ezlopi_item_name_magnetic_strength_y_axis == item->cloud_properties.item_name)
                {
                    if (fabs(__prev[1] - user_data->Y) > 0.1)
                    {
                        ezlopi_device_value_updated_from_device_broadcast(item);
                    }
                }
                if (ezlopi_item_name_magnetic_strength_z_axis == item->cloud_properties.item_name)
                {
                    if (fabs(__prev[2] - user_data->Z) > 0.1)
                    {
                        ezlopi_device_value_updated_from_device_broadcast(item);
                    }
                }
                if (ezlopi_item_name_temp == item->cloud_properties.item_name)
                {
                    if (fabs(__prev[3] - user_data->T) > 0.5)
                    {
                        ezlopi_device_value_updated_from_device_broadcast(item);
                    }
                }
                if (ezlopi_item_name_angle_position == item->cloud_properties.item_name)
                {
                    if (fabs(__prev[4] - user_data->azimuth) > 1)
                    {
                        ezlopi_device_value_updated_from_device_broadcast(item);
                    }
                }
            }
        }

        ret = 1;
    }
    return ret;
}

static void __gy271_calibration_task(void* params) // calibrate task
{
    vTaskDelay(4000 / portTICK_PERIOD_MS);
    l_ezlopi_item_t* item = (l_ezlopi_item_t*)params;
    if (item)
    {
        int calibrationData[3][2] = { {0, 0},  // xmin,xmax
                                     {0, 0},  // ymin,ymax
                                     {0, 0} }; // zmin,zmax// Initialization added!
        s_gy271_data_t* user_data = (s_gy271_data_t*)item->user_arg;
        if (user_data)
        {
            for (uint16_t i = 0; i <= 50; i++)
            {
                __gy271_get_raw_max_min_values(item, calibrationData);
                vTaskDelay(200 / portTICK_PERIOD_MS);
            }

            TRACE_W(".....................Calculating Paramter.......................");
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

            TRACE_I("Bias :--- _Xaxis=%6ld | _Yaxis=%6ld | _Zaxis=%6ld ",
                user_data->calib_factor.bias_axis[0],
                user_data->calib_factor.bias_axis[1],
                user_data->calib_factor.bias_axis[2]);

            TRACE_I("Delta :--- _Xaxis=%6ld | _Yaxis=%6ld | _Zaxis=%6ld ",
                user_data->calib_factor.delta_axis[0],
                user_data->calib_factor.delta_axis[1],
                user_data->calib_factor.delta_axis[2]);
            TRACE_I("Delta_AVG :--- %6f", user_data->calib_factor.delta_avg);

            TRACE_I("Scale :--- _Xaxis=%6f | _Yaxis=%6f | _Zaxis=%6f ",
                user_data->calib_factor.scale_axis[0],
                user_data->calib_factor.scale_axis[1],
                user_data->calib_factor.scale_axis[0]);
            TRACE_W("......................CALIBRATION COMPLETE.....................");
            user_data->calibration_complete = true;
        }
    }
    ezpi_core_process_set_is_deleted(ENUM_EZLOPI_SENSOR_GY271_CALLIBRATION_TASK);
    vTaskDelete(NULL);
}
