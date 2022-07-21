#ifndef __HUB_ROOM_H__
#define __HUB_ROOM_H__
#include <string>
#include <cstring>
#include "frozen.h"

using namespace std;

class room
{
protected:
    room() {}
    static room *room_;

public:
    static room *get_instance(void);
    room(room &other) = delete;
    void operator=(const room &) = delete;

    static string list(const char *payload, uint32_t len, struct json_token *method, uint32_t msg_count);
};

#endif // __HUB_ROOM_H__