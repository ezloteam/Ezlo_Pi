#ifndef __AES_LIB_H__
#define __AES_LIB_H__

#include "mbedtls/aes.h"
#include "cstring"
#include "trace.h"
#include "string"

class aes_lib
{
private:
    uint8_t iv[16];
    uint8_t aes_key[16];
    mbedtls_aes_context aes_ctx;

protected:
public:
    void reset_iv(void);
    void set_key(uint8_t *key);
    void ecb_encrypt(uint8_t *in_data, uint8_t *out_data, uint32_t len);
    void ecb_decrypt(uint8_t *in_data, uint8_t *out_data, uint32_t len);
    void cbc_encrypt(uint8_t *in_data, uint8_t *out_data, uint32_t len);
    void cbc_decrypt(uint8_t *in_data, uint8_t *out_data, uint32_t len);
};

#endif // __AES_LIB_H__