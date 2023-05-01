#ifndef __MAC_UUID_H__
#define __MAC_UUID_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <esp_system.h>
#include <esp_wifi.h>
#include <mbedtls/sha1.h>

#ifdef __cplusplus
extern "C"
{
#endif

void ezlopi_generate_UUID(char* uuid);

#ifdef __cplusplus
}
#endif

#endif // __MAC_UUID_H__