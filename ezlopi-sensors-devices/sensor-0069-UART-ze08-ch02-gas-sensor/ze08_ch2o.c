#include "ze08_ch2o.h"
#include <string.h>

static uint8_t ze08_ch2o_sensor_checksum(uint8_t array[], uint8_t length);
static void ezlopi_ze08_ch2o_upcall(uint8_t* buffer, uint32_t output_len, s_ezlopi_uart_object_handle_t uart_object_handle);
static void ze08_ch2o_parse(uint8_t* buffer, ze08_ch2o_sensor_data_t* data);

bool ze08_ch2o_sensor_init(s_ezlopi_uart_t *ze08_uart_config, ze08_ch2o_sensor_data_t* data)
{
    if (ze08_uart_config->enable)
    {
        return ezlopi_uart_init(ze08_uart_config->baudrate, ze08_uart_config->tx, ze08_uart_config->rx, ezlopi_ze08_ch2o_upcall, data);
    }
    
    return false;
}

static uint8_t ze08_ch2o_sensor_checksum(uint8_t array[], uint8_t length)	
{
    uint8_t sum = 0;

    for(int i = 1; i < length-1; i++)
    {
		sum += array[i];
    }

    sum = (~sum) + 1;
    return sum;
}

static void ezlopi_ze08_ch2o_upcall(uint8_t* buffer, uint32_t output_len, s_ezlopi_uart_object_handle_t uart_object_handle)
{
    ze08_ch2o_sensor_data_t* data = (ze08_ch2o_sensor_data_t*) uart_object_handle->arg;

#warning "DO NOT user printf"
#if 0
#if DEBUG_ZE08_CH2O
    for (size_t i = 0; i < ZE08_BUFFER_MAXLENGTH; i++)
    {
        printf("0x%X ", buffer[i]);
    }
    
    printf("\n");
#endif
#endif 
    ze08_ch2o_parse(buffer, data);
}


static void ze08_ch2o_parse(uint8_t* buffer, ze08_ch2o_sensor_data_t* data)		//new data was recevied
{
    uint8_t sumNum = ze08_ch2o_sensor_checksum(buffer, ZE08_BUFFER_MAXLENGTH);
    
    if( (buffer[0] == 0xFF) && (buffer[1] == 0x17) && (buffer[2] == 0x04) && (buffer[ZE08_BUFFER_MAXLENGTH-1] == sumNum) )	//head bit and sum are all right
    {
        float ppb = (unsigned int)buffer[4]<<8 | buffer[5];		// bit 4: ppm high 8-bit; bit 5: ppm low 8-bit
        data->ppm = ppb / 1000.0;	//1ppb = 1000ppm
        data->available = true;
    }
}