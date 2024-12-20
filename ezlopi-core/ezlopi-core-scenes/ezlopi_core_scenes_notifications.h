#ifndef _EZLOPI_CORE_SCENES_NOTIFICATIONS_H_
#define _EZLOPI_CORE_SCENES_NOTIFICATIONS_H_

#ifdef CONFIG_EZPI_SERV_ENABLE_MESHBOTS

#include "cjext.h"

// void ezlopi_scenes_notifications_add(cJSON *cj_notifications);
void ezlopi_scenes_notifications_remove(cJSON* cj_notifications);

#endif  // CONFIG_EZPI_SERV_ENABLE_MESHBOTS

#endif // _EZLOPI_CORE_SCENES_NOTIFICATIONS_H_
