
#include "trace.h"
#include "string.h"
#include "esp_err.h"
#include "stdbool.h"
#include "ezlopi_i2c_master.h"
#include "ezlopi_timer.h"
#include "cJSON.h"
#include "driver/gpio.h"
#include "ezlopi_cloud_category_str.h"
#include "ezlopi_item_name_str.h"
#include "ezlopi_cloud_subcategory_str.h"
#include "ezlopi_cloud_device_types_str.h"
#include "ezlopi_cloud_value_type_str.h"
#include "ezlopi_device_value_updated.h"

#include "gyGPS6MV2.h"
#include "sensor_0053_UART_GYGPS6MV2.h"

//------------------------------------------------------------------------------
static const uint32_t Latitude_item_id = 1;
static const uint32_t Longitude_item_id = 2;
static const uint32_t GPS_fix_item_id = 3;
static const uint32_t Sea_level_item_id = 4;
static const uint32_t Geoid_item_id = 5;

static bool gps_init_guard = false;
static bool gps_message_guard = false;
static char gps_sentence[MAX_GPGGA_SENTENCE_SIZE + 30];
static char gps_cir_buf[CIR_BUFSIZE];

//------------------------------------------------------------------------------
static s_ezlopi_device_properties_t *properties = NULL;
// static s_ezlopi_device_properties_t *Latitude_properties = NULL;
// static s_ezlopi_device_properties_t *Longitude_properties = NULL;
// static s_ezlopi_device_properties_t *GPS_fix_properties = NULL;
// static s_ezlopi_device_properties_t *Mean_sea_level_properties = NULL;
// static s_ezlopi_device_properties_t *Geoid_Separation_properties = NULL;
//------------------------------------------------------------------------------
#define ADD_PROPERTIES_DEVICE_LIST(_properties, device_id, category, subcategory, item_name, item_id, value_type, cjson_device, sensor_0053_UART_gps6mv2_data) \
    {                                                                                                                                                          \
        _properties = sensor_0053_gps6mv2_prepare_properties(device_id, category, subcategory, item_name, item_id,                                             \
                                                             value_type, cjson_device, sensor_0053_UART_gps6mv2_data);                                         \
        if (NULL != _properties)                                                                                                                               \
        {                                                                                                                                                      \
            add_device_to_list(prep_arg, _properties, NULL);                                                                                                   \
        }                                                                                                                                                      \
    }

static int add_device_to_list(s_ezlopi_prep_arg_t *prep_arg, s_ezlopi_device_properties_t *properties, void *user_arg);
static int sensor_uart_gps6mv2_prepare_and_add(void *arg);
static s_ezlopi_device_properties_t *sensor_0053_gps6mv2_prepare_properties(uint32_t DEVICE_ID, const char *CATEGORY, const char *SUB_CATEGORY,
                                                                            const char *ITEM_NAME, uint32_t ITEM_ID, const char *VALUE_TYPE,
                                                                            cJSON *cjson_device, GPS6MV2_t *sensor_0053_UART_gps6mv2_data);
static int sensor_uart_gps6mv2_init(s_ezlopi_device_properties_t *properties);
static void sensor_uart_gps6mv2_get_item(s_ezlopi_device_properties_t *properties, void *args);
static int sensor_uart_gps6mv2_get_value_cjson(s_ezlopi_device_properties_t *properties, void *args);
static int sensor_uart_gps6mv2_update_values(s_ezlopi_device_properties_t *properties);
static void Retrieve_GPGGA_sentence();
static void ezlopi_uart_gps6mv2_upcall(uint8_t *buffer, s_ezlopi_uart_object_handle_t uart_object_handle, void *user_args);
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
int sensor_0053_UART_GPS6MV2(e_ezlopi_actions_t action, s_ezlopi_device_properties_t *properties, void *arg, void *user_arg)
{
    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        sensor_uart_gps6mv2_prepare_and_add(arg);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        sensor_uart_gps6mv2_init(properties);
        break;
    }
    case EZLOPI_ACTION_HUB_GET_ITEM:
    {
        sensor_uart_gps6mv2_get_item(properties, arg);
        break;
    }
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        sensor_uart_gps6mv2_get_value_cjson(properties, arg);
        break;
    }
    case EZLOPI_ACTION_NOTIFY_1000_MS:
    {
        if (gps_init_guard)
        {
            if (1 == properties->ezlopi_cloud.item_id)
            {
                gps_message_guard = true;
                sensor_uart_gps6mv2_update_values(properties);
            }
            gps_message_guard = false;
            ezlopi_device_value_updated_from_device(properties);
        }
        break;
    }
    default:
        break;
    }
    return 0;
}
//------------------------------------------------------------------------------

