#ifndef __EZLOPI_UTIL_UUID_H__
#define __EZLOPI_UTIL_UUID_H__

#define RAND_LENGTH 4

typedef unsigned char uuid_t[16];

#define uuid_generate(out) ezlopi_util_uuid_generate_random(out)

void ezlopi_util_uuid_generate_random(uuid_t out);
void ezlopi_util_uuid_unparse(const uuid_t uuid, char *out);
void ezlopi_util_uuid_copy(uuid_t dst, const uuid_t src);
void ezlopi_util_uuid_parse(const char *in, uuid_t uuid);

#endif // __EZLOPI_UTIL_UUID_H__