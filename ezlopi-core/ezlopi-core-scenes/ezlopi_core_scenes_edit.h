#ifndef __EZLOPI_SCENES_EDIT_H__
#define __EZLOPI_SCENES_EDIT_H__

#ifdef CONFIG_EZPI_SERV_ENABLE_MESHBOTS

#include "string.h"
#include "stdint.h"
#include "cjext.h"

int ezlopi_core_scene_edit_store_updated_to_nvs(cJSON* cj_updated_scene);
int ezlopi_core_scene_edit_update_id(uint32_t scene_id, cJSON* cj_updated_scene);

#endif  // CONFIG_EZPI_SERV_ENABLE_MESHBOTS

#endif // __EZLOPI_SCENES_EDIT_H__
