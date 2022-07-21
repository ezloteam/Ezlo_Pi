#ifndef __HUB_FAVORITE_LIST_H__
#define __HUB_FAVORITE_LIST_H__

#include <string>

using namespace std;

class favorite
{
protected:
    favorite() {}
    static favorite *favorite_;

public:
    static favorite *get_instance(void);
    favorite(favorite &other) = delete;
    void operator=(const favorite &) = delete;

    static string list(const char *payload, uint32_t len, struct json_token *method, uint32_t msg_count);
};

#endif // __HUB_FAVORITE_LIST_H__