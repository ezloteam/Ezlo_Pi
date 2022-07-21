#ifndef __HUB_SCENES_LIST_H__
#define __HUB_SCENES_LIST_H__

#include <string>

using namespace std;

class scenes
{
protected:
    scenes() {}
    static scenes *scenes_;

public:
    static scenes *get_instance(void);
    scenes(scenes &other) = delete;
    void operator=(const scenes &) = delete;

    static string list(const char *payload, uint32_t len, struct json_token *method, uint32_t msg_count);
};

#endif // __HUB_SCENES_LIST_H__