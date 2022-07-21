#ifndef __WEB_PROVISIONING_H__
#define __WEB_PROVISIONING_H__

#include <string>
using namespace std;

class web_provisioning
{
private:
protected:
    web_provisioning() {}
    static web_provisioning *web_provisioning_;

public:
    void init(string &uri);
    static web_provisioning *get_instance(void);

    uint32_t get_message_count(void);
    static void send_to_nma_websocket(string &data);
    static void message_upcall(const char *payload, uint32_t len);
    static std::string parse_device_from_web_payload(void *ws_ctx, string web_payload);

    web_provisioning(web_provisioning &other) = delete;
    void operator=(const web_provisioning &) = delete;
};

#endif // __WEB_PROVISIONING_H__