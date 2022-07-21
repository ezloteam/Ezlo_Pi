#ifndef __HUB_DATA_LIST_H__
#define __HUB_DATA_LIST_H__

#include <string>

using namespace std;

class data
{
protected:
    data() {}
    static data *data_;

public:
    static data *get_instance(void);
    data(data &other) = delete;
    void operator=(const data &) = delete;

    static string list(const char *payload, uint32_t len, struct json_token *method, uint32_t msg_count);
};

#endif // __HUB_DATA_LIST_H__