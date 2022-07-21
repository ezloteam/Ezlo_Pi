#ifndef __HUB_GATEWAYS_H__
#define __HUB_GATEWAYS_H__

#include <string>

using namespace std;

class gateways
{
protected:
    gateways() {}
    static gateways *gateways_;

public:
    static gateways *get_instance(void);
    gateways(gateways &other) = delete;
    void operator=(const gateways &) = delete;

    static string list(const char *payload, uint32_t len, struct json_token *method, uint32_t msg_count);
};

#endif // __HUB_GATEWAYS_H__