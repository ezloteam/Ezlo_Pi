#ifndef _IR_BLASTER_DATA_OPERATION_H_
#define _IR_BLASTER_DATA_OPERATION_H_

#include "stdint.h"

static const uint32_t start_bytes = 0;
static const uint32_t ir_blaseter_learned_data_type = 0;

char *create_base64_learned_data_packet(const uint32_t *timing_data_hex_string, uint32_t length);
// int get_packet_type(const char* packet);
// uint32_t *hex_string_2_timing_array(char *hex_string_data, uint32_t *timing_array_len);
uint32_t hex_string_2_uint32(char *data, uint32_t num_char_len);
void print_timing_array(char *array_name, uint32_t *timing_data, uint32_t len);
uint32_t* get_protocol_and_command_from_packet(const char* protocol_and_command_packet, int* protocol, uint32_t* command_array_size);
int timing_array_length(char *hex_string_data, uint32_t *len);
int hex_string_2_timing_array(char *hex_string_data, uint32_t decoded_timing_data[]);



#endif //_IR_BLASTER_DATA_OPERATION_H_