static s_ezlopi_device_properties_t *sensor_0053_gps6mv2_prepare_properties(uint32_t DEVICE_ID, const char *CATEGORY, const char *SUB_CATEGORY, const char *ITEM_NAME, uint32_t ITEM_ID, const char *VALUE_TYPE, cJSON *cjson_device, GPS6MV2_t *sensor_0053_UART_gps6mv2_data)
{
    s_ezlopi_device_properties_t *sensor_uart_gps6mv2_properties = NULL;

    if ((NULL != cjson_device))
    {
        sensor_uart_gps6mv2_properties = (s_ezlopi_device_properties_t *)malloc(sizeof(s_ezlopi_device_properties_t));
        if (sensor_uart_gps6mv2_properties)
        {
            memset(sensor_uart_gps6mv2_properties, 0, sizeof(s_ezlopi_device_properties_t));
            sensor_uart_gps6mv2_properties->interface_type = EZLOPI_DEVICE_INTERFACE_UART;

            char *device_name = NULL;
            switch (ITEM_ID)
            {
            case 1:
                device_name = "Latitude";
                break;
            case 2:
                device_name = "Longitude";
                break;
            case 3:
                device_name = "GPS-Fix Valid";
                break;
            case 4:
                device_name = "Antenna-Altitude";
                break;
            case 5:
                device_name = "Geoid Seperation";
                break;
            default:
                break;
            }
            // designate device according to item name
            ASSIGN_DEVICE_NAME(sensor_uart_gps6mv2_properties, device_name);
            sensor_uart_gps6mv2_properties->ezlopi_cloud.category = CATEGORY;
            sensor_uart_gps6mv2_properties->ezlopi_cloud.subcategory = SUB_CATEGORY;
            sensor_uart_gps6mv2_properties->ezlopi_cloud.item_name = ITEM_NAME;
            sensor_uart_gps6mv2_properties->ezlopi_cloud.device_type = dev_type_sensor;
            sensor_uart_gps6mv2_properties->ezlopi_cloud.value_type = VALUE_TYPE;
            sensor_uart_gps6mv2_properties->ezlopi_cloud.has_getter = true;
            sensor_uart_gps6mv2_properties->ezlopi_cloud.has_setter = false;
            sensor_uart_gps6mv2_properties->ezlopi_cloud.reachable = true;
            sensor_uart_gps6mv2_properties->ezlopi_cloud.battery_powered = false;
            sensor_uart_gps6mv2_properties->ezlopi_cloud.show = true;
            sensor_uart_gps6mv2_properties->ezlopi_cloud.room_name[0] = '\0';
            sensor_uart_gps6mv2_properties->ezlopi_cloud.device_id = DEVICE_ID;
            sensor_uart_gps6mv2_properties->ezlopi_cloud.room_id = ezlopi_cloud_generate_room_id();
            sensor_uart_gps6mv2_properties->ezlopi_cloud.item_id = ITEM_ID;

            CJSON_GET_VALUE_INT(cjson_device, "gpio_tx", sensor_uart_gps6mv2_properties->interface.uart.tx);
            CJSON_GET_VALUE_INT(cjson_device, "gpio_rx", sensor_uart_gps6mv2_properties->interface.uart.rx);
            CJSON_GET_VALUE_INT(cjson_device, "baud", sensor_uart_gps6mv2_properties->interface.uart.baudrate);

            sensor_uart_gps6mv2_properties->user_arg = sensor_0053_UART_gps6mv2_data; // structure containing calib_factors & data_val
        }
    }
    return sensor_uart_gps6mv2_properties;
}

//------------------------------------------------------------------------------
static int add_device_to_list(s_ezlopi_prep_arg_t *prep_arg, s_ezlopi_device_properties_t *properties, void *user_arg)
{
    int ret = 0;
    if (properties)
    {
        if (0 == ezlopi_devices_list_add(prep_arg->device, properties, user_arg))
        {
            free(properties);
        }
        else
        {
            ret = 1;
        }
    }
    return ret;
}
//------------------------------------------------------------------------------

