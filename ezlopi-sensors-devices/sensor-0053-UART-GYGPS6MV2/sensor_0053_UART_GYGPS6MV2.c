#include <string.h>
#include "ezlopi_util_trace.h"

#include "ezlopi_core_timer.h"
#include "ezlopi_core_cloud.h"
#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_core_valueformatter.h"
#include "ezlopi_core_device_value_updated.h"

#include "ezlopi_hal_uart.h"

#include "ezlopi_cloud_items.h"
#include "ezlopi_cloud_constants.h"

#include "gyGPS6MV2.h"
#include "sensor_0053_UART_GYGPS6MV2.h"

//------------------------------------------------------------------------
static int __0053_prepare(void *arg);
static int __0053_init(l_ezlopi_item_t *item);
static int __0053_get_value_cjson(l_ezlopi_item_t *item, void *arg);
static int __0053_notify(l_ezlopi_item_t *item);

static int __sensor_uart_gps6mv2_update_values(l_ezlopi_item_t *item);
static void __retrieve_GPGGA_sentence(l_ezlopi_item_t *item);
static void __uart_gps6mv2_upcall(uint8_t *buffer, uint32_t output_len, s_ezlopi_uart_object_handle_t uart_object_handle);

static void __prepare_device_cloud_properties(l_ezlopi_device_t *device, cJSON *cj_device);
static void __prepare_lat_item_cloud_properties(l_ezlopi_item_t *item, cJSON *cj_device, GPS6MV2_t *gps_arg);
static void __prepare_long_item_cloud_properties(l_ezlopi_item_t *item, cJSON *cj_device, GPS6MV2_t *gps_arg);
static void __prepare_fix_item_cloud_properties(l_ezlopi_item_t *item, cJSON *cj_device, GPS6MV2_t *gps_arg);
static void __prepare_sea_level_item_cloud_properties(l_ezlopi_item_t *item, cJSON *cj_device, GPS6MV2_t *gps_arg);
static void __prepare_geiod_item_cloud_properties(l_ezlopi_item_t *item, cJSON *cj_device, GPS6MV2_t *gps_arg);
static void __prepare_item_interface_properties(l_ezlopi_item_t *item, cJSON *cj_device);

