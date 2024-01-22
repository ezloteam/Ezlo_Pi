#ifndef _EZLOPI_CORE_ACTIONS_H_
#define _EZLOPI_CORE_ACTIONS_H_

#include "string.h"

typedef enum e_ezlopi_actions
{
#define EZLOPI_ACTION(action, name) EZLOPI_ACTION_##action,
#include "ezlopi_core_actions_list.h"
} e_ezlopi_actions_t;

char *ezlopi_actions_to_string(e_ezlopi_actions_t action);

#endif // _EZLOPI_CORE_ACTIONS_H_