static int sensor_uart_gps6mv2_prepare_and_add(void *arg)
{
    int ret = 0;
    s_ezlopi_prep_arg_t *prep_arg = (s_ezlopi_prep_arg_t *)arg; // create a ' GPS6MV2_t ' type dummy pointer
    GPS6MV2_t *sensor_0053_UART_gps6mv2_data = (GPS6MV2_t *)malloc(sizeof(GPS6MV2_t));

    if ((NULL != prep_arg) && (NULL != prep_arg->cjson_device) && (NULL != sensor_0053_UART_gps6mv2_data))
    {
        memset(sensor_0053_UART_gps6mv2_data, 0, sizeof(GPS6MV2_t));
        uint32_t device_id = ezlopi_cloud_generate_device_id();
        ADD_PROPERTIES_DEVICE_LIST(properties, device_id, category_generic_sensor, subcategory_not_defined, ezlopi_item_name_angle_position, Latitude_item_id, value_type_angle, prep_arg->cjson_device, sensor_0053_UART_gps6mv2_data);

        device_id = ezlopi_cloud_generate_device_id();
        ADD_PROPERTIES_DEVICE_LIST(properties, device_id, category_generic_sensor, subcategory_not_defined, ezlopi_item_name_angle_position, Longitude_item_id, value_type_angle, prep_arg->cjson_device, sensor_0053_UART_gps6mv2_data);

        device_id = ezlopi_cloud_generate_device_id();
        ADD_PROPERTIES_DEVICE_LIST(properties, device_id, category_generic_sensor, subcategory_not_defined, ezlopi_item_name_basic, GPS_fix_item_id, value_type_bool, prep_arg->cjson_device, sensor_0053_UART_gps6mv2_data);

        device_id = ezlopi_cloud_generate_device_id();
        ADD_PROPERTIES_DEVICE_LIST(properties, device_id, category_generic_sensor, subcategory_not_defined, ezlopi_item_name_distance, Sea_level_item_id, value_type_length, prep_arg->cjson_device, sensor_0053_UART_gps6mv2_data);

        device_id = ezlopi_cloud_generate_device_id();
        ADD_PROPERTIES_DEVICE_LIST(properties, device_id, category_generic_sensor, subcategory_not_defined, ezlopi_item_name_distance, Geoid_item_id, value_type_length, prep_arg->cjson_device, sensor_0053_UART_gps6mv2_data);
    }
    return ret;
}

static int sensor_uart_gps6mv2_init(s_ezlopi_device_properties_t *properties)
{
    int ret = 0;
    if (!gps_init_guard)
    {
        if (GPIO_IS_VALID_GPIO(properties->interface.uart.tx) && GPIO_IS_VALID_GPIO(properties->interface.uart.rx))
        {
            s_ezlopi_uart_object_handle_t ezlopi_uart_object_handle = ezlopi_uart_init(properties->interface.uart.baudrate, properties->interface.uart.tx,
                                                                                       properties->interface.uart.rx, ezlopi_uart_gps6mv2_upcall, NULL);
            properties->interface.uart.channel = ezlopi_uart_get_channel(ezlopi_uart_object_handle);
            ret = 1;
        }
        // TRACE_W(" Initailization complete......");
        gps_init_guard = true;
    }
    return ret;
}

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

static void ezlopi_uart_gps6mv2_upcall(uint8_t *buffer, s_ezlopi_uart_object_handle_t uart_object_handle, void *user_args)
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
            Retrieve_GPGGA_sentence();           // CALL a function that extracts the 'GPGGA_sentence' from gps_cir_buf[]
            memset(gps_cir_buf, 0, sizeof(gps_cir_buf)); // reset gps_cir_buf[]
        }

        free(another_buffer);
    }
}

static void sensor_uart_gps6mv2_get_item(s_ezlopi_device_properties_t *properties, void *args)
{
    int lat_angle_val = 0, long_angle_val = 0, total_sat = 0, gps_quality = 0, antenna_alti = 0, geoid = 0;
    bool GPS_FIX = false;
    cJSON *cjson_properties = (cJSON *)args;
    GPS6MV2_t *sensor_0053_UART_gps6mv2_data = (GPS6MV2_t *)properties->user_arg;

    if (cjson_properties && sensor_0053_UART_gps6mv2_data)
    {
        switch (properties->ezlopi_cloud.item_id)
        {
        case 1:
        {
            lat_angle_val = atoi(sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.Latitude.lat_degree) + (atoi(sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.Latitude.lat_min)) / 60;
            // TRACE_I("latitude : %d *deg", lat_angle_val);
            cJSON_AddNumberToObject(cjson_properties, "value", lat_angle_val);
            cJSON_AddStringToObject(cjson_properties, "scale", "north_pole_degress");
            break;
        }

        case 2:
        {
            long_angle_val = atoi(sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.Longitude.long_degree) + (atoi(sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.Longitude.long_min)) / 60;
            // TRACE_I("Longitude : %d *deg", long_angle_val);
            cJSON_AddNumberToObject(cjson_properties, "value", long_angle_val);
            cJSON_AddStringToObject(cjson_properties, "scale", "north_pole_degress");
            break;
        }

        case 3:
        {
            total_sat = atoi(sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.Satellites_used);
            // TRACE_I("No. of Satellites : %d ", total_sat);

            gps_quality = ((int)(sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.Positon_fix_quality) - 48); // converting character into 'integer'
            gps_quality = (gps_quality < 0) ? 0 : ((gps_quality > 9) ? 9 : gps_quality);
            // TRACE_I("GPSFix_Quality : %d ", gps_quality);
            GPS_FIX = ((total_sat > 2) && (gps_quality != 0)) ? true : false;
            cJSON_AddBoolToObject(cjson_properties, "value", GPS_FIX);
            break;
        }

        case 4:
        {
            antenna_alti = atoi(sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.Mean_sea_level);
            // TRACE_I("Antenna Altitude for mean_sea_level : %d m", antenna_alti);
            cJSON_AddNumberToObject(cjson_properties, "value", antenna_alti);
            cJSON_AddStringToObject(cjson_properties, "scale", "meter");
            break;
        }

        case 5:
        {
            geoid = atoi(sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.Geoid_Separation);
            // TRACE_I("Geoid seperation : %d m", geoid);
            cJSON_AddNumberToObject(cjson_properties, "value", geoid);
            cJSON_AddStringToObject(cjson_properties, "scale", "meter");
            break;
        }
        }
    }
}

