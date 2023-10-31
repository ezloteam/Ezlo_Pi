#include "aes_lib.h"

void aes_lib::set_key(uint8_t *key)
{
    memset(iv, 0, sizeof(iv));
    mbedtls_aes_init(&aes_ctx);
    mbedtls_aes_setkey_enc(&aes_ctx, aes_key, 128);
}

void aes_lib::reset_iv(void)
{
    memset(iv, 0, sizeof(iv));
}

#define CHECK_LEN(len)                                           \
    {                                                            \
        if (len % 16)                                            \
        {                                                        \
            TRACE_E("Input data length is not multiple of 16!"); \
            return;                                              \
        }                                                        \
    }

void aes_lib::ecb_encrypt(uint8_t *in_data, uint8_t *out_data, uint32_t len)
{
    CHECK_LEN(len);
    for (int i = 0; i < len; i += 16)
    {
        mbedtls_aes_crypt_ecb(&aes_ctx, MBEDTLS_AES_ENCRYPT, (const uint8_t *)in_data + i, (uint8_t *)out_data + i);
    }
}
void aes_lib::ecb_decrypt(uint8_t *in_data, uint8_t *out_data, uint32_t len)
{
    CHECK_LEN(len);
    for (int i = 0; i < len; i += 16)
    {
        mbedtls_aes_crypt_ecb(&aes_ctx, MBEDTLS_AES_DECRYPT, (const uint8_t *)in_data + i, (uint8_t *)out_data + i);
    }
}
void aes_lib::cbc_encrypt(uint8_t *in_data, uint8_t *out_data, uint32_t len)
{
    CHECK_LEN(len);
    mbedtls_aes_crypt_cbc(&aes_ctx, MBEDTLS_AES_ENCRYPT, len, iv, (uint8_t *)in_data, (uint8_t *)out_data);
}
void aes_lib::cbc_decrypt(uint8_t *in_data, uint8_t *out_data, uint32_t len)
{
    CHECK_LEN(len);
    mbedtls_aes_crypt_cbc(&aes_ctx, MBEDTLS_AES_DECRYPT, len, iv, (uint8_t *)in_data, (uint8_t *)out_data);
}