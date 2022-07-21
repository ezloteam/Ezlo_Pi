#ifndef __HUB_ITEMS_LIST_H__
#define __HUB_ITEMS_LIST_H__

#include <string>

using namespace std;

class items
{
protected:
    items() {}
    static items *items_;

public:
    static items *get_instance(void);
    items(items &other) = delete;
    void operator=(const items &) = delete;

    static string list(const char *payload, uint32_t len, struct json_token *method, uint32_t msg_count);
    static string update(const char *payload, uint32_t len, struct json_token *method, uint32_t msg_count);
    static string set_value(const char *payload, uint32_t len, struct json_token *method, uint32_t msg_count);
    static string update(const char *payload, uint32_t len, struct json_token *method, uint32_t msg_count, int device_index);
};

#endif // __HUB_ITEMS_LIST_H__