#ifndef __EZLOPI_UTIL_UUID_H__
#define __EZLOPI_UTIL_UUID_H__

#define RAND_LENGTH 4

typedef unsigned char ezlopi_uuid_t[16];

#define uuid_generate(out) ezlopi_util_uuid_generate_random(out)

void ezlopi_util_uuid_generate_random(ezlopi_uuid_t out);
void ezlopi_util_uuid_unparse(const ezlopi_uuid_t uuid, char *out);
void ezlopi_util_uuid_copy(ezlopi_uuid_t dst, const ezlopi_uuid_t src);
void ezlopi_util_uuid_parse(const char *in, ezlopi_uuid_t uuid);

#endif // __EZLOPI_UTIL_UUID_H__