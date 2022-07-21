#ifndef __HUB_REGISTERATION_H__
#define __HUB_REGISTERATION_H__

#include <string>
#include "websocket_client.h"

using namespace std;

class registeration
{
private:
    static void registeration_process(void *pv);

protected:
    registeration() {}
    static registeration *registeration_;

public:
    void init(websocket_client *ws_client);
    static registeration *get_instance(void);
    registeration(registeration &other) = delete;
    void operator=(const registeration &) = delete;

    static string registered(const char *payload, uint32_t len, struct json_token *method, uint32_t msg_count);
};

#endif // __HUB_REGISTERATION_H__