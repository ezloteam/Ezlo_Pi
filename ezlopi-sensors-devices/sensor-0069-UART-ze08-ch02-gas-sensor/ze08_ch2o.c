/* ===========================================================================
** Copyright (C) 2024 Ezlo Innovation Inc
**
** Under EZLO AVAILABLE SOURCE LICENSE (EASL) AGREEMENT
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met:
**
** 1. Redistributions of source code must retain the above copyright notice,
**    this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. Neither the name of the copyright holder nor the names of its
**    contributors may be used to endorse or promote products derived from
**    this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
** AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
** ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
** LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
** CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
** SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
** INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
** CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
** ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
** POSSIBILITY OF SUCH DAMAGE.
** ===========================================================================
*/
/**
* @file    ze08_ch2o.c
* @brief   perform some function on ze08_ch2o
* @author  xx
* @version 0.1
* @date    xx
*/

/*******************************************************************************
*                          Include Files
*******************************************************************************/
#include "ze08_ch2o.h"
#include <string.h>


/*******************************************************************************
*                          Extern Data Declarations
*******************************************************************************/

/*******************************************************************************
*                          Extern Function Declarations
*******************************************************************************/

/*******************************************************************************
*                          Type & Macro Definitions
*******************************************************************************/

/*******************************************************************************
*                          Static Function Prototypes
*******************************************************************************/
static uint8_t ze08_ch2o_sensor_checksum(uint8_t array[], uint8_t length);
static void ezlopi_ze08_ch2o_upcall(uint8_t *buffer, uint32_t output_len, s_ezlopi_uart_object_handle_t uart_object_handle);
static void ze08_ch2o_parse(uint8_t *buffer, ze08_ch2o_sensor_data_t *data);
/*******************************************************************************
*                          Static Data Definitions
*******************************************************************************/

/*******************************************************************************
*                          Extern Data Definitions
*******************************************************************************/

/*******************************************************************************
*                          Extern Function Definitions
*******************************************************************************/

bool ZE08_ch2o_sensor_init(s_ezlopi_uart_t *ze08_uart_config, ze08_ch2o_sensor_data_t *data)
{
    if (ze08_uart_config->enable)
    {
        return EZPI_hal_uart_init(ze08_uart_config->baudrate, ze08_uart_config->tx, ze08_uart_config->rx, ezlopi_ze08_ch2o_upcall, data);
    }

    return false;
}

/*******************************************************************************
*                         Static Function Definitions
*******************************************************************************/

static uint8_t ze08_ch2o_sensor_checksum(uint8_t array[], uint8_t length)
{
    uint8_t sum = 0;

    for (int i = 1; i < length - 1; i++)
    {
        sum += array[i];
    }

    sum = (~sum) + 1;
    return sum;
}

static void ezlopi_ze08_ch2o_upcall(uint8_t *buffer, uint32_t output_len, s_ezlopi_uart_object_handle_t uart_object_handle)
{
    ze08_ch2o_sensor_data_t *data = (ze08_ch2o_sensor_data_t *)uart_object_handle->arg;

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

static void ze08_ch2o_parse(uint8_t *buffer, ze08_ch2o_sensor_data_t *data)		//new data was recevied
{
    uint8_t sumNum = ze08_ch2o_sensor_checksum(buffer, ZE08_BUFFER_MAXLENGTH);

    if ((buffer[0] == 0xFF) && (buffer[1] == 0x17) && (buffer[2] == 0x04) && (buffer[ZE08_BUFFER_MAXLENGTH - 1] == sumNum))	//head bit and sum are all right
    {
        float ppb = (unsigned int)buffer[4] << 8 | buffer[5];		// bit 4: ppm high 8-bit; bit 5: ppm low 8-bit
        data->ppm = ppb / 1000.0;	//1ppb = 1000ppm
        data->available = true;
    }
}

/*******************************************************************************
*                          End of File
*******************************************************************************/