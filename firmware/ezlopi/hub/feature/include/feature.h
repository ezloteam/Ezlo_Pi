#ifndef __HUB_FEATURE_H__
#define __HUB_FEATURE_H__

#include <string>

using namespace std;

class feature
{
protected:
    feature() {}
    static feature *feature_;

public:
    static feature *get_instance(void);
    feature(feature &other) = delete;
    void operator=(const feature &) = delete;

    static string list(const char *payload, uint32_t len, struct json_token *method);
};

#endif // __HUB_FEATURE_H__