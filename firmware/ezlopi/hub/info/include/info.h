#ifndef __HUB_INFO_H__
#define __HUB_INFO_H__

#include <string>

using namespace std;

class info
{
protected:
    info() {}
    static info *info_;

public:
    static info *get_instance(void);
    info(info &other) = delete;
    void operator=(const info &) = delete;

    static string get(const char *payload, uint32_t len, struct json_token *method, uint32_t msg_count);
};

#endif // __HUB_INFO_H__