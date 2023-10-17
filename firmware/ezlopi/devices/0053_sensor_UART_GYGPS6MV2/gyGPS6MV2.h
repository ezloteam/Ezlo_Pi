#ifndef _GYGPS6MV2_H_
#define _GYGPS6MV2_H_

#include "0053_sensor_UART_GYGPS6MV2.h"
//-------------------------------------------------------------------------
// LIST of MESSAGE PARSING FUNCTIONS
//-------------------------------------------------------------------------

void parse_and_assign_GPGGA_message(GPS6MV2_t *sensor_0053_UART_gps6mv2_data);

#endif //_GYGPS6MV2_H_