//------------------------------------------------------------------------
int sensor_0053_UART_GYGPS6MV2(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg)
{
    int ret = 0;
    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        ret = __0053_prepare(arg);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        ret = __0053_init(item);
        break;
    }
    case EZLOPI_ACTION_HUB_GET_ITEM:
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        ret = __0053_get_value_cjson(item, arg);
        break;
    }
    case EZLOPI_ACTION_NOTIFY_1000_MS:
    {
        ret = __0053_notify(item);
        break;
    }
    default:
    {
        break;
    }
    }

    return ret;
}
//---------------------------------------------------------------------------------------------------------
static void __prepare_device_cloud_properties(l_ezlopi_device_t *device, cJSON *cj_device)
{
    device->cloud_properties.category = category_generic_sensor;
    device->cloud_properties.subcategory = subcategory_not_defined;
    device->cloud_properties.device_type = dev_type_sensor;
    device->cloud_properties.info = NULL;
    device->cloud_properties.device_type_id = NULL;
}
static void __prepare_lat_item_cloud_properties(l_ezlopi_item_t *item, cJSON *cj_device, GPS6MV2_t *gps_arg)
{
    item->cloud_properties.show = true;
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = false;
    item->cloud_properties.item_name = ezlopi_item_name_angle_position;
    item->cloud_properties.value_type = value_type_angle;
    item->cloud_properties.scale = scales_north_pole_degress;
    gps_arg->Latitude_item_id = ezlopi_cloud_generate_item_id();
    item->cloud_properties.item_id = gps_arg->Latitude_item_id;
    //----- CUSTOM DATA STRUCTURE -----------------------------------------
    item->user_arg = (void *)gps_arg;
}
static void __prepare_long_item_cloud_properties(l_ezlopi_item_t *item, cJSON *cj_device, GPS6MV2_t *gps_arg)
{
    item->cloud_properties.show = true;
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = false;
    item->cloud_properties.item_name = ezlopi_item_name_angle_position;
    item->cloud_properties.value_type = value_type_angle;
    item->cloud_properties.scale = scales_north_pole_degress;
    gps_arg->Longitude_item_id = ezlopi_cloud_generate_item_id();
    item->cloud_properties.item_id = gps_arg->Longitude_item_id;
    //----- CUSTOM DATA STRUCTURE -----------------------------------------
    item->user_arg = (void *)gps_arg;
}
static void __prepare_fix_item_cloud_properties(l_ezlopi_item_t *item, cJSON *cj_device, GPS6MV2_t *gps_arg)
{
    item->cloud_properties.show = true;
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = false;
    item->cloud_properties.item_name = ezlopi_item_name_basic;
    item->cloud_properties.value_type = value_type_bool;
    item->cloud_properties.scale = NULL;
    gps_arg->Fix_item_id = ezlopi_cloud_generate_item_id();
    item->cloud_properties.item_id = gps_arg->Fix_item_id;
    //----- CUSTOM DATA STRUCTURE -----------------------------------------
    item->user_arg = (void *)gps_arg;
}
static void __prepare_sea_level_item_cloud_properties(l_ezlopi_item_t *item, cJSON *cj_device, GPS6MV2_t *gps_arg)
{
    item->cloud_properties.show = true;
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = false;
    item->cloud_properties.item_name = ezlopi_item_name_distance;
    item->cloud_properties.value_type = value_type_length;
    item->cloud_properties.scale = scales_meter;
    gps_arg->Sea_level_item_id = ezlopi_cloud_generate_item_id();
    item->cloud_properties.item_id = gps_arg->Sea_level_item_id;
    //----- CUSTOM DATA STRUCTURE -----------------------------------------
    item->user_arg = (void *)gps_arg;
}
static void __prepare_geiod_item_cloud_properties(l_ezlopi_item_t *item, cJSON *cj_device, GPS6MV2_t *gps_arg)
{
    item->cloud_properties.show = true;
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = false;
    item->cloud_properties.item_name = ezlopi_item_name_distance;
    item->cloud_properties.value_type = value_type_length;
    item->cloud_properties.scale = scales_meter;
    gps_arg->Geoid_item_id = ezlopi_cloud_generate_item_id();
    item->cloud_properties.item_id = gps_arg->Geoid_item_id;
    //----- CUSTOM DATA STRUCTURE -----------------------------------------
    item->user_arg = (void *)gps_arg;
}

