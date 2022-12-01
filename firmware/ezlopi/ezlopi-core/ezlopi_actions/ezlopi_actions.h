#ifndef __EZLOPI_ACTIONS_H__
#define __EZLOPI_ACTIONS_H__

#include "string.h"

typedef enum e_ezlopi_actions
{
#define EZLOPI_ACTIONS(action, name) EZLOPI_ACTION_##action,
#include "ezlopi_actions_list.h"
} e_ezlopi_actions_t;

char *ezlopi_actions_to_string(e_ezlopi_actions_t action);

#endif //
