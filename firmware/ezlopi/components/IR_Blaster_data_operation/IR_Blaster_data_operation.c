
#include "IR_Blaster_data_operation.h"
#include "IR_Blaster_encoder_decoder.h"
#include "stdio.h"
#include "math.h"


char *create_base64_learned_data_packet(const uint32_t *timing_data_hex_string, uint32_t length)
{
    char *test_base64 = NULL;
    char* encoded_packet = NULL;
    uint32_t out_len = 0;
    uint32_t timing_data_len = length;
    uint32_t string_data_len = (timing_data_len * 4) + 1;
    uint32_t header_size = 6;
    uint32_t length_size = 4;

    char *string_data = malloc(header_size + length_size + string_data_len);
    if (string_data)
    {
        snprintf(string_data, header_size + string_data_len, "%04X%02X", start_bytes, ir_blaseter_learned_data_type);
        timing_data_to_hex_32_string_with_size(timing_data_hex_string, timing_data_len, string_data + header_size, string_data_len + length_size);
        test_base64 = string_to_base64(string_data, strlen(string_data), &out_len);
        encoded_packet = (char*)malloc(out_len+1);
        memset(encoded_packet, 0, out_len+1);
        snprintf(encoded_packet, out_len+1, test_base64, out_len);

        free(string_data);
        string_data = NULL;
    }
    return encoded_packet;
}

int get_packet_type(const char* packet)
{
	uint32_t offset = 0;
	int type = -1;
	if(strncmp(packet+offset, "0000", 4) == 0)
	{
		offset += 4;
		if(strncmp(packet+offset, "00", 2) == 0)
		{
			type = 0;
		}
		if(strncmp(packet+offset, "01", 2) == 0)
		{
			type = 1;
		}
	}
	else
	{
		printf("No header was detected!!");
	}
	return type;
}

uint32_t *hex_string_2_timing_array(char *hex_string_data, uint32_t *timing_array_len)
{
    uint32_t *timing_array = NULL;

    if (hex_string_data)
    {
        uint32_t header_idx = 0, header_char_len = 4;
        uint32_t type_idx = 4, type_char_len = 2;
        uint32_t length_idx = 6, length_char_len = 4;

        uint32_t header = hex_string_2_uint32(hex_string_data + header_idx, header_char_len);
        uint32_t type = hex_string_2_uint32(hex_string_data + type_idx, type_char_len);
        uint32_t length = hex_string_2_uint32(hex_string_data + length_idx, length_char_len);

        if (0 == (length % 4))
        {
            uint32_t tmp_array_len = length / 4;
            timing_array = malloc(tmp_array_len);


            uint32_t timing_char_len = 4;
            uint32_t timing_idx = 10;

            if (timing_array)
            {
                *timing_array_len = tmp_array_len;
                
                for (uint32_t idx = 0; idx < tmp_array_len; idx++)
                {
                    timing_array[idx] = hex_string_2_uint32(hex_string_data + timing_idx + (idx * timing_char_len), timing_char_len);
                }
            }
        }
    }

    return timing_array;
}

uint32_t hex_string_2_uint32(char *data, uint32_t num_char_len)
{
    uint32_t value = 0;
    if (data)
    {
        char tmp_data[8];
        memset(tmp_data, 0, sizeof(tmp_data));
        snprintf(tmp_data, 8, "%.*s", num_char_len, data);
        value = strtoul(tmp_data, NULL, 16);
    }

    return value;
}

void print_timing_array(char *array_name, uint32_t *timing_data, uint32_t len)
{
    printf("%s: [ ", array_name);
    for (uint32_t idx = 0; idx < len; idx++)
    {
        if (idx == (len - 1))
        {
            printf("%d ", timing_data[idx]);
        }
        else
        {
            printf("%d, ", timing_data[idx]);
        }
    }
    printf("]\r\n");
}

