/*
	Single-file, STB-style, library to generate UUID:s. No dependencies
	except for OS-provided functionality.
	version 0.1, August, 2016
	Copyright (C) 2016- Fredrik Kihlander
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.
	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:
	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.
	Fredrik Kihlander
*/

#ifndef __UUID_H_INCLUDED__
#define __UUID_H_INCLUDED__

#include "ctype.h"
#include "string.h"
#include "inttypes.h"
#include <stdbool.h>

typedef struct s_uuid
{
	unsigned char bytes[16];
} s_uuid_t;

/**
 * Set s_uuid_t to the null_uuid.
 */
void uuid0_generate(s_uuid_t *res);

/**
 * Generate an s_uuid_t of version 4 ( Random ) into res.
 * @note res will be the null_uuid on failure.
 */
void uuid4_generate(s_uuid_t *res);

/**
 * Return the type of the provided s_uuid_t.
 *
 * @return 0 if it is the null-s_uuid_t
 *         1 MAC address & date-time
 *         2 DCE Security
 *         3 MD5 hash & namespace
 *         4 Random
 *         5 SHA-1 hash & namespace
 *
 *         -1 on an invalid s_uuid_t.
 */
int uuid_type(s_uuid_t *id);

/**
 * Converts an s_uuid_t to string.
 * @param id s_uuid_t to convert.
 * @param out pointer to char-buffer where to write s_uuid_t, s_uuid_t is NOT 0-terminated
 *            and is expected to be at least 36 bytes.
 * @return out
 */
char *uuid_to_string(s_uuid_t *id, char *out);

/**
 * Convert a string to an s_uuid_t.
 * @param str to convert.
 * @param out s_uuid_t to parse to.
 * @return true on success.
 */
bool uuid_from_string(const char *str, s_uuid_t *out);

/**
 * Copy s_uuid_t from src to dst.
 */
void uuid_copy(const s_uuid_t *src, s_uuid_t *dst);

// typedef struct s_uuid_to_str
// {
// 	char str[37];
// 	_uuid_to_str(s_uuid_t *id)
// 	{
// 		uuid_to_string(id, str);
// 		str[36] = '\0';
// 	}
// } s_uuid_to_str_t;

// /**
//  * Helper macro to convert s_uuid_t to string.
//  */
// #define UUID_TO_STRING(id) _uuid_to_str(id).str

#endif // __UUID_H_INCLUDED__