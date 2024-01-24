#include "ezlopi_core_actions.h"

char *ezlopi_action_names[] = {
#define EZLOPI_ACTION(action, name) name,
#include "ezlopi_core_actions_list.h"
};

char *ezlopi_actions_to_string(e_ezlopi_actions_t action)
{
    char *ret = NULL;

    if (EZLOPI_ACTION_MAX > action)
    {
        ret = ezlopi_action_names[action];
    }

    return ret;
}