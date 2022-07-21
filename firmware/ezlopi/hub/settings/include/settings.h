#ifndef __HUB_SETTINGS_H__
#define __HUB_SETTINGS_H__

#include <string>

using namespace std;

class settings
{
private:
protected:
    settings() {}
    static settings *settings_;

public:
    static settings *get_instance(void);
    settings(settings &other) = delete;
    void operator=(const settings &) = delete;

    static string list(const char *payload, uint32_t len, struct json_token *method, uint32_t msg_count);
};

#endif // __HUB_SETTINGS_H__