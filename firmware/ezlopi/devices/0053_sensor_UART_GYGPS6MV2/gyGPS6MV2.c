#include "trace.h"
#include "string.h"
#include "gyGPS6MV2.h"

//-------------------------------------------------------------------------
// GPGGA MESSAGE PARSING FUNCTION
//-------------------------------------------------------------------------

void parse_and_assign_GPGGA_message(GPS6MV2_t *sensor_0053_UART_gps6mv2_data)
{
    // TRACE_W("GPGGA_sentence => %s", sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.GPGGA_sentence);
    uint8_t len = strlen(sensor_0053_UART_gps6mv2_data->GPGGA_data_structure.GPGGA_sentence); // length => contents
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
                // TRACE_W("age_from_last:";
                break;
            case 14: // station_ID
                // TRACE_W("station_ID:"");
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

    //-------------------------------------------------------------------------
    // Statemnent that displays extracted GPGGA_data
    //-------------------------------------------------------------------------
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

//-------------------------------------------------------------------------
// ___ MESSAGE PARSING FUNCTION
//-------------------------------------------------------------------------
