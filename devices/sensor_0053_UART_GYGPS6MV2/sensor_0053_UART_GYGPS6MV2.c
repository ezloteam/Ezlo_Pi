
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

#include "sensor_0053_UART_GYGPS6MV2.h"

static bool init_guard = false;
static bool message_guard = false;
static char sentence[100];

#define Latitude_item_id 1
#define Longitude_item_id 2
#define GPS_fix_item_id 3
#define Sea_level_item_id 4
#define Geoid_item_id 5

//------------------------------------------------------------------------------
static s_ezlopi_device_properties_t *Latitude_properties = NULL;
static s_ezlopi_device_properties_t *Longitude_properties = NULL;
static s_ezlopi_device_properties_t *GPS_fix_properties = NULL;
static s_ezlopi_device_properties_t *Mean_sea_level_properties = NULL;
static s_ezlopi_device_properties_t *Geoid_Separation_properties = NULL;
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
static void parse_and_assign_GPGGA_message(GPS6MV2_t *sensor_0053_UART_gps6mv2_data);
static int sensor_uart_gps6mv2_get_value_cjson(s_ezlopi_device_properties_t *properties, void *args);
static int sensor_uart_gps6mv2_update_values(s_ezlopi_device_properties_t *properties);

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
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        sensor_uart_gps6mv2_get_value_cjson(properties, arg);
        break;
    }
    case EZLOPI_ACTION_NOTIFY_1000_MS:
    {
        static uint8_t count = 1;
        if (init_guard)
        {
            if (count > 4)
            {
                message_guard = false;
                count = 1;
                // if greater than 3sec post the values to cloud
                ezlopi_device_value_updated_from_device(properties);
            }
            else if (count % 3 == 0)
            {
                message_guard = true;
                sensor_uart_gps6mv2_update_values(properties);
            }
            count++;
        }
        break;
    }
    default:
    {
        break;
    }
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
            // "dev_name" : factory_info_h
            switch (ITEM_ID)
            {
            case Latitude_item_id:
                device_name = "Longitude";
                break;
            case Longitude_item_id:
                device_name = "Latitude";
                break;
            case GPS_fix_item_id:
                device_name = "GPS-Fix Valid";
                break;
            case Sea_level_item_id:
                device_name = "Antenna-Altitude";
                break;
            case Geoid_item_id:
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
        ADD_PROPERTIES_DEVICE_LIST(Latitude_properties, device_id, category_generic_sensor, subcategory_not_defined, ezlopi_item_name_angle_position, Latitude_item_id, value_type_angle, prep_arg->cjson_device, sensor_0053_UART_gps6mv2_data);

        device_id = ezlopi_cloud_generate_device_id();
        ADD_PROPERTIES_DEVICE_LIST(Longitude_properties, device_id, category_generic_sensor, subcategory_not_defined, ezlopi_item_name_angle_position, Longitude_item_id, value_type_angle, prep_arg->cjson_device, sensor_0053_UART_gps6mv2_data);

        device_id = ezlopi_cloud_generate_device_id();
        ADD_PROPERTIES_DEVICE_LIST(GPS_fix_properties, device_id, category_generic_sensor, subcategory_not_defined, ezlopi_item_name_basic, GPS_fix_item_id, value_type_bool, prep_arg->cjson_device, sensor_0053_UART_gps6mv2_data);

        device_id = ezlopi_cloud_generate_device_id();
        ADD_PROPERTIES_DEVICE_LIST(Mean_sea_level_properties, device_id, category_generic_sensor, subcategory_not_defined, ezlopi_item_name_distance, Sea_level_item_id, value_type_length, prep_arg->cjson_device, sensor_0053_UART_gps6mv2_data);

        device_id = ezlopi_cloud_generate_device_id();
        ADD_PROPERTIES_DEVICE_LIST(Geoid_Separation_properties, device_id, category_generic_sensor, subcategory_not_defined, ezlopi_item_name_distance, Geoid_item_id, value_type_length, prep_arg->cjson_device, sensor_0053_UART_gps6mv2_data);
    }
    return ret;
}

