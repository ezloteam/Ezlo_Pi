#ifndef __CERTIFICATES_H__
#define __CERTIFICATES_H__
#ifdef __cplusplus
extern "C"
{
#endif

    const char *certificates_get_ca_certificates(void);
    const char *certificates_get_shared_key(void);
    const char *certificates_get_private_key(void);
    const char *certificates_get_public_key(void);

#ifdef __cplusplus
}
#endif

#endif // __CERTIFICATES_H__