static void __prepare_item_interface_properties(l_ezlopi_item_t *item, cJSON *cj_device)
{
    CJSON_GET_VALUE_INT(cj_device, ezlopi_dev_type_str, item->interface_type);
    CJSON_GET_VALUE_INT(cj_device, ezlopi_gpio_tx_str, item->interface.uart.tx);
    CJSON_GET_VALUE_INT(cj_device, ezlopi_gpio_rx_str, item->interface.uart.rx);
    CJSON_GET_VALUE_INT(cj_device, ezlopi_baud_str, item->interface.uart.baudrate);

    /*Here we decide, when uart is allowed to initialize*/
    GPS6MV2_t *sensor_0053_UART_gps6mv2_data = (GPS6MV2_t *)item->user_arg;

    if ((sensor_0053_UART_gps6mv2_data->Latitude_item_id) == item->cloud_properties.item_id)
    {
        item->interface.uart.enable = true;
    }
    else
    {
        item->interface.uart.enable = false;
    }
}
//---------------------------------------------------------------------------------------------------------
static int __0053_prepare(void *arg)
{
    int ret = 0;
    s_ezlopi_prep_arg_t *device_prep_arg = (s_ezlopi_prep_arg_t *)arg;
    if (device_prep_arg)
    {
        cJSON *cjson_device = device_prep_arg->cjson_device;
        if (cjson_device)
        { // the structure to hold GPS_parameter_values
            GPS6MV2_t *sensor_0053_UART_gps6mv2_data = (GPS6MV2_t *)malloc(sizeof(GPS6MV2_t));
            if (NULL != sensor_0053_UART_gps6mv2_data)
            {
                memset(sensor_0053_UART_gps6mv2_data, 0, sizeof(GPS6MV2_t));
                l_ezlopi_device_t *gps_device = ezlopi_device_add_device(cjson_device);
                if (gps_device)
                {
                    __prepare_device_cloud_properties(gps_device, device_prep_arg->cjson_device);

                    l_ezlopi_item_t *lat_item = ezlopi_device_add_item_to_device(gps_device, sensor_0053_UART_GYGPS6MV2);
                    if (lat_item)
                    {
                        lat_item->cloud_properties.device_id = gps_device->cloud_properties.device_id;
                        __prepare_lat_item_cloud_properties(lat_item, cjson_device, sensor_0053_UART_gps6mv2_data);
                        __prepare_item_interface_properties(lat_item, cjson_device);
                    }
                    l_ezlopi_item_t *long_item = ezlopi_device_add_item_to_device(gps_device, sensor_0053_UART_GYGPS6MV2);
                    if (long_item)
                    {
                        long_item->cloud_properties.device_id = gps_device->cloud_properties.device_id;
                        __prepare_long_item_cloud_properties(long_item, cjson_device, sensor_0053_UART_gps6mv2_data);
                        __prepare_item_interface_properties(long_item, cjson_device);
                    }
                    l_ezlopi_item_t *fix_item = ezlopi_device_add_item_to_device(gps_device, sensor_0053_UART_GYGPS6MV2);
                    if (fix_item)
                    {
                        fix_item->cloud_properties.device_id = gps_device->cloud_properties.device_id;
                        __prepare_fix_item_cloud_properties(fix_item, cjson_device, sensor_0053_UART_gps6mv2_data);
                        __prepare_item_interface_properties(fix_item, cjson_device);
                    }
                    l_ezlopi_item_t *sea_level_item = ezlopi_device_add_item_to_device(gps_device, sensor_0053_UART_GYGPS6MV2);
                    if (sea_level_item)
                    {
                        sea_level_item->cloud_properties.device_id = gps_device->cloud_properties.device_id;
                        __prepare_sea_level_item_cloud_properties(sea_level_item, cjson_device, sensor_0053_UART_gps6mv2_data);
                        __prepare_item_interface_properties(sea_level_item, cjson_device);
                    }
                    l_ezlopi_item_t *geiod_item = ezlopi_device_add_item_to_device(gps_device, sensor_0053_UART_GYGPS6MV2);
                    if (geiod_item)
                    {
                        geiod_item->cloud_properties.device_id = gps_device->cloud_properties.device_id;
                        __prepare_geiod_item_cloud_properties(geiod_item, cjson_device, sensor_0053_UART_gps6mv2_data);
                        __prepare_item_interface_properties(geiod_item, cjson_device);
                    }
                    ret = 1;
                    if ((NULL == lat_item) && (NULL == long_item) && (NULL == fix_item) && (NULL == sea_level_item) && (NULL == geiod_item))
                    {
                        ret = -1;
                        ezlopi_device_free_device(gps_device);
                        free(sensor_0053_UART_gps6mv2_data);
                    }
                }
                else
                {
                    ret = -1;
                    free(sensor_0053_UART_gps6mv2_data);
                }
            }
        }
    }

    return ret;
}
//----------------------------------------------------------------------------------------------------------------------
static int __0053_init(l_ezlopi_item_t *item)
{
    int ret = 0;
    if (item)
    {
        if ((true == item->interface.uart.enable) && GPIO_IS_VALID_GPIO(item->interface.uart.tx) && GPIO_IS_VALID_GPIO(item->interface.uart.rx))
        {
            s_ezlopi_uart_object_handle_t ezlopi_uart_object_handle = ezlopi_uart_init(item->interface.uart.baudrate, item->interface.uart.tx, item->interface.uart.rx, __uart_gps6mv2_upcall, item);
            item->interface.uart.channel = ezlopi_uart_get_channel(ezlopi_uart_object_handle);
            // TRACE_W(" Initailization complete......");
            ret = 1;
        }
        if (0 == ret)
        {
            ret = -1;
            if (item->user_arg)
            {
                free(item->user_arg);
                item->user_arg = NULL;
            }
        }
    }
    return ret;
}
//------------------------------------------------------------------------------
static int __0053_get_value_cjson(l_ezlopi_item_t *item, void *arg)
{
    int ret = 0;
    if (item && arg)
    {
        cJSON *cj_result = (cJSON *)arg;
        if (cj_result)
        {
            GPS6MV2_t *sensor_0053_UART_gps6mv2_data = (GPS6MV2_t *)item->user_arg;

            if ((sensor_0053_UART_gps6mv2_data->Latitude_item_id) == item->cloud_properties.item_id)
            {
                char *valueFormatted = ezlopi_valueformatter_float(sensor_0053_UART_gps6mv2_data->prev_lat_angle_val);
                cJSON_AddStringToObject(cj_result, ezlopi_valueFormatted_str, valueFormatted);
                cJSON_AddNumberToObject(cj_result, ezlopi_value_str, sensor_0053_UART_gps6mv2_data->prev_lat_angle_val);
                free(valueFormatted);
            }
            if ((sensor_0053_UART_gps6mv2_data->Longitude_item_id) == item->cloud_properties.item_id)
            {
                char *valueFormatted = ezlopi_valueformatter_float(sensor_0053_UART_gps6mv2_data->prev_long_angle_val);
                cJSON_AddStringToObject(cj_result, ezlopi_valueFormatted_str, valueFormatted);
                cJSON_AddNumberToObject(cj_result, ezlopi_value_str, sensor_0053_UART_gps6mv2_data->prev_long_angle_val);
                free(valueFormatted);
            }

            if ((sensor_0053_UART_gps6mv2_data->Fix_item_id) == item->cloud_properties.item_id)
            {
                const char *valueFormatted = ezlopi_valueformatter_bool(sensor_0053_UART_gps6mv2_data->prev_GPS_FIX);
                cJSON_AddStringToObject(cj_result, ezlopi_valueFormatted_str, valueFormatted);
                cJSON_AddBoolToObject(cj_result, ezlopi_value_str, sensor_0053_UART_gps6mv2_data->prev_GPS_FIX);
            }

            if ((sensor_0053_UART_gps6mv2_data->Sea_level_item_id) == item->cloud_properties.item_id)
            {
                char *valueFormatted = ezlopi_valueformatter_float(sensor_0053_UART_gps6mv2_data->prev_antenna_alti);
                cJSON_AddStringToObject(cj_result, ezlopi_valueFormatted_str, valueFormatted);
                cJSON_AddNumberToObject(cj_result, ezlopi_value_str, sensor_0053_UART_gps6mv2_data->prev_antenna_alti);
                free(valueFormatted);
            }

            if ((sensor_0053_UART_gps6mv2_data->Geoid_item_id) == item->cloud_properties.item_id)
            {
                char *valueFormatted = ezlopi_valueformatter_float(sensor_0053_UART_gps6mv2_data->prev_geoid);
                cJSON_AddStringToObject(cj_result, ezlopi_valueFormatted_str, valueFormatted);
                cJSON_AddNumberToObject(cj_result, ezlopi_value_str, sensor_0053_UART_gps6mv2_data->prev_geoid);
                free(valueFormatted);
            }
            ret = 1;
        }
    }
    return ret;
}