static int sensor_uart_gps6mv2_init(s_ezlopi_device_properties_t *properties)
{
    int ret = 0;
    if (!init_guard)
    {
        if (GPIO_IS_VALID_GPIO(properties->interface.uart.tx) && GPIO_IS_VALID_GPIO(properties->interface.uart.rx))
        {
            s_ezlopi_uart_object_handle_t ezlopi_uart_object_handle = ezlopi_uart_init(properties->interface.uart.baudrate, properties->interface.uart.tx,
                                                                                       properties->interface.uart.rx, ezlopi_uart_gps6mv2_upcall, NULL);
            properties->interface.uart.channel = ezlopi_uart_get_channel(ezlopi_uart_object_handle);
            ret = 1;
        }
        init_guard = true;
        TRACE_W(" Initailization complete......");
    }
    return ret;
}

static void ezlopi_uart_gps6mv2_upcall(uint8_t *buffer, s_ezlopi_uart_object_handle_t uart_object_handle, void *user_args)
{
    // TRACE_E("\nUART_Buffer => \n%s", buffer);

    // find the position of '$GSGGA' in incoming paragraph
    char *another_buffer = (char *)malloc(256);
    memcpy(another_buffer, buffer, 256);

    /**                             2000
     *      buffer = 'dkfhkhdf .... $GPGGA,061731.00,2740.52772,....,-41.3,M,,*40.....'
     *                              ^
     *                             ptr1 => $GPGGA,061731.00,2740.52772,....,-41.3,M,,*40.....
     *
     */
    char *ptr1 = strstr(another_buffer, "$GPGGA"); // returns a pointer points to the first character of the found 'another_buffer' in "$GPGGA"
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
        { // NOW , separate the GSGGA-message and copy to global variable 'sentence'
            ptr2 += 3;
            TRACE_E("GPGGA message size = %d (< %d)", (ptr2 - ptr1), MAX_GPGGA_SENTENCE_SIZE);
            if ((ptr2 - ptr1) > 0 && (ptr2 - ptr1) < MAX_GPGGA_SENTENCE_SIZE)
            {
                TRACE_W("sentence (OLD) => %s", sentence);
                if (message_guard == false)
                {
                    // TRACE_E("Message_Guard => OFF ; NEW message copied.... in 'sentence'");
                    memset(&sentence, 0, sizeof(sentence));
                    sentence[99] = '\0';
                    memcpy(sentence, ptr1, (ptr2 - ptr1)); // +3 because the checksum consists three character more [*XX]
                    TRACE_W("sentence (NEW) => %s", sentence);
                }
                // else
                // {
                //     TRACE_E(" Message_Guard => ON ; Cannot copy NEW message.... in 'sentence'");
                // }
            }
        }
    }
    // else
    // {
    //     TRACE_E("NO GPGGA message found in recent stack....\n");
    // }
    free(another_buffer);
}

static int sensor_uart_gps6mv2_get_value_cjson(s_ezlopi_device_properties_t *properties, void *args)
{
    int ret = 0;
    cJSON *cjson_properties = (cJSON *)args;
    GPS6MV2_t *sensor_0053_UART_gps6mv2_data = (GPS6MV2_t *)properties->user_arg;

    if (cjson_properties && sensor_0053_UART_gps6mv2_data)
    {
        // longitude & latitude
        if (Latitude_item_id == properties->ezlopi_cloud.item_id)
        {
            int lat_angle_val = atoi(sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.Latitude.lat_degree) + (atoi(sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.Latitude.lat_min)) / 60;
            TRACE_I("latitude : %d *deg", lat_angle_val);
            cJSON_AddNumberToObject(cjson_properties, "value", lat_angle_val);
            cJSON_AddStringToObject(cjson_properties, "scale", "north_pole_degress");
        }

        if (Longitude_item_id == properties->ezlopi_cloud.item_id)
        {
            int long_angle_val = atoi(sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.Longitude.long_degree) + (atoi(sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.Longitude.long_min)) / 60;
            TRACE_I("Longitude : %d *deg", long_angle_val);
            cJSON_AddNumberToObject(cjson_properties, "value", long_angle_val);
            cJSON_AddStringToObject(cjson_properties, "scale", "north_pole_degress");
        }

        if (GPS_fix_item_id == properties->ezlopi_cloud.item_id)
        {
            int total_sat = atoi(sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.Satellites_used);
            TRACE_I("No. of Satellites : %d ", total_sat);
            int gps_quality = atoi(&sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.Positon_fix_quality);
            TRACE_I("GPSFix_Quality : %d ", gps_quality);
            bool GPS_FIX = ((total_sat > 2) && (gps_quality != 0)) ? true : false;
            cJSON_AddBoolToObject(cjson_properties, "value", GPS_FIX);
        }

        if (Sea_level_item_id == properties->ezlopi_cloud.item_id)
        {
            int antenna_alti = atoi(sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.Mean_sea_level);
            TRACE_I("Antenna Altitude for mean_sea_level : %d m", antenna_alti);
            cJSON_AddNumberToObject(cjson_properties, "value", antenna_alti);
            cJSON_AddStringToObject(cjson_properties, "scale", "meter");
        }

        if (Geoid_item_id == properties->ezlopi_cloud.item_id)
        {
            int geoid = atoi(sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.Geoid_Separation);
            TRACE_I("Geoid seperation : %d m", geoid);
            cJSON_AddNumberToObject(cjson_properties, "value", geoid);
            cJSON_AddStringToObject(cjson_properties, "scale", "meter");
        }

        ret = 1;
    }
    return ret;
}
//------------------------------------------------------------------------------

