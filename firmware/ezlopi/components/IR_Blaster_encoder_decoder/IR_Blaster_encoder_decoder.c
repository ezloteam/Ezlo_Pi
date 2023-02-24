#include "IR_Blaster_encoder_decoder.h"
#include "math.h"

static int mod_table[] = {0, 2, 1};
static const unsigned char base64_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static char decoding_table[256];

static void build_decoding_table(void)
{
    memset(decoding_table, 0, sizeof(decoding_table));

    for (int i = 0; i < 64; i++)
    {
        uint8_t jjj = (unsigned char)base64_table[i];
        // ESP_LOGI(TAG, "Building decoding table -> %d\r\n", jjj);
        if (jjj < sizeof(decoding_table))
        {
            decoding_table[(unsigned char)base64_table[i]] = i;
        }
        else
        {
            ESP_LOGI(TAG, "error: buffer overflow!");
        }
    }
}

void timing_data_to_hex_32_string_with_size(const uint32_t *in_buffer, uint32_t in_len, char *out_buffer, uint32_t out_buffer_len)
{
    snprintf(out_buffer, out_buffer_len, "%04X", 4 * in_len);
    if (in_buffer && in_len && out_buffer && out_buffer_len && out_buffer_len >= (in_len * 4))
    {
        for (int idx = 0; idx < in_len; idx++)
        {
            snprintf(out_buffer + strlen(out_buffer), out_buffer_len - strlen(out_buffer), "%04X", in_buffer[idx]);
        }
    }
}

char *string_to_base64(char *in_data, uint32_t in_len, uint32_t *out_len)
{
    build_decoding_table();
    size_t tmp_out_len = 4 * floor((in_len + 2) / 3);
    *out_len = tmp_out_len;
    char *encoded_data = malloc(tmp_out_len);

    if (NULL != encoded_data)
    {
        for (int i = 0, j = 0; i < in_len;)
        {

            uint32_t octet_a = (i < in_len) ? (unsigned char)in_data[i++] : 0;
            uint32_t octet_b = (i < in_len) ? (unsigned char)in_data[i++] : 0;
            uint32_t octet_c = (i < in_len) ? (unsigned char)in_data[i++] : 0;

            uint32_t triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;

            encoded_data[j++] = base64_table[(triple >> (3 * 6)) & 0x3F];
            encoded_data[j++] = base64_table[(triple >> (2 * 6)) & 0x3F];
            encoded_data[j++] = base64_table[(triple >> (1 * 6)) & 0x3F];
            encoded_data[j++] = base64_table[(triple >> (0 * 6)) & 0x3F];
        }

        for (int i = 0; i < mod_table[in_len % 3]; i++)
        {
            encoded_data[tmp_out_len - 1 - i] = '=';
        }
    }

    return encoded_data;
}

char *base64_to_string(char *in_data)
{
    unsigned char *decoded_data = NULL;

    if (in_data)
    {
        uint32_t input_length = strlen(in_data);
        // ESP_LOGI(TAG, "input_length %d\n", input_length);
        uint32_t output_length = (input_length / 4) * 3;

        if (0 == (input_length % 4))
        {
            if (in_data[input_length - 1] == '=')
            {
                (output_length)--;
            }

            if (in_data[input_length - 2] == '=')
            {
                (output_length)--;
            }

            decoded_data = malloc(output_length + 1);

            if (NULL != decoded_data)
            {
                for (int i = 0, j = 0; i < input_length;)
                {

                    uint32_t sextet_a = in_data[i] == '=' ? 0 & i++ : decoding_table[(unsigned char)in_data[i++]];
                    uint32_t sextet_b = in_data[i] == '=' ? 0 & i++ : decoding_table[(unsigned char)in_data[i++]];
                    uint32_t sextet_c = in_data[i] == '=' ? 0 & i++ : decoding_table[(unsigned char)in_data[i++]];
                    uint32_t sextet_d = in_data[i] == '=' ? 0 & i++ : decoding_table[(unsigned char)in_data[i++]];

                    uint32_t triple = (sextet_a << 3 * 6) + (sextet_b << 2 * 6) + (sextet_c << 1 * 6) + (sextet_d << 0 * 6);

                    if (j < output_length)
                    {
                        decoded_data[j++] = (triple >> 2 * 8) & 0xFF;
                        decoded_data[j++] = (triple >> 1 * 8) & 0xFF;
                        decoded_data[j++] = (triple >> 0 * 8) & 0xFF;
                    }
                    else
                    {
                        ESP_LOGI(TAG, "buffer overflow!\n");
                    }
                }
            }
            else
            {
                ESP_LOGI(TAG, "malloc failed!\n");
            }
        }
    }
    else
    {
        ESP_LOGI(TAG, "In data: %s\n", in_data ? in_data : "null");
    }
    return (char*)decoded_data;
}