static int __0053_notify(l_ezlopi_item_t *item)
{
    if (item)
    {
        GPS6MV2_t *sensor_0053_UART_gps6mv2_data = (GPS6MV2_t *)item->user_arg;
        if ((sensor_0053_UART_gps6mv2_data->Latitude_item_id) == item->cloud_properties.item_id)
        {
            // Invoking data Update only, in this item_id
            (sensor_0053_UART_gps6mv2_data->gps_message_guard) = true;
            __sensor_uart_gps6mv2_update_values(item);

            // checking for new values
            float lat_angle_val = (float)atoi(sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.Latitude.lat_degree) + ((float)atoi(sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.Latitude.lat_min)) / 60.0f;
            if ((sensor_0053_UART_gps6mv2_data->prev_lat_angle_val) != lat_angle_val)
            {
                (sensor_0053_UART_gps6mv2_data->prev_lat_angle_val) = lat_angle_val;
                (sensor_0053_UART_gps6mv2_data->gps_message_guard) = false;
                ezlopi_device_value_updated_from_device_v3(item);
            }
        }
        if ((sensor_0053_UART_gps6mv2_data->Longitude_item_id) == item->cloud_properties.item_id)
        {
            // checking for new values
            float long_angle_val = (float)atoi(sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.Longitude.long_degree) + ((float)atoi(sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.Longitude.long_min)) / 60.0f;
            if ((sensor_0053_UART_gps6mv2_data->prev_long_angle_val) != long_angle_val)
            {
                (sensor_0053_UART_gps6mv2_data->prev_long_angle_val) = long_angle_val;
                (sensor_0053_UART_gps6mv2_data->gps_message_guard) = false;
                ezlopi_device_value_updated_from_device_v3(item);
            }
        }
        if ((sensor_0053_UART_gps6mv2_data->Fix_item_id) == item->cloud_properties.item_id)
        {
            // checking for new values
            int total_sat = (float)atoi(sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.Satellites_used);
            int gps_quality = ((int)(sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.Positon_fix_quality) - 48); // converting character into 'integer'
            gps_quality = (gps_quality < 0) ? 0 : ((gps_quality > 9) ? 9 : gps_quality);

            bool GPS_FIX = ((total_sat > 2) && (gps_quality != 0)) ? true : false;
            if ((sensor_0053_UART_gps6mv2_data->prev_GPS_FIX) != GPS_FIX)
            {
                (sensor_0053_UART_gps6mv2_data->prev_GPS_FIX) = GPS_FIX;
                (sensor_0053_UART_gps6mv2_data->gps_message_guard) = false;
                ezlopi_device_value_updated_from_device_v3(item);
            }
        }
        if ((sensor_0053_UART_gps6mv2_data->Sea_level_item_id) == item->cloud_properties.item_id)
        {
            // checking for new values
            float antenna_alti = (float)atoi(sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.Mean_sea_level);
            if ((sensor_0053_UART_gps6mv2_data->prev_antenna_alti) != antenna_alti)
            {
                (sensor_0053_UART_gps6mv2_data->prev_antenna_alti) = antenna_alti;
                (sensor_0053_UART_gps6mv2_data->gps_message_guard) = false;
                ezlopi_device_value_updated_from_device_v3(item);
            }
        }

        if ((sensor_0053_UART_gps6mv2_data->Geoid_item_id) == item->cloud_properties.item_id)
        {
            // checking for new values
            float geoid = (float)atoi(sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.Geoid_Separation);
            if ((sensor_0053_UART_gps6mv2_data->prev_geoid) != geoid)
            {
                (sensor_0053_UART_gps6mv2_data->prev_geoid) = geoid;
                (sensor_0053_UART_gps6mv2_data->gps_message_guard) = false;
                ezlopi_device_value_updated_from_device_v3(item);
            }
        }
    }
    return 1;
}
//------------------------------------------------------------------------------
static int __sensor_uart_gps6mv2_update_values(l_ezlopi_item_t *item)
{
    int ret = 0, len = 0;
    // 'void_type' addrress -> 'GPS6MV2_t' address
    GPS6MV2_t *sensor_0053_UART_gps6mv2_data = (GPS6MV2_t *)item->user_arg;

    if (NULL != item)
    {
        // replace the gps_sentence of GPGGA structure
        if ((sensor_0053_UART_gps6mv2_data->gps_sentence) != NULL)
        {
            // reset the GPGGA_sentence array
            len = sizeof(sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.GPGGA_sentence);
            memset(sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.GPGGA_sentence, 0, sizeof(len));
            sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.GPGGA_sentence[len - 1] = '\0';
            strncpy(sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.GPGGA_sentence, (sensor_0053_UART_gps6mv2_data->gps_sentence), (len - 1));
        }

        // Check availability of Checksum in the message
        if (strchr((sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.GPGGA_sentence), '*') != NULL)
        {
            //-------------------------------------------------------------------------
            // GPGGA MESSAGE PARSING FUNCTION
            //-------------------------------------------------------------------------
            parse_and_assign_GPGGA_message(sensor_0053_UART_gps6mv2_data);
        }
        (sensor_0053_UART_gps6mv2_data->gps_message_guard) = false;
    }
    return ret;
}

