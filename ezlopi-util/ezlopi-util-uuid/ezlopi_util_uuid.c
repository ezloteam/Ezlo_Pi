/* ===========================================================================
** Copyright (C) 2025 Ezlo Innovation Inc
**
** Under EZLO AVAILABLE SOURCE LICENSE (EASL) AGREEMENT
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met:
**
** 1. Redistributions of source code must retain the above copyright notice,
**    this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. Neither the name of the copyright holder nor the names of its
**    contributors may be used to endorse or promote products derived from
**    this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
** AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
** ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
** LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
** CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
** SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
** INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
** CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
** ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
** POSSIBILITY OF SUCH DAMAGE.
** ===========================================================================
*/

/**
 * @file    main.c
 * @brief   perform some function on data
 * @author
 * @version 0.1
 * @date    1st January 2024
 */

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include <stdlib.h>
#include <time.h>

// #include "config.h"
#include "ezlopi_util_uuid.h"

/*******************************************************************************
 *                          Extern Data Declarations
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Declarations
 *******************************************************************************/

/*******************************************************************************
 *                          Type & Macro Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Static Function Prototypes
 *******************************************************************************/
static int __hex2dec(char c);

/*******************************************************************************
 *                          Static Data Definitions
 *******************************************************************************/
const unsigned char hex[16] = "0123456789abcdef";

/*******************************************************************************
 *                          Extern Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Definitions
 *******************************************************************************/

/**
 * @brief Global/extern function template example
 * Convention : Use capital letter for initial word on extern function
 * @param arg
 */
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

        i += 2;

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

/*******************************************************************************
 *                          Static Function Definitions
 *******************************************************************************/

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

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
