#include "cJSON.h"
#include "trace.h"
#include "items.h"
#include "stdint.h"

#include "ezlopi_actions.h"
#include "ezlopi_timer.h"
#include "ezlopi_uart.h"
#include "ezlopi_cloud.h"
#include "ezlopi_devices_list.h"
#include "ezlopi_device_value_updated.h"
#include "ezlopi_cloud_category_str.h"
#include "ezlopi_cloud_subcategory_str.h"
#include "ezlopi_item_name_str.h"
#include "ezlopi_cloud_device_types_str.h"
#include "ezlopi_cloud_value_type_str.h"
#include "ezlopi_cloud_scales_str.h"

#include "gyGPS6MV2.h"
#include "0053_sensor_UART_GYGPS6MV2.h"

//------------------------------------------------------------------------
static bool gps_init_guard = false;
static bool gps_message_guard = false;
static char gps_sentence[MAX_GPGGA_SENTENCE_SIZE + 30];
static char gps_cir_buf[CIR_BUFSIZE];
static GPS6MV2_t sensor_0053_UART_gps6mv2_data = {0};

static uint32_t Latitude_item_id = 0;
static uint32_t Longitude_item_id = 0;
static uint32_t Fix_item_id = 0;
static uint32_t Sea_level_item_id = 0;
static uint32_t Geoid_item_id = 0;
//------------------------------------------------------------------------
static int __0053_prepare(void *arg);
static int __0053_init(l_ezlopi_item_t *item);
static int __0053_get_value_cjson(l_ezlopi_item_t *item, void *arg);
static int __0053_notify(l_ezlopi_item_t *item);
static int sensor_uart_gps6mv2_update_values(l_ezlopi_item_t *item);
static void Retrieve_GPGGA_sentence();
static void ezlopi_uart_gps6mv2_upcall(uint8_t *buffer, s_ezlopi_uart_object_handle_t uart_object_handle);

static void __prepare_device_cloud_properties(l_ezlopi_device_t *device, cJSON *cj_device);
static void __prepare_lat_item_cloud_properties(l_ezlopi_item_t *item, cJSON *cj_device);
static void __prepare_long_item_cloud_properties(l_ezlopi_item_t *item, cJSON *cj_device);
static void __prepare_fix_item_cloud_properties(l_ezlopi_item_t *item, cJSON *cj_device);
static void __prepare_sea_level_item_cloud_properties(l_ezlopi_item_t *item, cJSON *cj_device);
static void __prepare_geiod_item_cloud_properties(l_ezlopi_item_t *item, cJSON *cj_device);
static void __prepare_item_interface_properties(l_ezlopi_item_t *item, cJSON *cj_device);
//------------------------------------------------------------------------
int sensor_0053_uart_gps_v3(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg)
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
    // case EZLOPI_ACTION_HUB_GET_ITEM:
    // {
    //     ret = __0053_get_item(item, arg);
    //     break;
    // }
    case EZLOPI_ACTION_SET_VALUE:
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
    char *dev_name = NULL;
    CJSON_GET_VALUE_STRING(cj_device, "dev_name", dev_name);
    ASSIGN_DEVICE_NAME_V2(device, dev_name);
    device->cloud_properties.category = category_generic_sensor;
    device->cloud_properties.subcategory = subcategory_not_defined;
    device->cloud_properties.device_type = dev_type_sensor;
    device->cloud_properties.device_id = ezlopi_cloud_generate_device_id();
}
static void __prepare_lat_item_cloud_properties(l_ezlopi_item_t *item, cJSON *cj_device)
{
    item->cloud_properties.show = true;
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = true;
    item->cloud_properties.item_name = ezlopi_item_name_angle_position;
    item->cloud_properties.value_type = value_type_angle;
    item->cloud_properties.scale = scales_north_pole_degress;
    Latitude_item_id = ezlopi_cloud_generate_item_id();
    item->cloud_properties.item_id = Latitude_item_id;
    //----- CUSTOM DATA STRUCTURE -----------------------------------------
    item->user_arg = &sensor_0053_UART_gps6mv2_data;
}
static void __prepare_long_item_cloud_properties(l_ezlopi_item_t *item, cJSON *cj_device)
{
    item->cloud_properties.show = true;
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = true;
    item->cloud_properties.item_name = ezlopi_item_name_angle_position;
    item->cloud_properties.value_type = value_type_angle;
    item->cloud_properties.scale = scales_north_pole_degress;
    Longitude_item_id = ezlopi_cloud_generate_item_id();
    item->cloud_properties.item_id = Longitude_item_id;
    //----- CUSTOM DATA STRUCTURE -----------------------------------------
    item->user_arg = &sensor_0053_UART_gps6mv2_data;
}
static void __prepare_fix_item_cloud_properties(l_ezlopi_item_t *item, cJSON *cj_device)
{
    item->cloud_properties.show = true;
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = true;
    item->cloud_properties.item_name = ezlopi_item_name_basic;
    item->cloud_properties.value_type = value_type_bool;
    item->cloud_properties.scale = NULL;
    Fix_item_id = ezlopi_cloud_generate_item_id();
    item->cloud_properties.item_id = Fix_item_id;
    //----- CUSTOM DATA STRUCTURE -----------------------------------------
    item->user_arg = &sensor_0053_UART_gps6mv2_data;
}
static void __prepare_sea_level_item_cloud_properties(l_ezlopi_item_t *item, cJSON *cj_device)
{
    item->cloud_properties.show = true;
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = true;
    item->cloud_properties.item_name = ezlopi_item_name_distance;
    item->cloud_properties.value_type = value_type_length;
    item->cloud_properties.scale = scales_meter;
    Sea_level_item_id = ezlopi_cloud_generate_item_id();
    item->cloud_properties.item_id = Sea_level_item_id;
    //----- CUSTOM DATA STRUCTURE -----------------------------------------
    item->user_arg = &sensor_0053_UART_gps6mv2_data;
}
static void __prepare_geiod_item_cloud_properties(l_ezlopi_item_t *item, cJSON *cj_device)
{
    item->cloud_properties.show = true;
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = true;
    item->cloud_properties.item_name = ezlopi_item_name_distance;
    item->cloud_properties.value_type = value_type_length;
    item->cloud_properties.scale = scales_meter;
    Geoid_item_id = ezlopi_cloud_generate_item_id();
    item->cloud_properties.item_id = Geoid_item_id;
    //----- CUSTOM DATA STRUCTURE -----------------------------------------
    item->user_arg = &sensor_0053_UART_gps6mv2_data;
}