static int sensor_uart_gps6mv2_update_values(s_ezlopi_device_properties_t *properties)
{
    int ret = 0;
    // 'void_type' addrress -> 'GPS6MV2_t' address
    GPS6MV2_t *sensor_0053_UART_gps6mv2_data = (GPS6MV2_t *)properties->user_arg;

    if (NULL != properties)
    {
        // replace the sentence of GPGGA structure
        if (sentence != NULL)
        {
            // reset the GPGGA_sentence array
            int len = sizeof(sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.GPGGA_sentence);
            memset(sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.GPGGA_sentence, 0, sizeof(len));
            sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.GPGGA_sentence[len - 1] = '\0';
            strncpy(sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.GPGGA_sentence, sentence, (len - 1));

            // TRACE_I("UPDATE_TAG : Copying [%s] to => 'GPGGA_sentence[70]", sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.GPGGA_sentence);
        }

        // Check availability of Checksum in the message
        if (strchr((sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.GPGGA_sentence), '*') != NULL)
        { // split the message and store it in 'GPS6MV2_t' data structure
            parse_and_assign_GPGGA_message(sensor_0053_UART_gps6mv2_data);
        }
        message_guard = false;
    }
    return ret;
}

//-------------------------------------------------------------------------

static void parse_and_assign_GPGGA_message(GPS6MV2_t *sensor_0053_UART_gps6mv2_data)
{
    TRACE_I("PARSE_TAG : Parsing the message ...[%s]", sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.GPGGA_sentence);
    uint8_t len = strlen(sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.GPGGA_sentence);
    char *dummy_container = (char *)malloc((len + 1));
    if (NULL != dummy_container)
    {
        // duplicate the original message into dummy container ---> to perform parsing
        strncpy(dummy_container, (sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.GPGGA_sentence), len);
        uint8_t comma_count = 0; // or 'i'th word number
        uint8_t start_index = 0;
        uint8_t diff = 0;
        for (uint8_t i = 0; i < len; i++)
        {
            if (dummy_container[i] == '$')
            {
                comma_count = 0;
            }
            if (dummy_container[i] == ',')
            {
                comma_count++;
                start_index = i; // assigns the present starting index of comma
            }
            if (dummy_container[i] == '*')
            {
                comma_count = 15;
                start_index = i; // assigns the present starting index of comma
            }
            switch (comma_count)
            {
            case 0:
                break;
            case 1: // UTC time
                if (dummy_container[start_index + 1] == ',')
                { // applies only once
                    memset(&sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.UTC_time, 0, sizeof(sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.UTC_time));
                    break;
                }
                else
                {
                    if ((diff = (i - start_index)) > 0)
                    {
                        if (diff < 3) // fixed for hour
                        {
                            // sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.UTC_time.hour[strlen(sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.UTC_time.hour)] = *(dummy_container + i);
                            sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.UTC_time.hour[diff - 1] = *(dummy_container + i);
                        }
                        else if (diff > 2 && diff < 5) // fixed for min
                        {
                            // sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.UTC_time.min[strlen(sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.UTC_time.min)] = *(dummy_container + i);
                            sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.UTC_time.min[diff - 3] = *(dummy_container + i);
                        }
                        else if (diff > 4 && diff < 10)
                        {
                            // sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.UTC_time.sec[strlen(sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.UTC_time.sec)] = *(dummy_container + i);
                            sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.UTC_time.sec[diff - 5] = *(dummy_container + i);
                        }
                    }
                }
                break;
            case 2: // Latitude
                if (dummy_container[start_index + 1] == ',')
                { // applies only once
                    memset(&sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.Latitude, 0, sizeof(sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.Latitude));
                    break;
                }
                else
                {
                    if ((diff = (i - start_index)) > 0)
                    {
                        if (diff < 3) // fixed for lat_degree
                        {
                            // sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.Latitude.lat_degree[strlen(sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.Latitude.lat_degree)] = *(dummy_container + i);
                            sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.Latitude.lat_degree[diff - 1] = *(dummy_container + i);
                        }
                        else if (diff > 2 && diff < 11)
                        {
                            // sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.Latitude.lat_min[strlen(sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.Latitude.lat_min)] = *(dummy_container + i);
                            sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.Latitude.lat_min[diff - 3] = *(dummy_container + i);
                        }
                    }
                }
                break;
            case 3: // North/South indicator
                if (dummy_container[start_index + 1] == ',')
                { // applies only once
                    memset(&sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.N_S_indicator, 0, sizeof(sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.N_S_indicator));
                    break;
                }
                else
                {
                    if ((diff = (i - start_index)) == 1)
                    {
                        sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.N_S_indicator = *(dummy_container + i);
                    }
                }
                break;
            case 4: // Longitude
                if (dummy_container[start_index + 1] == ',')
                { // applies only once
                    memset(&sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.Longitude, 0, sizeof(sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.Longitude));
                    break;
                }
                else
                {
                    if ((diff = (i - start_index)) > 0)
                    {
                        if (diff < 4) // fixed for long_degree
                        {
                            // sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.Longitude.long_degree[strlen(sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.Longitude.long_degree)] = *(dummy_container + i);
                            sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.Longitude.long_degree[diff - 1] = *(dummy_container + i);
                        }
                        else if (diff > 3 && diff < 12)
                        {
                            // sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.Longitude.long_min[strlen(sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.Longitude.long_min)] = *(dummy_container + i);
                            sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.Longitude.long_min[diff - 4] = *(dummy_container + i);
                        }
                    }
                }
                break;
            case 5: // East/West indicator
                if (dummy_container[start_index + 1] == ',')
                { // applies only once
                    memset(&sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.E_W_indicator, 0, sizeof(sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.E_W_indicator));
                    break;
                }
                else
                {
                    if ((diff = (i - start_index)) == 1)
                    {
                        sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.E_W_indicator = *(dummy_container + i);
                    }
                }
                break;
            case 6: // Positon_Fix_Indicator
                if (dummy_container[start_index + 1] == ',')
                { // applies only once
                    memset(&sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.Positon_fix_quality, 0, sizeof(sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.Positon_fix_quality));
                    break;
                }
                else
                {
                    if ((diff = (i - start_index)) == 1)
                    {
                        sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.Positon_fix_quality = *(dummy_container + i);
                    }
                }
                break;
            case 7: // Satellites_used
                if (dummy_container[start_index + 1] == ',')
                { // applies only once
                    memset(&sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.Satellites_used, 0, sizeof(sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.Satellites_used));
                    break;
                }
                else
                {
                    if ((diff = (i - start_index)) > 0)
                    {
                        if (diff < 3) // fixed for satellite array
                        {
                            // sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.Satellites_used[strlen(sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.Satellites_used)] = *(dummy_container + i);
                            sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.Satellites_used[diff - 1] = *(dummy_container + i);
                        }
                    }
                }
                break;
            case 8: // HDOP
                if (dummy_container[start_index + 1] == ',')
                { // applies only once
                    memset(&sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.HDOP, 0, sizeof(sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.HDOP));
                    break;
                }
                else
                {
                    if ((diff = (i - start_index)) > 0)
                    {
                        if (diff < 5) // fixed for HDOP array
                        {
                            // sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.HDOP[strlen(sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.HDOP)] = *(dummy_container + i);
                            sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.HDOP[diff - 1] = *(dummy_container + i);
                        }
                    }
                }
                break;
            case 9: // Mean_sea_lvl
                if (dummy_container[start_index + 1] == ',')
                { // applies only once
                    memset(&sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.Mean_sea_level, 0, sizeof(sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.Mean_sea_level));
                    break;
                }
                else
                {
                    if ((diff = (i - start_index)) > 0)
                    {
                        if (diff < 7) // fixed for mean_sea_lvl array
                        {
                            // sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.Mean_sea_level[strlen(sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.Mean_sea_level)] = *(dummy_container + i);
                            sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.Mean_sea_level[diff - 1] = *(dummy_container + i);
                        }
                    }
                }
                break;
            case 10: // MSL_Unit
                if (dummy_container[start_index + 1] == ',')
                { // applies only once
                    memset(&sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.MSL_Unit, 0, sizeof(sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.MSL_Unit));
                    break;
                }
                else
                {
                    if ((diff = (i - start_index)) == 1)
                    {
                        sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.MSL_Unit = *(dummy_container + i);
                    }
                }
                break;
            case 11: // Geoid_Separation
                if (dummy_container[start_index + 1] == ',')
                { // applies only once
                    memset(&sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.Geoid_Separation, 0, sizeof(sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.Geoid_Separation));
                    break;
                }
                else
                {
                    if ((diff = (i - start_index)) > 0)
                    {
                        if (diff < 7) // fixed for Geoid_seperation array
                        {
                            // sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.Geoid_Separation[strlen(sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.Geoid_Separation)] = *(dummy_container + i);
                            sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.Geoid_Separation[diff - 1] = *(dummy_container + i);
                        }
                    }
                }
                break;
            case 12: // Goid Seperation_Unit
                if (dummy_container[start_index + 1] == ',')
                { // applies only once
                    memset(&sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.GS_Unit, 0, sizeof(sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.GS_Unit));
                    break;
                }
                else
                {
                    if ((diff = (i - start_index)) == 1)
                    {
                        sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.GS_Unit = *(dummy_container + i);
                    }
                }
                break;
            case 13: // age_from_last
                // TRACE_W("age_from_last: %s\n",XXX);
                break;
            case 14: // station_ID
                // TRACE_W("station_ID: %s\n",XXX);
                break;
            case 15: // Checksum
                if (dummy_container[start_index + 1] == ',')
                { // applies only once
                    memset(&sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.Checksum, 0, sizeof(sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.Checksum));
                    break;
                }
                else
                {
                    if ((diff = (i - start_index)) > 0)
                    {
                        if (diff < 3) // fixed for checksum array
                        {
                            // sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.Checksum[strlen(sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.Checksum)] = *(dummy_container + i);
                            sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.Checksum[diff - 1] = *(dummy_container + i);
                        }
                    }
                }
                break;
            }
        }
        free(dummy_container);
    }

    TRACE_E("UTC_time : %s:%s:%s ", sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.UTC_time.hour,
            sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.UTC_time.min,
            sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.UTC_time.sec);
    // TRACE_E("Latitude : %s deg, %s min ", sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.Latitude.lat_degree,
    //         sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.Latitude.lat_min);
    // TRACE_E("N_S_indicator : %c ", sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.N_S_indicator);
    // TRACE_E("Longitude : %s deg, %s min ", sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.Longitude.long_degree,
    //         sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.Longitude.long_min);
    // TRACE_E("E_W_indicator : %c ", sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.E_W_indicator);
    // TRACE_E("Positon_fix_quality : %c ", sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.Positon_fix_quality);
    // TRACE_E("Satellites_used : %s ", sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.Satellites_used);
    // TRACE_E("HDOP : %s ", sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.HDOP);
    // TRACE_E("Mean_sea_level : %s ", sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.Mean_sea_level);
    // TRACE_E("MSL_Unit : %c ", sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.MSL_Unit);
    // TRACE_E("Geoid_Separation : %s ", sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.Geoid_Separation);
    // TRACE_E("GS_Unit : %c ", sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.GS_Unit);
    // TRACE_E("age_from_last : %s ", sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.age_from_last);
    // TRACE_E("station_ID : %s ", sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.station_ID);
    // TRACE_E("Checksum : %s ", sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.Checksum);
}