//-------------------------------------------------------------------------
static void __uart_gps6mv2_upcall(uint8_t *buffer, uint32_t output_len, s_ezlopi_uart_object_handle_t uart_object_handle)
{
    // TRACE_E("UART_Buffer => \n%s\n", buffer);
    char *another_buffer = (char *)malloc(256);
    if (another_buffer && (uart_object_handle->arg))
    {
        memcpy(another_buffer, buffer, 256);
        l_ezlopi_item_t *item = (l_ezlopi_item_t *)uart_object_handle->arg;
        GPS6MV2_t *sensor_0053_UART_gps6mv2_data = (GPS6MV2_t *)item->user_arg;

        if (strlen(sensor_0053_UART_gps6mv2_data->gps_cir_buf) < (CIR_BUFSIZE)-255)
        {
            strncpy(((sensor_0053_UART_gps6mv2_data->gps_cir_buf) + strlen(sensor_0053_UART_gps6mv2_data->gps_cir_buf)), another_buffer, strlen(another_buffer));
        }
        else
        {
            // TRACE_E("----------- 2. CIR_BUFF - FULL -------------");
            // TRACE_S("CIRCULAR_BUFFER  => [%d] \n%s", strlen(gps_cir_buf), gps_cir_buf);
            __retrieve_GPGGA_sentence(item);                                                                             // CALL a function that extracts the 'GPGGA_sentence' from gps_cir_buf[]
            memset((sensor_0053_UART_gps6mv2_data->gps_cir_buf), 0, sizeof(sensor_0053_UART_gps6mv2_data->gps_cir_buf)); // reset gps_cir_buf[]
        }
        free(another_buffer);
    }
}