static void __prepare_item_interface_properties(l_ezlopi_item_t *item, cJSON *cj_device)
{
    CJSON_GET_VALUE_INT(cj_device, "dev_type", item->interface_type);
    CJSON_GET_VALUE_INT(cj_device, "gpio_tx", item->interface.uart.tx);
    CJSON_GET_VALUE_INT(cj_device, "gpio_rx", item->interface.uart.rx);
    CJSON_GET_VALUE_INT(cj_device, "baud", item->interface.uart.baudrate);
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
        {
            l_ezlopi_device_t *gps_device = ezlopi_device_add_device();
            if (gps_device)
            {
                __prepare_device_cloud_properties(gps_device, device_prep_arg->cjson_device);
                l_ezlopi_item_t *lat_item = ezlopi_device_add_item_to_device(gps_device, sensor_0053_uart_gps_v3);
                if (lat_item)
                {
                    __prepare_lat_item_cloud_properties(lat_item, cjson_device);
                    __prepare_item_interface_properties(lat_item, cjson_device);
                }
                l_ezlopi_item_t *long_item = ezlopi_device_add_item_to_device(gps_device, sensor_0053_uart_gps_v3);
                if (long_item)
                {
                    __prepare_long_item_cloud_properties(long_item, cjson_device);
                    __prepare_item_interface_properties(long_item, cjson_device);
                }
                l_ezlopi_item_t *fix_item = ezlopi_device_add_item_to_device(gps_device, sensor_0053_uart_gps_v3);
                if (fix_item)
                {
                    __prepare_fix_item_cloud_properties(fix_item, cjson_device);
                    __prepare_item_interface_properties(fix_item, cjson_device);
                }
                l_ezlopi_item_t *sea_level_item = ezlopi_device_add_item_to_device(gps_device, sensor_0053_uart_gps_v3);
                if (sea_level_item)
                {
                    __prepare_sea_level_item_cloud_properties(sea_level_item, cjson_device);
                    __prepare_item_interface_properties(sea_level_item, cjson_device);
                }
                l_ezlopi_item_t *geiod_item = ezlopi_device_add_item_to_device(gps_device, sensor_0053_uart_gps_v3);
                if (geiod_item)
                {
                    __prepare_geiod_item_cloud_properties(geiod_item, cjson_device);
                    __prepare_item_interface_properties(geiod_item, cjson_device);
                }
                if ((NULL == lat_item) && (NULL == long_item) && (NULL == fix_item) && (NULL == sea_level_item) && (NULL == geiod_item))
                {
                    ezlopi_device_free_device(gps_device);
                }
                ret = 1;
            }
            else
            {
                ezlopi_device_free_device(gps_device);
            }
        }
    }
    return 0;
}
//------------------------------------------------------------------------------
static int __0053_init(l_ezlopi_item_t *item)
{
    int ret = 0;
    if (item)
    {
        if ((!gps_init_guard) && GPIO_IS_VALID_GPIO(item->interface.uart.tx) && GPIO_IS_VALID_GPIO(item->interface.uart.rx))
        {
            s_ezlopi_uart_object_handle_t ezlopi_uart_object_handle = ezlopi_uart_init(item->interface.uart.baudrate, item->interface.uart.tx, item->interface.uart.rx, ezlopi_uart_gps6mv2_upcall, item);
            item->interface.uart.channel = ezlopi_uart_get_channel(ezlopi_uart_object_handle);
            // TRACE_W(" Initailization complete......");
            gps_init_guard = true;
            ret = 1;
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
        int lat_angle_val = 0, long_angle_val = 0, total_sat = 0, gps_quality = 0, antenna_alti = 0, geoid = 0;
        bool GPS_FIX = false;
        cJSON *cj_result = (cJSON *)arg;
        GPS6MV2_t *sensor_0053_UART_gps6mv2_data = (GPS6MV2_t *)item->user_arg;
        if (cj_result && sensor_0053_UART_gps6mv2_data)
        {
            char valueFormatted[20];
            if (Longitude_item_id == item->cloud_properties.item_id)
            {
                lat_angle_val = atoi(sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.Latitude.lat_degree) + (atoi(sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.Latitude.lat_min)) / 60;
                // TRACE_I("latitude : %d *deg", lat_angle_val);
                snprintf(valueFormatted, 20, "%d", lat_angle_val);
                cJSON_AddBoolToObject(cj_result, "valueFormatted", valueFormatted);
                cJSON_AddNumberToObject(cj_result, "value", lat_angle_val);
            }

            if (Latitude_item_id == item->cloud_properties.item_id)
            {
                long_angle_val = atoi(sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.Longitude.long_degree) + (atoi(sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.Longitude.long_min)) / 60;
                // TRACE_I("Longitude : %d *deg", long_angle_val);
                snprintf(valueFormatted, 20, "%d", long_angle_val);
                cJSON_AddBoolToObject(cj_result, "valueFormatted", valueFormatted);
                cJSON_AddNumberToObject(cj_result, "value", long_angle_val);
            }

            if (Fix_item_id == item->cloud_properties.item_id)
            {
                total_sat = atoi(sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.Satellites_used);
                // TRACE_I("No. of Satellites : %d ", total_sat);

                gps_quality = ((int)(sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.Positon_fix_quality) - 48); // converting character into 'integer'
                gps_quality = (gps_quality < 0) ? 0 : ((gps_quality > 9) ? 9 : gps_quality);
                // TRACE_I("GPSFix_Quality : %d ", gps_quality);
                GPS_FIX = ((total_sat > 2) && (gps_quality != 0)) ? true : false;
                // snprintf(valueFormatted, 20, "%d", GPS_FIX);
                cJSON_AddBoolToObject(cj_result, "valueFormatted", (GPS_FIX) ? "true" : "false");
                cJSON_AddBoolToObject(cj_result, "value", GPS_FIX);
            }

            if (Sea_level_item_id == item->cloud_properties.item_id)
            {
                antenna_alti = atoi(sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.Mean_sea_level);
                // TRACE_I("Antenna Altitude for mean_sea_level : %d m", antenna_alti);
                snprintf(valueFormatted, 20, "%d", antenna_alti);
                cJSON_AddBoolToObject(cj_result, "valueFormatted", valueFormatted);
                cJSON_AddNumberToObject(cj_result, "value", antenna_alti);
            }

            if (Geoid_item_id == item->cloud_properties.item_id)
            {
                geoid = atoi(sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.Geoid_Separation);
                // TRACE_I("Geoid seperation : %d m", geoid);
                snprintf(valueFormatted, 20, "%d", geoid);
                cJSON_AddBoolToObject(cj_result, "valueFormatted", valueFormatted);
                cJSON_AddNumberToObject(cj_result, "value", geoid);
            }

            ret = 1;
        }
    }
    return ret;
}

static int __0053_notify(l_ezlopi_item_t *item)
{
    if ((NULL != item) && (gps_init_guard))
    {
        if (Latitude_item_id == item->cloud_properties.item_id)
        {
            gps_message_guard = true;
            sensor_uart_gps6mv2_update_values(item);
        }
        gps_message_guard = false;
        ezlopi_device_value_updated_from_device(item);
    }
    return 1;
}
//------------------------------------------------------------------------------
static int sensor_uart_gps6mv2_update_values(l_ezlopi_item_t *item)
{
    int ret = 0, len = 0;
    // 'void_type' addrress -> 'GPS6MV2_t' address
    GPS6MV2_t *sensor_0053_UART_gps6mv2_data = (GPS6MV2_t *)item->user_arg;

    if (NULL != item)
    {
        // replace the gps_sentence of GPGGA structure
        if (gps_sentence != NULL)
        {
            // reset the GPGGA_sentence array
            len = sizeof(sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.GPGGA_sentence);
            memset(sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.GPGGA_sentence, 0, sizeof(len));
            sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.GPGGA_sentence[len - 1] = '\0';
            strncpy(sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.GPGGA_sentence, gps_sentence, (len - 1));
        }

        // Check availability of Checksum in the message
        if (strchr((sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.GPGGA_sentence), '*') != NULL)
        {
            //-------------------------------------------------------------------------
            // GPGGA MESSAGE PARSING FUNCTION
            //-------------------------------------------------------------------------
            parse_and_assign_GPGGA_message(sensor_0053_UART_gps6mv2_data);
        }
        gps_message_guard = false;
    }
    return ret;
}

//-------------------------------------------------------------------------
static void Retrieve_GPGGA_sentence()
{
    if (NULL == strstr(gps_cir_buf, "$GPTXT"))
    {
        // TRACE_I("................Extracting GPGGA gps_sentence ...........");
        // find the position of '$GSGGA' in the circular buffer
        /**                                 2000
         *   circular_buffer = 'dkfhkhdf .... $GPGGA,061731.00,2740.52772,....,-41.3,M,,*40.....'
         *                                    ^
         *                             ptr1 => $GPGGA,061731.00,2740.52772,....,-41.3,M,,*40.....
         *
         */
        char *ptr1 = strstr(gps_cir_buf, "$GPGGA"); // returns a pointer points to the first character of the found 'another_buffer' in "$GPGGA"
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
                    if (gps_message_guard == false)
                    {
                        memset(&gps_sentence, 0, sizeof(gps_sentence));
                        memcpy(gps_sentence, ptr1, (ptr2 - ptr1)); // (dest_addr , from_addr , length)
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

static void ezlopi_uart_gps6mv2_upcall(uint8_t *buffer, s_ezlopi_uart_object_handle_t uart_object_handle)
{
    // TRACE_E("UART_Buffer => \n%s\n", buffer);
    char *another_buffer = (char *)malloc(256);
    if (another_buffer)
    {
        memcpy(another_buffer, buffer, 256);
        if (strlen(gps_cir_buf) < (CIR_BUFSIZE)-255)
        {
            strncpy((gps_cir_buf + strlen(gps_cir_buf)), another_buffer, strlen(another_buffer));
        }
        else
        {
            // TRACE_E("----------- 2. CIR_BUFF - FULL -------------");
            // TRACE_I("CIRCULAR_BUFFER  => [%d] \n%s", strlen(gps_cir_buf), gps_cir_buf);
            Retrieve_GPGGA_sentence();                   // CALL a function that extracts the 'GPGGA_sentence' from gps_cir_buf[]
            memset(gps_cir_buf, 0, sizeof(gps_cir_buf)); // reset gps_cir_buf[]
        }
        free(another_buffer);
    }
}

//-------------------------------------------------------------------------