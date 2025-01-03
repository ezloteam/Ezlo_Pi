

#ifndef __EZLOPI_CORE_OFFLINE_LOGIN_H_
#define __EZLOPI_CORE_OFFLINE_LOGIN_H_

#include "cjext.h"
#include "ezlopi_core_errors.h"

ezlopi_error_t ezlopi_core_offline_login_perform(cJSON *cj_params);
ezlopi_error_t ezlopi_core_offline_logout_perform();
bool is_user_logged_in();

#endif // __EZLOPI_CORE_OFFLINE_LOGIN_H_
