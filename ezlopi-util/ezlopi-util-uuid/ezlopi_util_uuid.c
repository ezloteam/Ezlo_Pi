#include <stdlib.h>
#include <time.h>

#include "config.h"
#include "ezlopi_util_uuid.h"

const unsigned char hex[16] = "0123456789abcdef";

static int __hex2dec(char c);

void ezlopi_util_uuid_generate_random(ezlopi_uuid_t out)
{
    int i, j, rnd;
    static int seeded = 0;

    if (!seeded)
    {
        srand(time(NULL));
        seeded = 1;
    }

    for (i = 0; i < (16 / RAND_LENGTH); i++)
    {
        rnd = rand();

        for (j = 0; j < RAND_LENGTH; j++)
        {
            out[i * RAND_LENGTH + j] = (0xff & rnd >> (8 * j));
        }
    }

    // set the version to 4
    out[6] = (out[6] & 0x0f) | 0x40;

    // set the variant to 1 (a)
    out[8] = (out[8] & 0x0f) | 0xa0;
}

void ezlopi_util_uuid_parse(const char *in, ezlopi_uuid_t uuid)
{
    int i, j;

    i = j = 0;

    do
    {
        switch (in[i])
        {
        case '-':
        {
            break;
        }
        default:
        {
            uuid[j++] = (__hex2dec(in[i]) << 4) | __hex2dec(in[i + 1]);
        }
        }

        i+=2;
        
    } while (j < 16 && i < 36);
}

void ezlopi_util_uuid_unparse(const ezlopi_uuid_t uuid, char *out)
{
    int i, j;

    i = j = 0;

    do
    {
        switch (j)
        {
        case 4:
        case 6:
        case 8:
        case 10:
            out[i++] = '-';
        }

        out[i++] = hex[(uuid[j] >> 4)];
        out[i++] = hex[(0xf & uuid[j])];

        j++;
    } while (j < 16);

    out[36] = 0;
}

void ezlopi_util_uuid_copy(ezlopi_uuid_t dst, const ezlopi_uuid_t src)
{
    int i;
    for (i = 0; i < sizeof(ezlopi_uuid_t); i++)
    {
        dst[i] = src[i];
    }
}

static int __hex2dec(char c)
{
    int i;

    for (i = 0; i < 16; i++)
    {
        if (hex[i] == c)
            return i;
    }

    return -1;
}
