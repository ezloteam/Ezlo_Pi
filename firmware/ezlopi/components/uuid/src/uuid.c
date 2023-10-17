#include "uuid.h"
#include <ctype.h>
#include <string.h>

#if defined(__LINUX__) || defined(__linux__) || defined(__ANDROID__)
#include <stdio.h>
#endif

#if defined(_MSC_VER)
#include <Objbase.h>
#pragma comment(lib, "Ole32.lib")
#endif

#if defined(__APPLE__)
#include <CoreFoundation/CFUUID.h>
#endif

char *uuid_to_string(s_uuid_t *id, char *out)
{
    static const char TOHEXCHAR[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

    char *c = out;
    int src_byte = 0;
    for (int i = 0; i < 4; ++i)
    {
        *c++ = TOHEXCHAR[(id->bytes[src_byte] >> 4) & 0xF];
        *c++ = TOHEXCHAR[id->bytes[src_byte] & 0xF];
        ++src_byte;
    }
    *c++ = '-';

    for (int i = 0; i < 2; ++i)
    {
        *c++ = TOHEXCHAR[(id->bytes[src_byte] >> 4) & 0xF];
        *c++ = TOHEXCHAR[id->bytes[src_byte] & 0xF];
        ++src_byte;
    }
    *c++ = '-';

    for (int i = 0; i < 2; ++i)
    {
        *c++ = TOHEXCHAR[(id->bytes[src_byte] >> 4) & 0xF];
        *c++ = TOHEXCHAR[id->bytes[src_byte] & 0xF];
        ++src_byte;
    }
    *c++ = '-';

    for (int i = 0; i < 2; ++i)
    {
        *c++ = TOHEXCHAR[(id->bytes[src_byte] >> 4) & 0xF];
        *c++ = TOHEXCHAR[id->bytes[src_byte] & 0xF];
        ++src_byte;
    }
    *c++ = '-';

    for (int i = 0; i < 6; ++i)
    {
        *c++ = TOHEXCHAR[(id->bytes[src_byte] >> 4) & 0xF];
        *c++ = TOHEXCHAR[id->bytes[src_byte] & 0xF];
        ++src_byte;
    }

    return out;
}

bool uuid_from_string(const char *str, s_uuid_t *out)
{
    char uuid_str[32];
    char *outc = uuid_str;
    for (int i = 0; i < 36; ++i)
    {
        char c = str[i];
        if (i == 8 || i == 13 || i == 18 || i == 23)
        {
            if (c != '-')
                return false;
        }
        else
        {
            if (!isxdigit(c))
                return false;
            *outc = (char)tolower(c);
            ++outc;
        }
    }

#define UUID_HEXCHRTO_DEC(c) (unsigned char)((c) <= '9' ? ((c) - '0') : 10 + (c) - ((c) <= 'F' ? 'A' : 'a'))

    for (int byte = 0; byte < 16; ++byte)
    {
        unsigned char v1 = UUID_HEXCHRTO_DEC(uuid_str[byte * 2]);
        unsigned char v2 = UUID_HEXCHRTO_DEC(uuid_str[byte * 2 + 1]);
        out->bytes[byte] = (unsigned char)((v1 << 4) | v2);
    }
#undef UUID_HEXCHRTO_DEC

    return true;
}

void uuid0_generate(s_uuid_t *res)
{
    memset(res, 0x0, sizeof(s_uuid_t));
}

void uuid4_generate(s_uuid_t *res)
{
    uuid0_generate(res);

#if defined(__LINUX__) || defined(__linux__) || defined(__ANDROID__)
    FILE *f = fopen("/proc/sys/kernel/random/s_uuid_t", "rb");
    if (f == 0x0)
        return;

    char uuid_str[36];
    size_t read = fread(uuid_str, 1, sizeof(uuid_str), f);
    fclose(f);
    if (read != 36)
        return;
    uuid_from_string(uuid_str, res);
#elif defined(_MSC_VER)
    GUID g;
    HRESULT hres = CoCreateGuid(&g);
    if (hres != S_OK)
        return;
    // ... endian swap to little endian to make s_uuid_t memcpy:able ...
    g.Data1 = ((g.Data1 & 0x00FF) << 24) | ((g.Data1 & 0xFF00) << 8) | ((g.Data1 >> 8) & 0xFF00) | ((g.Data1 >> 24) & 0x00FF);
    g.Data2 = (WORD)(((g.Data2 & 0x00FF) << 8) | ((g.Data2 & 0xFF00) >> 8));
    g.Data3 = (WORD)(((g.Data3 & 0x00FF) << 8) | ((g.Data3 & 0xFF00) >> 8));
    memcpy(res->bytes, &g, sizeof(res->bytes));
#elif defined(__APPLE__)
    CFUUIDRef new_uuid = CFUUIDCreate(0x0);
    CFUUIDBytes bytes = CFUUIDGetUUIDBytes(new_uuid);

    res->bytes[0] = bytes.byte0;
    res->bytes[1] = bytes.byte1;
    res->bytes[2] = bytes.byte2;
    res->bytes[3] = bytes.byte3;
    res->bytes[4] = bytes.byte4;
    res->bytes[5] = bytes.byte5;
    res->bytes[6] = bytes.byte6;
    res->bytes[7] = bytes.byte7;
    res->bytes[8] = bytes.byte8;
    res->bytes[9] = bytes.byte9;
    res->bytes[10] = bytes.byte10;
    res->bytes[11] = bytes.byte11;
    res->bytes[12] = bytes.byte12;
    res->bytes[13] = bytes.byte13;
    res->bytes[14] = bytes.byte14;
    res->bytes[15] = bytes.byte15;
    CFRelease(new_uuid);
#else
// #  error "unhandled platform"
#endif
}

int uuid_type(s_uuid_t *id)
{
    switch ((id->bytes[6] & 0xF0) >> 4)
    {
    case 0:
        for (int i = 0; i < 16; ++i)
            if (id->bytes[i] != 0)
                return -1;
        return 0;
    case 1:
        return 1;
    case 2:
        return 2;
    case 3:
        switch ((id->bytes[8] & 0xF0) >> 4)
        {
        case 8:
        case 9:
        case 10:
        case 11:
            return 4;
        default:
            return -1;
        }
        break;
    case 4:
        switch ((id->bytes[8] & 0xF0) >> 4)
        {
        case 8:
        case 9:
        case 10:
        case 11:
            return 4;
        default:
            return -1;
        }
        break;
    case 5:
        return 5;
    }
    return -1;
}

void uuid_copy(const s_uuid_t *src, s_uuid_t *dst)
{
    memcpy(dst, src, sizeof(s_uuid_t));
}
