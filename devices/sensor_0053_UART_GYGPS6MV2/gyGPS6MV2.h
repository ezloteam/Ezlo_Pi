#ifndef _TCS230_H_
#define _TCS230_H_

#include "sensor_0053_UART_GYGPS6MV2.h"
//-------------------------------------------------------------------------
// LIST of MESSAGE PARSING FUNCTIONS
//-------------------------------------------------------------------------
void parse_and_assign_GPGGA_message(GPS6MV2_t *sensor_0053_UART_gps6mv2_data);

#endif