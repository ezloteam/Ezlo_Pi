#ifndef __HUB_MODES_H__
#define __HUB_MODES_H__

#include <string>

using namespace std;

class modes
{
protected:
    modes() {}
    static modes *modes_;

public:
    static modes *get_instance(void);
    modes(modes &other) = delete;
    void operator=(const modes &) = delete;

    static string get(const char *payload, uint32_t len, struct json_token *method, uint32_t msg_count);
};

#endif // __HUB_MODES_H__