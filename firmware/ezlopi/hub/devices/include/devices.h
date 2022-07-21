#ifndef __HUB_DEVICE_LIST_H__
#define __HUB_DEVICE_LIST_H__

#include <string>

using namespace std;

class devices
{
protected:
    devices() {}
    static devices *devices_;

public:
    static devices *get_instance(void);
    devices(devices &other) = delete;
    void operator=(const devices &) = delete;

    static string list(const char *payload, uint32_t len, struct json_token *method, uint32_t msg_count);
    static string settings_list(const char *payload, uint32_t len, struct json_token *method, uint32_t msg_count);
};

#endif // __HUB_DEVICE_LIST_H__