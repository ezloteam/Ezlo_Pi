

#ifndef _IR_BLASTER_ENCODER_DECODER_H_
#define _IR_BLASTER_ENCODER_DECODER_H_

#include "stdint.h"
#include "stddef.h"
#include "string.h"
#include "stdlib.h"
#include "esp_log.h"
#include "stdio.h"

#define TAG "IR_BLASTER_ENCODER_DECODER"


void timing_data_to_hex_32_string_with_size(const uint32_t *in_buffer, uint32_t in_len, char *out_buffer, uint32_t out_buffer_len);
char *string_to_base64(char *in_data, uint32_t in_len, uint32_t *out_len);
char *base64_to_string(char *in_data);


#endif //_IR_BLASTER_ENCODER_DECODER_H_