static int sensor_uart_gps6mv2_get_value_cjson(s_ezlopi_device_properties_t *properties, void *args)
{
    int ret = 0;
    int lat_angle_val = 0, long_angle_val = 0, total_sat = 0, gps_quality = 0, antenna_alti = 0, geoid = 0;
    bool GPS_FIX = false;
    cJSON *cjson_properties = (cJSON *)args;
    GPS6MV2_t *sensor_0053_UART_gps6mv2_data = (GPS6MV2_t *)properties->user_arg;

    if (cjson_properties && sensor_0053_UART_gps6mv2_data)
    {
        switch (properties->ezlopi_cloud.item_id)
        {
        case 1:
        {
            lat_angle_val = atoi(sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.Latitude.lat_degree) + (atoi(sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.Latitude.lat_min)) / 60;
            // TRACE_I("latitude : %d *deg", lat_angle_val);
            cJSON_AddNumberToObject(cjson_properties, "value", lat_angle_val);
            cJSON_AddStringToObject(cjson_properties, "scale", "north_pole_degress");
            break;
        }

        case 2:
        {
            long_angle_val = atoi(sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.Longitude.long_degree) + (atoi(sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.Longitude.long_min)) / 60;
            // TRACE_I("Longitude : %d *deg", long_angle_val);
            cJSON_AddNumberToObject(cjson_properties, "value", long_angle_val);
            cJSON_AddStringToObject(cjson_properties, "scale", "north_pole_degress");
            break;
        }

        case 3:
        {
            total_sat = atoi(sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.Satellites_used);
            // TRACE_I("No. of Satellites : %d ", total_sat);

            gps_quality = ((int)(sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.Positon_fix_quality) - 48); // converting character into 'integer'
            gps_quality = (gps_quality < 0) ? 0 : ((gps_quality > 9) ? 9 : gps_quality);
            // TRACE_I("GPSFix_Quality : %d ", gps_quality);
            GPS_FIX = ((total_sat > 2) && (gps_quality != 0)) ? true : false;
            cJSON_AddBoolToObject(cjson_properties, "value", GPS_FIX);
            break;
        }

        case 4:
        {
            antenna_alti = atoi(sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.Mean_sea_level);
            // TRACE_I("Antenna Altitude for mean_sea_level : %d m", antenna_alti);
            cJSON_AddNumberToObject(cjson_properties, "value", antenna_alti);
            cJSON_AddStringToObject(cjson_properties, "scale", "meter");
            break;
        }

        case 5:
        {
            geoid = atoi(sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.Geoid_Separation);
            // TRACE_I("Geoid seperation : %d m", geoid);
            cJSON_AddNumberToObject(cjson_properties, "value", geoid);
            cJSON_AddStringToObject(cjson_properties, "scale", "meter");
            break;
        }
        }
        ret = 1;
    }

    return ret;
}
//------------------------------------------------------------------------------

static int sensor_uart_gps6mv2_update_values(s_ezlopi_device_properties_t *properties)
{
    int ret = 0, len = 0;
    // 'void_type' addrress -> 'GPS6MV2_t' address
    GPS6MV2_t *sensor_0053_UART_gps6mv2_data = (GPS6MV2_t *)properties->user_arg;

    if (NULL != properties)
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