static void __retrieve_GPGGA_sentence(l_ezlopi_item_t *item)
{
    if (NULL != item)
    {
        GPS6MV2_t *sensor_0053_UART_gps6mv2_data = (GPS6MV2_t *)item->user_arg;
        if (NULL == strstr((sensor_0053_UART_gps6mv2_data->gps_cir_buf), "$GPTXT"))
        {
            // TRACE_E("................Extracting GPGGA gps_sentence ...........");
            // find the position of '$GSGGA' in the circular buffer
            /**                                 2000
             *   circular_buffer = 'dkfhkhdf .... $GPGGA,061731.00,2740.52772,....,-41.3,M,,*40.....'
             *                                    ^
             *                             ptr1 => $GPGGA,061731.00,2740.52772,....,-41.3,M,,*40.....
             *
             */
            char *ptr1 = strstr((sensor_0053_UART_gps6mv2_data->gps_cir_buf), "$GPGGA"); // returns a pointer points to the first character of the found 'another_buffer' in "$GPGGA"
                                                                                         // otherwise a null pointer if "$GPGGA" is not present in 'another_buffer'.
                                                                                         // If "[$GPGGA]" destination string, points to an empty string, 'another_buffer' is returned

            // Continue only if the 'GSGGA-message' exists
            if (NULL != ptr1)
            {

                /**                                                        2053
                 *      ptr1 => '$GPGGA,061731.00,2740.52772,....,-41.3,M,,*40
                 *                                                            ^
                 *                                                         *40_ <- ptr2.....
                 *
                 */

                char *ptr2 = strchr(ptr1, '*'); // *  <- ptr2
                                                // check if ptr2 exists
                if (NULL != ptr2)
                {              // NOW , separate the GSGGA-message and copy to global variable 'gps_sentence'
                    ptr2 += 3; // +3 because the checksum consists three character more [*XX]
                    // TRACE_E("GPGGA message size = %d (< %d)", (ptr2 - ptr1), MAX_GPGGA_SENTENCE_SIZE);
                    if ((ptr2 - ptr1) > 0 && (ptr2 - ptr1) < MAX_GPGGA_SENTENCE_SIZE)
                    {
                        if (false == (sensor_0053_UART_gps6mv2_data->gps_message_guard))
                        {
                            memset(&(sensor_0053_UART_gps6mv2_data->gps_sentence), 0, sizeof(sensor_0053_UART_gps6mv2_data->gps_sentence));
                            memcpy((sensor_0053_UART_gps6mv2_data->gps_sentence), ptr1, (ptr2 - ptr1)); // (dest_addr , from_addr , length)
                            // TRACE_W("gps_sentence (NEW) => %s", gps_sentence);
                        }
                        else
                        {
                            TRACE_E(" gps_Message_Guard => ON ; Cannot copy NEW message.... in 'gps_sentence'");
                        }
                    }
                }
            }
            else
            {
                TRACE_E("NO GPGGA message found in current circular buffer stack....");
            }
        }
    }
}

//-------------------------------------------------------------------------