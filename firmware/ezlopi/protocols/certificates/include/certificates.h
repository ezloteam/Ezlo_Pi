#ifndef __CERTIFICATES_H__
#define __CERTIFICATES_H__

// #pragma once

class certificates
{
public:
    static const char *get_ca_certificates(void);
    static const char *get_shared_key(void);
    static const char *get_private_key(void);
    static const char *get_public_key(void);
};

#endif // __CERTIFICATES_H__